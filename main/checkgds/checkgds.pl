#!/usr/bin/perl 
print STDOUT <<"!END_TITLE";
#############################################################################
##                 Layout GDS pinname Verification V9710                   ##
##                          by Hung-Chun Li                                ##
#############################################################################
!END_TITLE
$VERSION_DOC=<<"!END_VERSION";
97.10.06	Fix Spice multiple lines format
		Fix PMD multiple lines format
!END_VERSION

$HELP_DOC=<<"!END_HELP";
Usage: checkgds.pl [options] <gdsfile> ...
  [options]
	+help		display this message.
	+rule		display gds pinname checking rules.
	+msg		display error/message description and output format

	+size		report cell size in gds but do not compare with pmd.
	+gdt		input files are gdt text files.
	+pad		force all gds cells as pad I/O cells

	-l <logfile>	log file name, default is 'checkgds.log'.
	-t <gdtdir>	gdt destination directory, default is <GDT>.
	{-p <pmddir>}	pmd directory, if not specify do not check pmd
	{-s <spidir>}	spice directory, if not specify do not check spice

  [arguments]
	<gdsfile>	can be gdsfile name or directory name

Example:
	checkgds inv0.gds 
	checkgds -p pmddir gdsdir/
	checkgds -p pmddir1 -p pmddir2 gdsdir/*.gds
	checkgds -p pmddir -s spidir gdsdir/*

!END_HELP

$RULE_DOC=<<"!END_RULE";
#############################################################################
GDS CHECKING RULES:

   1. GDS should be flattened or pin text are all on the top structure,
      Only top structure will be checked.

   2. Texts are extracted from GDS:
      Texts in Layer 62(TEXT) are recognized as Cell Label.
      Texts in Layer 4x(TEXT) are recognized as pinnames.
         40(TEXT1), 41(TEXT2), 42(TEXT3) ,43(TEXT4) and 44(TEXT5) 
      Texts in Layer 16(METAL) will be reported as ERROR.
         16(METAL1), 18(METAL2), 28(METAL2), 31(METAL2), 33(METAL2)
      Texts in other Layers are considered as Internal pins (WARNING)

   3. Pin names and Cell Label should be UPPER CASE.

   4. Cell Label should be the same as GDS structure name and cell name.

   5. Each cell should contains default 'VDD' and 'VSS' pins.

   6. Each Pin name should has a corresponding pin in [pmd]/[spi] file.

   7. Cell Width and Height is extracted from cell boundary (Layer 62)
      and will be checked with pmd.

#############################################################################
!END_RULE

$MSG_DOC=<<"!END_MSG";
[ OUTPUT FILE FORMAT ]
    .CELL <cellname>
    .SNAM <pad macro>                       // structure name of pad macro
    .UNIT <unit>                            // coordination unit (um)
    .SIZE <width> <height> (<X0 Y0 X1 Y1>)  // unit of width/height is um
    .PIN  <pin> (<layer> ...)               // Valid Pin in TEXT layer
    .ERR  <pin> (<layer>@<X,Y> ...)         // Pin in METAL (ERROR)
    .INT  <pin> (<layer>@<X,Y> ...)         // Extra Internal Pin (WARNING)
    .SPI  <pin> ...                         // Pin sequence in spice
    .PMD  <pin> ...                         // Pin sequence in pmd
    .ENDC

[ MESSAGES ]
== FILE ==
  *** ERROR *** Can not open gdt file '<cell>.gdt'!
  *** ERROR *** Can not open gds file '<cell>.gds'!
  *** ERROR *** Can not open pmd file '<cell>.pmd'!
  *** ERROR *** Can not open spice file '<cell>.spi'!

== UPPER CASE ==
  *** ERROR *** Structure Name (<strname>) should be UPPER CASE.
  ** WARNING ** Spice cell name '<name>' should be UPPER CASE.
  *** ERROR *** pinname '<pin>' should be UPPER CASE.
  *** ERROR *** pin '<pin>' in [spi] should be UPPER CASE.
  *** ERROR *** pin '<pin>' in [pmd] should be UPPER CASE.

== MISSING & MISMATCH ==
  ** WARNING ** Cell label is missing in GDS.
  *** ERROR *** Cell label '<cellname>' does not match with Structure Name.
  *** ERROR *** 'VDD' is missing.
  *** ERROR *** 'VSS' is missing.
  ** WARNING ** pin '<pin>' in [spi] should be defined as global.
  *** ERROR *** pin '<pin>' is in [spi] but not found in GDS.
  *** ERROR *** pin '<pin>' is not found in [spi].
  *** ERROR *** pin '<pin>' is in [pmd] but not found in GDS.
  *** ERROR *** pin '<pin>' is not found in [pmd].
  *** ERROR *** Spice subckt '<CELL>' not found.

== EXTRA ==
  *** ERROR *** internal pins exist (in metal layers).
  ** WARNING ** extra pins exist (in unknown layers).

== BOUNDARY ==
  *** ERROR *** Missing cell boundary.
  *** ERROR *** SIZE does not match with [pmd](<width>,<height>).
  *** ERROR *** pin '<pin>' (<layer>@<X,Y>) is out of range.
  ** WARNING ** Boundary Layer of '<strname>' is redefined.
  ** WARNING ** Boundary Layer of '<strname>' is not a rectangle.

!END_MSG
$prog_name=$0;
@pmddirs=();
@spidirs=();
$gdtdir="GDT";
$gdtflag=0;
$sizeflag=0;
$padflag=0;
$logfile="checkgds.log";
$layer_boundary=62;
while($opt=shift(@ARGV)) {
  if ($opt =~ s/^\+//) {
     if ($opt=~/^h/) {
        print STDOUT $HELP_DOC;
        exit(0);
     } elsif ($opt eq "rule") {
        print STDOUT $RULE_DOC;
        exit(0);
     } elsif ($opt eq "msg") {
        print STDOUT $MSG_DOC;
        exit(0);
     } elsif ($opt eq "size") {
        $sizeflag=1;
     } elsif ($opt eq "gdt") {
        $gdtflag=1;
     } elsif ($opt eq "pad") {
        $padflag=1;
     } else {
        print STDOUT "ERROR options: +$opt\n";
     }
  } elsif ($opt =~ s/^\-//) {
     if ($opt eq "p") {
        push(@pmddirs,shift(@ARGV));
     } elsif ($opt eq "s") {
        push(@spidirs,shift(@ARGV));
     } elsif ($opt eq "t") {
        $gdtdir=shift(@ARGV);
     } elsif ($opt eq "l") {
        $logfile=shift(@ARGV);
     } else {
        print STDOUT "ERROR options: -$opt\n";
     }
  } else {
     push(@gdsrecs,$opt);
  }
}
if (@gdsrecs==0) {
   print STDOUT $HELP_DOC;
   exit(0);
}

print STDOUT "## LOGFILE: $logfile\n";
open OUT,">$logfile" || die("Can not open logfile '$logfile'\n");
select OUT;
$|=1;
%TEXT_LAYER =( 16=>"METAL1",
               18=>"METAL2",
               28=>"METAL3",
               31=>"METAL4",
               33=>"METAL5",
               40=>"TEXT1",
               41=>"TEXT2",
               42=>"TEXT3",
               43=>"TEXT4",
               44=>"TEXT5",
               $layer_boundary=>"BOUNDARY");

for $pmddir(@pmddirs) {
  Message("## PMDDIR : $pmddir\n");
}
for $spidir(@spidirs) {
  Message("## SPIDIR : $spidir\n");
}

Message("## GDTDIR : $gdtdir\n");

if ($gdtflag==1) {
   for $gdtrec(@gdsrecs) {
     ($gdtfile,$gdsname)=split(/\:/,$gdtrec,2);
     if (-d $gdtfile) {
       Message("## GDTDIR : $gdtfile\n");
       push(@gdtfiles,<$gdtfile/*.gdt>);
     } elsif (-f $gdtfile) {
       push(@gdtfiles,$gdtfile);
     } else {
       ErrorMsg("Can not open gdt file '$gdtfile'!");
     }
   }
} else {
   for $gdsrec(@gdsrecs) {
     ($gdsfile,$gdsname)=split(/\:/,$gdsrec,2);
     if (-d $gdsfile) {
       Message("## GDSDIR : $gdsfile\n");
       push(@gdsfiles,<$gdsfile/*.gds>);
     } elsif (-f $gdsfile) {
       push(@gdsfiles,$gdsrec);
     } else {
       ErrorMsg("Can not open gds file '$gdsfile'!");
     }
   }
   if (scalar(@gdsfiles)!=0) {
      if (system("readgds","-d",$gdtdir,@gdsfiles)) {
         die("readgds program error!\n");
      }
   }
   for $gdtrec(@gdsfiles) {
     ($gdtfile,$gdtname)=split(/\:/,$gdtrec,2);
     if ($gdtname ne "") {
        $gdtfile="$gdtname.gdt";
     } else {
        $gdtfile=~s/\.gds$/\.gdt/;
        $gdtfile=~s/^.*\///g;
     }
     push(@gdtfiles,"$gdtdir/$gdtfile");
   }
}
Message("## PAD FLAG IS ON\n") if ($padflag);
$total_error=$error=0;
$total_warn=$warning=0;
@STRUCTURE=();
for $gdtfile(@gdtfiles) {
    %CELNAME=%PINNAME=%INTPIN=%EXTPIN=%TEXT=%BOUNDARY=%SIZE=%SNAME=();
    Message("===================================================\n");
    Message("Reading  '$gdtfile' ....\n");
    if (open (GDT,$gdtfile)==0) {
        ErrorMsg("Can not open gdt file '$gdtfile'!");
    } else {
        $cell=uc($gdtfile);
        $cell=~s/^.*\///g;
        $cell=~s/\..*$//g;
        while($line=<GDT>) {
          while($line=~s/\\\n$//) { $line.=<GDT>};
          $line=~s/^\s+//g;
          next if ($line!~/./);
          if ($line=~/^UNITS\s+([^\s]+)\s+([^\s]+)/) {
             $unit=$1;
          } elsif ($line=~/^STRNAME\s+\"([^\"]*)\"/) {
             print STDOUT "  STRUCTURE $1\r";
             $strname=uc($1);
             if ($strname ne $1) {
                ErrorMsg("Structure Name ($1) should be UPPER CASE.");
             }
             %LAYER=();
             $srefs=" ";
          } if ($strname eq $cell) {
            if ($line=~/^ENDSTR/) {
               push(@STRUCTURE,$strname);
               $CELNAME{$strname}=$LAYER{"BOUNDARY"};
               $PINNAME{$strname}=compress(
                                  $LAYER{"TEXT1"}.$LAYER{"TEXT2"}.
                                  $LAYER{"TEXT3"}.$LAYER{"TEXT4"}.
                                  $LAYER{"TEXT5"}
                                  );
               $INTPIN{$strname}=compress(
                                 $LAYER{"METAL1"}.$LAYER{"METAL2"}.
                                 $LAYER{"METAL3"}.$LAYER{"METAL4"}.
                                 $LAYER{"METAL5"}
                                 );
               $EXTPIN{$strname}=compress($LAYER{"OTHER"});
               $UNIT{$strname}=$unit;
               $SNAME{$strname}=compress($srefs);
            } elsif ($line=~/^LAYER\s+(\d+)\s/) {
               $layer=$1;
            } elsif ($line=~/^ENDEL/) {
               $element="";
            } elsif ($line=~/^(BOUNDARY|TEXT|SREF)/) {
               $element=$1;
               $string=""; $position="";
            } elsif ($line=~/^SNAME\s+\"(.*)\"/) {
               $srefs.=$1." ";
            } elsif ($element eq "TEXT") {
               if ($line=~/^STRING\s+\"(.*)\"/) {
                  $string=$1;
                  $string=~s/\:.*//o;
                  $TEXT{$string}.="$layer\@$position ";
                  if (exists $TEXT_LAYER{$layer}) {
                     $LAYER{$TEXT_LAYER{$layer}}.="$string ";
                  } else {
                     $LAYER{"OTHER"}.="$string ";
                  }
               } elsif ($line=~s/^XY\s+//) {
                  $line=~s/\s+$//;
                  $line=~s/\s+/\,/g;
                  $position=$line;
               }
            } elsif (($element eq "BOUNDARY") && ($layer==$layer_boundary) 
               && ($line=~s/^XY\s+//g)) {
               @X=@Y=();
               if (exists $BOUNDARY{$strname}) {
                 WarnMsg("Boundary Layer of '$strname' is redefined.\n");
                 @xy=split(/\s+/,$BOUNDARY{$strname});
                 @X=($xy[0],$xy[2]);
                 @Y=($xy[1],$xy[3]);
               }
               while($line) {
                 ($x,$y,$line)=split(/\s+/,$line,3);
                 push(@X,$x);
                 push(@Y,$y);
               }
               if (@X!=5) {
                 WarnMsg("Boundary Layer of '$strname' is not a rectangle.\n");
               }
               @X=sort(@X); @Y=sort(@Y);
               $xm=shift(@X); $ym=shift(@Y);
               $xM=pop(@X); $yM=pop(@Y);
               $SIZE{$strname}=sprintf("%.3f %.3f",($xM-$xm)*$unit,($yM-$ym)*$unit);
               $BOUNDARY{$strname}="$xm $ym $xM $yM";
            }
          } else {
          }
        }
        close GDT;
    }
    printf STDOUT "\r%60s\r","";
    if (exists $CELNAME{$cell}) {
        $strname=$cell;
        $cellname=$CELNAME{$strname};
        $cellname=~s/\s+$//g;
        @PINS=sort(split(/\s+/,$PINNAME{$strname}));
        @ERRPINS=sort(split(/\s+/,$INTPIN{$strname}));
        @EXTPINS=sort(split(/\s+/,$EXTPIN{$strname}));
        ($WIDTH,$HEIGHT)=split(/\s+/,$SIZE{$strname});
        @RANGE=split(/\s+/,$BOUNDARY{$strname});
        $error=$warning=0;
        read_spi($strname);
        read_pmd($strname);
        print STDOUT "Checking $strname ...\n";
        print OUT ".CELL\t$strname\n";
	if ($SNAME{$strname} ne "") {
	   for $sname(split(/\s+/,$SNAME{$strname})) {
             print OUT ".SNAM\t$sname\n";
	   }
	}
        print OUT ".UNIT\t$UNIT{$strname}\n";
        if ($cellname !~ /./) {
           WarnMsg("Cell label is missing in GDS.");
        } elsif ($cellname ne $strname) {
           print OUT ".LABEL\t$cellname\n";
           ErrorMsg("Cell label '$cellname' does not match with Structure Name.");
        }
        if ($WIDTH==0 || $HEIGHT==0) {
           ErrorMsg("Missing cell boundary.");
        } else {
           print OUT ".SIZE\t$WIDTH\t$HEIGHT\t(@RANGE)\n";
        } 
        %PMDMASK=%SPIMASK=();
        for $pin(@PINS) { 
            $layers=compress(join(" ",grep{s/\@.*$//;}split(/\s+/,$TEXT{$pin})));
            print OUT ".PIN\t$pin\t($layers)\n";
            if ($pin ne uc($pin)) {
               ErrorMsg("pinname '$pin' should be UPPER CASE.");
            }
            for $item(split(/\s+/,$TEXT{$pin})) {
               ($layer,$position)=split(/\@/,$item);
               @XY=split(/\,+/,$position);
               if (($XY[0]<$RANGE[0])||($XY[1]<$RANGE[1])||
                   ($XY[0]>$RANGE[2])||($XY[1]>$RANGE[3])) {
                  ErrorMsg("pin '$pin' ($item) is out of range.");
               }
            }
            $PMDMASK{uc($pin)}=1;
            $SPIMASK{uc($pin)}=1;
        }
        if (!exists $PMDMASK{"VDD"}) {
           ErrorMsg("'VDD' is missing.");
        }
        delete($PMDMASK{"VDD"});
        delete($SPIMASK{"VDD"});
        if (!exists $PMDMASK{"VSS"}) {
           ErrorMsg("'VSS' is missing.");
        }
        delete($PMDMASK{"VSS"});
        delete($SPIMASK{"VSS"});
        if ($spifile ne "") {
           print OUT ".SPI\t".join(" ",@SPIPIN)."\n";
           for $pin(@SPIPIN) {
               if ($pin ne uc($pin)) {
                  ErrorMsg("pin '$pin' in [spi] should be UPPER CASE.");
               }
               $pin=uc($pin);
               if (($pin eq "VDD")||($pin eq "VSS")) {
                  WarnMsg("pin '$pin' in [spi] should be defined as global.");
               } elsif (!exists $SPIMASK{$pin}) {
                  ErrorMsg("pin '$pin' is in [spi] but not found in GDS.");
               } else {
                 delete($SPIMASK{$pin});
               }
           }
           for $pin(keys(%SPIMASK)) {
               ErrorMsg("pin '$pin' is not found in [spi].");
           }
        }
        if ($pmdfile ne "") {
           if ($sizeflag && (($WIDTH != $width_pmd)||($HEIGHT != $height_pmd))) {
              ErrorMsg("SIZE does not match with [pmd]($width_pmd,$height_pmd).");
           }
           print OUT ".PMD\t".join(" ",@PMDPIN)."\n";
           for $pin(@PMDPIN) {
               if ($pin ne uc($pin)) {
                  ErrorMsg("pin '$pin' in [pmd] should be UPPER CASE.");
               }
               $pin=uc($pin);
               if (!exists $PMDMASK{$pin}) {
                  ErrorMsg("pin '$pin' is in [pmd] but not found in GDS.");
               } else {
                 delete($PMDMASK{$pin});
               }
           }
           for $pin(keys(%PMDMASK)) {
               ErrorMsg("pin '$pin' is not found in [pmd].");
           }
        }
        if (@ERRPINS) {
           ErrorMsg("pins exist in metal layers.");
           for $pin(@ERRPINS) {
              $texts=" ";
              for $item(split(/\s+/,$TEXT{$pin})) {
                  ($layer,$pos)=split(/\@/,$item,2);
                  if ($TEXT_LAYER{$layer}=~/^METAL/) {
                     $texts.=$item." ";
                  }
              }
              print OUT ".ERR\t$pin\t($texts)\n";
           }
        }
        if (@EXTPINS) {
           WarnMsg("extra internal pins exist (in unknown layers).");
           for $pin(@EXTPINS) {
              $texts=" ";
              for $item(split(/\s+/,$TEXT{$pin})) {
                  ($layer,$pos)=split(/\@/,$item,2);
                  if (!exists $TEXT_LAYER{$layer}) {
                     $texts.=$item." ";
                  }
              }
              print OUT ".INTT\t$pin\t($texts)\n";
           }
        }
        print OUT ".ENDC\n";
        if ($warning) { Message("  ** $warning WARNINGs found.\n");}
        if ($error) { Message("  ** $error ERRORs found.\n");}
        $total_error+=$error;
        $total_warn+=$warning;
    }
}
$cell_num=@STRUCTURE;
Message("===================================================\n");
Message("Total $cell_num cells were checked.\n");
Message("Total $total_warn WARNINGs were found.\n");
Message("Total $total_error ERRORs were found.\n");
Message("===================================================\n");
close OUT;
return 0;

sub read_pmd {
my ($cell)=@_;
   @PMDPIN=();
   $pmdfile="";
   return 0 if (@pmddirs==0);
   $cell=lc($cell);
   for $pmddir(@pmddirs) {
      $pmdfile="$pmddir/$cell.pmd";
      open(PMD,$pmdfile)|| next;
      Message("Reading '$pmdfile'\n");
      while($line=<PMD>) {
        while($line=~s/\-\n$//g) { $line.=<PMD>;}
        $line=~s/^\s+//g;
        next if ($line=~/^#/);
        if ($line=~/^Pins\s*\:/) {
           $flag=1;
        } elsif ($line=~/^TDparams\s*\:/) {
           $flag=2;
        } elsif ($line=~/^(Timing|Capacitances)\s*\:/) {
           $flag=0;
        } elsif ($flag==1) {
           ($pin,$rest)=split(/(\s|\()/,$line,2);
           push(@PMDPIN,$pin);
        } elsif ($flag==2) {
           if ($line=~/Width\s+(.*)$/) { $width_pmd=$1;}
           elsif ($line=~/Height\s+(.*)$/) { $height_pmd=$1;}
        }
      }
      close PMD;
      return 1;
   }
   $pmdfile="";
   ErrorMsg("Can not open pmd file '$cell.pmd'!");
   return 0;
}

sub read_spi {
my ($cell)=@_;
   @SPIPIN=();
   $spifile="";
   return 0 if (@spidirs==0);
   $cell=lc($cell);
   for $spidir(@spidirs) {
      $spifile="$spidir/$cell.spi";
      open(SPI,$spifile)|| next;
      Message("Reading '$spifile'\n");
      $spiname="";
      $lastline="";
      while(($line=<SPI>)||($lastline ne "")) {
        chop($line);
        $line=~s/^\s+//g;
        if ($line=~s/^\+//) { 
           $lastline.=$line;
           next;
	}
        if ($lastline=~/^\*/) {
        } elsif ($lastline=~/^.SUBCKT\s+($cell)\s+(.*?)$/i) {
           $spiname=$1;
           @SPIPIN=split(/\s+/,$2);
           if ($spiname ne uc($spiname)) {
              WarnMsg("Spice cell name '$spiname' should be UPPER CASE.");
           }
        }
        $lastline = $line;
      }
      close SPI;
      if ($spiname eq "") {
         ErrorMsg("Spice subckt '".uc($cell)."' not found.");
      }
      return 1;
   }
   $spifile="";
   ErrorMsg("Can not open spice file '$cell.spi'!");
   return 0;
}

sub compress {
my ($str)=@_;
my (@pins,@out,$pin,$pre);
    @pins=sort(split(/\s+/,$str));
    $pre="";
    for $pin(@pins) {
        if ($pre ne $pin) {
           push(@out,$pin);
           $pre=$pin;
        }
    }
    return join(" ",@out);
}


sub ErrorMsg {
   my($msg)=@_;
   Message("  *** ERROR *** $msg\n");
   $error++;
}

sub WarnMsg {
   my($msg)=@_;
   Message("  ** WARNING ** $msg\n");
   $warning++;
}

sub Message {
   my($msg)=@_;
   print OUT $msg;
   print STDOUT $msg;
}
