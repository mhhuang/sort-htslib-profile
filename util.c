#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "util.h"

long getUs(struct timeval t) {
  return t.tv_usec + t.tv_sec * 1000000;
}

long getTimeDiffInUs(struct timeval t_start, struct timeval t_end) {
  return getUs(t_end) - getUs(t_start);
}

void increase_buffer_size(bam1_t ***buffer, int *max_size, int cur_size) {
  //fprintf(stdout, "max_size: %d\n", *max_size);
  //struct timeval t_start, t_end;
  //gettimeofday(&t_start, NULL);

  int old_size = cur_size;
  *max_size = *max_size ? *max_size << 1 : 0x10000;
  *buffer = (bam1_t**)realloc(*buffer, *max_size * sizeof(bam1_t*));
  int i;
  for(i = old_size; i < *max_size; ++i) (*buffer)[i] = NULL;

  //gettimeofday(&t_end, NULL);
  //long elapsed = getTimeDiffInUs(t_start, t_end);
  //fprintf(stdout, "realloc takes %ld us\n", elapsed);
}

void write_buffer(samFile *sf, bam1_t **buf, int size, bam_hdr_t *h) {
  struct timeval t_start, t_end;
  gettimeofday(&t_start, NULL);

  int i;
  for(i = 0; i < size; ++i) {
    int ret = sam_write1(sf, h, buf[i]);
    if (ret < 0) {
      fprintf(stderr, "ERROR: sam_write1()\n");
    }
  }

  gettimeofday(&t_end, NULL);
  fprintf(stdout,"write: time for the last %d records, %ld us\n",
      getTimeDiffInUs(t_start, t_end), size);
  fflush(stdout);
}

