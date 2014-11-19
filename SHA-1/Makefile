CFLAGS = -Wall
LIBS = 
PROG = sha1

SRCS = main.c sha1.c
OBJ_FILES = 

all: gcc

gcc:	$(SRCS)
	gcc $(CFLAGS) -o $(PROG) $(SRCS) $(OBJ_FILES) $(LIBS)

clang:	$(SRCS)
	clang $(CFLAGS) -o $(PROG) $(SRCS) $(OBJ_FILES) $(LIBS)

clean:
	rm -f $(PROG)
