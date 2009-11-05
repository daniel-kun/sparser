#include "sparser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int sexpr_dumper(
	int *depth,
	sparser_action_t action,
	const char *symbol_begin, int symbol_len)
{
	int i;
	char action_name[255];
	char *buf = (char*)malloc(symbol_len + 1);
	strncpy(buf, symbol_begin, symbol_len);
	buf[symbol_len] = '\0';

	switch (action)
	{
	case sparser_error:
		printf("ERROR! \"%s\"", buf);
		break;
	case sparser_elem_begin:
		if (*depth > 0)
			printf("\n");
		for (i = 0; i < *depth; ++i)
			printf("   ");
		printf("(\"%s\"", buf);
		++*depth;
		break;
	case sparser_elem_param:
		printf(" \"%s\"", buf);
		break;
	case sparser_elem_end:
		printf(")");
		--*depth;
		break;
	default:
		strcpy(action_name, "unknown action");
		break;
	}

	free(buf);
	return 0;
}

int main(int argc, char *argv[])
{
	int d, depth;
	const char *p;
	char *free_me;
	long file_size;
	sparser_t sparser;

	depth = 0;
	sparser_init(&sparser);
	sparser_set_user_data(&sparser, &depth);
	sparser_set_callback(&sparser, sexpr_dumper);

	p = 0;
	free_me = 0;
	if (argc == 2)
	{
		FILE *f = fopen(argv[1], "r");
		if (f)
		{
			fseek(f, 0, SEEK_END);
			file_size = ftell(f);
			free_me = (char*)malloc(file_size);
			fseek(f, 0, SEEK_SET);
			fread(free_me, file_size, 1, f);
			p = free_me;
			fclose(f);
		}
	}
	if (p == 0)
		p = 
			"(window\n"
			"	(geometry (x 10) (y 20) (width 100) (height 300))\n"
			"   (frameGeometry (x 5) (y 12) (width 110) (height 126)))";
	if (sparser_parse(&sparser, &p) != 0)
		printf("Parse failed.");
	free(free_me);

	printf("\n");

	scanf("\n", &d);
	return 0;
}
