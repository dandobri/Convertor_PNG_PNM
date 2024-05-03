#include "decode.h"
#include <stdlib.h>
#include <zlib.h>

int decode(unsigned char *buf, const size_t *buf_size, unsigned long *decompr_size, unsigned char *res)
{
	if (uncompress(res, decompr_size, buf, *buf_size))
	{
		return ERROR_UNKNOWN;
	}
	return SUCCESS;
}
int sub_filtration(unsigned char *buf, const size_t *first_el, const size_t *last_el, const int *byte)
{
	int res;
	for (size_t i = *first_el + *byte; i < *last_el; i++)
	{
		res = buf[i] + buf[i - *byte];
		buf[i] = res % 256;
	}
	return SUCCESS;
}
int up_filtration(unsigned char *buf, const size_t *first_el, const size_t *last_el)
{
	if (*first_el != 1)
	{
		int res;
		for (size_t i = *first_el; i < *last_el; i++)
		{
			res = buf[i] + buf[i - (*last_el - *first_el) - 1];
			buf[i] = res % 256;
		}
	}
	return SUCCESS;
}
int average_filtration(unsigned char *buf, const size_t *first_el, const size_t *last_el, const int *byte)
{
	int res;
	if (*first_el == 1)
	{
		for (size_t i = *first_el + *byte; i < *last_el; i++)
		{
			res = buf[i] + buf[i - *byte] / 2;
			buf[i] = res % 256;
		}
	}
	else
	{
		for (int i = 0; i < *byte; i++)
		{
			res = buf[*first_el + i] - buf[*first_el + i - (*last_el - *first_el) - 1] / 2;
			buf[*first_el + i] = res % 256;
		}
		for (size_t i = *first_el + *byte; i < *last_el; i++)
		{
			if (buf[i - *byte] >= buf[i - (*last_el - *first_el) - 1])
			{
				res = buf[i] + (buf[i - (*last_el - *first_el) - 1] + (buf[i - *byte] - buf[i - (*last_el - *first_el) - 1]) / 2);
				buf[i] = res % 256;
			}
			else
			{
				res = buf[i] + (buf[i - *byte] + (buf[i - (*last_el - *first_el) - 1] - buf[i - *byte]) / 2);
				buf[i] = res % 256;
			}
		}
	}
	return SUCCESS;
}
int paeth_filtration(unsigned char *buf, const size_t *first_el, const size_t *last_el, const int *byte)
{
	if (*first_el == 1)
	{
		for (int i = 0; i < *byte; i++)
		{
			buf[*first_el + *byte] = buf[*first_el + *byte];
		}
		for (size_t i = *first_el + *byte; i < *last_el; i++)
		{
			buf[i] = buf[i] - buf[i - *byte];
		}
	}
	else
	{
		for (int i = 0; i < *byte; i++)
		{
			int res = buf[i] + buf[*first_el + *byte + i - (*last_el - *first_el) - 1];
			buf[*first_el + *byte] = res % 256;
		}
		for (size_t i = *first_el + *byte; i < *last_el; i++)
		{
			unsigned char a = buf[i - *byte];
			unsigned char b = buf[i - (*last_el - *first_el) - 1];
			unsigned char c = buf[i - (*last_el - *first_el) - 1 - *byte];
			int p = a + b - c;
			int p_a = abs(p - buf[i - *byte]);
			int p_b = abs(p - buf[i - (*last_el - *first_el) - 1]);
			int p_c = abs(p - buf[i - (*last_el - *first_el) - 1 - *byte]);
			int res;
			if (p_a <= p_b && p_a <= p_c)
			{
				res = buf[i] + a;
				buf[i] = res % 256;
			}
			else if (p_b <= p_c)
			{
				res = buf[i] + b;
				buf[i] = res % 256;
			}
			else
			{
				res = buf[i] + c;
				buf[i] = res % 256;
			}
		}
	}
	return SUCCESS;
}

