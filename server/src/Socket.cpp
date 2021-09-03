#include "Socket.h"
std::unordered_map<int,int> Socket::fdCount;
bool Socket::checkSocket()
{
    if (socket_fd < 0)
    {
        httperror("Socket: invalid socket_fd");
        return false;
    }
    return true;
}
Socket::Socket():socket_fd(-1),ip_port(0)
{
}
Socket::Socket(Socket& other)
{
    this->socket_fd=other.getSocketFd();
    fdCount[socket_fd]++;
    this->ip_addr = other.getaddr();
    this->ip_port = other.getport();
}
int Socket::getSocketFd() const
{
    return socket_fd;
}
int Socket::setReuseAddr()
{
    if (!checkSocket())
        return -1;
    int opt = 1;
    int ret = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof opt);
    if (ret == -1)
    {
        std::stringstream info;
        info << "Socket: Fail to set socket_fd option <"
            << __FILE__ << "> " << "at " << __LINE__ <<
            " thread: " << getpid();
        httperror(info.str());
    }
    return 0;
}
int Socket::setNonblock()
{
    if (!checkSocket())
        return -1;
    int opt = fcntl(socket_fd, F_GETFL);
    fcntl(socket_fd, F_SETFL, opt | O_NONBLOCK);
    return 0;
}
Socket::~Socket()
{
    if (socket_fd > 0)
    {
        int count=--fdCount[socket_fd];
        if (count <= 0)
        {
            close();
        }
    }
}
bool Socket::close()
{
    if (socket_fd >= 0)
    {
        ::close(socket_fd);
        fdCount.erase(socket_fd);
        socket_fd = -1;
        ip_addr.clear();
        ip_port = 0;
        return true;
    }
    return false;
    
}
int Socket::setReusePort()
{
    if (!checkSocket())
        return -1;
    int opt = 1;
    int ret = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof opt);
    if (ret == -1)
    {
        std::stringstream info;
        info << "Socket: Fail to set socket_fd option <"
            << __FILE__ << "> " << "at " << __LINE__ <<
            " thread: " << getpid();
        httperror(info.str());
    }
    return 0;
}
int Socket::setSocket(int fd)
{
    if (getSocketFd()>=0)
    {
        httperror("Socket: Try to initialize Socket for multiple times");
        return -1;
    }
    if (fd < 0)
    {
        httperror("Socket: invalid socket_fd");
        return -1;
    }
    socket_fd = fd;
    fdCount[fd]++;
    return 0;
}

int listenSocket::init(const std::string& ipaddr, uint16_t port)
{
    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(sockaddr_in));
    int ret = 0;
    server_addr.sin_family = AF_INET;
    if (port < 1024 || port>65535)
    {
        httperror("ListenSocket: Invalid port, choose the default value: 8080");
        port = 8080;
    }
    server_addr.sin_port = htons(port);
    if (ipaddr.empty())
    {
        inet_pton(AF_INET, "0,0,0,0", &server_addr.sin_addr.s_addr);
    }
    else
    {
        ret = inet_pton(AF_INET, ipaddr.data(), &server_addr.sin_addr.s_addr);
        if (!ret)
        {
            httperror("ListenSocket: Invalid ip address, choose the default value");
            inet_pton(AF_INET, "0,0,0,0", &server_addr.sin_addr.s_addr);
        }
    }
    ret = socket(AF_INET, SOCK_STREAM, 0);
    if (ret > 0)
    {
        setSocket(ret);
    }
    else
    {
        std::stringstream info;
        info << "LishenSocket: Fail to get socket_fd <"
            << __FILE__ << "> " << "at " << __LINE__ <<
            " thread: " << getpid();
        httperror(info.str());
        return -1;
    }	
    setReuseAddr();
    setReusePort();
    ret = bind(getSocketFd(), (sockaddr*)&server_addr, sizeof(sockaddr));
    if (ret < 0)
    {
        std::stringstream info;
        info << "LishenSocket: Fail to bind socket_fd:"<<strerror(errno)<<" <"
            << __FILE__ << "> " << "at " << __LINE__ <<
            " thread: " << getpid();
        httperror(info.str());
        return -1;
    }
    ret = listen(getSocketFd(), MAX_LISTEN_NUM);
    if (ret < 0)
    {
        std::stringstream info;
        info << "LishenSocket: Fail to start listening:"<< strerror(errno)<< " <"
            << __FILE__ << "> " << "at " << __LINE__ <<
            " thread: " << getpid();
        httperror(info.str());
        return -1;
    }
    sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(sockaddr_in));
    socklen_t addr_len;
    getsockname(getSocketFd(), (sockaddr*)&local_addr, &addr_len);
    ip_addr=std::string(inet_ntoa(local_addr.sin_addr));
    ip_port = port;
    return 0;
}

std::string listenSocket::getaddr() const
{
    return ip_addr;
}

uint16_t listenSocket::getport() const
{
    return ip_port;
}

int connectSocket::init(int fd)
{
    int ret = setSocket(fd);
    if (ret < 0)
        return ret;
    sockaddr_in clientAddr;
    socklen_t len;
    getpeername(fd, (sockaddr*)&clientAddr, &len);
    ip_addr = std::string(inet_ntoa(clientAddr.sin_addr));
    ip_port = ntohs(clientAddr.sin_port);
    return 0;
}

std::string connectSocket::getaddr() const
{
    return ip_addr;
}

uint16_t connectSocket::getport() const
{
    return ip_port;
}
