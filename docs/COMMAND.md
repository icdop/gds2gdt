# GDS UTILITY COMMAND USAGE

##              Layout GDS Reader (GDS2GDT) Version: V2005r2               ##

Usage:	gds2gdt  [options] <gdsfile:cell> ...

	-logfile	<log file name>
	-destination	<output destination directory>
	-extension      <output file extension
	-outputfile     <output gdt file name>
	-width          <width>
	-filter         <filter file>

	+verbose	display detail message
	+line		include line number of each record
	+length		include length of each record
	+no_header	do not include header information
	+hierarchy	include hierarchical structure
	+stdout		write GDT to standard output
	+stdin		read GDS from standard input


##               Layout GDS Writer (GDT2GDX) Version: V2005r2              ##

Usage:	gdt2gdx  [options] <gdtfile:cell> ...

	-logfile	 <log file name>
	-destination    <output destination directory>
	-extension      <output file extension
	-outputfile     <output gdx file name>

	+verbose	display detail message
	+stdout		write GDS to standard output
	+stdin          read GDT from standard input


##                 Layout GDS pinname Verification V9710                   ##

Usage:	checkgds.pl [options] <gdsfile> ...
  
	+help		display this message.
	+rule		display gds pinname checking rules.
	+msg		display error/message description and output format

	+size		report cell size in gds but do not compare with pmd.
	+gdt		input files are gdt text files.
	+pad		force all gds cells as pad I/O cells

	-l <logfile>	log file name, default is 'checkgds.log'.
	-t <gdtdir>	gdt destination directory, default is <GDT>.
	-p <pmddir>	pmd directory, if not specify do not check pmd
	-s <spidir>	spice directory, if not specify do not check spice

  [arguments]
  
	<gdsfile>	can be gdsfile name or directory name

  Example:
  
	% checkgds.pl inv0.gds
	
	% checkgds.pl -p pmddir gdsdir/	
	
	% checkgds.pl -p pmddir1 -p pmddir2 gdsdir/*.gds
	
	% checkgds.pl -p pmddir -s spidir gdsdir/

=============================================================================
### GDS PIN NAME CHECKING RULES:

   1. GDS should be flattened or pin text are all on the top structure,
      Only top structure will be checked.

   2. Texts are extracted from GDS:
      * Texts in Layer 62(TEXT) are recognized as Cell Label.
      * Texts in Layer 4x(TEXT) are recognized as pinnames.
         - 40(TEXT1), 41(TEXT2), 42(TEXT3) ,43(TEXT4) and 44(TEXT5) 
      * Texts in Layer 16(METAL) will be reported as ERROR.
         - 16(METAL1), 18(METAL2), 28(METAL2), 31(METAL2), 33(METAL2)
      * Texts in other Layers are considered as Internal pins (WARNING)

   3. Pin names and Cell Label should be UPPER CASE.

   4. Cell Label should be the same as GDS structure name and cell name.

   5. Each cell should contains default 'VDD' and 'VSS' pins.

   6. Each Pin name should has a corresponding pin in [pmd]/[spi] file.

   7. Cell Width and Height is extracted from cell boundary (Layer 62)
      and will be checked with pmd.

=============================================================================
