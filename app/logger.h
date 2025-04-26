#pragma once
#include<bits/stdc++.h>
#include"path.h"

class Logger{
public:
	enum Level{
		LDEBUG=0,
		LINFO,
		LWARN,
		LERROR,
		LFATAL,
		LEVEL_COUNT
	};
	static Logger *getInstance();
	void open(const std::string&filename);
	void close();
	void lprintf(Level level,const std::string&file,int line,const char*format,...);
	template<typename... Args>
	void lOutPut(Level level,const std::string&file,int line,Args ... x);
	Logger(const Logger&)=delete; // 禁止拷贝构造
	Logger& operator=(const Logger&)=delete; // 禁止赋值操作
private:
	Logger();
	~Logger();
private:
	std::string m_filename;
	std::ofstream m_fout;
	static const char *s_level[LEVEL_COUNT];
	static Logger *m_instance;
};
#define logDebug(...) (Logger::getInstance()->lOutPut(Logger::LDEBUG,getFileName().string(),getLine(),##__VA_ARGS__))
#define logInfo(...) (Logger::getInstance()->lOutPut(Logger::LINFO,getFileName().string(),getLine(),##__VA_ARGS__))
#define logWarn(...) (Logger::getInstance()->lOutPut(Logger::LWARN,getFileName().string(),getLine(),##__VA_ARGS__))
#define logError(...) (Logger::getInstance()->lOutPut(Logger::LERROR,getFileName().string(),getLine(),##__VA_ARGS__))
#define logFatal(...) (Logger::getInstance()->lOutPut(Logger::LFATAL,getFileName().string(),getLine(),##__VA_ARGS__))


template<typename... Args>
void Logger::lOutPut(Level level, const std::string&file, int line, Args ... x){
	static time_t tim;
	static struct tm ptm;
	static char timestamp[32];
	tim=std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	#ifdef __WIN32__
	localtime_s(&ptm,&tim);//线程安全
	#else
	localtime_r(&tim,&ptm);//线程安全
	#endif
	
	memset(timestamp,0,sizeof(timestamp));
	strftime(timestamp,sizeof(timestamp),"%Y-%m-%d %H:%M:%S",&ptm);
	m_fout<<'['<<timestamp<<']';
	m_fout<<'['<<s_level[level]<<"]["<<file<<':'<<line<<"] ";
	(m_fout<<...<<x);m_fout<<'\n';
	m_fout.flush();
}