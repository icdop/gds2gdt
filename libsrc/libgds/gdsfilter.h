#ifndef __GDS_FILTER_H__
#define __GDS_FILTER_H__

extern HASH_TABLE struct_table;
extern HASH_TABLE element_table;
extern HASH_TABLE layer_table;

extern int append_gds_struct_filter();
extern int assign_gds_struct_filter();
extern int assign_gds_element_filter();
extern int assign_gds_layer_filter();

extern long proc_gds_to_gdt_with_filter();

#endif
