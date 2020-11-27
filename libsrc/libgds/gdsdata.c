/*
 * $Id $
 *
 *
 */
#include "gdsdata.h"

static char* GDS_DATA_TYPES[8] = {
    "NODATA",
    "BITARRAY",
    "I2",
    "I4",
    "R4",
    "R8",
    "ASCII",
    "UNKNOWN"
};

int gds_size[8]= {0,2,2,4,4,8,512,512};



GDS_DEF_STRUCT gds_def_array[]={
/*
 *
 *   Record      Record         Parameter   Parameter  Parameter
 *     ID        Name             Type        Size      Number
 *    (id)      (name[15])       (type)      (size)     (num)
 */
     {0x00,	"HEADER",	GDS_I2,		2,	1   },
     {0x01,	"BGNLIB",	GDS_I2,		2,	12  },
     {0x02,	"LIBNAME",	GDS_ASCII,	-512,	1   },
     {0x03,	"UNITS",	GDS_R8,		8,	2   },
     {0x04,	"ENDLIB",	GDS_NODATA,	0,	0   },
     {0x05,	"BGNSTR",	GDS_I2,		2,	12  },
     {0x06,	"STRNAME",	GDS_ASCII,	-128,	1   }, /* Up to 128 char */
     {0x07,	"ENDSTR",	GDS_NODATA,	0,	0   },
     {0x08,	"BOUNDARY",	GDS_NODATA,	0,	0   },
     {0x09,	"PATH",		GDS_NODATA,	0,	0   },
     {0x0A,	"SREF",		GDS_NODATA,	0,	0   },
     {0x0B,	"AREF",		GDS_NODATA,	0,	0   },
     {0x0C,	"TEXT",		GDS_NODATA,	0,	0   },
     {0x0D,	"LAYER",	GDS_I2,		2,	1   },
     {0x0E,	"DATATYPE",	GDS_I2,		2,	1   },
     {0x0F,	"WIDTH",	GDS_I4,		4,	1   },
     {0x10,	"XY",		GDS_I4,		4,	-1024 },
     {0x11,	"ENDEL",	GDS_NODATA,	0,	0   },
     {0x12,	"SNAME",	GDS_ASCII,	-128,	1   }, /* Up to 128 char */
     {0x13,	"COLROW",	GDS_I2,		2,	2   },
     {0x14,	"TEXTNODE",	GDS_UNKNOWN,	0,	0   },
     {0x15,	"NODE",		GDS_NODATA,	0,	0   },
     {0x16,	"TEXTTYPE",	GDS_I2,		2,	1   },
     {0x17,	"PRESENTATION",	GDS_BITARRAY,	2,	1   },
     {0x18,	"SPACING",	GDS_UNKNOWN,	0, 	0   },
     {0x19,	"STRING",	GDS_ASCII,	-512,	1   }, /* Up to 512 char */
     {0x1A,	"STRANS",	GDS_BITARRAY,	2,	1   },
     {0x1B,	"MAG",		GDS_R8,		8,	1   },
     {0x1C,	"ANGLE",	GDS_R8,		8,	1   },
     {0x1D,	"UINTEGER",	GDS_UNKNOWN,	0,	0   },
     {0x1E,	"USTRING",	GDS_UNKNOWN,	0,	0   },
     {0x1F,	"REFLIBS",	GDS_ASCII,	45,	2   },
     {0x20,	"FONTS",	GDS_ASCII,	44,	4   },
     {0x21,	"PATHTYPE",	GDS_I2,		2,	1   },
     {0x22,	"GENERATIONS",	GDS_I2,		2,	1   },
     {0x23,	"ATTRTABLE",	GDS_ASCII,	44,	1   },
     {0x24,	"STYPTABLE",	GDS_ASCII,	44,	1   },
     {0x25,	"STRTYPE",	GDS_ASCII,	44,	1   },
     {0x26,	"ELFLAGS",	GDS_BITARRAY,	2,	1   },
     {0x27,	"ELKEY",	GDS_UNKNOWN,	0,	0   },
     {0x28,	"LINKTYPE",	GDS_UNKNOWN,	0,	0   },
     {0x29,	"LINKKEYS",	GDS_UNKNOWN,	0,	0   },
     {0x2A,	"NODETYPE",	GDS_I2,		2,	1   },
     {0x2B,	"PROPATTR",	GDS_I2,		2,	1   },
     {0x2C,	"PROPVALUE",	GDS_ASCII,	-512,	1   },
     {0x2D,	"BOX",		GDS_NODATA,	0,	0   },
     {0x2E,	"BOXTYPE",	GDS_I2,		2,	1   },
     {0x2F,	"PLEX",		GDS_I4,		4,	1   },
     {0x30,	"BGNEXTN",	GDS_UNKNOWN,	0,	0   },
     {0x31,	"ENDEXTN",	GDS_UNKNOWN,	0,	0   },
     {0x32,	"TAPENUM",	GDS_UNKNOWN,	0,	0   },
     {0x33,	"TAPECODE",	GDS_UNKNOWN,	0,	0   },
     {0x34,	"STRCLASS",	GDS_UNKNOWN,	0,	0   },
     {0x35,	"RESERVED",	GDS_UNKNOWN,	0,	0   },
     {0x36,	"FORMAT",	GDS_I2,		2,	1   },
     {0x37,	"MASK",		GDS_ASCII,	-512,	1   },
     {0x38,	"EMDMASKS",	GDS_NODATA,	0,	0   },
     {0x39,	"LIBDIRSIZE",	GDS_UNKNOWN,	0,	0   },	
     {0x3A,	"SRFNAME",	GDS_ASCII,	-512,	1   },
     {0x3B,	"LIBSECUR",	GDS_UNKNOWN,	0,	0   },
     {0x40,     "",		GDS_UNKNOWN,	0,	0   }
};


/****************************/
/* decode bin code to text */
/****************************/

static char _tmp_GDS_buf_[MAX_OUTBUF_SIZE];
static char* decode_GDS_I2(dchars,size)
unsigned char *dchars;
int size;
{
     int value=dchars[0];
     value=dchars[1]+value*256;
     sprintf(_tmp_GDS_buf_," %d",value);
     return _tmp_GDS_buf_;
}

static char* decode_GDS_I4(dchars,size)
unsigned char *dchars;
int size;
{
     long value=dchars[0];
     value=dchars[1]+value*256;
     value=dchars[2]+value*256;
     value=dchars[3]+value*256;
     if (dchars[0] & 0x80) { value = (value ^ 0xFFFFFFFF) + 1 ; value = -value; }
     sprintf(_tmp_GDS_buf_," %ld",value);
     return _tmp_GDS_buf_;
}

static char* decode_GDS_R4(dchars,size)
unsigned char *dchars;
int size;
{
    double val = 0.0;
    int i;
    int exponent = dchars[0];
    char isneg;
    if (isneg = (exponent & 0x80)) exponent &= ~0x80;
    exponent -= 64;
    for (i = 3; i > 0; i--) {
        val += dchars[i];
        val /= 256.0;
    }
    /* Now raise the mantissa to the exponent */
    if (exponent > 0) {
        while (exponent-- > 0)
            val *= 16.0;
    } else if (exponent < 0) {
        while (exponent++ < 0)
            val /= 16.0;
    }
    /* Make it negative if necessary */
    if (isneg) val = -val;
     sprintf(_tmp_GDS_buf_," %lg",val);
     return _tmp_GDS_buf_;
}


static char* decode_GDS_R8(dchars,size)
unsigned char *dchars;
int size;
{
    double val = 0.0;
    int i;
    int exponent = dchars[0];
    char isneg;
    if (isneg = (exponent & 0x80)) exponent &= ~0x80;
    exponent -= 64;
    for (i = 7; i > 0; i--) {
        val += dchars[i];
        val /= 256.0;
    }
    /* Now raise the mantissa to the exponent */
    if (exponent > 0) {
        while (exponent-- > 0)
            val *= 16.0;
    } else if (exponent < 0) {
        while (exponent++ < 0)
            val /= 16.0;
    }
    /* Make it negative if necessary */
    if (isneg) val = -val;
    sprintf(_tmp_GDS_buf_," %lg",val);
    return _tmp_GDS_buf_;
}

static char* decode_GDS_ASCII(dchars,size)
unsigned char *dchars;
int size;
{
    strcpy(_tmp_GDS_buf_," \"");
    if (size>512) {
       io_error("string size (%d > 512) is too large: '%10.10s'\n",size,dchars);
    } else {
      strncpy(_tmp_GDS_buf_+2,(char*)dchars,size);
      _tmp_GDS_buf_[size+2]=0;
    }
    strcat(_tmp_GDS_buf_,"\"");
    return _tmp_GDS_buf_;
}

static char* decode_GDS_BITARRAY(dchars,size)
unsigned char *dchars;
int size;
{
    sprintf(_tmp_GDS_buf_," 0x%02x%02x",dchars[0],dchars[1]);
    return _tmp_GDS_buf_;
}

/****************************/
/* encode text to bin value */
/****************************/
static int encode_GDS_I2(buf,dchars,size)
char *buf;
unsigned char *dchars;
int size;
{
    int value = 0;
    sscanf(buf,"%d",&value);
    dchars[0]=value/256;
    dchars[1]=value%256;
    return 2;
} 

static int encode_GDS_I4(buf,dchars,size)
char *buf;
unsigned char *dchars;
int size;
{
    double  value = 0.0;
    int  rest;
    int  i;
    sscanf(buf,"%lf",&value);
    if (value<0.0) value+=65536.0*65536.0;
    for(i=3;i>=0;i--) {
      rest = value / 256.0;
      dchars[i]= (int)(value - rest * 256.0);
      value = rest;
    }
    return 4;
} 

static int encode_GDS_ASCII(buf,dchars,size)
char *buf;
unsigned char *dchars;
int size;
{
  char *ptr;
  int  i=0;
  int  max=abs(size);
  ptr=buf;
  if (*ptr=='\"') ptr++;
  while(*ptr && (*ptr!='\"') && (i<max)) {
    if (*ptr==0x5C) ptr++; /*back slash */
    dchars[i]=*ptr;
    ptr++;i++;
  }
  while((i<size)||(i%2)) dchars[i++]=0;  
  dchars[i]=0;
//  io_printf("DEBUG:%s\n",dchars);
  return i;
}

static int encode_GDS_BITARRAY(buf,dchars,size)
char *buf;
unsigned char *dchars;
int size;
{
    int value = 0;
    sscanf(buf,"%x",&value);
    dchars[0]=value / 256;
    dchars[1]=value % 256;
    return 2;
}

static int encode_GDS_R4(buf,dchars,size)
char *buf;
unsigned char *dchars;
int size;
{
    double val = 0.0;
    int i;
    int exponent = 0;
    char isneg;
    
    sscanf(buf,"%lf",&val);
    if (isneg=(val<0.0)) { val=-val; }
    exponent += 64;
    if (val==0.0) {
    } else if (val>=1.0) {
       while(val>=1.0) {
         exponent++;
         val/=16.0;
       }
    } else if (val<(1.0/16.0)) {
       while(val<(1.0/16.0)) {
         exponent--;
         val*=16.0;
       }
    }
    dchars[0]=exponent;
    if (isneg) dchars[0] |= 0x80;
    for(i=1; i<4; i++) {
      val*=256.0;
      dchars[i]=(int)val;
      val-=dchars[i];
    }
    return 4;
}

static int encode_GDS_R8(buf,dchars,size)
char *buf;
unsigned char *dchars;
int size;
{
    double val = 0.0;
    int i;
    int exponent = 0;
    char isneg;
    
    sscanf(buf,"%lf",&val);
    if (isneg=(val<0.0)) { val=-val; }
    exponent += 64;
    if (val==0.0) {
    } else if (val>=1.0) {
       while(val>=1.0) {
         exponent++;
         val/=16.0;
       }
    } else if (val<(1.0/16.0)) {
       while(val<(1.0/16.0)) {
         exponent--;
         val*=16.0;
       }
    }
    dchars[0]=exponent;
    if (isneg) dchars[0] |= 0x80;
    for(i=1; i<8; i++) {
      val*=256.0;
      dchars[i]=(int)val;
      val-=dchars[i];
    }
    return 8;
}


/**********************************/
/* exported procedure             */
/**********************************/



static char* (*decode_gds_data_proc[])()={
	NULL,
	decode_GDS_BITARRAY,
	decode_GDS_I2,
	decode_GDS_I4,
	decode_GDS_R4,
	decode_GDS_R8,
	decode_GDS_ASCII,
	NULL
};

char *decode_gds_data(type,dchars,size)
int type;
unsigned char *dchars;
int size;
{
  return decode_gds_data_proc[type](dchars,size);
}


static int (*encode_gds_data_proc[])()={
	NULL,
	encode_GDS_BITARRAY,
	encode_GDS_I2,
	encode_GDS_I4,
	encode_GDS_R4,
	encode_GDS_R8,
	encode_GDS_ASCII,
        NULL
};

int encode_gds_data(type,buf,dchars,size)
int type;
char *buf;
unsigned char *dchars;
int size;
{
  return encode_gds_data_proc[type](buf,dchars,size);
}


