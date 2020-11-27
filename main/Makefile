.KEEP_STATE:
PKGNAME  = gdsutil
PKGVERN  = v2017.9

PROG_CMDS = showbin gds2gdt gdt2gdx gds2gdx

main publish clean:
	@for dir in $(PROG_CMDS); do \
	(cd $$dir; echo "*** Entering [$$dir]..."; make $@ ); \
	done;
	
