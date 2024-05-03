#ifndef BACKLOG_ITMO_C_PNG_DANDOBRI_MAIN_WRITE_TO_FILE_H
#define BACKLOG_ITMO_C_PNG_DANDOBRI_MAIN_WRITE_TO_FILE_H
#include "return_codes.h"

#include <stdio.h>
#include <malloc.h>
int write_to_file(char *write_file, const unsigned int *width, const unsigned int *height, const int *type_color,
                  const int *pointer, const size_t *res_size, const unsigned char *filter_res);
#endif
