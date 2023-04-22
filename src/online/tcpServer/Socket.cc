#include "Socket.hh"
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "ErrorCheck.hh"

namespace nycpp
{
    Socket::Socket()
    {
        _fd = ::socket(AF_INET, SOCK_STREAM, 0);
        ERROR_CHECK(_fd, -1, "socket");
    }

    Socket::Socket(int fd)
        :_fd(fd)
    {}

    Socket::~Socket()
    {
        while(::close(_fd) < 0 && errno == EINTR );
    }

    int Socket::getFd() const
    {
        return _fd;
    }

    void Socket::shutDownWrite()
    {
        int ret = ::shutdown(_fd, SHUT_WR);
        ERROR_CHECK(ret, -1, "socket shutdown");
    }

    void Socket::setNonBlock()
    {
        int flag = ::fcntl(_fd, F_GETFL, 0);
        flag = flag | O_NONBLOCK;
        ::fcntl(_fd, F_SETFL, flag);
    }
}
