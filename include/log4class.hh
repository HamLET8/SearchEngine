#pragma once
#include <log4cpp/PatternLayout.hh>
#include <log4cpp/RollingFileAppender.hh>
#include <log4cpp/Category.hh>
#include <log4cpp/Priority.hh>
#include <string>
#include <utility>

#define addPrefix(msg) std::string().append(__FILE__).append("\t").append(__func__).append("\t").append(std::to_string(__LINE__)).append("\t").append(msg).c_str()

#define logError(msg, ...) Mylogger::getInstance()->error(addPrefix(msg), ##__VA_ARGS__)	
#define logFatal(msg, ...) Mylogger::getInstance()->fatal(addPrefix(msg), ##__VA_ARGS__)	
#define logInfo(msg, ...) Mylogger::getInstance()->info(addPrefix(msg), ##__VA_ARGS__)	
#define logDebug(msg, ...) Mylogger::getInstance()->debug(addPrefix(msg), ##__VA_ARGS__)	
#define logCrit(msg, ...) Mylogger::getInstance()->crit(addPrefix(msg), ##__VA_ARGS__)	
#define logWarn(msg, ...) Mylogger::getInstance()->warn(addPrefix(msg), ##__VA_ARGS__)	


class Mylogger
{

public:
    static Mylogger * getInstance();
	static void destory();

	template<typename...T>
	void fatal(const char *msg, T... args);

	template<typename...T>
	void crit(const char *msg, T... args);
	
	template<typename...T>
	void warn(const char *msg, T... args);
	
	template<typename...T>
	void error(const char *msg, T... args);
	
	template<typename...T>
	void debug(const char *msg, T... args);
	
	template<typename...T>
	void info(const char *msg, T... args);
	
private:
	
    Mylogger();
	~Mylogger();
    
private:
    static Mylogger* instance;
    log4cpp::Category& _root;
	std::string _conf;
};

template<typename...T>
void Mylogger::fatal(const char* msg, T...args){
    _root.fatal(msg, args...);
}

template<typename...T>
void Mylogger::crit(const char* msg, T...args){
    _root.crit(msg, args...);
}

template<typename...T>
void Mylogger::warn(const char* msg, T...args){
    _root.warn(msg, args...);
}

template<typename...T>
void Mylogger::error(const char* msg, T...args){
    _root.error(msg, args...);
}

template<typename...T>
void Mylogger::debug(const char* msg, T...args){
    _root.debug(msg, args...);
}

template<typename...T>
void Mylogger::info(const char* msg, T...args){
    _root.info(msg, args...);
}
