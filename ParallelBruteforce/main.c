/* 
 * File:   main.c
 * Author: TBD
 *
 * Created on 13. Januar 2015, 11:10
 */

#include <mpi.h>

#include "core_headers.h"

#if 1
void readlines(MPI_File *in, const int rank, const int size,
               char ***lines, int *nlines) {
    MPI_Offset filesize;
    MPI_Offset localsize;
    MPI_Offset start;
    MPI_Offset end;
    char *chunk;

    /* figure out who reads what */

    MPI_File_get_size(*in, &filesize);
    printf("Filesize: %ld\n",filesize);
    localsize = filesize;
    start = 0;
    end   = localsize - 1;

    /* add overlap to the end of everyone's chunk... */
/*
    end += overlap;
*/

    

    /* allocate memory */
    chunk = (char*)malloc( (localsize + 1)*sizeof(char));
    MPI_File_read(*in, chunk, localsize, MPI_CHAR, MPI_STATUS_IGNORE);
    printf("Start: %ld, Size: %ld\n", start, localsize);
    chunk[localsize] = '\0';

    /*
     * everyone calculate what their start and end *really* are by going 
     * from the first newline after start to the first newline after the
     * overlap region starts (eg, after end - overlap + 1)
     */

    int locstart=0, locend=localsize;
   
    localsize = locend-locstart+1;

    /* Now let's copy our actual data over into a new array, with no overlaps */
    char *data = (char *)malloc((localsize+1)*sizeof(char));
    memcpy(data, &(chunk[locstart]), localsize);
    data[localsize] = '\0';
    free(chunk);

    /* Now we'll count the number of lines */
    *nlines = 0;
    for (int i=0; i<localsize; i++)
        if (data[i] == '\n') (*nlines)++;

    /* Now the array lines will point into the data array at the start of each line */
    /* assuming nlines > 1 */
    *lines = (char **)malloc((*nlines)*sizeof(char *));
    (*lines)[0] = strtok(data,"\r\n");
    for (int i=1; i<(*nlines); i++){
        char* line= strtok(NULL, "\r\n");
        (*lines)[i] = line;
    }

    return;
}

void processlines(char **lines, const int nlines, const int rank) {
    for (int i=0; i<nlines; i++) {
        printf("[%d] %s\n",rank, lines[i]);
    }
}

int main(int argc, char** argv) {
    int maxNum;
    int nTasks, rank;
    MPI_Init( NULL, NULL );
    MPI_Comm_size( MPI_COMM_WORLD, &nTasks );
    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_File fh;
    int ret = MPI_File_open(MPI_COMM_WORLD, "testHashes.txt", MPI_MODE_RDONLY, MPI_INFO_NULL,&fh);if(ret < 0) {
                    printf("MPI_File_open failed");
                    exit(1);
                }
    if (rank == 0)
	{
		// The master thread will need to receive all computations from all other threads.
		MPI_Status status;

		// MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source, int tag, MPI_Comm comm, MPI_Status *status)
		// We need to go and receive the data from all other threads.
		// The arbitrary tag we choose is 1, for now.
                
                
                
                MPI_Bcast(&fh,1,, 0,MPI_COMM_WORLD);
                
	}
	else
	{
		// We are finished with the results in this thread, and need to send the data to thread 1.
		// MPI_Send(void *buf, int count, MPI_Datatype datatype, int dest, int tag, MPI_Comm comm)
		// The destination is thread 0, and the arbitrary tag we choose for now is 1.
                MPI_File fh;
                
                int ret = MPI_File_open(MPI_COMM_WORLD, "testHashes.txt", MPI_MODE_RDONLY, MPI_INFO_NULL,&fh);
                if(ret < 0) {
                    printf("MPI_File_open failed");
                    exit(1);
                }
                printf("huhu\n");
                const int overlap=100;
                char **lines;
                int nlines;
                readlines(&fh, rank, nTasks, &lines, &nlines);
                printf("huhu2\n");
                processlines(lines, nlines, rank);
                printf("huhu3\n");
		//int MPI_File_read(fh, void *buf,int count, MPI_CHAR, MPI_Status *status);
	}

	MPI_Finalize();
}
#endif

#if 0
int main(int argc, char** argv) {
    HashAlgorithm *hashAlgo = createHashAlgorithm("SHA256");
    uchar hash[SHA256_SIZE];
    
    /*oder auch:
    HashAlgorithm *hashAlgo = createHashAlgorithm("SHA1");
    uchar hash[SHA1_SIZE];*/
    
   //getHashFromFile(hashAlgo, "main.c", &hash);
    getHashFromString(hashAlgo,"Robin", &hash);
    hashAlgo->print(hash);
    printf("\n");
    
    getHashFromString(hashAlgo,"Julian", &hash);
    hashAlgo->print(hash);
    printf("\n");
    
    getHashFromString(hashAlgo,"Nils", &hash);
    hashAlgo->print(hash);
    printf("\n");
  
    freeHash(hashAlgo);
    
    return 0;
}


/* ACTION */

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &pool_size);
  last_guy = pool_size - 1;
  if (my_rank == MASTER_RANK) i_am_the_master = TRUE;

  if (i_am_the_master) {

    /* read the command line */

    while ((c = getopt(argc, argv, "f:h")) != EOF)
      switch(c) {
      case 'f': 
	filename = optarg;
#ifdef DEBUG
	printf("input file: %s\n", filename);
#endif
	break;
      case 'h':
	SYNOPSIS;
	input_error = TRUE;
	break;
      case '?':
	SYNOPSIS;
	input_error = TRUE;
	break;
      } /* end of switch(c) */

    /* Check if the command line has initialized filename and
     * number_of_blocks.
     */

    if (filename == NULL) {
      SYNOPSIS;
      input_error = TRUE;
    }

    if (input_error) MPI_Abort(MPI_COMM_WORLD, 1);

    filename_length = strlen(filename) + 1;

    /* This is another way of exiting, but it can be done only
       if no files have been opened yet. */

  } /* end of "if (i_am_the_master)"; reading the command line */

    /* If we got this far, the data read from the command line
       should be OK. */
  
  MPI_Bcast(&filename_length, 1, MPI_INT, MASTER_RANK, MPI_COMM_WORLD);
  if (! i_am_the_master) filename = (char*) malloc(filename_length);
#ifdef DEBUG
  printf("%3d: allocated space for filename\n", my_rank);
#endif
  MPI_Bcast(filename, filename_length, MPI_CHAR, MASTER_RANK, MPI_COMM_WORLD);
#ifdef DEBUG
  printf("%3d: received broadcast\n", my_rank);
  printf("%3d: filename = %s\n", my_rank, filename);
#endif

  MPI_Barrier(MPI_COMM_WORLD);

  /* Default I/O error handling is MPI_ERRORS_RETURN */

  file_open_error = MPI_File_open(MPI_COMM_WORLD, filename, 
		                  MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);

  if (file_open_error != MPI_SUCCESS) {

    char error_string[BUFSIZ];
    int length_of_error_string, error_class;

    MPI_Error_class(file_open_error, &error_class);
    MPI_Error_string(error_class, error_string, &length_of_error_string);
    printf("%3d: %s\n", my_rank, error_string);

    MPI_Error_string(file_open_error, error_string, &length_of_error_string);
    printf("%3d: %s\n", my_rank, error_string);

    MPI_Abort(MPI_COMM_WORLD, file_open_error);
  }

  MPI_File_get_size(fh, &total_number_of_bytes);
#ifdef DEBUG
  printf("%3d: total_number_of_bytes = %lld\n", my_rank, total_number_of_bytes);
#endif

  number_of_bytes_ll = total_number_of_bytes / pool_size;

  /* If pool_size does not divide total_number_of_bytes evenly,
     the last process will have to read more data, i.e., to the
     end of the file. */

  max_number_of_bytes_ll = 
    number_of_bytes_ll + total_number_of_bytes % pool_size;

  if (max_number_of_bytes_ll < INT_MAX) {

    if (my_rank == last_guy)
      number_of_bytes = (int) max_number_of_bytes_ll;
    else
      number_of_bytes = (int) number_of_bytes_ll;

    read_buffer = (char*) malloc(number_of_bytes);
#ifdef DEBUG
    printf("%3d: allocated %d bytes\n", my_rank, number_of_bytes);
#endif

    my_offset = (MPI_Offset) my_rank * number_of_bytes_ll;
#ifdef DEBUG
    printf("%3d: my offset = %lld\n", my_rank, my_offset);
#endif
    MPI_File_seek(fh, my_offset, MPI_SEEK_SET);

    MPI_Barrier(MPI_COMM_WORLD);

    start = MPI_Wtime();
    MPI_File_read(fh, read_buffer, number_of_bytes, MPI_BYTE, &status);
    finish = MPI_Wtime();
    MPI_Get_count(&status, MPI_BYTE, &count);
#ifdef DEBUG
    printf("%3d: read %d bytes\n", my_rank, count);
#endif
    MPI_File_get_position(fh, &my_offset);
#ifdef DEBUG
    printf("%3d: my offset = %lld\n", my_rank, my_offset);
#endif

    io_time = finish - start;
    MPI_Allreduce(&io_time, &longest_io_time, 1, MPI_DOUBLE, MPI_MAX,
		  MPI_COMM_WORLD);
    if (i_am_the_master) {
      printf("longest_io_time       = %f seconds\n", longest_io_time);
      printf("total_number_of_bytes = %lld\n", total_number_of_bytes);
      printf("transfer rate         = %f MB/s\n", 
	     total_number_of_bytes / longest_io_time / MBYTE);
    }
  }
  else {
    if (i_am_the_master) {
      printf("Not enough memory to read the file.\n");
      printf("Consider running on more nodes.\n");
    }
  } /* of if(max_number_of_bytes_ll < INT_MAX) */

  MPI_File_close(&fh);

  MPI_Finalize();
#endif
