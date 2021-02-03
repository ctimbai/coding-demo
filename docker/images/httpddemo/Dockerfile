FROM centos:8.1.1911

RUN mv /etc/yum.repos.d/CentOS-Base.repo /etc/yum.repos.d/CentOS-Base.repo.backup \
    && mv /etc/yum.repos.d/CentOS-AppStream.repo /etc/yum.repos.d/CentOS-AppStream.repo.bak \
    && mv /etc/yum.repos.d/CentOS-centosplus.repo /etc/yum.repos.d/CentOS-centosplus.repo.bak \
    && mv /etc/yum.repos.d/CentOS-Extras.repo /etc/yum.repos.d/CentOS-Extras.repo.bak \
    && mv /etc/yum.repos.d/CentOS-PowerTools.repo /etc/yum.repos.d/CentOS-PowerTools.repo.bak \
    && curl -o /etc/yum.repos.d/CentOS-Base.repo http://mirrors.aliyun.com/repo/Centos-8.repo \
    && yum makecache
RUN yum install -y httpd
COPY file1 /var/www/html/
ADD  file2.tar.gz /var/www/html/
CMD ["/sbin/httpd", "-D", "FOREGROUND"]
