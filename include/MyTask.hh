#pragma once

#include "WebPageSearcher.hh"
#include "TcpConnection.hh"
#include "KeyRecommander.hh"
#include "Dictionary.hh"

#include "unistd.h"
#include <sw/redis++/redis++.h>


namespace nycpp
{

    class MyTask{
    public:
    
        MyTask(const string& msg, const TcpConnectionPtr& connPtr,
        WebPageSearcher& webPageSearcher, KeyRecommander& recommander, 
        sw::redis::Redis& redis)
            :_msg(msg), 
             _connPtr(connPtr),
             _webPageSearcher(webPageSearcher),
             _recommander(recommander),
             _redis(redis)
        {  }
    
        void process();
    private:
        string _msg;
        TcpConnectionPtr _connPtr;
        WebPageSearcher& _webPageSearcher;
        KeyRecommander& _recommander;
        sw::redis::Redis& _redis;
    
    
    };
    
}