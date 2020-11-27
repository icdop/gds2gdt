/*
 *  Extract GDS to GDX (Binary format)
 *  --------------------------------
 *  Author: Albert Li
 *  Date  : 2017/09/01
 *
 */
#include "gds2gdx.h"

static const char* BANNER="\
#############################################################\n\
##               Layout GDS Extraction V2000               ##\n\
##                 Version: %-10s                     ##\n\
#############################################################\n\
";
static const char* HELP_DOC="\
Usage: %s [options] <gdsfile:cell> ...\n\
	-logfile	<log file name>\n\
	-destination	<output destination directory>\n\
	-extension      <output file extension\n\
	-outputfile    <output gds file name>\n\
\n\
	-linewidth      <width>\n\
	-structure	<structure>,<structure>,...\n\
	-element	<elm name>,<elm name>,...\n\
	-layer		<layer id>,<layer id>,...\n\
\n\
	+verbose	display detail message\n\
	+hierarchy	include hierarchical structure\n\
	+stdout		write GDS to standard output\n\
	+stdin		read GDS from standard input\n\
\n";


int main(argc,argv)
int argc;
char *argv[];
{
  int  i,j;
  char *opt,*ptr,*arg;
  char *cell,*file;
  char logfile[MAX_FILENAME_LEN]=SYSLOG;
  char gdsin[MAX_FILENAME_LEN]="";
  char gdxout[MAX_FILENAME_LEN]="";
  char destdir[MAX_FILENAME_LEN]="";
  char destination[MAX_FILENAME_LEN]="";
  char extension[MAX_FILENAME_LEN]="gdx";
  char struct_name[MAX_FILENAME_LEN]="";
  
  static char *filelist    = NULL;
  static char *structure   = NULL;
  static char *element_set = NULL;
  static char *layer_set   = NULL;
  
  BOOL flag_search_all    = TRUE;
  BOOL flag_stdout   = FALSE;
  BOOL flag_stdin    = FALSE;
  
  if (argc<2) { 
     printf(BANNER,VERSION);
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
  io_printf(BANNER,PKGVERN);
  io_printf("## DATE      : %s\n",get_time());
  *destination=0;
  copy_str(&filelist,NULL);
  copy_str(&element_set,NULL);
  copy_str(&layer_set,NULL);
  i=1;
  io_printf("## COMMAND   : %s\n",argv[0]);
  while(i<argc) {
     opt=argv[i++];
     if (*opt=='+') {
       io_printf("##               %s\n",opt);
       opt++;
       if (strcasecmp(opt,"no_header")==0) {
          flag_disp_header = FALSE;
       } else if (strcasecmp(opt,"hierarchy")==0) {
          flag_disp_hier  = TRUE;
       } else if (strcasecmp(opt,"stdout")==0) {
          flag_stdout  = TRUE;
       } else if (strcasecmp(opt,"stdin")==0) {
          flag_stdin  = TRUE;
       } else if (strcasecmp(opt,"line")==0) {
          flag_disp_line = TRUE;
       } else if (strcasecmp(opt,"length")==0) {
          flag_disp_length = TRUE;
       } else if (strcasecmp(opt,"verbose")==0) {
          flag_msg_verbose = TRUE;
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
       } else if (strncasecmp(opt,"width",strlen(opt))==0) {
          flag_disp_line=FALSE;
          sscanf(arg,"%d",&max_line_width);
       } else if (strncasecmp(opt,"structure",strlen(opt))==0) {
          flag_search_all = FALSE;
          string_enqueue_sep(structure,arg,",");
       } else if (strncasecmp(opt,"element",strlen(opt))==0) {
          string_enqueue_sep(element_set,arg,",");
       } else if (strncasecmp(opt,"layer",strlen(opt))==0) {
          string_enqueue_sep(layer_set,arg,",");
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
  assign_gds_struct_filter(structure);
  assign_gds_element_filter(element_set);
  assign_gds_layer_filter(layer_set);
  if (io_error_count()==0) {
    string_for_all_sep(filelist,opt,","){
      file=strtok(opt,":");
      cell=strtok(NULL,":");
      strcpy(gdsin,file);
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
      if (strcmp(gdsin,gdxout)==0) {
         io_error("Input GDS file '%s' conflict with output file '%s'.\n",gdsin, gdxout);
      } else {
         translate_gds_to_gdx(gdsin,(flag_stdout)?0:gdxout);
      }
    } string_for_all_end;
    if (flag_stdin)  translate_gds_to_gdx(0,0);
    io_printf("##========================================\n");
  }
  io_summary();
  close_syslog_file();
  return 0;
}


