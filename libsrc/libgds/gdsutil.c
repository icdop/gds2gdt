/*
 * $Id $
 *
 *
 */
#include "libutil.h"  
#include "gdsdata.h"
#include "gdsutil.h"
#include "gdsfilter.h"

int  max_line_width      = 0;
int  gds_record_num      = 0;
int  gdt_line_num        = 0;

BOOL flag_msg_verbose    = FALSE;
BOOL flag_disp_line      = FALSE;
BOOL flag_disp_header	 = TRUE;
BOOL flag_disp_length	 = FALSE;
BOOL flag_disp_hier	 = FALSE;

static unsigned char gdsinpbuf[MAX_GDSBUF_SIZE] = "";
static unsigned char gdxoutbuf[MAX_GDSBUF_SIZE] = "";

int check_gds_header(gdsfp)
FILE *gdsfp;
{
  unsigned char header[GDSHEADERLENGTH];
  fseek(gdsfp,-fread(header,1,GDSHEADERLENGTH,gdsfp),1);
  return  ((GDSRECID(header)==GDS_HEADER) && (GDSRECTYPE(header)==GDS_I2));
}

int read_gds_record(gdsfp,gdsbuf,bufsize)
FILE *gdsfp;
unsigned char *gdsbuf;
int  bufsize;
{
    unsigned int len;
    if (fread(gdsbuf,1,GDSHEADERLENGTH,gdsfp)==GDSHEADERLENGTH) {
       gds_record_num++;	     
       len = GDSRECLEN(gdsbuf);
       if (len==0) {
           return 0;
       } else if (len<GDSHEADERLENGTH) {
           io_error("[#%04d] Record length(%d) is not correct around <%ld>\n",gds_record_num,len,ftell(gdsfp)); 
       } else if (len>bufsize) { 
           io_error("[#%04d] Record length(%d) is too large(>%d) around <%ld>\n",gds_record_num,len,bufsize,ftell(gdsfp)); 
       } else if (fread(gdsbuf+GDSHEADERLENGTH,1,len-GDSHEADERLENGTH,gdsfp)!=len-GDSHEADERLENGTH) {
           io_error("[#%04d] Data length(%d) is not correct around <%ld>\n",gds_record_num,len,ftell(gdsfp));
       } else {
           return len;
       }
    } else if (!feof(gdsfp)) {
       io_error("[#%04d] Data header is not correct around <%ld>\n",gds_record_num,ftell(gdsfp));
    }
    return -1;
}


long proc_gds_to_gdt(len,gdsbuf,gdtbufptr)
long len;
unsigned char *gdsbuf;
char **gdtbufptr;
{
   unsigned int  i,j;
   unsigned int  size;
   unsigned int  gdsid;
   unsigned int  gdstype;
   long buflen;
   GDS_DEF gdsdef;

   gdsid   = GDSRECID(gdsbuf);
   gdstype = GDSRECTYPE(gdsbuf);
   gdsdef  = gds_def_array + gdsid;
   
   copy_str(gdtbufptr,gdsdef->name);
   size=(gdsdef->size)?abs(gdsdef->size):gds_size[gdstype];
   if (size>0) {
      for(j=0, i=GDSHEADERLENGTH; i<len; j++,i+=size) {
        if (size>len-i) size=len-i;
        append_str(gdtbufptr,decode_gds_data(gdstype,gdsbuf+i,size));
      }
      if (gdsdef->num<0) {
        if (j>abs(gdsdef->num))
           io_error("[#%04d] Data number(%d) of '%s' exceeds limitation(%d)!\n", gds_record_num,
             len/size,gdsdef->name,gdsdef->num);
      } else {
        if (j!=gdsdef->num)
           io_error("[#%04d] Data number(%d) of '%s' does not match(%d)!\n", gds_record_num,
             len/size,gdsdef->name,gdsdef->num);
      }
   }
   append_str(gdtbufptr,"\n");
   buflen = strlen(*gdtbufptr);
   return buflen;
}



long proc_gdt_to_gdx(len,inpbuf,outbuf)
long len;
char *inpbuf;
char *outbuf;
{
  GDS_DEF gdsdef;
  char *ptr,*token;
  char *keyword;
  long buflen;
  
  buflen=0;
  outbuf[0]=outbuf[1]=0;
  for(ptr=inpbuf;isspace(*ptr);ptr++);
  keyword=strtok(ptr," \t\n");
  while(keyword && (*keyword=='[')) {
	  keyword=strtok(NULL," \t\n");
  }
  if (keyword==NULL) return 0;
  for(gdsdef=gds_def_array; *gdsdef->name; gdsdef++) {
     if (strcasecmp(keyword,gdsdef->name)==0) {
        buflen=GDSHEADERLENGTH;
        if (gdsdef->size) {
           if (gdsdef->type==GDS_ASCII) {
             while((token=strtok(NULL,"\""))) {
//               io_printf("ASCII:%s\n",token);
               buflen+=encode_gds_data(gdsdef->type,token,outbuf+buflen,gdsdef->size);
               ptr=strtok(NULL,"\"");
             }
           } else {
             while(token=strtok(NULL," \t\n")) {
               buflen+=encode_gds_data(gdsdef->type,token,outbuf+buflen,gdsdef->size);
             }
           }
        }
        outbuf[0]=buflen / 256;
        outbuf[1]=buflen % 256;
        outbuf[2]=gdsdef->id;
        outbuf[3]=gdsdef->type;
        return buflen;
     }
  }
  io_warning("Unknown keyword: %s\n",keyword);
  return buflen;
}

long proc_gds_to_gdx(len,gdsbuf,gdxbuf)
long len;
unsigned char *gdsbuf;
unsigned char *gdxbuf;
{
   static char **gdtbufptr=NULL;
   long gdtbuf_size;
   long gdxbuf_size;
   
   gdtbuf_size=proc_gds_to_gdt_with_filter(len,gdsbuf,gdtbufptr);
   gdxbuf_size=proc_gdt_to_gdx(gdtbuf_size,*gdtbufptr,gdxbuf);
   return gdxbuf_size;
}

long loop_gds_to_gdt(gdsinfp,gdtoutfp,proc_func)
FILE *gdsinfp;
FILE *gdtoutfp;
long (*proc_func)();
{
  long outbuf_size;
  int reclen;
  int i,q;
  char *ptr;
  static char *gdtoutbuf=NULL;

  gds_record_num = 0;
  while((reclen=read_gds_record(gdsinfp,gdsinpbuf,MAX_GDSBUF_SIZE))>0) {
     outbuf_size+=(*proc_func)(reclen,gdsinpbuf,&gdtoutbuf);
     
     switch(GDSRECID(gdsinpbuf)) {
     case GDS_STRNAME:
	if (flag_msg_verbose)
        io_printf("## (%06ld) %s\n",ftell(gdsinfp),gdsinpbuf+GDSHEADERLENGTH);
	else
        fprintf(stderr,"\r%60s\r## (%ld) %s\r","",ftell(gdsinfp),gdsinpbuf+GDSHEADERLENGTH);
        break;
     }
     if (flag_disp_line) fprintf(gdtoutfp,"[#%04d] ",gds_record_num);
     if (flag_disp_length) fprintf(gdtoutfp,"[%03d] ",reclen);

     if ((max_line_width <= 0) || (strlen(gdtoutbuf) < max_line_width)) {
        fprintf(gdtoutfp,"%s",gdtoutbuf);
     } else {
        for(i=q=0,ptr=gdtoutbuf; *ptr ; ptr++,i++) {
          if (*ptr=='\t') *ptr=' ';
          fputc(*ptr,gdtoutfp);
          if ((i>max_line_width) && strchr(" ,;",*ptr)) {
             if (q) { i=0; fprintf(gdtoutfp,"\\\n");} 
             else   { i=8; fprintf(gdtoutfp,"\\\n\t"); }
          } 
          if (*ptr=='\"') q=~q;
        }
        fputc('\n',gdtoutfp);
     }
  }
  return outbuf_size;
}

long loop_gdt_to_gdx(gdtinfp,gdxoutfp,proc_func)
FILE *gdtinfp;
FILE *gdxoutfp;
int (*proc_func)();
{
  char *Buffer;
  long gdx_buflen;
  long gdx_len=0;

  while(!feof(gdtinfp)) {
    Buffer=read_lines(gdtinfp,MATCH_MULTIPLE,"\\");
    gdt_line_num++;
    if (Buffer && (*Buffer!='#') &&(*Buffer!='*')) { 
       gdx_buflen=(*proc_func)(strlen(Buffer),Buffer,gdxoutbuf);
       switch(GDSRECID(gdxoutbuf)) {
       case GDS_STRNAME:
          if (flag_msg_verbose)
          io_printf("## (%06ld) %s\n",ftell(gdtinfp),gdxoutbuf+GDSHEADERLENGTH);
          else
          fprintf(stderr,"\r%60s\r## (%06ld) %s\r","",ftell(gdtinfp),gdxoutbuf+GDSHEADERLENGTH);
          break;
       }
       fwrite(gdxoutbuf,gdx_buflen,1,gdxoutfp);
       gdx_len += gdx_buflen;
    }
  }
  return gdx_len;
}

long loop_gds_to_gdx(gdsinfp,gdxoutfp,proc_func)
FILE *gdsinfp;
FILE *gdxoutfp;
long (*proc_func)();
{
  static char *gdtoutbuf=NULL;
  long gdx_buflen;
  long gdx_len=0;
  int reclen;
  char *ptr;
  

  gds_record_num = 0;
  while((reclen=read_gds_record(gdsinfp,gdsinpbuf,MAX_GDSBUF_SIZE))>0) {
     gdx_buflen=(*proc_func)(reclen,gdsinpbuf,&gdtoutbuf);
     switch(GDSRECID(gdsinpbuf)) {
     case GDS_STRNAME:
	if (flag_msg_verbose)
        io_printf("## (%06ld) %s\n",ftell(gdsinfp),gdxoutbuf+GDSHEADERLENGTH);
	else
        fprintf(stderr,"\r%60s\r## (%06ld) %s\r","",ftell(gdsinfp),gdxoutbuf+GDSHEADERLENGTH);
        break;
     }
     fwrite(gdxoutbuf,gdx_buflen,1,gdxoutfp);
     gdx_len += gdx_buflen;
  }
  return gdx_len;
}
