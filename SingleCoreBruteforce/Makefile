CFLAGS = -Wall -fopenmp -Ofast -std=c99
LIBS = 
PROG = sha1

SRCS = main.c sha1.c md5.c sph_md5.c sha256.c sha2.c sha1_prop.c hashing_algo.c password_algo.c utils.c bruteforce.c
OBJ_FILES = 

all: gcc

gcc:	$(SRCS)
	gcc $(CFLAGS) -o $(PROG) $(SRCS) $(OBJ_FILES) $(LIBS)

clang:	$(SRCS)
	clang $(CFLAGS) -o $(PROG) $(SRCS) $(OBJ_FILES) $(LIBS)

clean:
	rm -f $(PROG)
