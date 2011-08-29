#ifndef TR_TR_PARSER_H
#define TR_TR_PARSER_H

#include <stddef.h>
#include <limits.h>

#include "char_vector.h"

#define OCTAL_LITERAL_MAX_LENGTH (3)
#define OCTAL_LITERAL_MAX_VALUE  (UCHAR_MAX)

//VERIFY(sizeof(int) > sizeof(char));
#define INVALID_CHAR (CHAR_MAX+1)

typedef struct {
	int err;
	char* msg;

	const char* input;
	size_t err_pos;
} tr_parser_error_t;

void tr_fatal_error(const char* err_fmt, ...);

void tr_parser_error(tr_parser_error_t* error_out, const char* err_pos,
	                 const char* err_fmt, ...);

int tr_parser_error_check(tr_parser_error_t* error_out);

void tr_parser_error_reset(tr_parser_error_t* error_out, const char *input);

char_vector_t* tr_parser_parse(const char *str, size_t target_length,
	                           tr_parser_error_t* error_out);

#endif // #ifndef TR_TR_PARSER_H
