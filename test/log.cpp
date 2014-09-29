/**
 * 记录日志
 *Author:
 *   Joskin@yeah.net
 *Copyright:
 *   Copyright(C) 2011.02
 **/
#include <string.h>
#include <fcntl.h>
#include <cstdio>
#include <unistd.h>
#include "log.h"

char  APP_DIR[MAX_PATH] = {0};
short LOG_LEVEL = DBG;          //日志等级(默认:DEBUG)
bool  LOG_OTHER = true;         //记录文件信息(默认: 记录源文件信息)

pthread_mutex_t tm_lock;
char m_proc_name[256] = {0};
int  m_chk_count = 0;

static void init_log() __attribute__ ((constructor));
static void free_log() __attribute__ ((destructor));

static void get_srcinfo(const char *f_name, char *info)
{
    int len = strlen(f_name);
    for (int i=len-1; i>=0; i--)
    {
        if (f_name[i] == '\\' || f_name[i] == '/' || f_name[i] == ':')
        {
            memcpy(info, &f_name[i+1], len-i-1);
            break;
        }
        if (i == 0)
        {
            strcpy(info, f_name);
        }
    }
}

static void get_localtime(struct tm *local, int *ms, bool *CheckLogSize)
{
    pthread_mutex_lock(&tm_lock);
#ifdef _WIN32_
    struct _timeb tb;
	_ftime(&tb);
#else
    struct timeb tb;
	ftime(&tb);
#endif
    localtime_r(&tb.time, local);
    if (64 == ++m_chk_count)
    {
        m_chk_count = 0;
        *CheckLogSize = true;
    }
    *ms = (int)tb.millitm;
    pthread_mutex_unlock(&tm_lock);
}
//----------------------------------------------------------------------------

void init_log()
{
#ifdef _WIN32_
    GetModuleFileName(NULL, APP_DIR, sizeof(APP_DIR));
#else
    readlink("/proc/self/exe", APP_DIR, sizeof(APP_DIR));
#endif

    get_srcinfo(APP_DIR, m_proc_name);

    for (int i = strlen(APP_DIR); i > 0; i--)
    {
        if (APP_DIR[i] == 0x22 || APP_DIR[i] == 0x20)
        {
            APP_DIR[i] = 0;
            continue;
        }
        if (APP_DIR[i] == '\\' || APP_DIR[i] == '/')
        {
            APP_DIR[i] = 0;
            break;
        }
    }
    if (APP_DIR[0] == 0x22)
    {
        memcpy(APP_DIR, &APP_DIR[1], strlen(APP_DIR) - 1);
        APP_DIR[strlen(APP_DIR) - 1] = 0;
    }

#if defined(_LOGSCR) || defined(_LOGFILE)
    pthread_mutex_init(&tm_lock, NULL);

#ifdef _LOGFILE
    char dir_bak[MAX_PATH];
    strcpy(dir_bak, APP_DIR);
    strcat(dir_bak, "/logs/");
    if (0 != access(dir_bak, F_OK))
    {
#ifdef _WIN32_
        mkdir(dir_bak);
#else
        mkdir(dir_bak, 0755);
#endif
    }
#endif  //_WIN32_
#endif  //_LOGSCR || _LOGFILE
}

void free_log()
{
#if defined(_LOGSCR) || defined(_LOGFILE)
    pthread_mutex_destroy(&tm_lock);
#endif
}
//----------------------------------------------------------------------------

// 格式化日志
void _trace_log(const short level, const char* f_name, const char* f_func, const int f_line, const char* buf)
{
#if !defined(_LOGSCR) && !defined(_LOGFILE)
    return ;
#endif

    bool bCheckLogSize = false; //检测文件大小
	struct tm local;            //本地时间
	int ms;                     //毫秒
    get_localtime(&local, &ms, &bCheckLogSize);

    //日志时间
    char log_dt[256] = {0}, ch_dt[64] = {0}, f_info[256] = {0};
	strftime(ch_dt, sizeof(ch_dt), "%m/%d %H:%M:%S", &local);
    //当前调用本函数的线程ID
    int tid = gettid();

    if (LOG_OTHER)      //记录源代码信息?
    {
        //格式化函数调用信息
        get_srcinfo(f_name, f_info);

        switch (level)
        {
        case ERR:
            sprintf(log_dt, "[%s.%03u][%s][ERR][%04d][%s:%s:%d]", ch_dt, ms, m_proc_name, tid, f_info, f_func, f_line);
            break;
        case DBG:
            sprintf(log_dt, "[%s.%03u][%s][DBG][%04d][%s:%s:%d]", ch_dt, ms, m_proc_name, tid, f_info, f_func, f_line);
            break;
        default:
            sprintf(log_dt, "[%s.%03u][%s][SYS][%04d][%s:%s:%d]", ch_dt, ms, m_proc_name, tid, f_info, f_func, f_line);
            break;
        }
    }
    else
    {
        switch (level)
        {
        case ERR:
            sprintf(log_dt, "[%s.%03u][ERR][%04d]", ch_dt, ms, tid);
            break;
        case DBG:
            sprintf(log_dt, "[%s.%03u][DBG][%04d]", ch_dt, ms, tid);
            break;
        default:
            sprintf(log_dt, "[%s.%03u][SYS][%04d]", ch_dt, ms, tid);
            break;
        }
    }

    //写文件日志
#ifdef _LOGFILE
    char log_file[MAX_PATH];
    strcpy(log_file, APP_DIR);
    strcat(log_file, "/test-log.txt");


    if (level <= LOG_LEVEL)
    {
        FILE *log_dev = fopen(log_file, "a+");
        if (NULL == log_dev) return ;

        fprintf(log_dev, "%s %s\n", log_dt, buf);

        if (bCheckLogSize)
        {
            char dir_bak[MAX_PATH];
            struct stat statbuf;
            fstat(fileno(log_dev), &statbuf);
            fclose(log_dev);

            //日志改名备份
            if (statbuf.st_size > MAX_LOGSIZE)
            {
                char tmp[32];
                strftime(tmp, sizeof(tmp), "%Y%m/", &local);

                strcpy(dir_bak, APP_DIR);
                strcat(dir_bak, "/logs/");
                strcat(dir_bak, tmp);
                if (0 != access(dir_bak, F_OK))
#ifdef _WIN32
                    mkdir(dir_bak);
#else
                    mkdir(dir_bak, 0755);
#endif
                strftime(tmp, sizeof(tmp), "%m%d-%H%M%S.log", &local);
                strcat(dir_bak, tmp);

                rename(log_file, dir_bak);
            }
        }
        else
        {
            fclose(log_dev);
        }

    }
#endif  //_LOGFILE

    //屏幕显示
#ifdef _LOGSCR
    fprintf(stderr, "%s %s\n", log_dt, buf);
    fflush(stderr);
#endif  //_LOGSCR
}

//格式打印内存数据
static void fmt_mem(FILE *dev, const unsigned char *mem, const int len)
{
    int tid = gettid();
    bool bCheckLogSize = false; //检测文件大小
	struct tm local;            //本地时间
	int ms;                     //毫秒
    get_localtime(&local, &ms, &bCheckLogSize);
    char log_dt[256] = {0}, ch_dt[64] = {0};
    strftime(ch_dt, sizeof(ch_dt), "%m/%d %H:%M:%S", &local);

    sprintf(log_dt, "[%s.%03u][%s][DBG][%04d]", ch_dt, ms, m_proc_name, tid);
    fprintf(dev, log_dt);

    if (NULL == mem || len <= 0)
    {
        fprintf(dev, " (null)");
        return ;
    }

    fprintf(dev, "[%5d] ", 0);
    for (int i=0; i<len; i++)
    {
        fprintf(dev, "%02X ", mem[i]);
        if ((i + 1) % 20 == 0)
        {
            if (i + 1 == len) break;
            fprintf(dev, "\n");
            fprintf(dev, log_dt);
            fprintf(dev, "[%5d] ", i+1);
        }
    }
    fprintf(dev, "\n");
}

// 写内存日志
void _trace_mem(const short level, const unsigned char *mem, const int len)
{
#if !defined(_LOGSCR) && !defined(_LOGFILE)
    return ;
#endif

#ifdef _LOGFILE
    if (DBG == level)
    {
        char log_file[MAX_PATH];
        strcpy(log_file, APP_DIR);
        strcat(log_file, "/test-log.txt");

        if (DBG == LOG_LEVEL)
        {
            FILE *log_dev = fopen(log_file, "a+");
            if (NULL != log_dev)
            {
                fmt_mem(log_dev, mem, len);
                fclose(log_dev);
            }
        }
    }
#endif

#ifdef _LOGSCR
    fmt_mem(stderr, mem, len);
    fflush(stderr);
#endif
}
