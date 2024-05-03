#ifndef BACKLOG_ITMO_C_PNG_DANDOBRI_MAIN_DECODE_H
#define BACKLOG_ITMO_C_PNG_DANDOBRI_MAIN_DECODE_H
#include "return_codes.h"

#include <zlib.h>
int decode(unsigned char *buf, const size_t *buf_size, unsigned long int *decompr_size, unsigned char *res);
int sub_filtration(unsigned char *buf, const size_t *first_el, const size_t *last_el, const int *byte);
int up_filtration(unsigned char *buf, const size_t *first_el, const size_t *last_el);
int average_filtration(unsigned char *buf, const size_t *first_el, const size_t *last_el, const int *byte);
int paeth_filtration(unsigned char *buf, const size_t *first_el, const size_t *last_el, const int *byte);
#endif
