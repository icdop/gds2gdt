#include "header.h"


static char out_buffer[MAX_OUTBUF_SIZE];

void util_print_cpu_stats(fp)
FILE *fp;
{
    struct rusage rusage;
    struct rlimit rlp;
    int text, data, vm_limit, vm_soft_limit;
    double user, system, scale;
    char hostname[MAX_FILENAME_LEN];
//    caddr_t sbrk();
    int end, etext, edata;
    int vm_text, vm_init_data, vm_uninit_data, vm_sbrk_data;
 
    /* Get the hostname */
    gethostname(hostname, MAX_FILENAME_LEN);
    hostname[MAX_FILENAME_LEN-1] = '\0';               /* just in case */
 
    /* Get the virtual memory sizes */
    vm_text = ((long) (&etext)) / 1024.0 + 0.5;
    vm_init_data = ((long) (&edata) - (long) (&etext)) / 1024.0 + 0.5;
    vm_uninit_data = ((long) (&end) - (long) (&edata)) / 1024.0 + 0.5;
    vm_sbrk_data = ((long) sbrk(0) - (long) (&end)) / 1024.0 + 0.5;
 
    /* Get virtual memory limits */
    (void) getrlimit(RLIMIT_DATA, &rlp);
    vm_limit = rlp.rlim_max / 1024.0 + 0.5;
    vm_soft_limit = rlp.rlim_cur / 1024.0 + 0.5;
 
    /* Get usage stats */
    (void) getrusage(RUSAGE_SELF, &rusage);
    user = rusage.ru_utime.tv_sec + rusage.ru_utime.tv_usec/1.0e6;
    system = rusage.ru_stime.tv_sec + rusage.ru_stime.tv_usec/1.0e6;
    scale = (user + system)*100.0;
    if (scale == 0.0) scale = 0.001;
 
    (void) fprintf(fp, "Runtime Statistics\n");
    (void) fprintf(fp, "------------------\n");
    (void) fprintf(fp, "Machine name: %s\n", hostname);
    (void) fprintf(fp, "User time   %6.1f seconds\n", user);
    (void) fprintf(fp, "System time %6.1f seconds\n\n", system);
 
    text = rusage.ru_ixrss / scale + 0.5;
    data = (rusage.ru_idrss + rusage.ru_isrss) / scale + 0.5;
    (void) fprintf(fp, "Average resident text size       = %5dK\n", text);
    (void) fprintf(fp, "Average resident data+stack size = %5dK\n", data);
    (void) fprintf(fp, "Maximum resident size            = %5ldK\n\n",
        rusage.ru_maxrss/2);
    (void) fprintf(fp, "Virtual text size                = %5dK\n",
        vm_text);
    (void) fprintf(fp, "Virtual data size                = %5dK\n",
        vm_init_data + vm_uninit_data + vm_sbrk_data);
    (void) fprintf(fp, "    data size initialized        = %5dK\n",
        vm_init_data);
    (void) fprintf(fp, "    data size uninitialized      = %5dK\n",
        vm_uninit_data);
    (void) fprintf(fp, "    data size sbrk               = %5dK\n",
        vm_sbrk_data);
    (void) fprintf(fp, "Virtual memory limit             = %5dK (%dK)\n\n",
        vm_soft_limit, vm_limit);
 
    (void) fprintf(fp, "Major page faults = %ld\n", rusage.ru_majflt);
    (void) fprintf(fp, "Minor page faults = %ld\n", rusage.ru_minflt);
    (void) fprintf(fp, "Swaps = %ld\n", rusage.ru_nswap);
    (void) fprintf(fp, "Input blocks = %ld\n", rusage.ru_inblock);
    (void) fprintf(fp, "Output blocks = %ld\n", rusage.ru_oublock);
    (void) fprintf(fp, "Context switch (voluntary) = %ld\n", rusage.ru_nvcsw);
    (void) fprintf(fp, "Context switch (involuntary) = %ld\n", rusage.ru_nivcsw);
}


char *system_status()
{
//    caddr_t sbrk();
    struct rusage rusage;
    double user,system;
    int Shour,Sminute,Ssecond;
    int hour,minute,second;
    (void) getrusage(RUSAGE_SELF, &rusage);
    system = rusage.ru_stime.tv_sec + rusage.ru_stime.tv_usec/1.0e6;
    Shour   = system / 3600.0;
    Sminute = system / 60.0 - Shour * 60.0;
    Ssecond = system - Shour * 3600.0 - Sminute * 60.0;
    user = rusage.ru_utime.tv_sec + rusage.ru_utime.tv_usec/1.0e6;
    hour   = user / 3600.0;
    minute = user / 60.0 - hour * 60.0;
    second = user - hour * 3600.0 - minute * 60.0;
    sprintf(out_buffer,"cpu: %6.2f sec, user: %02d:%02d:%02d, mem: %5.1fM",
      system,hour,minute,second,(double)(long)sbrk(0)/(1024.0*1024.0));
    if (strlen(out_buffer)) {
      fprintf(stderr,"ERROR: output buffer overflow!");
    }
    return out_buffer;
}

/*
 *   util_cpu_time -- return a long which represents the elapsed processor
 *   time in milliseconds since some constant reference
 */
static long curr_time;

long util_cpu_time()
{
    struct rusage rusage;
    (void) getrusage(RUSAGE_SELF, &rusage);
    curr_time = (long) rusage.ru_utime.tv_sec*1000 + rusage.ru_utime.tv_usec/1000;
    return (curr_time);
}

/*  get_time                                               */
/*  Creates banner string with date and time for display.  */
char datetime_str[128]="";
char *get_time()
{
  char *dtstr;
  int i;

  struct timeval tp;
  struct timezone tzp;

  gettimeofday(&tp, &tzp);
  dtstr = ctime((time_t*)&tp);

  /* month day */
  for (i = 0; i <= 5; ++i)
    datetime_str[i] = dtstr[i+4];
  if (datetime_str[4] == '0')
    datetime_str[4] = ' ';
  datetime_str[6] = ',';
  datetime_str[7] = ' ';
  /* year */
  for(i = 8; i <= 11; ++i)
    datetime_str[i] = dtstr[i+12];
  datetime_str[12] = ' ';
  datetime_str[13] = ' ';
  for (i = 14; i <= 21; ++i)
    datetime_str[i] = dtstr[i-3];
  datetime_str[22] = '\0';
  return datetime_str;
}


long elapse_time()
{
  long  last_time=curr_time;
  return (util_cpu_time()-last_time);
}

static char username_str[128]="";
char *get_user()
{
  char *name;
  username_str[0]=0;
  name=(char*)getenv("USER");
  if (name) {
     strncat(username_str,name,45);
  }
  return username_str;
}

