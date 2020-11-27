#ifndef __GDSUTIL_H__
#define __GDSUTIL_H__

extern int  max_line_width;
extern int  gds_record_num;
extern int  gdt_line_num;

extern BOOL flag_msg_verbose;
extern BOOL flag_disp_line;
extern BOOL flag_disp_length;
extern BOOL flag_disp_header;
extern BOOL flag_disp_hier;

extern int check_gds_header();
extern int read_gds_record();
extern long loop_gds_to_gdt();
extern long loop_gdt_to_gdx();
extern long loop_gds_to_gdx();
extern long proc_gds_to_gdt();
extern long proc_gdt_to_gdx();
extern long proc_gds_to_gdx();

#endif
