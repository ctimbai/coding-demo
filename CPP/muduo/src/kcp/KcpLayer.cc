#include "KcpLayer.h"
#include "KcpUtils.h"

#include "reactor/EventLoop.h"
#include "logging/Logging.h"

#include <sys/eventfd.h>
#include <unistd.h>

using namespace libcpp;
using namespace std::placeholders;

/*
 * event fd to notify app that data/new conn arrive
 */
static int createEventfd()
{
  int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  if (evtfd < 0)
  {
    LOG_ERROR << "Failed in eventfd";
    abort();
  }
  return evtfd;
}

KcpLayer::KcpLayer(EventLoop* loop, const InetAddress& addr)
  : loop_(loop),
    inetAddr_(addr),
    socket_(sockets::createSockfdNonBlockingOrDie(sockets::SOCK_TYPE_UDP)),
    channel_(loop, socket_.fd()),
    dataFd_(createEventfd()),
    dataChannel_(loop, dataFd_),
    connFd_(createEventfd()),
    connChannel_(loop, connFd_),
    nextConv_(1)
{
  channel_.setReadCallback(
      std::bind(&KcpLayer::handleRead, this, _1));
  channel_.enableReading();
}

KcpLayer::~KcpLayer()
{
  for (auto& conn : connections_) {
    ikcp_flush(conn.second.kcpcb_);
    ikcp_release(conn.second.kcpcb_);
  }
}


void KcpLayer::setConnectionCallback(const EventCallback& cb)
{
  connChannel_.setReadCallback(cb);
  connChannel_.enableReading();
}

void KcpLayer::setMessageCallback(const EventCallback& cb)
{
  dataChannel_.setReadCallback(cb);
  dataChannel_.enableReading();
}


/*****************************************************************/
/*                      User level call                          */
/*****************************************************************/
int64_t KcpLayer::getConnectionConv()
{
  int64_t id = 1;
  ssize_t n = ::read(connFd_, &id, sizeof id);
  if (n != sizeof id) {
    LOG_ERROR << "connFd_ reads " << n << " bytes instead of " << sizeof id;
  }
  /* 
   * if id is positive, it means new conn
   * if id is negative, it means close
   */
  return id;
}


void KcpLayer::listen()
{
  socket_.setReuseAddr(true);
  socket_.bind(inetAddr_);
}

void KcpLayer::connect()
{
  ssize_t n = ::sendto(socket_.fd(), KCP_HANDSHAKE_SYN, strlen(KCP_HANDSHAKE_SYN),
                         0, (const struct sockaddr*) &inetAddr_.getSockAddrInet(), sizeof inetAddr_);
  if (n < 0) {
    LOG_SYSERR << "handshake syn send error";
  }
}

void KcpLayer::send(IUINT32 conv, const std::string& msg)
{
  LOG_TRACE << "ikcp_send " << msg;
  ikcpcb* cb = connections_[conv].kcpcb_;
  ikcp_send(cb, msg.data(), msg.size());
  kcpUpdate(cb);
}

int KcpLayer::receive(IUINT32* conv, char* buf, int len)
{
  int64_t id = 1;
  ssize_t n = ::read(dataFd_, &id, sizeof id);
  if (n != sizeof id) {
    LOG_ERROR << "dataFd_ reads " << n << " bytes instead of " << sizeof id;
  }

  *conv = static_cast<IUINT32>(id);
  // FIXME: EAGAIN
  LOG_DEBUG << "peeksize " << ikcp_peeksize(connections_[*conv].kcpcb_);
  int nrecv = ikcp_recv(connections_[*conv].kcpcb_, buf, len);
  return nrecv;
}


/*****************************************************************/
/*                   KCP stack procedure                         */
/*****************************************************************/
void KcpLayer::handleRead(TimeStamp receivedTime)
{
  struct sockaddr_in peerAddr;
  ssize_t n = recvUdpMsg(&peerAddr);

  if (n > 0) {
    std::string payload = datagram_.string();
    if (isHandshakeSyn(payload)) {
      /* server-side create a new kcp connection */
      ikcpcb* cb = ikcp_create(nextConv_, this);
      connections_[nextConv_].kcpcb_ = cb;
      connections_[nextConv_].peerAddr_ = peerAddr;
      ikcp_nodelay(cb, 1, 10, 2, 1);
      ikcp_setoutput(cb, &KcpLayer::output);
      kcpUpdate(cb);
      // TODO: set params
      std::string ack(KCP_HANDSHAKE_ACK, strlen(KCP_HANDSHAKE_ACK));
      ack += std::to_string(nextConv_);
      LOG_TRACE << ack;
      ssize_t n = ::sendto(socket_.fd(), ack.data(), ack.size(),
                             0, (const struct sockaddr*) &peerAddr, sizeof peerAddr);
      if (n < 0) {
        LOG_SYSERR << "handshake syn send error";
      }

      notifyConnection(static_cast<int64_t>(nextConv_));
      nextConv_++;
    }
    else if (isHandshakeAck(payload)) {
      /* client-side create a new kcp connection */
      IUINT32 id = std::stoul(payload.substr(payload.find_last_not_of("0123456789") + 1));
      LOG_TRACE << "ack bring conv id = " << id;
      ikcpcb* cb = ikcp_create(id, this);
      connections_[id].kcpcb_ = cb;
      connections_[nextConv_].peerAddr_ = peerAddr;
      ikcp_nodelay(cb, 1, 10, 2, 1);
      ikcp_setoutput(cb, &KcpLayer::output);
      kcpUpdate(cb);
      // TODO: how notify app the connection established
      notifyConnection(static_cast<int64_t>(id));
    }
    else if (isClosePacket(payload)) {
      // TODO: close a connection
    }
    else {
      /* write dataFd_ to notify user app */
      IUINT32 id = ikcp_getconv(static_cast<const void*>(payload.data()));
      ikcpcb* cb = connections_[id].kcpcb_;
      LOG_TRACE << payload << " datagram_ size = " << datagram_.length();
      ikcp_input(cb, datagram_.data(), datagram_.length());
      /* check when to call update */
      kcpUpdate(cb);
      if (payload.size() > 24) notifyData(id);
    }
  }
}

void KcpLayer::notifyData(int64_t conv)
{
  ssize_t n = ::write(dataFd_, &conv, sizeof conv);
  if (n != sizeof conv)
  {
    LOG_SYSERR << "dataFd writes " << n << " bytes instead of " << sizeof conv;
  }
}

void KcpLayer::notifyConnection(int64_t conv)
{
  ssize_t n = ::write(connFd_, &conv, sizeof conv);
  if (n != sizeof conv)
  {
    LOG_SYSERR << "connFd writes " << n << " bytes instead of " << sizeof conv;
  }
}


void KcpLayer::kcpUpdate(ikcpcb* cb)
{
  IUINT32 current = static_cast<IUINT32>(TimeStamp::now().microSecondsSinceEpoch());
  ikcp_update(cb, current);
  //IUINT32 future = ikcp_check(cb, current);
  //loop_->runAfter(static_cast<double>(future-current) / 1000, std::bind(&KcpLayer::kcpUpdate, this, cb));
}

ssize_t KcpLayer::recvUdpMsg(struct sockaddr_in* peerAddr)
{
  /*
   * ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags);
   */
  datagram_.reset();

  // TODO: buffer more than one packet
  char extrabuf[65536];
  struct iovec vec[2];
  const size_t writable = datagram_.avail();
  vec[0].iov_base = datagram_.current();
  vec[0].iov_len = writable;
  vec[1].iov_base = extrabuf;
  vec[1].iov_len = sizeof extrabuf;

  struct msghdr msg;
  msg.msg_name = (void*)peerAddr;
  msg.msg_namelen = sizeof(*peerAddr);
  msg.msg_iov = vec;
  msg.msg_iovlen = 2;
  msg.msg_control = nullptr;
  msg.msg_controllen = 0;
  msg.msg_flags = 0;

  const ssize_t n = ::recvmsg(socket_.fd(), &msg, MSG_WAITALL);
  if (n < 0) {
    LOG_SYSERR << "recvmsg error";
  }
  else if (static_cast<size_t>(n) <= writable) {
    datagram_.add(n);
    /* we need to record the peer addr */
    //::memcpy(peerAddr, msg.msg_name, msg.msg_namelen);
  }
  else {
    datagram_.add(n);
    /* we need to record the peer addr */
    //::memcpy(peerAddr, msg.msg_name, msg.msg_namelen);
  }

  return n;
}

ssize_t KcpLayer::sendUdpMsg(IUINT32 conv, const char* buf, int len)
{
  LOG_TRACE << "output " << std::string(buf, len);
  struct sockaddr_in destAddr = connections_[conv].peerAddr_;
  //struct sockaddr_in destAddr;
  //::memcpy(&destAddr, &connections_[conv].peerAddr_, sizeof(struct sockaddr_in));
  ssize_t n = ::sendto(socket_.fd(), buf, len, 0, (const struct sockaddr*) &destAddr, sizeof destAddr);
  if (n < 0) {
    LOG_SYSERR << "sendUdpMsg error";
  }
  return n;
}


int KcpLayer::output(const char* buf, int len, ikcpcb* kcp, void* user)
{
  return ((KcpLayer*)user)->sendUdpMsg(kcp->conv, buf, len);
}
