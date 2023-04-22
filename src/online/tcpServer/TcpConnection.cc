#include "TcpConnection.hh"
#include "EventLoop.hh"

#include <sys/socket.h>
#include <sstream>
#include <memory>
#include "ErrorCheck.hh"
#include "log4class.hh"

namespace nycpp
{
    TcpConnection::TcpConnection(int cliFd, EventLoop* loopPtr)
        :_clientSock(cliFd),
         _sockIO(cliFd),
         _localAddr(getLocalAddr()),
         _peerAddr(getPeerAddr()),
         _isShutDownWrite(false),
         _loopPtr(loopPtr)
    { 
        logInfo("new connection from %s", _peerAddr.ip().c_str());
    }


    InetAddress TcpConnection::getLocalAddr()
    {
        struct sockaddr_in addr;
        socklen_t len = sizeof(struct sockaddr_in);
        int ret = ::getsockname(_clientSock.getFd(), (struct sockaddr*)&addr, &len);
        ERROR_CHECK(ret, -1, "getsockname");
     
        return InetAddress(addr);
    }


    InetAddress TcpConnection::getPeerAddr()
    {
        struct sockaddr_in addr;
        socklen_t len = sizeof(struct sockaddr_in);
        int ret = ::getpeername(_clientSock.getFd(), (struct sockaddr*)&addr, &len);
        ERROR_CHECK(ret, -1, "getpeername");

        return InetAddress(addr);
    }

    // train mode     | size  |  content  |
    void TcpConnection::send(const string& msg)
    {
        const size_t msgLen = msg.size();

        _sockIO.writen(&msgLen, sizeof(size_t));
        _sockIO.writen(msg.c_str(), msgLen);        
    }

    // At first recive size of message
    string TcpConnection::recv()
    {
        size_t msgLen = 0;
        _sockIO.readn(&msgLen, sizeof(size_t));
        logInfo("read %d bytes message from : %d", msgLen, _clientSock.getFd());

        char *buf = new char[msgLen + 1]();
        _sockIO.readn(buf, msgLen);

        string msg(buf, msgLen);
        delete[] buf;
        return msg;
    }

    string TcpConnection::recvLine()
    {
        const size_t MAX_LINE_LENGTH = 1024; // Limit maximum line length to prevent buffer overflow
        char* buf = new char[MAX_LINE_LENGTH];
        size_t ret = _sockIO.readline(buf, MAX_LINE_LENGTH);

        if (ret == 0) // Connection closed
        {
            delete[] buf;
            return {};
        }
        else if (ret > MAX_LINE_LENGTH) // Line too long
        {
            logError("recv: line too long\n");
            delete[] buf;
            return {};
        }
        else if (buf[ret - 1] != '\n') // Incomplete line
        {
            logError("recv: incomplete line\n");
            delete[] buf;
            return {};
        }

        string res(buf, ret - 1);
        delete[] buf;

        return res;
    }

    string TcpConnection::show() const
    {
        std::ostringstream oss;
        oss << "[tcp](loacl)" << _localAddr.ip() << ":" << _localAddr.port()
            << " --> "
            << "(peer)" << _peerAddr.ip() << ":" << _peerAddr.port();

        return oss.str();
    }

    // shutdown write
    bool TcpConnection::isClosed() const
    {
        char tmp_buf[128] = {0};
        int ret = 0;

        do
        {
            ret = ::recv(_clientSock.getFd(), tmp_buf, sizeof(tmp_buf), MSG_PEEK);        
        } while (ret == -1 && errno == EINTR);

        return ret == 0;   
    }

    void TcpConnection::setConnectionCallBack(const TcpConnectionCallBack& onConnectionCb)
    {
        _onConnectionCb = onConnectionCb;
    }

    void TcpConnection::setMessageCallBack(const TcpConnectionCallBack& onMessageCb)
    {
        _onMessageCb = onMessageCb;
    }

    void TcpConnection::setCloseCallBack(const TcpConnectionCallBack& onCloseCb)
    {
        _onCloseCb = onCloseCb;
    }

    void TcpConnection::handleConnectionCallBack()
    {
        if(_onConnectionCb)
        {
            _onConnectionCb(shared_from_this());
        }
    }

    void TcpConnection::handleMessageCallBack()
    {
        if(_onMessageCb)
        {
            _onMessageCb(shared_from_this());
        }
    }

    void TcpConnection::handleCloseCallBack()
    {
        if(_onCloseCb)
        {
            _onCloseCb(shared_from_this());
        }
    }

    void TcpConnection::notifyLoop(const string& msg)
    {
        _loopPtr->setPendingCallBack(std::bind(&TcpConnection::send, this, msg));
        _loopPtr->writeCounter();
    }

};