#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>

#include "util.h"


int main(int argc, char *argv[])
{
  int c;
  int pattern = 0, nof_thread = 1, compress_level = 5;
  char *input_file = NULL, *output_file = NULL;
  char *tmp_prefix = NULL;
  while ((c = getopt(argc, argv, "i:p:n:l:i:o:T:")) >= 0) {
    switch (c) {
      case 'p': pattern = atoi(optarg); break;
      case 'n': nof_thread = atoi(optarg); break;
      case 'l': compress_level = atoi(optarg); break;
      case 'i': input_file = optarg; break;
      case 'o': output_file = optarg; break;
      case 'T': tmp_prefix = optarg; break;
    }
  }

  fprintf(stdout, "pattern: %d\n", pattern);
  fprintf(stdout, "nof_thread: %d\n", nof_thread);
  fprintf(stdout, "compress_level: %d\n", compress_level);

  switch(pattern) {
    case 0:
      single_file_test(input_file, output_file, nof_thread, compress_level);
      break;
    case 1:
      if (tmp_prefix == NULL) {
        //fprintf(stdout, "Needs to specify tmp_prefix via -T\n");
        char tmp[4];
        strcpy(tmp, "tmp");
        tmp_prefix = tmp;
      }
      sort_routine_test(input_file, output_file, nof_thread, compress_level,
          tmp_prefix);
      break;
  }
    
  return 0;
}
