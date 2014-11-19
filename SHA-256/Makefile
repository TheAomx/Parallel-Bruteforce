CFLAGS = -Wall
LIBS = 
PROG = sha256

SRCS = sha256_example.c sha256.c
OBJ_FILES = 

all: gcc

gcc:	$(SRCS)
	gcc $(CFLAGS) -o $(PROG) $(SRCS) $(OBJ_FILES) $(LIBS)

clang:	$(SRCS)
	clang $(CFLAGS) -o $(PROG) $(SRCS) $(OBJ_FILES) $(LIBS)

clean:
	rm -f $(PROG)
