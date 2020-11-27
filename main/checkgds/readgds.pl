#!/usr/bin/perl
$HELP_DOC=<<"!END_HELP";
##      GDS Reader, v9707 by Hung-Chun Li ##
Usage: readgds.pl <gds name> ..

!END_HELP

die($HELP_DOC) if (@ARGV==0);

$DEF=<<"!END_DEF";
; VAR TYPE
; 00 -- No data
; 01 -- 2-bytes bit array
; 02 -- 2-bytes integer
; 03 -- 4-bytes integer
; 04 -- 4-bytes real
; 05 -- 8-bytes real
; 06 -- ascii string

;ID	NAME		VAR	SIZE	LEN
;==	========= 	===	====	===
00	HEADER		02	2	1
01	BGNLIB		02	2	12
02	LIBNAME		06	45	1
03	UNITS		05	8	2
04	ENDLIB		00	0	0
05	BGNSTR		02	2	12
06	STRNAME		06	32	1
07	ENDSTR		00	0	0
08	BOUNDARY	00	0	0
09	PATH		00	0	0
0A	SREF		00	0	0
0B	AREF		00	0	0
0C	TEXT		00	0	0
0D	LAYER		02	2	1
0E	DATATYPE	02	2	1
0F	WIDTH		03	4	1
10	XY		03	4	200
11	ENDEL		00	0	0
12	SNAME		06	32	1
13	COLROW		02	2	1
15	NODE		00	0	0
16	TEXTTYPE	02	2	1
17	PRESENTATION	01	2	1
18	SPACING		
19	STRING		06	512	1
1A	STRANS		01	2	1
1B	MAG		05	8	1
1C	ANGLE		05	8	1
1D	UINTEGER
1E	USTRING
1F	REFLIBS		06	45	2
20	FONTS		06	44	4
21	PATHTYPE	02	2	1
22	GENERATIONS	02	2	1
23	ATTRTABLE	06	44	1
24	STYPTABLE	06	44	1
25	STRTYPE		06	44	1
26	ELFLAGS		01	2	1
27	ELKEY
28	LINKTYPE
29	LINKKEYS
2A	NODETYPE	02	2	1
2B	PROPATTR	02	2	1
2C	PROPVALUE	06	4096	1
2D	BOX		00	0	0
2E	BOXTYPE		02	2	1
2F	PLEX		03	4	1
30	BGNEXTN
31	ENDEXTN
32	TAPENUM
33	TAPECODE
34	STRCLASS
35	RESERVED
36	FORMAT		02	2	1
37	MASK		06	4096	1
38	EMDMASKS	00	0	0
39	LIBDIRSIZE	
3A	SRFNAME		06	4096	1
3B	LIBSECUR
!END_DEF
@DEF=split(/\n+/,$DEF);
for $line(@DEF) {
  $line=~s/\;.*$//g;
  next if ($line=~/^\s*$/);
  $id=$name=$type=$size=$len="";
  ($id,$name,$type,$size,$len)=split(/\s+/,$line);
  $id=hex($id);
  $type=hex($type);
  $IDNAME{$id}=$name;
  $VARTYPE{$id}=$type;
  $VARSIZE{$id}=$size;
  $VARLEN{$id}=$len;
#  print "$id $name $type $size $len\n";
}
%SIZE=(0=>0,1=>2,2=>2,3=>4,4=>4,5=>8,6=>4096);

$gdsin=shift @ARGV;
open(GDSIN,$gdsin);
binmode(GDSIN);
print STDERR "Opening GDS '$gdsin'...\n";
while(!eof(GDSIN)) {
  read(GDSIN,$head,4)||next;
  $hex=unpack("H*",$head);
  next if ( $hex eq "00000000");
  next if (length($hex) != 8);
  $len=unpack("S",$head)-4;
  $id=unpack("\@2C",$head);
  $vartype=unpack("\@3C",$head);
  if (exists $IDNAME{$id}) {
    print $IDNAME{$id};
  } else {
    printf "KEYWORD(%02X)",$id;
    $VARLEN{$id}=-1;
    $VARTYPE{$id}=$vartype;
    $VARSIZE{$id}=$SIZE{$vartype};
  }
  if ($len>0) {
    read(GDSIN,$input,$len);
    if (($vartype>0) &&($VARTYPE{$id}==$vartype)) {
      $ptr=0;
      $cnt=0;
      while((($cnt<$VARLEN{$id})||($VARLEN{$id}<0))&&($ptr<$len)) {
         print " ";
         if ($vartype==1) {
           $val=unpack("\@$ptr\S",$input);
         } elsif ($vartype==2) {
           $val=unpack("\@$ptr\S",$input);
         } elsif ($vartype==3) {
           $val=unpack("\@$ptr\i",$input);
         } elsif ($vartype==5) {
           $val=unpack("\@$ptr\H8",$input);
         } elsif ($vartype==6) {
           $val="\"".substr($input,$ptr,$VARSIZE{$id})."\"";
         }
         $cnt++;
         $ptr+=$VARSIZE{$id};
         print "$val";
      }
    } else {
      printf ":### Incorrect variable type %02X(%02X) ###",$vartype,$VARTYPE{$id};
    }
  }
  print "\n";
}
close(GDSIN);
