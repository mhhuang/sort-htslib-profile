#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <unistd.h>

#include "htslib/sam.h"

#include "util.h"

int single_file_test(char* input_file, char* output_file,
    int nof_thread, int compress_level) {

  if (input_file == NULL) {
    fprintf(stderr, "ERROR: Needs to specify input files\n");
    return 1;
  }
  if (output_file == NULL) {
    fprintf(stderr, "ERROR: Needs to specify output files\n");
    return 1;
  }

  // process input file
  samFile *sf = sam_open_format(input_file, "r", NULL);
  if (sf == NULL) {
    //const char *message = strerror(errno);
    //fprintf(stderr, "Error in opening file %s: %s\n", input_file, message);
    fprintf(stderr, "Error in opening file %s\n", input_file);
    return 1;
  }
  bam_hdr_t *header = sam_hdr_read(sf);
  if (header == NULL) {
    fprintf(stderr, "Error in reading header for file %s\n", input_file);
    return 1;
  }

  // output file
  char modeout[12];
  sam_open_mode(modeout+1, output_file, NULL);
  strcpy(modeout, "wb");
  if (compress_level > 0) {
    sprintf(strchr(modeout, '\0'), "%d", compress_level < 9 ? compress_level : 9);
  }
  samFile *sf_out = sam_open_format(output_file, modeout, NULL);
  if (sf_out == NULL) {
    fprintf(stderr, "ERROR in open file %s\n", output_file);
    return 1;
  }
  int ret;
  ret = sam_hdr_write(sf_out, header);
  if (ret < 0) {
    fprintf(stderr, "ERROR in write header for file %s\n", output_file);
    return 1;
  }


  if (nof_thread > 1) hts_set_threads(sf_out, nof_thread);

  bam1_t **buffer = NULL, *record;
  int max_size = 0, cur_size = 0;
  int i;

  // start reading sam/bam records

  struct timeval t_start, t_end;
  gettimeofday(&t_start, NULL);
  for(i = 0;;) {
    cur_size = i;
    if (cur_size == max_size) {
      increase_buffer_size(&buffer, &max_size, cur_size);
    }
    if (buffer[i] == NULL) buffer[i] = bam_init1();
    record = buffer[i];
  
    ret = sam_read1(sf, header, record);
    if (ret < 0) break; // read error or end of file

    ++i;
    if (i < 1000000) continue; // continue to read

    gettimeofday(&t_end, NULL);
    fprintf(stdout, "read:  time for the last 1000000 records, %ld us\n",
        getTimeDiffInUs(t_start, t_end));
    fflush(stdout);

    // start write buffer
    write_buffer(sf_out, buffer, i, header);

    // reset
    i = 0;
    gettimeofday(&t_start, NULL);
  }
  if (ret != -1) {
      fprintf(stderr, "Error: truncated file. Aborting...\n");
      goto err;
  }

err:
  for(i = 0; i < max_size; ++i) {
    bam_destroy1(buffer[i]);
  }
  //free(buffer);
  bam_hdr_destroy(header);
  sam_close(sf);
  sam_close(sf_out);


  // TODO(mhhuang) reading in bam again to test read throughput
  // process input file
  sf = sam_open_format(output_file, "r", NULL);
  if (sf == NULL) {
    fprintf(stderr, "Error in opening file %s\n", output_file);
    return 1;
  }
  header = sam_hdr_read(sf);
  if (header == NULL) {
    fprintf(stderr, "Error in reading header for file %s\n", input_file);
    return 1;
  }

  if (nof_thread > 1) hts_set_threads(sf, nof_thread);

  //bam1_t **buffer = NULL, *record;
  //int max_size = 0, cur_size = 0;
  //int i;

  // start reading bam records
  //struct timeval t_start, t_end;
  max_size = 0;
  cur_size = 0;
  gettimeofday(&t_start, NULL);
  for(i = 0;;) {
    cur_size = i;
    if (cur_size == max_size) {
      increase_buffer_size(&buffer, &max_size, cur_size);
    }
    if (buffer[i] == NULL) buffer[i] = bam_init1();
    record = buffer[i];
  
    ret = sam_read1(sf, header, record);
    if (ret < 0) break; // read error or end of file

    ++i;
    if (i < 1000000) continue; // continue to read

    gettimeofday(&t_end, NULL);
    fprintf(stdout, "readbam:  time for the last 1000000 records, %ld us\n",
        getTimeDiffInUs(t_start, t_end));
    fflush(stdout);

    // start write buffer
    //write_buffer(sf_out, buffer, i, header);

    // reset
    i = 0;
    gettimeofday(&t_start, NULL);
  }
  if (ret != -1) {
      fprintf(stderr, "Error: truncated file. Aborting...\n");
      goto err2;
  }

err2:
  for(i = 0; i < max_size; ++i) {
    bam_destroy1(buffer[i]);
  }
  free(buffer);
  bam_hdr_destroy(header);
  sam_close(sf);

  return 1;

}
