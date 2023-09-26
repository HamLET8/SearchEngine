#include "log4class.hh"

Mylogger* Mylogger::instance = Mylogger::getInstance();


Mylogger::Mylogger()
    :_conf("%d: %p %c %x: %m%n"), _root(log4cpp::Category::getRoot())
{
    log4cpp::PatternLayout* ppl = new log4cpp::PatternLayout();
    ppl->setConversionPattern(_conf);

    log4cpp::RollingFileAppender* rfa = new log4cpp::RollingFileAppender("RollingFileAppender", "log/logfile.log", 1023*1024, 10);
    rfa->setLayout(ppl);

    _root.setPriority(log4cpp::Priority::DEBUG);
    _root.addAppender(rfa);
}


Mylogger::~Mylogger(){
    delete instance;
    log4cpp::Category::shutdown();
}


Mylogger* Mylogger::getInstance(){
        if(instance == nullptr){
            instance =  new Mylogger();
        }
        return instance; 
}

