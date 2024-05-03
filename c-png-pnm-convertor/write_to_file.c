#include "write_to_file.h"
int write_to_file(char *write_file, const unsigned int *width, const unsigned int *height, const int *type_color, const int *pointer, const size_t *res_size, const unsigned char *filter_res)
{
	FILE *write = fopen(write_file, "w");
	if (write == NULL)
	{
		return ERROR_CANNOT_OPEN_FILE;
	}
	if (*type_color == 0)
	{
		if (!fprintf(write, "P5\n%u %u\n255\n", *width, *height))
		{
			return ERROR_CANNOT_OPEN_FILE;
		}
		if (!fwrite(filter_res, sizeof(unsigned char), *res_size, write))
		{
			return ERROR_CANNOT_OPEN_FILE;
		}
	}
	else if (*type_color == 2 || *type_color == 6)
	{
		if (!fprintf(write, "P6\n%u %u\n255\n", *width, *height))
		{
			return ERROR_CANNOT_OPEN_FILE;
		}
		if (!fwrite(filter_res, sizeof(unsigned char), *res_size, write))
		{
			return ERROR_CANNOT_OPEN_FILE;
		}
	}
	else if (*type_color == 3)
	{
		if (*pointer == 5)
		{
			if (!fprintf(write, "P5\n%u%ud\n255\n", *width, *height))
			{
				return ERROR_CANNOT_OPEN_FILE;
			}
			if (!fwrite(filter_res, sizeof(unsigned char), *res_size, write))
			{
				return ERROR_CANNOT_OPEN_FILE;
			}
		}
		else
		{
			if (!fprintf(write, "P6\n%u %u\n255\n", *width, *height))
			{
				return ERROR_CANNOT_OPEN_FILE;
			}
			if (!fwrite(filter_res, sizeof(unsigned char), *res_size, write))
			{
				return ERROR_CANNOT_OPEN_FILE;
			}
		}
	}
	fclose(write);
	return SUCCESS;
}
