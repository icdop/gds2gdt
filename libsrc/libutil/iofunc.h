#if !defined(__libutil_IOFUNC_H)
#define __libutil_IOFUNC_H

#include <stdio.h>
#include <stdarg.h>
#include <string.h>

extern int set_syslog_file();
extern int open_syslog_file();
extern int close_syslog_file();
extern int set_sysout_fp(FILE *);
extern int io_log( const char* format, ...);
extern int io_printf( const char* format, ...);
extern int io_error( const char* format, ...);
extern int io_warning( const char* format, ...);
extern int io_summary();
extern int io_error_count();
extern int io_warning_count();
extern int io_print_cpu_stats();
#endif
