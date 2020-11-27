/*
 *  Convert GDT (Text format) to GDS
 *  --------------------------------
 *  Author: Albert Li
 *  Date  : 2017/09/01
 *
 */
#include "gdt2gdx.h"

static const char* BANNER="\
#############################################################\n\
##               Layout GDS Writer (GDT2GDX) %-10s    ##\n\
##                 GDSUTIL Version: %-10s             ##\n\
#############################################################\n\
";
static char* HELP_DOC="\
Usage: %s [options] <gdtfile:cell> ...\n\
	-logfile	<log file name>\n\
	-destination    <output destination directory>\n\
	-extension      <output file extension\n\
	-outputfile     <output gdx file name>\n\
\n\
	+verbose        display detail message\n\
	+stdout	        write GDS to standard output\n\
	+stdin          read GDT from standard input\n\
\n";


int main(argc,argv)
int argc;
char *argv[];
{
  int  i,j;
  char *opt,*ptr,*arg;
  char *cell,*file;
  char logfile[MAX_FILENAME_LEN]=SYSLOG;
  char gdtin[MAX_FILENAME_LEN]="";
  char gdxout[MAX_FILENAME_LEN]="";
  char destdir[MAX_FILENAME_LEN]="";
  char destination[MAX_FILENAME_LEN]="";
  char extension[MAX_FILENAME_LEN]="gdx";
  char struct_name[MAX_FILENAME_LEN]="";
  
  char *filelist=NULL;
  
  BOOL flag_stdout = FALSE;
  BOOL flag_stdin  = FALSE;

  if (argc<2) { 
     printf(BANNER,VERSION,PKGVERN);
     printf(HELP_DOC,PROGRAM);
     return -1;
  }
  i=1;
  while(i<argc) {
     opt=argv[i++];
     if (strncasecmp(opt,"-logfile",strlen(opt))==0) {
       if (((arg=argv[i++])==NULL)||(i>argc)) {
          io_error("Too few argument for '%s'\n",opt);
          arg="";
       } else {
          strcpy(logfile,arg);
       }
     }
  }
  set_syslog_file(logfile);
  set_sysout_fp(stdout);
  open_syslog_file();
  io_printf(BANNER,VERSION,PKGVERN);
  io_printf("## DATE      : %s\n",get_time());
  *destination=0;
  copy_str(&filelist,NULL);
  i=1;
  io_printf("## COMMAND   : %s\n",argv[0]);
  while(i<argc) {
     opt=argv[i++];
     if (*opt=='+') {
       io_printf("##               %s\n",opt);
       opt++;
       if (strcasecmp(opt,"verbose")==0) {
          flag_msg_verbose = TRUE;
       } else if (strcasecmp(opt,"stdout")==0) {
          flag_stdout = TRUE;
       } else if (strcasecmp(opt,"stdin")==0) {
          flag_stdin = TRUE;
       } else {
          io_warning("Unknown options '+%s'\n",opt);
       }
     } else if (*opt=='-') {
       if (((arg=argv[i++])==NULL)||(i>argc)) {
          io_error("Too few argument for '%s'\n",opt);
          arg="";
       }
       io_printf("##               %s %s\n",opt,arg);
       opt++;
       if (strncasecmp(opt,"logfile",strlen(opt))==0) {
          strcpy(logfile,arg);
       } else if (strncasecmp(opt,"destination",strlen(opt))==0) {
          strcpy(destination,arg);
       } else if (strncasecmp(opt,"extension",strlen(opt))==0) {
          strcpy(extension,arg);
       } else if (strncasecmp(opt,"outputfile",strlen(opt))==0) {
          strcpy(gdxout,arg);
       } else {
          io_error("Unknown options '-%s'\n",opt);
       }
     } else {
       string_enqueue_sep(filelist,opt,",");
     }
  }
  if (*destination) {
     io_printf("## OUTPUT    : %s\n",destination);
     if (fclose(fopen(destination,"r"))!=0 && mkdir(destination,0755)!=0) {
        io_error("Can not create output directory '%s'.\n",destination);
        return -1;
     }
  }
  if (io_error_count()==0) {
    string_for_all_sep(filelist,opt,","){
      file=strtok(opt,":");
      cell=strtok(NULL,":");
      strcpy(gdtin,file);
      while(ptr=strchr(file,'/')) file=ptr+1;
      if (*destination=='\0') {
         if (file==opt) {
            strcpy(destdir,"");
         } else {
            *(file-1)=0;
            strcpy(destdir,opt);
         }
      } else {
         strcpy(destdir,destination);
      }
      if (cell) {
         strcpy(struct_name,cell);
      } else {
         strcpy(struct_name,file);
         strtok(struct_name,".");
      }
      if (*gdxout==0) {
         if (*destdir) {
            sprintf(gdxout,"%s/%s.%s",destdir,struct_name,extension);
         } else {
            sprintf(gdxout,"%s.%s",struct_name,extension);
         }
      }
      translate_gdt_to_gdx(gdtin,flag_stdout?0:gdxout);
    } string_for_all_end;
    if (flag_stdin) translate_gdt_to_gdx(0,flag_stdout?0:(*gdxout)?gdxout:"gdt2gdx.gdx");
    io_printf("##========================================\n");
  }
  io_summary();
  close_syslog_file();
  return 0;
}


