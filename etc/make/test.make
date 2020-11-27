#
#
# <parameters in Makefile>  
#  TEST.title  = description of the testcase
#  TEST.target = <target data file>
#  TEST.golden = <golden data file>
#  TEST.check  = <compare script>
#
# <golden file>
#  golden/TEST.golden : golden output file
#  golden/TEST.check  : golden TEST.check result (optional) 
#
# <output file>
#  TEST/run.log    : output log file at executing the test command
#
#  TEST/check.log  : output of comparing run data with golden data
#  TEST/diff.log   : comparison result of TEST.check and TEST.waive
#  TEST/.status : 
#        "PASS" if TEST/diff.log is T
#        "FAIL" if TEST/diff.log is not empty

# <result>
#  TEST.PASS   : 
#  TEST.FAIL   :
#
TEST_TSTAMP = `date +"%F %T"`

$(TEST_SUITE):
	@rm -fr $@ @.FAIL @.PASS
	@mkdir -p $@
	@echo "##########################################################"
	@echo "$@.title  : $($@.title)"
	@echo "$@.start  : $(TEST_TSTAMP)"
	@#
	@echo "$@.target : make $($@.target)"
	@echo -n "$(TEST_TSTAMP) RUN " >> $@/.status
	@echo `make $($@.target) 2>&1 > $@/01_target.log` >> $@/.status
	@#
	@echo -n "$(TEST_TSTAMP) CHECK " >> $@/.status
	@if (test "$($@.check)" != "") then \
	  echo "$@.check  : $($@.check)"; \
	  echo `$($@.check) 2>&1 > $@/02_check.log` >> $@/.status ; \
	elif (test "$($@.golden)" != "") then \
	  echo "$@.check  : cmp -b $($@.target) $($@.golden)"; \
	  echo `cmp -b $($@.target) $($@.golden) 2>&1  > $@/02_check.log ` >> $@/.status ; \
	elif (test -s $($@.target)) then \
	  echo "$@.check  : test -s $($@.target)"; \
	  echo `test -s $($@.target) 2>&1  > $@/02_check.log ` >> $@/.status ; \
	else \
	  echo "ERROR: '$($@.target)' does not exist." > $@/02_check.log ; \
	  echo "ERROR: '$($@.target)' does not exist." >> $@/.status ; \
	fi
	@#
	@echo -n "$(TEST_TSTAMP) DIFF" >> $@/.status
	@if (test -s $@.waive) then \
	   echo `diff $@/02_check.log $@.waive > $@/03_diff.log` >> $@/.status ;\
	else \
	   touch $@/no_waive; \
	   echo `diff $@/02_check.log $@/no_waive > $@/03_diff.log` >> $@/.status ;\
	fi
	@#
	@if (test -s $@/03_diff.log) then \
	   TEST_RESULT=FAIL; \
	else \
	   TEST_RESULT=PASS; \
	fi ; \
	echo "$@.finish : $(TEST_TSTAMP)" ; \
	echo "$(TEST_TSTAMP) $$TEST_RESULT" >> $@/.status ; \
	echo "$@.result : $$TEST_RESULT"; \
	touch $@.$$TEST_RESULT

clean_test_result:
	@for test in $(TEST_SUITE); do \
	  rm -fr $$test/ $$test.FAIL $$test.PASS ; \
	  echo "rm -fr $$test/ $$test.FAIL $$test.PASS" ; \
	done;
