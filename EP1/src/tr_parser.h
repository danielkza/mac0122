#ifndef TR_TR_PARSER_H
#define TR_TR_PARSER_H


#define OCTAL_LITERAL_MAX_LENGTH (3)
#define OCTAL_LITERAL_MAX_VALUE  (UCHAR_MAX)

typedef struct {
	char* msg;
	char* start;
	char* err_pos;
} tr_parser_error_t;

int tr_parser_parse_octal_literal(const char **str);
int tr_parser_parse_one_char(const char **str, tr_parser_error_t* error_out);
char_vector_t* tr_parser_parse(const char *str, size_t target_length,
	                           tr_parser_error_t* error_out);
void tr_parser_error(const char* err_msg, const char* err_pos,
	                 tr_parser_error_t* error_out);


#endif // #ifndef TR_TR_PARSER_H
