#include "return_codes.h"
#include "png_file.h"

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
int main(int argc, char *argv[])
{
	FILE *read_file = fopen(argv[1], "r");
	if (read_file == NULL)
	{
		return ERROR_CANNOT_OPEN_FILE;
	}
	int empty = 0;
	if (argc == 4)
	{
		empty = 1;
	}
	int error = reading_writing_png(read_file, argv[3], argv[2], &empty);
	fclose(read_file);
	if (error != SUCCESS)
	{
		fprintf(stderr, "\npng file is not supported");
		return error;
	}
	return SUCCESS;
}
