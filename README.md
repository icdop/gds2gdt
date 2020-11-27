# Generic GDS UTILITY
- The utility is developed by Albert Li since 1996.

## Purpose:
  The main purpose of this Generic GDS utility is to convert binary GDS-II file 
to a readable text format (GDT), so user can validate the design content,
modify or manipulate the content using simple script languate such as tcl or perl 
without decoding the GDS-II binary format.

## How to build the executable

### 1. Compile Utility Library
  * libutil - general utility routine, such hash, list, link ....
  * libgds  - GDS-II reader/writter

	Example:
	
		% cd libsrc/
		% make clean
		% make update


### 2. Build Main Program
  * gds2gdt - convert GDS (binary format) to GDT (text format)
  * gdt2gdx - convert GDT (text format) to GDX (binary format)
  * gds2gdx - convert GDS (binary format) to GDX (binary format)

	Example:

		% cd main/	
		% make clean
		% make publish


### 3. Run Test Prgoram
  * TEST1 - check GDS2GDT+GDT2GDX  (muxdelay.gds <=> muxdelay.gdx)
  * TEST2 - check GDS2GDT (muxdelay.gdt <=> output.gdt)
  * TEST3 - check GDS2GDX (muxdelay.gds <=> output.gdx)
  
	Example:

		% cd run/	
		% make TEST1

