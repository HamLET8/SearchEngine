#pragma once 
#include "NonCopyable.hh"


namespace nycpp
{
    class Socket : private NonCopyable
    {
    public: 
        Socket();
        explicit Socket(int);
        ~Socket();

        int getFd() const;
        void shutDownWrite();
        void setNonBlock();
    private:
        int _fd;
    };
}