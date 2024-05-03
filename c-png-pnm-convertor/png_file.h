#ifndef BACKLOG_ITMO_C_PNG_DANDOBRI_MAIN_PNG_FILE_H
#define BACKLOG_ITMO_C_PNG_DANDOBRI_MAIN_PNG_FILE_H
#include "return_codes.h"
#include "write_to_file.h"
#include "decode.h"

#include <stdio.h>
#include <string.h>
#include <malloc.h>
typedef struct
{
	size_t size;
	int count_chunk;
	unsigned char *chunk;
} Chunk;
int reading_writing_png(FILE *read_file, char *back, char *write_file, int *empty);
void clear(Chunk *list);
int png_check(FILE *read_file);
int ihdr_check(unsigned int *width, unsigned int *height, int *type_color, const unsigned char *ihdr);
int idat_chunk_check(FILE *read_file, unsigned char *buffer, size_t *size_idat_chunk, const int *size);
int plte_check(FILE *read_file, unsigned char *plte_buf, const size_t *size_plte_chunk, int *res_file_format);
int decoder(unsigned char *buf, unsigned char *res, const unsigned int *width, const unsigned int *height, const int *byte);
int comand_line_arg_check(unsigned char *bkgd_chuck, char *back, const int *type_color);
int bkgd_check(unsigned char *bkgd, int *type_color, const int *empty, char *back);
int alpha(unsigned char *buf, const unsigned char *bkgd, unsigned char *res, const size_t *buf_size, const int *type_color);
#endif
