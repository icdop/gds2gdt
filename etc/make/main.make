main: $(PROG_NAME)

publish: $(MAIN_BIN)/$(PROG_NAME)

compile: $(PROG_OBJS)

$(MAIN_BIN)/$(PROG_NAME): $(PROG_NAME)
	$(CP) $(PROG_NAME) $(MAIN_BIN)/$(PROG_NAME)
	
$(PROG_NAME): $(PROG_NAME).c $(PROG_INCS) $(PROG_OBJS)
	$(CC) $(CFLAGS) $(PROG_NAME).c $(PROG_OBJS) $(EXTR_LIBS) -o $(PROG_NAME)

.c.o:
	$(CC) $(CFLAGS) $<

clean:
	rm -f $(PROG_NAME) core *.o
