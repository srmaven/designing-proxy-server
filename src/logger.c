#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include "logger.h"

#define LOG_FILE "logs/proxy.log"

void write_log(const char *level, const char *format, va_list args) {
    FILE *fp = fopen(LOG_FILE, "a");
    if (!fp) return;

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    fprintf(fp, "[%04d-%02d-%02d %02d:%02d:%02d] [%s] ",
        t->tm_year+1900, t->tm_mon+1, t->tm_mday,
        t->tm_hour, t->tm_min, t->tm_sec, level);
    
    vfprintf(fp, format, args);
    fprintf(fp, "\n");
    fclose(fp);
}

void log_request(const char *method, const char *path, const char *host) {
    FILE *fp = fopen(LOG_FILE, "a");
    if (!fp) return;
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    fprintf(fp, "[%04d-%02d-%02d %02d:%02d:%02d] [REQUEST] %s %s Host:%s\n",
        t->tm_year+1900, t->tm_mon+1, t->tm_mday,
        t->tm_hour, t->tm_min, t->tm_sec, method, path, host);
    fclose(fp);
}

void log_error(const char *message) {
    FILE *fp = fopen(LOG_FILE, "a");
    if (!fp) return;
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    fprintf(fp, "[%04d-%02d-%02d %02d:%02d:%02d] [ERROR] %s\n",
        t->tm_year+1900, t->tm_mon+1, t->tm_mday,
        t->tm_hour, t->tm_min, t->tm_sec, message);
    fclose(fp);
}

void log_info(const char *message) {
    FILE *fp = fopen(LOG_FILE, "a");
    if (!fp) return;
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    fprintf(fp, "[%04d-%02d-%02d %02d:%02d:%02d] [INFO] %s\n",
        t->tm_year+1900, t->tm_mon+1, t->tm_mday,
        t->tm_hour, t->tm_min, t->tm_sec, message);
    fclose(fp);
}