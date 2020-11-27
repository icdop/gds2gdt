.KEEP_STATE:

PACKAGES = libutil libgds

all: publish

publish package compile clean:
	@for dir in $(PACKAGES); do \
	(cd $$dir; echo "*** Entering [$$dir]..."; make $@); \
	done;

$(PACKAGES):
	cd $@; make update;
