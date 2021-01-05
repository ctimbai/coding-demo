#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/rtnetlink.h>
#include <net/if.h>

#define NLMSG_TAIL(nmsg) \
    ((struct rtattr *) (((void *) (nmsg)) + NLMSG_ALIGN((nmsg)->nlmsg_len)))

struct iplink_req {
    struct nlmsghdr     n;
    struct ifinfomsg    i;
    char            buf[1024];
};

int addattr_l(struct nlmsghdr *n, int maxlen, int type, const void *data,
        int alen)
{
    int len = RTA_LENGTH(alen);
    struct rtattr *rta;

    if (NLMSG_ALIGN(n->nlmsg_len) + RTA_ALIGN(len) > maxlen) {
        fprintf(stderr,
                "addattr_l ERROR: message exceeded bound of %d\n",
                maxlen);
        return -1;
    }
    rta = NLMSG_TAIL(n);
    rta->rta_type = type;
    rta->rta_len = len;
    if (alen)
        memcpy(RTA_DATA(rta), data, alen);
    n->nlmsg_len = NLMSG_ALIGN(n->nlmsg_len) + RTA_ALIGN(len);
    return 0;
}

int main(int argc, char **argv) {
    char *nsname;
    char *devname;
    int fd;
    int ret;

    struct iplink_req req = {
        .n.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg)),
        .n.nlmsg_flags = NLM_F_REQUEST | NLM_F_ACK,
        .n.nlmsg_type = RTM_NEWLINK,
        .i.ifi_family = AF_UNSPEC,
    };

    // 模仿ip命令，可以是文件fd，也可以是进程pid
    int netns;

    if (argc < 3) {
        printf("Usage: %s dev_name netns_name\n", argv[0]);
        return -1;
    }

    devname = argv[1];
    nsname = argv[2];

    // 填充netns，可以是文件fd，也可以是进程pid
    netns = open(nsname, O_RDONLY);
    if (netns >= 0) {
        addattr_l(&req.n, sizeof(req), IFLA_NET_NS_FD, &netns, 4);
    } else if (nsname[0] >= '0' && nsname[0] <= '1') {
        // 当做进程pid处理，实例代码不做完善校验
        netns = atoi(nsname);
        addattr_l(&req.n, sizeof(req), IFLA_NET_NS_PID, &netns, 4);
    } else {
        fprintf(stderr, "Invalid netns value \"%s\"\n", nsname);
        return -1;
    }

    // 填充网络设备索引号
    req.i.ifi_index = if_nametoindex(devname);
    if (req.i.ifi_index == 0) {
        fprintf(stderr, "Cannot find device \"%s\", %s\n", devname, strerror(errno));
        return -1;
    }

    // 发送请求
    fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (fd == -1) {
        fprintf(stderr, "socket failed, %s\n", strerror(errno));
        return -1;
    }

    ret = send(fd, &req, req.n.nlmsg_len, 0);
    if (ret == -1) {
        fprintf(stderr, "send failed, %s\n", strerror(errno));
        return -1;
    }

    // 接收请求结果
    ret = recv(fd, &req, sizeof(req), 0);
    if (ret == -1) {
        fprintf(stderr, "recv failed, %s\n", strerror(errno));
        return -1;
    }
    if (req.n.nlmsg_type == NLMSG_ERROR) {
        struct nlmsgerr *err;
        err = NLMSG_DATA(&req.n);
        if (err->error != 0) {
            fprintf(stderr, "%s\n", strerror(-err->error));
            return -1;
        }
    } else {
        fprintf(stderr, "unknown nlmsg_type %hu\n", req.n.nlmsg_type);
        return -1;
    }

    return 0;
}