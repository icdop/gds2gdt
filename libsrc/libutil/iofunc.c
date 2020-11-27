#include "header.h"
#include "status.h"
#include "iofunc.h"


static char sysout_buf[MAX_OUTBUF_SIZE] ="";
static FILE *sysout_fp   = NULL;
static FILE *syslog_fp   = NULL;
static char syslog_fname[MAX_FILENAME_LEN] = "system.log";
static int total_error   = 0;
static int total_warning = 0;
static int error_cnt     = 0;
static int warning_cnt   = 0;


#define MAX_SYSLOG_QUEUE  20
static int syslog_cnt=0;
static FILE *syslog_queue_fp[MAX_SYSLOG_QUEUE];
static char syslog_queue_fname[MAX_SYSLOG_QUEUE][MAX_FILENAME_LEN];
static int syslog_error[MAX_SYSLOG_QUEUE];
static int syslog_warning[MAX_SYSLOG_QUEUE];

int push_syslog() 
{
  strcpy(syslog_queue_fname[syslog_cnt],syslog_fname);
  syslog_queue_fp[syslog_cnt]=syslog_fp;
  syslog_warning[syslog_cnt]=total_warning;
  syslog_error[syslog_cnt]=total_error;
  return syslog_cnt++;
}

int pop_syslog() {
  if (syslog_cnt>0) syslog_cnt--;
  strcpy(syslog_fname,syslog_queue_fname[syslog_cnt]);
  syslog_fp=syslog_queue_fp[syslog_cnt];
  total_warning = syslog_warning[syslog_cnt];
  total_error = syslog_error[syslog_cnt];
  warning_cnt = error_cnt = 0;
  return syslog_cnt;
}

int set_sysout_fp(fp)
FILE *fp;
{
  if (fp!=NULL) {
    sysout_fp=fp;
    return 1;
  }
  return 0;
}

int set_syslog_file(fname)
char *fname;
{   
    strncpy(syslog_fname,fname,MAX_FILENAME_LEN-1);
    syslog_fname[MAX_FILENAME_LEN-1]='\0';
    return 1;
}

int close_syslog_file()
{
  if ((syslog_fp!=NULL)&& (syslog_fp!=stdout)&& (syslog_fp!=stderr)) 
     fclose(syslog_fp);
  
  pop_syslog();
  
  return (total_error!=0);
}

int open_syslog_file()
{
  push_syslog();

  syslog_fp=fopen(syslog_fname,"w");
  sysout_fp=stdout;
  total_error = total_warning =0;
  error_cnt = warning_cnt = 0;
  
  if (syslog_fp==NULL){
     syslog_fp=stdout;
     io_error("Can not open system log file \"%s\".\n",syslog_fname);
     return 0;
  }
  return 1;
}

int io_log( const char* format, ...)
{
  va_list args;
  char    *fmt;

  va_start(args,format);
  if ((syslog_fp!=NULL)&& (syslog_fp!=stdout)) {
     vfprintf(syslog_fp,format,args);
     fflush(syslog_fp);
  }
  va_end(args);
  return 1;
}

int io_printf( const char* format, ...)
{
  va_list args;
  
  va_start(args,format);
  vsprintf(sysout_buf,format,args);
  va_end(args);
  if (sysout_fp!=NULL) {
     fprintf(sysout_fp,"%s",sysout_buf);
  }
  if ((syslog_fp!=NULL)&& (syslog_fp!=sysout_fp)) {
     fprintf(syslog_fp,"%s",sysout_buf);
     fflush(syslog_fp);
  }
  return 1;
}

int io_warning( const char* format, ...)
{
  va_list args;

  warning_cnt++;
  va_start(args,format);
  vsprintf(sysout_buf,format,args);
  va_end(args);
  if (sysout_fp!=NULL) {
     fprintf(sysout_fp,"WARNING: %s",sysout_buf);
  }
  if ((syslog_fp!=NULL)&& (syslog_fp!=sysout_fp)) { 
     fprintf(syslog_fp,"WARNING: %s",sysout_buf);
     fflush(syslog_fp);
  }
  va_end(args);
  return 1;
}

int io_error( const char* format, ...)
{
  va_list args;
  
  error_cnt++;
  va_start(args,format);
  vsprintf(sysout_buf,format,args);
  va_end(args);
  fprintf(stderr,"ERROR  : %s",sysout_buf);
  if ((syslog_fp!=NULL)&& (syslog_fp!=sysout_fp)) {
     fprintf(syslog_fp,"ERROR  : %s",sysout_buf);
     fflush(syslog_fp);
  }
  va_end(args);
  return 1;
}


int io_summary()
{
  total_error   += error_cnt;
  total_warning += warning_cnt;
  io_printf("\n");
  io_printf("*****************************************************************************\n");
  io_printf("**  FOUND %4d ERROR and %4d WARNING MESSAGE FROM LAST COUNT              **\n", error_cnt,warning_cnt);
  io_printf("*****************************************************************************\n");
  io_printf("**  TOTAL %4d ERROR and %4d WARNING MESSAGE FROM PROGRAM START           **\n", total_error,total_warning);
  io_printf("*****************************************************************************\n");
  io_printf("\n");
  error_cnt = warning_cnt = 0;
  return 1;
}


int io_error_count()
{
  return total_error;
}

int io_warning_count()
{
  return total_warning;
}

int io_print_cpu_stats()
{
  util_print_cpu_stats(sysout_fp);
  if ((syslog_fp!=NULL)&&(syslog_fp!=sysout_fp))
    util_print_cpu_stats(syslog_fp);
  return 1;
}
