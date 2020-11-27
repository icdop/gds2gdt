#if !defined(__libutil_STATUS_H)
#define __libutil_STATUS_H

extern void util_print_cpu_stats();
extern long until_cpu_time();
extern long elapse_time();
extern char *get_time();
extern char datetime_str[];
extern char *system_status();

#endif
