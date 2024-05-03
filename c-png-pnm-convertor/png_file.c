#include "png_file.h"
int reading_writing_png(FILE *read_file, char *back, char *write_file, int *empty)
{
	unsigned int width = 0;
	unsigned int height = 0;
	int type_color;
	Chunk idat;
	idat.size = 0;
	idat.count_chunk = 0;
	idat.chunk = NULL;
	Chunk plte;
	plte.size = 0;
	plte.count_chunk = 0;
	plte.chunk = NULL;
	Chunk bkgd;
	bkgd.size = 0;
	bkgd.count_chunk = 0;
	bkgd.chunk = NULL;
	Chunk ihdr;
	ihdr.size = 0;
	ihdr.count_chunk = 0;
	ihdr.chunk = NULL;
	Chunk trns;
	trns.size = 0;
	trns.count_chunk = 0;
	trns.chunk = NULL;
	Chunk list[5] = { idat, ihdr, plte, bkgd, trns };
	int error = png_check(read_file);
	if (error != SUCCESS)
	{
		fprintf(stderr, "its not png");
		return error;
	}
	int find_iend = 0;
	int res_file_format = 0;
	int size = 0;
	unsigned char trans[4] = {0, 0, 0, 0};
	unsigned char name_chunk[4] = {0, 0, 0, 0};
	unsigned char size_chunk[4] = {0, 0, 0, 0};
	while (find_iend == 0)
	{
		if (!fread(size_chunk, sizeof(unsigned char), 4, read_file))
		{
			clear(list);
			fprintf(stderr, "error in size_chunk_read");
			return ERROR_DATA_INVALID;
		}
		for (int i = 0; i < 4; i++)
		{
			trans[3 - i] = size_chunk[i];
		}
		size = *(int *)trans;
		if (!fread(name_chunk, sizeof(unsigned char), 4, read_file))
		{
			clear(list);
			fprintf(stderr, "error in name_chunk read");
			return ERROR_DATA_INVALID;
		}
		if (name_chunk[0] == 'I' && name_chunk[1] == 'H' && name_chunk[2] == 'D' && name_chunk[3] == 'R')
		{
			if (ihdr.count_chunk == 0)
			{
				ihdr.size = size;
				ihdr.chunk = (unsigned char *)malloc(sizeof(unsigned char) * (4 + ihdr.size));
				if (!fread(ihdr.chunk, sizeof(unsigned char), 4 + ihdr.size, read_file))
				{
					clear(list);
					fprintf(stderr, "error in reading ihdr");
					return ERROR_DATA_INVALID;
				}
				error = ihdr_check(&width, &height, &type_color, ihdr.chunk);
				free(ihdr.chunk);
				ihdr.chunk = NULL;
				if (error != SUCCESS)
				{
					fprintf(stderr, "error in ihdr_check");
					clear(list);
					return error;
				}
				if (type_color == 2 || type_color == 6)
				{
					bkgd.size = 3;
					unsigned char _chunk[3];
					bkgd.chunk = _chunk;
				}
				else if (type_color == 0 || type_color == 4 || type_color == 3)
				{
					bkgd.size = 1;
					unsigned char _chunk[1];
					bkgd.chunk = _chunk;
				}
				error = bkgd_check(bkgd.chunk, &type_color, empty, back);
				if (error != SUCCESS)
				{
					fprintf(stderr, "error in bkgd_check");
					clear(list);
					return error;
				}
				ihdr.count_chunk++;
			}
			else
			{
				clear(list);
				fprintf(stderr, "too many ihdr");
				return ERROR_DATA_INVALID;
			}
		}
		else if (name_chunk[0] >= 'a' && name_chunk[0] <= 'z')
		{
			if (name_chunk[0] == 'b' && name_chunk[1] == 'K' && name_chunk[2] == 'G' && name_chunk[3] == 'D')
			{
				bkgd.count_chunk++;
				if (bkgd.count_chunk != 1)
				{
					fprintf(stderr, "too many bkgd");
					clear(list);
					return ERROR_DATA_INVALID;
				}
				if (*empty == 0)
				{
					if (type_color == 2 || type_color == 6 || type_color == 0 || type_color == 3)
					{
						unsigned char _bkgd[bkgd.size * 2];
						if (!fread(_bkgd, sizeof(unsigned char), bkgd.size * 2, read_file))
						{
							fprintf(stderr, "error in read bkgd");
							clear(list);
							return ERROR_DATA_INVALID;
						}
						int count_ = 0;
						for (int i = 1; i < 2 * bkgd.size; i += 2)
						{
							bkgd.chunk[count_] = _bkgd[i];
							count_++;
						}
					}
					else if (type_color == 3)
					{
						if (!fread(bkgd.chunk, sizeof(unsigned char), 1, read_file))
						{
							clear(list);
							fprintf(stderr, "error in read bkgd");
							return ERROR_DATA_INVALID;
						}
					}
				}
				else
				{
					fseek(read_file, size, SEEK_CUR);
				}
				fseek(read_file, 4, SEEK_CUR);
			}
			else if (name_chunk[0] == 't' && name_chunk[1] == 'R' && name_chunk[2] == 'N' && name_chunk[3] == 'S')
			{
				trns.count_chunk++;
				if (trns.count_chunk != 1)
				{
					fprintf(stderr, "too many trns");
					clear(list);
					return ERROR_DATA_INVALID;
				}
				if (type_color == 4 || type_color == 6)
				{
					clear(list);
					fprintf(stderr, "not allowed type_color for trns");
					return ERROR_DATA_INVALID;
				}
				if (type_color == 0)
				{
					trns.size = 1;
				}
				else if (type_color == 2)
				{
					trns.size = 3;
				}
				else
				{
					trns.size = size;
				}
				if (type_color == 0 || type_color == 2)
				{
					for (int i = 0; i < trns.size; i++)
					{
						fseek(read_file, 1, SEEK_CUR);
						if (!fread(trns.chunk + i, sizeof(unsigned char), 1, read_file))
						{
							fprintf(stderr, "error in read trns");
							clear(list);
							return ERROR_DATA_INVALID;
						}
					}
				}
				else if (type_color == 3)
				{
					if (!fread(trns.chunk, sizeof(unsigned char), trns.size, read_file))
					{
						clear(list);
						fprintf(stderr, "error in read trns");
						return ERROR_DATA_INVALID;
					}
				}
				fseek(read_file, 4, SEEK_CUR);
			}
			else
			{
				fseek(read_file, 4 + size, SEEK_CUR);
			}
		}
		else if (name_chunk[0] == 'I' && name_chunk[1] == 'E' && name_chunk[2] == 'N' && name_chunk[3] == 'D')
		{
			find_iend = 1;
			fseek(read_file, 4, SEEK_CUR);
		}
		else if (name_chunk[0] == 'I' && name_chunk[1] == 'D' && name_chunk[2] == 'A' && name_chunk[3] == 'T')
		{
			if (idat.count_chunk == 0)
			{
				idat.chunk = (unsigned char *)malloc(size * sizeof(unsigned char));
				if (idat.chunk == NULL)
				{
					clear(list);
					fprintf(stderr, "not memory allocation");
					return ERROR_OUT_OF_MEMORY;
				}
				if (!fread(idat.chunk, sizeof(unsigned char), size, read_file))
				{
					clear(list);
					fprintf(stderr, "error in read idat");
					return ERROR_DATA_INVALID;
				}
				idat.size = size;
			}
			else
			{
				error = idat_chunk_check(read_file, idat.chunk, &idat.size, &size);
				if (error != SUCCESS)
				{
					fprintf(stderr, "error in read_idat");
					clear(list);
					return error;
				}
			}
			idat.count_chunk++;
			fseek(read_file, 4, SEEK_CUR);
		}
		else if (name_chunk[0] == 'P' && name_chunk[1] == 'L' && name_chunk[2] == 'T' && name_chunk[3] == 'E')
		{
			plte.count_chunk++;
			if (plte.count_chunk > 1 || type_color == 0 || type_color == 4)
			{
				clear(list);
				fprintf(stderr, "not allowed plte");
				return ERROR_DATA_INVALID;
			}
			if (type_color == 3)
			{
				plte.chunk = (unsigned char *)malloc(size * sizeof(unsigned char));
				if (plte.chunk == NULL)
				{
					clear(list);
                    fprintf(stderr, "error in read plte");
					return ERROR_OUT_OF_MEMORY;
				}
				plte.size = size;
				error = plte_check(read_file, plte.chunk, &plte.size, &res_file_format);
				if (error != SUCCESS)
				{
					clear(list);
					fprintf(stderr, "error in check plte");
					return error;
				}
			}
			else
			{
				fseek(read_file, 4 + size, SEEK_CUR);
			}
		}
	}
	if (type_color == 3)
	{
		bkgd.chunk[1] = plte.chunk[bkgd.chunk[0] + 1];
		bkgd.chunk[2] = plte.chunk[bkgd.chunk[0] + 2];
		bkgd.chunk[0] = plte.chunk[bkgd.chunk[0]];
	}
	if (bkgd.count_chunk == 0 && *empty == 0)
	{
		for (int i = 0; i < bkgd.size; i++)
		{
			bkgd.chunk[i] = 0;
		}
	}
	if (idat.count_chunk == 0)
	{
		clear(list);
		fprintf(stderr, "we havent idat chunks");
		return ERROR_DATA_INVALID;
	}
	int byte;
	if (type_color == 2 || type_color == 3)
	{
		byte = 3;
	}
	else if (type_color == 0)
	{
		byte = 1;
	}
	else if (type_color == 4)
	{
		byte = 2;
	}
	else
	{
		byte = 4;
	}
	unsigned long buf_size = height * (width * byte + 1);
	unsigned char *buf = (unsigned char *)malloc(sizeof(unsigned char) * buf_size);
	if (buf == NULL)
	{
		clear(list);
		fprintf(stderr, "not memory allocation");
		return ERROR_OUT_OF_MEMORY;
	}
	error = decode(idat.chunk, &idat.size, &buf_size, buf);
	if (error != SUCCESS)
	{
		clear(list);
		free(buf);
		buf = NULL;
		fprintf(stderr, "error in decode");
		return error;
	}
	free(idat.chunk);
	idat.chunk = NULL;
	size_t res_size = buf_size;
	res_size = res_size - height;
	unsigned char *res = (unsigned char *)malloc(sizeof(unsigned char) * res_size);
	if (res == NULL)
	{
		clear(list);
		free(buf);
		fprintf(stderr, "not memory allocation");
		return ERROR_OUT_OF_MEMORY;
	}
	error = decoder(buf, res, &width, &height, &byte);
	free(buf);
	buf = NULL;
	if (error != SUCCESS)
	{
		clear(list);
		free(res);
		fprintf(stderr, "error in decoder");
		return error;
	}
	if (type_color == 0 || type_color == 2)
	{
		int k;
		if (trns.count_chunk != 0)
		{
			if (type_color == 2)
			{
				for (size_t i = 0; i < res_size; i += 3)
				{
					k = trns.chunk[0];
					res[i] = k * res[i] + (1 - k) * bkgd.chunk[0];
					k = trns.chunk[1];
					res[i + 1] = k * res[i + 1] + (1 - k) * bkgd.chunk[1];
					k = trns.chunk[2];
					res[i + 2] = k * res[i + 2] + (1 - k) * bkgd.chunk[2];
				}
			}
			else
			{
				for (size_t i = 0; i < res_size; i++)
				{
					k = trns.chunk[0];
					res[i] = k * res[i] + (1 - k) * bkgd.chunk[0];
				}
			}
		}
		clear(list);
		error = write_to_file(write_file, &width, &height, &type_color, &res_file_format, &res_size, res);
		free(res);
		res = NULL;
		if (error != SUCCESS)
		{
			fprintf(stderr, "we cant write to file");
			return error;
		}
	}
	else if (type_color == 6 || type_color == 4)
	{
		size_t alpha_size = res_size - height * width;
		unsigned char *alpha_res = (unsigned char *)malloc(sizeof(unsigned char) * alpha_size);
		error = alpha(res, bkgd.chunk, alpha_res, &res_size, &type_color);
		if (error != SUCCESS)
		{
			clear(list);
			free(res);
			free(alpha_res);
			fprintf(stderr, "error in alpha_check");
			return error;
		}
		free(res);
		res = NULL;
		clear(list);
		error = write_to_file(write_file, &width, &height, &type_color, &res_file_format, &alpha_size, alpha_res);
		free(alpha_res);
		alpha_res = NULL;
		if (error != SUCCESS)
		{
			fprintf(stderr, "we cant write to file");
			return error;
		}
	}
	else if (type_color == 3)
	{
		int k;
		size_t idat_with_plte_size;
		unsigned char *idat_with_plte = NULL;
		if (res_file_format == 5)
		{
			idat_with_plte_size = byte * height * width;
			idat_with_plte = (unsigned char *)malloc(sizeof(unsigned char) * idat_with_plte_size);
			for (size_t i = 0; i < idat_with_plte_size; i++)
			{
				if (trns.count_chunk != 0)
				{
					k = trns.chunk[0] / 256;
					idat_with_plte[i] = k * plte.chunk[res[i] * 3] + (1 - k) * bkgd.chunk[0];
				}
				else
				{
					idat_with_plte[i] = plte.chunk[res[i] * 3];
				}
			}
		}
		else
		{
			idat_with_plte_size = 3 * byte * height * width;
			idat_with_plte = (unsigned char *)malloc(sizeof(unsigned char) * idat_with_plte_size);
			for (size_t i = 0; i < idat_with_plte_size / 3; i++)
			{
				if (trns.count_chunk != 0)
				{
					k = trns.chunk[res[i]];
					idat_with_plte[i] = k * plte.chunk[res[i] * 3] + (1 - k) * bkgd.chunk[0];
					k = trns.chunk[res[i + 1]];
					idat_with_plte[i + 1] = k * plte.chunk[res[i + 1] * 3] + (1 - k) * bkgd.chunk[1];
					k = trns.chunk[res[i + 2]];
					idat_with_plte[i + 2] = k * plte.chunk[res[i + 1] * 3] + (1 - k) * bkgd.chunk[2];
				}
				else
				{
					idat_with_plte[i] = plte.chunk[res[i] * 3];
					idat_with_plte[i + 1] = plte.chunk[res[i] * 3 + 1];
					idat_with_plte[i + 2] = plte.chunk[res[i] * 3 + 2];
				}
			}
		}
		free(res);
		res = NULL;
		clear(list);
		error = write_to_file(write_file, &width, &height, &type_color, &res_file_format, &idat_with_plte_size, idat_with_plte);
		free(idat_with_plte);
		idat_with_plte = NULL;
		if (error != SUCCESS)
		{
			fprintf(stderr, "we cant write to file");
			return error;
		}
	}
	clear(list);
	return SUCCESS;
}
void clear(Chunk *list)
{
	for (int i = 0; i < 5; i++)
	{
		if (list[i].chunk != NULL)
		{
			free(list[i].chunk);
		}
	}
}
int decoder(unsigned char *buf, unsigned char *res, const unsigned int *width, const unsigned int *height, const int *byte)
{
	size_t first = 0;
	size_t last = 0;
	size_t count = 0;
	size_t pro = *width * *byte;
	for (unsigned int i = 0; i < *height; i++)
	{
		first = (*width * *byte + 1) * i + 1;
		last = (*width * *byte + 1) * (i + 1);
		if (buf[first - 1] == 0)
		{
		}
		else if (buf[first - 1] == 1)
		{
			sub_filtration(buf, &first, &last, byte);
		}
		else if (buf[first - 1] == 2)
		{
			up_filtration(buf, &first, &last);
		}
		else if (buf[first - 1] == 3)
		{
			average_filtration(buf, &first, &last, byte);
		}
		else if (buf[first - 1] == 4)
		{
			paeth_filtration(buf, &first, &last, byte);
		}
		for (size_t j = 0; j < pro; j++)
		{
			res[count] = buf[first + j];
			count++;
		}
	}
	return SUCCESS;
}
int png_check(FILE *read_file)
{
	unsigned char png_chunk[8];
	if (!fread(png_chunk, sizeof(unsigned char), 8, read_file))
	{
		return ERROR_DATA_INVALID;
	}
	if (png_chunk[0] != (16 * 8 + 9) || png_chunk[1] != 'P' || png_chunk[2] != 'N' || png_chunk[3] != 'G')
	{
		return ERROR_UNKNOWN;
	}
	return SUCCESS;
}
int ihdr_check(unsigned int *width, unsigned int *height, int *type_color, const unsigned char *ihdr)
{
	unsigned char width_mas[4];
	for (int i = 0; i < 4; i++)
	{
		width_mas[3 - i] = ihdr[i];
	}
	*width = *(int *)width_mas;
	unsigned char height_mas[4];
	for (int i = 0; i < 4; i++)
	{
		height_mas[3 - i] = ihdr[4 + i];
	}
	*height = *(int *)height_mas;
	*type_color = ihdr[9];
	return SUCCESS;
}
int idat_chunk_check(FILE *read_file, unsigned char *buffer, size_t *size_idat_chunk, const int *size)
{
	void *real = realloc(buffer, sizeof(unsigned char) * (*size_idat_chunk + *size));
	if (real == NULL)
	{
		return ERROR_OUT_OF_MEMORY;
	}
	unsigned char *buf = (unsigned char *)malloc(sizeof(unsigned char) * *size);
	if (buf == NULL)
	{
		return ERROR_OUT_OF_MEMORY;
	}
	if (!fread(buf, sizeof(unsigned char), *size, read_file))
	{
		free(buf);
		return ERROR_DATA_INVALID;
	}
	for (size_t i = *size_idat_chunk ; i < *size_idat_chunk + *size; i++)
	{
		buffer[i] = buf[i - *size_idat_chunk];
	}
	*size_idat_chunk = *size + *size_idat_chunk;
	free(buf);
	buf = NULL;
	return SUCCESS;
}
int plte_check(FILE *read_file, unsigned char *plte_buf, const size_t *size_plte_chunk, int *res_file_format)
{
	if (*size_plte_chunk % 3 != 0 || *size_plte_chunk > 256 * 3)
	{
		return ERROR_PARAMETER_INVALID;
	}
	if (!fread(plte_buf, sizeof(unsigned char), *size_plte_chunk, read_file))
	{
		free(plte_buf);
		return ERROR_DATA_INVALID;
	}
	for (size_t i = 0; i < *size_plte_chunk; i += 3)
	{
		if (plte_buf[i] == plte_buf[i + 1] == plte_buf[i + 2])
		{
			*res_file_format = 5;
		}
		else
		{
			*res_file_format = 6;
			break;
		}
	}
	if (!fseek(read_file, 4, SEEK_CUR))
	{
		free(plte_buf);
		return ERROR_OUT_OF_MEMORY;
	}
	return SUCCESS;
}
int bkgd_check(unsigned char *bkgd, int *type_color, const int *empty, char *back)
{
	if (*empty == 1)
	{
		int error = comand_line_arg_check(bkgd, back, type_color);
		if (error != SUCCESS)
		{
			return error;
		}
	}
	return SUCCESS;
}
int alpha(unsigned char *buf, const unsigned char *bkgd, unsigned char *res, const size_t *buf_size, const int *type_color)
{
	long long count = 0;
	int k;
	if (*type_color == 6)
	{
		for (size_t i = 0; i < *buf_size; i += 4)
		{
			k = buf[i + 3] / 255;
			buf[i] = k * buf[i] + (1 - k) * bkgd[0];
			buf[i + 1] = k * buf[i + 1] + (1 - k) * bkgd[1];
			buf[i + 2] = k * buf[i + 2] + (1 - k) * bkgd[2];
			if(buf[i] < 0 || buf[i + 1] < 0 || buf[i + 2] < 0)
			{
				return ERROR_DATA_INVALID;
			}
			res[count] = buf[i];
			res[count + 1] = buf[i + 1];
			res[count + 2] = buf[i + 2];
			count += 3;
		}
	}
	else if (*type_color == 4)
	{
		for (size_t i = 0; i < *buf_size; i += 2)
		{
			k = buf[i + 1] / 255;
			buf[i] = k * buf[i] + (1 - k) * bkgd[0];
			if (buf[i] < 0)
			{
				return ERROR_DATA_INVALID;
			}
			res[count] = buf[i];
			count++;
		}
	}
	return SUCCESS;
}
int comand_line_arg_check(unsigned char *bkgd_chuck, char *back, const int *type_color)
{
	int R = -1;
	int G = -1;
	int B = -1;
	char number[100] = "";
	int count = 0;
	int translate = 0;
	for (int i = 0; i < strlen(back); i++)
	{
		if ((back[i] >= '0' && back[i] <= '9') || back[i] == '-')
		{
			number[count] = back[i];
			count++;
		}
		else
		{
			unsigned char *betw = (unsigned char *)malloc(sizeof(unsigned char) * count);
			if (number[0] == '-')
			{
				translate = 0;
			}
			else
			{
				for (int j = 0; j < count; j++)
				{
					betw[j] = number[j];
				}
				sscanf(betw, "%d", &translate);
			}
			free(betw);
			betw = NULL;
			if (translate >= 256)
			{
				translate = 0;
			}
			if (R == -1)
			{
				R = translate;
			}
			else
			{
				G = translate;
			}
			count = 0;
		}
	}
	unsigned char *final = (unsigned char *)malloc(sizeof(unsigned char) * count);
	if (number[0] == '-')
	{
		translate = 0;
	}
	else
	{
		for (int j = 0; j < count; j++)
		{
			final[j] = number[j];
		}
		sscanf(final, "%d", &translate);
	}
	free(final);
	final = NULL;
	if (translate >= 256)
	{
		translate = 0;
	}
	if (R == -1 && G == -1)
	{
		if (*type_color == 2 || *type_color == 6)
		{
			R = translate;
			G = translate;
			B = translate;
		}
		else if (*type_color == 0 ||*type_color == 3 || *type_color == 4)
		{
			R = translate;
		}
	}
	else
	{
		B = translate;
	}
	if (*type_color == 2 || *type_color == 6)
	{
		bkgd_chuck[0] = R;
		bkgd_chuck[1] = G;
		bkgd_chuck[2] = B;
	}
	else
	{
		bkgd_chuck[0] = R;
	}
	return SUCCESS;
}
