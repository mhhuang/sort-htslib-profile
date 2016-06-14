HTSLIB_PATH=htslib-1.3.1
CC= gcc
CFLAGS=		-g -Wall -Wno-unused-function -O2
DFLAGS=

SRCDIR= ./
OBJDIR= ./obj
ALLOBJ= $(OBJDIR)/util.o $(OBJDIR)/single_file_test.o $(OBJDIR)/sort_routine_test.o $(OBJDIR)/main.o
PROG= profile
LIBS= 

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) -c $(CFLAGS) $(DFLAGS) -I$(HTSLIB_PATH) -I. $< -o $@ 

all:$(PROG)

profile: $(ALLOBJ) 
	$(CC) -pthread $(CFLAGS) $(DFLAGS) $(ALLOBJ) -o $@ -L$(HTSLIB_PATH) -lhts $(LIBS) -lrt

clean:
	rm -f $(PROG) ${OBJDIR}/*.o
