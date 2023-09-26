#include "SocketIO.hh"
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include<sys/socket.h>
#include "ErrorCheck.hh"
#include "log4class.hh"

namespace nycpp
{

SocketIO::SocketIO(int fd)
    :_fd(fd)
{}

/*
    if fd is closed unexpected, return char number had been read
    if error, return count + 1  
*/

size_t SocketIO::readn(void* buf, size_t count)
{
    char* ptr = static_cast<char*>(buf);
    int left = count;
    int ret = 0;

    while (left > 0)
    {
        do{
            ret = ::recv(_fd, ptr, left, 0);
        }while(-1 == ret && errno == EINTR);
    
        if(-1 == ret )
        {
            ::perror("recv");
            return count + 1;  
        }
        else if( 0 == ret )
        {
            logError("socket fd: %d is closed.", _fd);            
            break;
        }
        else
        {
            ptr += ret;
            left -= ret;
        }
    }
    return count - left;
}

size_t SocketIO::writen(const void* buf, size_t count)
{
    int left = count;
    const char* ptr = static_cast<const char*>(buf);
    int ret = 0;
    while(left > 0){
    
        do{
            ret = ::send(_fd, ptr, left, 0);
        }while(-1 == ret && errno == EINTR);

        if(-1 == ret)
        {
            ::perror("send");
            logError("fd : %d send function retVal = -1 ", _fd);
            break;
        }
        else if(0 == ret)
        {

            logError("fd : %d send function retVal = 0.", _fd);
            break;
        }
        else{
            ptr += ret;
            left -= ret;
        }
    }

    return count - left;
}

size_t SocketIO::readline(char* buf, size_t max)
{
    int left = max;
    int total = 0;
    char* ptr = buf;
    int ret = 0;

    size_t rcv_size = 0;
    socklen_t optlen = sizeof(rcv_size);

    ret = ::getsockopt(_fd, SOL_SOCKET, SO_RCVBUF, &rcv_size, &optlen);
    ERROR_CHECK(ret, -1, "getsocket");
    
    if(rcv_size < max -1)
    {
        logError("buffer is smaller than max.");
        return max + 1;
    }

    while(left > 0)
    {
        do{
            ret = ::recv(_fd, ptr, left, MSG_PEEK);
        }while(-1 == ret && errno == EINTR);


        if(-1 == ret)
        {
            ::perror("recv");
            return max+1;
        }
        else if(0 == ret)
        {
            logError("socket fd : %d has been closed.", _fd);
            break;
        }
        else{
            int realReadCnt = 0;
            for(int idx = 0; idx < ret && idx < left; ++idx, ++realReadCnt)
            {
                if(ptr[idx] == '\n')
                {
                    int sz = idx + 1;

                    size_t ret = SocketIO::readn(ptr, sz);
                    if(ret < sz)
                    {
                        return max + 1;
                    }
                    else if( ret < sz)
                    {
                        total += ret;
                        left  -= ret;
                        return total; 
                    }

                    return total + sz;
                }
            }

            int ret = SocketIO::readn(ptr, realReadCnt);

            if(ret > realReadCnt){
                return max + 1;
            }
            total += ret;
            ptr += ret;
            left -= ret;
        }
    }

    char clean_buf[1024] = {0};
    do{
        ret = ::recv(_fd, clean_buf, sizeof(clean_buf), MSG_DONTWAIT);
    }while(ret > 0 || (ret == -1 && errno == EINTR));

    return total;
}

};//namespace