#ifndef UTIL_H
#define UTIL_H

#include "htslib/sam.h"

long getUs(struct timeval t);
long getTimeDiffInUs(struct timeval t_start, struct timeval t_end);

void increase_buffer_size(bam1_t ***buffer, int *max_size, int cur_size);
void write_buffer(samFile *sf, bam1_t **buf, int size, bam_hdr_t *h);

int single_file_test(char* input_file, char* output_file,
    int nof_thread, int compress_level);
int sort_routine_test(char* input_file, char* output_file,
    int nof_thread, int compress_level, char* tmp_prefix);
#endif
