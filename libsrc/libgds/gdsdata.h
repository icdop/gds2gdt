#ifndef __GDSDATA_H__
#define __GDSDATA_H__
#include "libutil.h"

/* Data Type */
#define GDS_NODATA	0		/* No data present */
#define GDS_BITARRAY	1		/* Bit array       */
#define GDS_I2		2		/* 2 byte integer  */
#define GDS_I4		3		/* 4 byte integer  */
#define GDS_R4		4		/* 4 byte real     */
#define GDS_R8		5		/* 8 byte real     */
#define GDS_ASCII	6		/* ASCII string    */
#define GDS_UNKNOWN	7		/* Unknown type    */

/* Record types */
#define GDS_HEADER		 0
#define GDS_BGNLIB		 1
#define GDS_LIBNAME		 2
#define GDS_UNITS		 3
#define GDS_ENDLIB		 4
#define GDS_BGNSTR		 5
#define GDS_STRNAME		 6
#define GDS_ENDSTR		 7
#define GDS_BOUNDARY		 8
#define GDS_PATH		 9
#define GDS_SREF		 10
#define GDS_AREF		 11
#define GDS_TEXT		 12
#define GDS_LAYER		 13
#define GDS_DATATYPE		 14
#define GDS_WIDTH		 15
#define GDS_XY		 	 16
#define GDS_ENDEL		 17
#define GDS_SNAME		 18
#define GDS_COLROW		 19
#define GDS_TEXTNODE		 20
#define GDS_NODE		 21
#define GDS_TEXTTYPE		 22
#define GDS_PRESENTATION	 23
#define GDS_SPACING		 24
#define GDS_STRING		 25
#define GDS_STRANS		 26
#define GDS_MAG		 	 27
#define GDS_ANGLE		 28
#define GDS_UINTEGER		 29
#define GDS_USTRING		 30
#define GDS_REFLIBS		 31
#define GDS_FONTS		 32
#define GDS_PATHTYPE		 33
#define GDS_GENERATIONS	 	 34
#define GDS_ATTRTABLE		 35
#define GDS_STYPTABLE		 36
#define GDS_STRTYPE		 37
#define GDS_ELFLAGS		 38
#define GDS_ELKEY		 39
#define GDS_LINKTYPE		 40
#define GDS_LINKKEYS		 41
#define GDS_NODETYPE		 42
#define GDS_PROPATTR		 43
#define GDS_PROPVALUE		 44
#define GDS_BOX		 	 45
#define GDS_BOXTYPE		 46
#define GDS_PLEX		 47
#define GDS_BGNEXTN		 48
#define GDS_ENDEXTN		 49
#define GDS_TAPENUM		 50
#define GDS_TAPECODE		 51
#define GDS_STRCLASS		 52
#define GDS_RESERVED		 53
#define GDS_FORMAT		 54
#define GDS_MASK		 55
#define GDS_ENDMASKS		 56
#define GDS_LIBDIRSIZE	 	 57
#define GDS_SRFNAME		 58
#define GDS_LIBSECUR		 59
#define GDS_NUMRECORDTYPES	 60

/*
   Flags for transforms
   If bit 1 is set, then mirroring about X before rotation.
   if bit 14 is set, then absolute magnification (optional GDS_MAG) recs
   if bit 15 is set, then absolute rotation (optional GDS_ANGLE) recs
*/
#define GDS_STRANS_UPSIDEDOWN	0x8000
#define GDS_STRANS_MAG		0x0004
#define GDS_STRANS_ROTATE		0x0002

#define GDSPATH_SQUAREFLUSH	0 /* Square end flush with endpoint */
#define GDSPATH_ROUND		1 /* Round end  */
#define GDSPATH_SQUAREPLUS	2 /* Square end + half-width extent */

#define GDS_LAYER_MAX		63	/* Largest gds layer or data type numbers */
#define GDSNAMELENGTH		32	/* Biggest gds string */
#define GDSHEADERLENGTH		4	/* Length of record header */
#define MAX_GDSBUF_SIZE         4200    /* Max record size */

#define GDSRECLEN(b)  ((unsigned int)b[0]*256 +(unsigned int)b[1]) 
#define GDSRECID(b)   (unsigned int)b[2]
#define GDSRECTYPE(b) (unsigned int)b[3]

typedef struct gds_def_struct {
  char id;              /* GDS record id (Byte 3) */
  char name[15];        /* GDS record symbod      */
  char type;            /* parameter type         */
  int  size;            /* record length          */
  int  num;             /* paramter number        */
} GDS_DEF_STRUCT, *GDS_DEF;

extern GDS_DEF_STRUCT gds_def_array[];
extern int gds_size[];
extern int encode_gds_data();
extern char* decode_gds_data();

#endif
