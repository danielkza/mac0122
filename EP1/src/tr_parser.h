#ifndef TR_TR_PARSER_H
#define TR_TR_PARSER_H


#define OCTAL_LITERAL_MAX_LENGTH (3)
#define OCTAL_LITERAL_MAX_VALUE  (UCHAR_MAX)

token_t* tr_parser_next_token(const char **str);
token_stack_t* tr_parser_tokenize(const char *str);
char tr_parser_unescape(const char *in, const char **out);


#endif // #ifndef TR_TR_PARSER_H
