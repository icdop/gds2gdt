

BINDIR  = ../bin

GDS2GDT = $(BINDIR)/gds2gdt 
GDT2GDX = $(BINDIR)/gdt2gdx
GDS2GDX = $(BINDIR)/gds2gdx

TEST_SUITE = 
TEST_SUITE += UTEST1
TEST_SUITE += TEST1
TEST_SUITE += TEST2
TEST_SUITE += TEST3
#TEST_SUITE += TEST4


all : $(TEST_SUITE)


include ../etc/make/test.make

clean: clean_test_result 
	rm -fr *.log *.gdx TEST* UTEST*

#
# TEST1:
#
# muxdelay.gds => muxdelay.gdt
# muxdelay.gdt => muxdelay.gdx 
#
# compare muxdelay.gdx with muxdelay.gds
# 
TEST1.title  = GDS2GDT/GDT2GDX conversion
TEST1.gds    = muxdelay.gds
TEST1.gdt    = muxdelay.gdt
TEST1.target = muxdelay.gdx
TEST1.golden = muxdelay.gds

$(TEST1.gdt): $(TEST1.gds)
	$(GDS2GDT) +verbose $(TEST1.gds) -logfile $(TEST1.gdt).log -o $(TEST1.gdt)

$(TEST1.target):  $(TEST1.gdt)
	$(GDT2GDX) +verbose $(TEST1.gdt) -logfile $(TEST1.target).log -o $(TEST1.target)

#
# TEST2:
#
# muxdelay.gdx => output.gdt
# compare output.gdt with muxdelay.gdt
#
TEST2.title  = GDT2GDX/GDX2GDT conversion
TEST2.source = muxdelay.gdx
TEST2.target = TEST2.gdt
TEST2.golden = muxdelay.gdt
TEST2.check  = diff $(TEST2.target) $(TEST2.golden)

$(TEST2.target):  $(TEST2.source)
	$(GDS2GDT) +verbose $(TEST2.source) -logfile TEST2.log -o $(TEST2.target) 
	
#
# TEST3:
#
# muxdelay.gds => output.gdx
# compare output.gdx with muxdelay.gds
#
TEST3.title  = GDS2GDX conversion
TEST3.source = muxdelay.gds
TEST3.target = TEST3.gdx
TEST3.golden = muxdelay.gds
TEST3.check  = cmp -l $(TEST3.target) $(TEST3.golden)

$(TEST3.target): $(TEST3.source) 
	$(GDS2GDX) +verbose $(TEST3.source) -logfile TEST3.log -o $(TEST3.target)  

#
# TEST4:
#
# muxdelay.gds => layer.gdt
# compare output.gdx with muxdelay.gds
#
TEST4.title  = GDS2GDT with layer
TEST4.target = layer.gdt
TEST4.golden = muxdelay.gdt
TEST4.check  = diff $(TEST4.target) $(TEST4.golden)

layer.gdt: $(GD2GDT) muxdelay.gds
	$(GDS2GDT) -layer 3 muxdelay.gds -logfile TEST4.log -o layer.gdt
	

#
# UTEST1:
#
# utest1.gdt => utest1.gdx 
# utest1.gdx => utest1.out
#
# compare utest1.gdt with utest1.out
# 
UTEST1.title  = 2019_1103 Support WHITE SPACE IN STRING
UTEST1.source = utest1.gdt
UTEST1.gdx    = utest1.gdx
UTEST1.target = UTEST1.out
UTEST1.golden = utest1.gdt
UTEST1.check  = diff $(UTEST1.target) $(UTEST1.golden)

$(UTEST1.target): $(UTEST1.gdx)
	$(GDS2GDT) +verbose $(UTEST1.gdx) -logfile $(UTEST1.target).log -o $(UTEST1.target)

$(UTEST1.gdx):  $(UTEST1.source)
	$(GDT2GDX) +verbose $(UTEST1.source) -logfile $(UTEST1.gdx).log -o $(UTEST1.gdx)

