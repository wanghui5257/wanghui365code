#ifndef _log_H_
#define _log_H_

//#include "define.h"

#include <stddef.h>
#include <stdarg.h>
#include <sys/timeb.h>
#include <pthread.h>
#include <sys/syscall.h>
#define gettid()    ( syscall(SYS_gettid) )

#define SYS             0           //系统级别
#define ERR             1           //错误级别
#define DBG             2           //调试级别(DEBUG,记录所有日志)

#ifndef MAX_PATH
#define MAX_PATH        1024
#endif
#define MAX_LOGSIZE     50 * 1024 * 1024

void _trace_log(const short level, const char* f_name, const char* f_func, const int f_line, const char* buf);
void _trace_mem(const short level, const unsigned char *mem, const int len);

extern char  APP_DIR[MAX_PATH];
extern short LOG_LEVEL;
extern bool  LOG_OTHER;
//extern pthread_mutex_t tm_lock;
#define _T(s)           (const SAChar *)(s)
//日志重定向
#if defined(_LOGSCR) || defined(_LOGFILE)
#define trace_log(level, fmt,...)   ( {char var[2048] = {0};  \
                                       snprintf(var, sizeof(var), fmt, ##__VA_ARGS__);  \
                                       _trace_log(level, (const char *)__FILE__, (const char *)__FUNCTION__, (const int)__LINE__, (const char *)var);} )
#define trace_mem(mem, len)         (  _trace_mem(LOG_LEVEL, mem, len) )
#else
#define trace_log(level, fmt,...)   ( {} )
#define trace_mem(mem, len)         ( {} )
#endif
#endif
