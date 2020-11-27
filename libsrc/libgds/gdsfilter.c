 /*
 * $Id $
 *
 *
 */
#include "libutil.h"
#include "gdsdata.h"
#include "gdsutil.h"
#include "gdsfilter.h"

#define MAX_STRUCT_LEN  512

HASH_TABLE struct_table   = NULL;
HASH_TABLE element_table  = NULL;
HASH_TABLE layer_table    = NULL;

static int	struct_num       = 0;
static int	element_num      = 0;
static int	ele_aref_num,ele_sref_num,ele_boundary_num,ele_path_num;
static int	ele_text_num,ele_box_num,ele_node_num;


long proc_gds_to_gdt_with_filter(len,gdsbuf,outbufptr)
long len;
unsigned char *gdsbuf;
char **outbufptr;
{
   static char bgnlib[256]="";
   static char bgnstr[256]="";
   static char library_name[GDSNAMELENGTH+1]="";
   static char struct_name[GDSNAMELENGTH+1]="";
   static GDS_DEF element=NULL;
   static long layer=-1;
   static LIST line_list=NULL;
   
   static BOOL match_library_flag=FALSE;
   static BOOL match_struct_flag=FALSE;
   static char **tmpbufptr = NULL;
   
   long   flag;
   char   *ptr;
   char   *buf = NULL;

   unsigned int  i,j;
   unsigned int  size;
   unsigned int  gdsid;
   unsigned int  gdstype;
   GDS_DEF gdsdef;

   gdsid   = GDSRECID(gdsbuf);
   gdstype = GDSRECTYPE(gdsbuf);
   gdsdef  = gds_def_array + gdsid;
   
   copy_str(outbufptr,gdsdef->name);
   size=(gdsdef->size)?abs(gdsdef->size):gds_size[gdstype];
   if (size>0) {
      for(j=0, i=GDSHEADERLENGTH; i<len; j++,i+=size) {
        if (size>len-i) size=len-i;
        append_str(outbufptr,decode_gds_data(gdstype,gdsbuf+i,size));
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
   copy_str (&buf, *outbufptr);
   switch(gdsid) {
     case GDS_LIBNAME:
       if (strtok(buf,"\"") && (ptr=strtok(NULL,"\""))) {
          strcpy(library_name,ptr);
          match_library_flag=TRUE;
       } else {
          match_library_flag=FALSE;
       }
       break;
     case GDS_ENDLIB:
       if (match_struct_flag)  {
          if (flag_msg_verbose) {
             if (struct_num ) 
                io_printf("##   STRUCT   : %4d \n",struct_num);
          }
       } else {
          fprintf(stderr,"%60s\r","");
       }
       strcpy(bgnlib,"");
       match_library_flag=FALSE;
       break;
     case GDS_BGNSTR:
       strcpy(bgnstr,buf+7);
       break;
     case GDS_ENDSTR:
       if (match_struct_flag)  {
          if (flag_msg_verbose) {
             if (ele_sref_num ) 
                io_printf("##   SREF     : %4d \n",ele_sref_num);
             if (ele_aref_num ) 
                io_printf("##   AREF     : %4d\n",ele_aref_num);
             if (ele_text_num ) 
                io_printf("##   TEXT     : %4d\n",ele_text_num);
             if (ele_boundary_num ) 
                io_printf("##   BOUNDARY : %4d\n",ele_boundary_num);
             if (ele_path_num ) 
                io_printf("##   PATH     : %4d\n",ele_path_num);
             if (ele_box_num ) 
                io_printf("##   BOX      : %4d\n",ele_box_num);
             if (ele_node_num ) 
                io_printf("##   NODE     : %4d\n",ele_node_num);
          }
       } else {
          fprintf(stderr,"%60s\r","");
       }
       strcpy(bgnstr,"");
       match_struct_flag=FALSE;
       break;
     case GDS_STRNAME:
       if (strtok(buf,"\"") && (ptr=strtok(NULL,"\""))) {
          strcpy(struct_name,ptr);
          if (hash_entries(struct_table)==0) {
             match_struct_flag=TRUE;
          } else if (hash_get(struct_table,(PTR)struct_name,(PTR*)&flag)) {
             flag++;
             hash_put(struct_table,(PTR)strdup(struct_name),(PTR)flag);
             match_struct_flag=TRUE;
          } else {
             if (flag_msg_verbose) 
             io_printf("## STRUCTURE = %-32s (BYPASSED)\n",struct_name);
             match_struct_flag=FALSE;
          }
          if (match_struct_flag) {
             io_printf("## STRUCTURE = %-32s\n",struct_name);
             struct_num++;
          } else {
             fprintf(stderr,"## STRUCTURE = %-32s\r",struct_name);
          }
       } else {
          match_struct_flag=FALSE;
       }
       element_num = ele_aref_num = ele_sref_num = ele_boundary_num = 
       ele_path_num = ele_text_num = ele_box_num = ele_node_num = 0;
       break;
     case GDS_SREF:
     case GDS_AREF:
       if (match_struct_flag) {
             element=gds_def_array+gdsid;
       }
       line_list=list_create(0);
       break;
     case GDS_BOUNDARY:
     case GDS_PATH:
     case GDS_TEXT:
     case GDS_BOX:
     case GDS_NODE:
       if (match_struct_flag) {
          if (hash_entries(element_table)) {
             if (!hash_get(element_table,(PTR)gds_def_array[gdsid].name,(PTR*)&element)) {
                element = NULL;
             }
          } else {
             element=gds_def_array+gdsid;
          }
       } 
       line_list=list_create(0);
       break;
     case GDS_LAYER:
       if (element) {
          if (strtok(buf,WHITE_SPACE_MASK) && 
             (ptr=strtok(NULL,WHITE_SPACE_MASK)))
             layer=atoi(ptr);
          if (hash_entries(layer_table)) {
             if (!hash_get(layer_table,(PTR)layer,(PTR*)&layer))
                layer=-2;
          }
       }
       break;
     case GDS_ENDEL:
       if (element && (layer!=-2)) {
            switch(element->id) {
              case GDS_SREF:
                ele_sref_num++;
                break;
              case GDS_AREF:
                ele_aref_num++;
                break;
              case GDS_BOUNDARY:
                ele_boundary_num++;
                break;
              case GDS_PATH:
                ele_path_num++;
                break;
              case GDS_TEXT:
                ele_text_num++;
                break;
              case GDS_BOX:
                ele_box_num++;
                break;
              case GDS_NODE:
                ele_node_num++;
                break;
              default:
                break;
            }
            element_num++;
       }
       list_free_all(line_list,ptr,free);
       element=NULL;
       layer=-1;
       break;
     case GDS_SNAME:
       if (element) {
          if (flag_msg_verbose) io_printf("##   %s %s\n",element->name,buf);
          list_enqueue(line_list,strdup(buf));
          if (flag_disp_hier) {
             if (strtok(buf,"\"") && (ptr=strtok(NULL,"\"")))
                append_gds_struct_filter(ptr);
          }
       }
       break;
     default:
 
       break;
   }
   free_str(&buf);
   
   if (layer==-2) {
      copy_str(outbufptr,"");
   } else {
      append_str(outbufptr,"\n");
   }
   return strlen(*outbufptr);
}

/**************/
/* GDS Filter */
/**************/

int append_gds_struct_filter(struct_set)
char *struct_set;
{
  char *ptr;
  long  flag;
  if (struct_table)
  string_for_all_sep(struct_set,ptr," \t\n,;"){
     if (!hash_get(struct_table,(PTR)ptr,(PTR)&flag))
        hash_put(struct_table,(PTR)strdup(ptr),(PTR)0);
  } string_for_all_end;
}

int assign_gds_struct_filter(struct_set)
char *struct_set;
{
  hash_free_nil(struct_table);
  struct_table=hash_create(strcasecmp,hash_str,hash_del_key);
  append_gds_struct_filter(struct_set);
}

int assign_gds_element_filter(element_set)
char *element_set;
{
  char *ptr;
  GDS_DEF element;
  
  hash_free_nil(element_table);
  element_table= hash_create(strcasecmp, hash_str, hash_del_key);
  string_for_all_sep(element_set,ptr," \t\n,;") {
    int j;
    for(j=0;j<GDS_NUMRECORDTYPES; j++) {
       if (strcasecmp(ptr,gds_def_array[j].name)==0) {
          hash_put(element_table,(PTR)strdup(gds_def_array[j].name),(PTR)(gds_def_array+j));
       }
    }
    if (!hash_get(element_table,(PTR)ptr,(PTR*)&element)) {
       io_error("Unknown element type '%s'\n",ptr);
    }
  } string_for_all_end;
}

int assign_gds_layer_filter(layer_set)
char *layer_set;
{
  long layer;
  char *ptr;
  
  hash_free_nil(layer_table);
  layer_table= hash_create(hash_cmp_num, hash_num, 0);
  string_for_all_sep(layer_set,ptr," \t\n,;") {
    layer=atoi(ptr);
    hash_put(layer_table,(PTR)layer,(PTR)layer);
  } string_for_all_end;
}

