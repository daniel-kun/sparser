#include "sparser.h"
#include <stdlib.h>
#include <string.h>

//------------------------------------------------------------------------------
void sparser_init(sparser_t *sparser)
{
	sparser->callback = 0;
	sparser->whitespace = 0;
	sparser->user = 0;
	sparser_set_whitespace(sparser, " \t\r\n");
}

//------------------------------------------------------------------------------
void sparser_set_callback(sparser_t *sparser, sparser_callback_t callback)
{
	sparser->callback = callback;
}

//------------------------------------------------------------------------------
void sparser_set_user_data(sparser_t *sparser, void *user_data)
{
	sparser->user = user_data;
}

//------------------------------------------------------------------------------
void sparser_set_whitespace(sparser_t *sparser, const char *whitespace)
{
	size_t len;

	free(sparser->whitespace);
	len = strlen(whitespace);
	sparser->whitespace = (char*)malloc(len);
	strcpy(sparser->whitespace, whitespace);
}

// All possible parse states
typedef enum 
{
	sparser_block_begin,
	sparser_block_name,
	sparser_block_name_finished,
	sparser_inside_param
} sparser_state_t;

//------------------------------------------------------------------------------
static int is_whitespace(char p, const char *whitespace)
{
	const char *white_p;
	for (white_p = whitespace; *white_p != '\0'; ++white_p)
	{
		if (p == *white_p)
		{
			return 1;
		}
	}
	return 0;
}

// The possible error-messages
static const char *sparser_error_empty_blockname =
	"Found an empty block-name (as in \"(  (expr))\")";
static const char *sparser_error_garbage =
	"Garbage before s-expression start";

//------------------------------------------------------------------------------
int sparser_parse(sparser_t *sparser, const char **p)
{
	sparser_state_t state;
	const char *block_name_begin, *block_name_end;
	const char *param_begin, *param_end;
	unsigned bracket_depth;
	int subparse_result;

	block_name_begin = 0;
	block_name_end = 0;

	param_begin = 0;
	param_end = 0;

	bracket_depth = 0;

	state = sparser_block_begin;

	for (; **p != '\0'; ++*p)
	{
		switch (state)
		{
		case sparser_block_begin:
			if (!is_whitespace(**p, sparser->whitespace))
			{
				if (**p == '(')
				{
					state = sparser_block_name;
					block_name_begin = *p + 1;
					++bracket_depth;
				}
				else if (**p == ')')
				{
					sparser->callback(
						sparser->user,
						sparser_elem_end,
						block_name_begin,
						block_name_end - block_name_begin);
					state = sparser_block_begin;
					if (--bracket_depth == 0)
					{
						++*p;
						return 0;
					}
				}
				else
				{
					state = sparser_inside_param;
					param_begin = *p;
					param_end = *p + 1;
				}
			}
			break;
		case sparser_block_name:
			if (is_whitespace(**p, sparser->whitespace))
			{
				sparser->callback(
					sparser->user,
					sparser_elem_begin,
					block_name_begin,
					*p - block_name_begin);
				block_name_end = *p;
				state = sparser_block_name_finished;
			}
			else if (**p == ')')
			{
				sparser->callback(
					sparser->user,
					sparser_elem_end,
					block_name_begin,
					*p - block_name_begin);
				if (--bracket_depth == 0)
				{
					++*p;
					return 0;
				}
			}
			else if (**p == '(')
			{
				subparse_result = sparser_parse(sparser, p);
				if (subparse_result)
					return subparse_result;
				state = sparser_block_begin;
				++bracket_depth;
			}
			break;
		case sparser_block_name_finished:
			if (**p == ')')
			{
				sparser->callback(
					sparser->user,
					sparser_elem_end,
					block_name_begin,
					block_name_end - block_name_begin);
				if (--bracket_depth == 0)
				{
					++*p;
					return 0;
				}
			}
			else if (**p == '(')
			{
				subparse_result = sparser_parse(sparser, p);
				if (subparse_result)
					return subparse_result;
				state = sparser_block_begin;
				++bracket_depth;
			}
			else if (!is_whitespace(**p, sparser->whitespace))
			{
				param_begin = *p;
				state = sparser_inside_param;
			}
			break;
		case sparser_inside_param:
			if (**p == ')')
			{
				sparser->callback(
					sparser->user,
					sparser_elem_param,
					param_begin,
					*p - param_begin);
				sparser->callback(
					sparser->user,
					sparser_elem_end,
					block_name_begin,
					block_name_end - block_name_begin);
				state = sparser_block_begin;
				if (--bracket_depth == 0)
					return 0;
			}
			else if (**p == '(')
			{
				subparse_result = sparser_parse(sparser, p);
				if (subparse_result)
					return subparse_result;
				state = sparser_block_begin;
				++bracket_depth;
			}
			else if (is_whitespace(**p, sparser->whitespace))
			{
				sparser->callback(
					sparser->user,
					sparser_elem_param,
					param_begin,
					*p - param_begin);
				state = sparser_block_begin;
			}
			break;
		default:
			return 3;
		}
	}
	return 0;
}

//------------------------------------------------------------------------------
void sparser_deinit(sparser_t *sparser)
{
	free(sparser->whitespace);
}

