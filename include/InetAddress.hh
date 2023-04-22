#pragma once
#include <arpa/inet.h>
#include <string>

namespace nycpp
{
    using std::string;

    class InetAddress
    {
    public:
        explicit InetAddress(const string&, unsigned short);
        explicit InetAddress(const struct sockaddr_in &);

        string ip() const;
        unsigned short port() const;
        const struct sockaddr_in* GetInetAddress() const;
    
    private:
        struct sockaddr_in _addr;
    };
}