/*
 * $Id $
 *
 *
 */
#include "libutil.h" 
#include "gdsmain.h" 
#include "gdsdata.h"
#include "gdsutil.h"
#include "gdsfilter.h"

int translate_gds_to_gdt(gdsin,gdtout)
char *gdsin;
char *gdtout;
{
  FILE *gdsinfp;
  FILE *gdtoutfp;
  char *buffer,*ptr;
  HASH_ENTRY hash_ele;
  
  io_printf("##========================================\n");
  if ((gdsin==0)||(*gdsin==0)) {
     gdsinfp=stdin;
     io_printf("## INPUT GDS FILE  : <stdin>\n");
  } else if ((gdsinfp=(FILE*)fopen(gdsin,"rb"))==NULL) {
     io_error("Can not open gds file '%s'\n",gdsin);
     return 0;
  } else {
     io_printf("## INPUT GDS FILE  : %s\n",gdsin);
  }
  if (!check_gds_header(gdsinfp)) {
     io_error("'%s' is not gds format file\n",gdsin);
     fclose(gdsinfp);
     return 0;
  }
  if ((gdtout==0)||(*gdtout==0)) {
     gdtoutfp=stdout;
  } else if ((gdtoutfp=(FILE*)fopen(gdtout,"w"))==NULL) {
     io_error("Can not output gdt file to '%s'\n",gdtout);
     fclose(gdsinfp);
     return 0;
  } else {
     io_printf("## OUTPUT GDT FILE : %s\n",gdtout);
  }
  if (hash_entries(struct_table)){
      hash_for_all_key(struct_table,ptr) {
        io_printf("## STRUCTURE : %s\n",ptr);
      } hash_for_all_end;
  } else {
      io_printf("## STRUCTURE : (ALL)\n");
  }
  if (hash_entries(element_table)) {
      io_printf("## ELEMENTS  :");
      hash_for_all_key(element_table,ptr) {
        io_printf(" %s",ptr);
      } hash_for_all_end;
      io_printf("\n");
  }
  if (hash_entries(layer_table)) {
      io_printf("## LAYERS    :");
      hash_for_all_key(layer_table,ptr) {
        io_printf(" %d",ptr);
      } hash_for_all_end;
      io_printf("\n");
  }
  io_printf("##----------------------------------------\n");
  if ((hash_entries(struct_table)==0)  && 
      (hash_entries(element_table)==0) &&
      (hash_entries(layer_table)==0))
    loop_gds_to_gdt(gdsinfp,gdtoutfp,proc_gds_to_gdt);
  else
    loop_gds_to_gdt(gdsinfp,gdtoutfp,proc_gds_to_gdt_with_filter);
  
  fclose(gdsinfp);
  fclose(gdtoutfp);
  if (hash_entries(struct_table)){
      hash_for_all_entry(struct_table,hash_ele) {
        if (hash_data(hash_ele)==0) {
           io_warning("STRUCTURE : \"%s\" is not found.\n",hash_key(hash_ele));
        }
      } hash_for_all_end;
  }
  return 1;
}
 
int translate_gds_to_gdx(gdsin,gdsout)
char *gdsin;
char *gdsout;
{
  FILE *gdsinfp;
  FILE *gdsoutfp;
  char *buffer,*ptr;
  HASH_ENTRY hash_ele;
  
  io_printf("##========================================\n");
  if ((gdsin==0)||(*gdsin==0)) {
     gdsinfp=stdin;
     io_printf("## INPUT GDS FILE  : <stdin>\n");
  } else if ((gdsinfp=(FILE*)fopen(gdsin,"rb"))==NULL) {
     io_error("Can not open gds file '%s'\n",gdsin);
     return 0;
  } else {
     io_printf("## INPUT GDS FILE  : %s\n",gdsin);
  }
  if (!check_gds_header(gdsinfp)) {
     io_error("'%s' is not gds format file\n",gdsin);
     fclose(gdsinfp);
     return 0;
  }
  if ((gdsout==0)||(*gdsout==0)) {
     gdsoutfp=stdout;
  } else if ((gdsoutfp=(FILE*)fopen(gdsout,"wb"))==NULL) {
     io_error("Can not output gds file to '%s'\n",gdsout);
     fclose(gdsinfp);
     return 0;
  } else {
     io_printf("## OUTPUT GDS FILE : %s\n",gdsout);
  }
  if (hash_entries(struct_table)){
      hash_for_all_key(struct_table,ptr) {
        io_printf("## STRUCTURE : %s\n",ptr);
      } hash_for_all_end;
  } else {
      io_printf("## STRUCTURE : (ALL)\n");
  }
  if (hash_entries(element_table)) {
      io_printf("## ELEMENTS  :");
      hash_for_all_key(element_table,ptr) {
        io_printf(" %s",ptr);
      } hash_for_all_end;
      io_printf("\n");
  }
  if (hash_entries(layer_table)) {
      io_printf("## LAYERS    :");
      hash_for_all_key(layer_table,ptr) {
        io_printf(" %d",ptr);
      } hash_for_all_end;
      io_printf("\n");
  }
  io_printf("##----------------------------------------\n");

  loop_gds_to_gdx(gdsinfp,gdsoutfp,proc_gds_to_gdx);
  
  fclose(gdsinfp);
  fclose(gdsoutfp);
  if (hash_entries(struct_table)){
      hash_for_all_entry(struct_table,hash_ele) {
        if (hash_data(hash_ele)==0) {
           io_warning("STRUCTURE : \"%s\" is not found.\n",hash_key(hash_ele));
        }
      } hash_for_all_end;
  }
  return 1;
}
 
int translate_gdt_to_gdx(gdtin,gdsout)
char *gdtin;
char *gdsout;
{
  FILE *gdtinfp;
  FILE *gdsoutfp;
  char *buffer,*ptr;
  long gds_len;
  
  io_printf("##========================================\n");
  if ((gdtin==NULL)||(*gdtin==0)) {
     gdtinfp=stdin;
  } else if ((gdtinfp=(FILE*)fopen64(gdtin,"r"))==NULL) {
     io_error("Can not open gds file '%s'\n",gdtin);
     return 0;
  } else {
     io_printf("## INPUT GDT FILE  : %s\n",gdtin);
  }
  if ((gdsout==NULL)||(*gdsout==0)) {
     gdsoutfp=stdout;
  } else if ((gdsoutfp=(FILE*)fopen64(gdsout,"wb"))==NULL) {
     io_error("Can not output gds file to '%s'\n",gdsout);
     fclose(gdtinfp);
     return 0;
  } else {
     io_printf("## OUTPUT GDS FILE : %s\n",gdsout);
  }
  gds_len = loop_gdt_to_gdx(gdtinfp,gdsoutfp,proc_gdt_to_gdx);
  gds_len = 2048 - (gds_len % 2048);
/*  if (gds_len>0) { fwrite("",1,gds_len,gdsoutfp); } */
  fclose(gdtinfp);
  fclose(gdsoutfp);
  return 1;
}

