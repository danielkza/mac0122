#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#ifndef POSIX
	#include "getopt/getopt.h"
#else
	#include <getopt.h>
#endif

#include "char_vector.h"
#include "tr_parser.h"
#include "tr_funcs.h"
#include "tr.h"

// ========================================================================= //

static int opt_translate      = 0,
           opt_delete         = 0,
	       opt_complement     = 0,
	       opt_squeeze        = 0,
		   opt_truncate_set1  = 0;

// ========================================================================= //

void get_options(int argc, char** argv, int *option_index);

void print_usage(void);
void print_version(void);
void print_help(void);

void tr_process_squeeze(const char_vector_t* set1,
	                    int complement);
void tr_process_delete(const char_vector_t* set1, const char_vector_t* set2,
	                   int complement, int squeeze);
void tr_process_translate(const char_vector_t* set1, const char_vector_t* set2,
	                      int complement, int squeeze);

// ========================================================================= //

#define TR_VERSION_STRING "1.0"
#define TR_AUTHOR         "Daniel Miranda"
#define TR_COPYRIGHT      "Copyright (c) 2011 " TR_AUTHOR
#define TR_LICENSE        "GNU General Public License Version 2"

#define p(s) fputs(s,stdout)

void print_usage(void)
{

p("\
Usage: tr [OPTION]... SET1 [SET2]\n\
Run \"tr --help\" for more information.\n\
");

}

void print_help(void)
{

p("\
Usage: tr [OPTION]... SET1 [SET2]\n\
"); p("\
Translate, squeeze, and/or delete characters from standard input,\n\
writing to standard output.\n\
\n\
  -c, -C, --complement    use the complement of SET1\n\
  -d, --delete            delete characters in SET1, do not translate\n\
  -s, --squeeze-repeats   replace each input sequence of a repeated character\n\
                            that is listed in SET1 with a single occurrence\n\
                            of that character\n\
  -t, --truncate-set1     first truncate SET1 to length of SET2\n\
  --help                  show this help and exit\n\
  --version               show version and exit\n\
"); p("\
\n\
SETs are specified as strings of characters.  Most represent themselves.\n\
Interpreted sequences are:\n\
\n\
  \\NNN            character with octal value NNN (1 to 3 octal digits)\n\
  \\\\              backslash\n\
  \\a              audible BEL\n\
  \\b              backspace\n\
  \\f              form feed\n\
  \\n              new line\n\
  \\r              return\n\
  \\t              horizontal tab\n\
"); p("\
  \\v              vertical tab\n\
  CHAR1-CHAR2     all characters from CHAR1 to CHAR2 in ascending order\n\
  [CHAR*]         in SET2, copies of CHAR until length of SET1\n\
  [CHAR*REPEAT]   REPEAT copies of CHAR, REPEAT octal if starting with 0\n\
  [:alnum:]       all letters and digits\n\
  [:alpha:]       all letters\n\
  [:blank:]       all horizontal whitespace\n\
  [:cntrl:]       all control characters\n\
  [:digit:]       all digits\n\
  [:graph:]       all printable characters, not including space\n\
  [:lower:]       all lower case letters\n\
  [:print:]       all printable characters, including space\n\
  [:punct:]       all punctuation characters\n\
  [:space:]       all horizontal or vertical whitespace\n\
  [:upper:]       all upper case letters\n\
  [:xdigit:]      all hexadecimal digits\n\
  [=CHAR=]        all characters which are equivalent to CHAR\n\
"); p("\
\n\
Translation occurs if -d is not given and both SET1 and SET2 appear.\n\
-t may be used only when translating.  SET2 is extended to length of\n\
SET1 by repeating its last character as necessary.  Excess characters\n\
of SET2 are ignored.  Only [:lower:] and [:upper:] are guaranteed to\n\
expand in ascending order; used in SET2 while translating, they may\n\
only be used in pairs to specify case conversion.  -s uses SET1 if not\n\
translating nor deleting; else squeezing uses SET2 and occurs after\n\
translation or deletion.\n\
");

}

void print_version(void)
{

p("tr version " TR_VERSION_STRING "\n");
p(TR_COPYRIGHT "\n");
p(TR_LICENSE "\n");
p(TR_AUTHOR "\n");

}

//#undef p

// ========================================================================= //

void tr_process_translate(const char_vector_t* set1, const char_vector_t* set2,
	                      int complement, int squeeze)
{
	int c, last = INVALID_CHAR;

	if(set1 == NULL || set2 == NULL)
		return;

	while((c = getchar()) != EOF) {
		c = tr_char_translate(c, set1, set2, complement);
			
		if(squeeze 
		   && tr_char_find_in_set(c, set2) != -1
		   && last != INVALID_CHAR
		   && last == c)
		{
			continue;
		}

		putchar(c);
		last = c;
	}
}

void tr_process_delete(const char_vector_t* set1, const char_vector_t* set2,
	                   int complement, int squeeze)
{
	int c, last = INVALID_CHAR;

	if(set1 == NULL || (squeeze && set2 == NULL))
		return;

	while((c = getchar()) != EOF) {
		if((tr_char_find_in_set(c, set1) != -1) == (!complement ? 1 : 0))
			continue;

		if(squeeze
		   && tr_char_find_in_set(c, set2) != -1
		   && last != INVALID_CHAR
		   && last == c)
		{
			continue;
		}
	
		putchar(c);
		last = c;
	}
}

void tr_process_squeeze(const char_vector_t* set1, int complement) {
	int c, last = INVALID_CHAR;

	if(set1 == NULL)
		return;

	while((c = getchar()) != EOF) {
		if((tr_char_find_in_set(c, set1) != -1) == (!complement ? 1 : 0)
		   && last != INVALID_CHAR
		   && last == c)
		{
			continue;
		}
	
		putchar(c);
		last = c;
	}
}

// ========================================================================= //

void tr_fatal_error(const char* err_fmt, ...)
{
	va_list ap;
	va_start(ap, err_fmt);

	if(vfprintf(stderr, err_fmt, ap) < 0) {
		fputs(err_fmt, stderr);
	}

	va_end(ap);

	exit(1);
}

// ========================================================================= //

void get_options(int argc, char** argv, int *option_index)
{
	while(1) {
		enum { GETOPT_HELP_VALUE = -2, GETOPT_VERSION_VALUE = -3 };
		
		static struct option long_options[] = {
			{"squeeze",         no_argument, NULL, 's'},
			{"squeeze-repeats", no_argument, NULL, 's'},
			{"delete",          no_argument, NULL, 'd'},
			{"complement",      no_argument, NULL, 'c'},
			{"truncate-set1",   no_argument, NULL, 't'},
			{"help",            no_argument, NULL, GETOPT_HELP_VALUE},
			{"version",         no_argument, NULL, GETOPT_VERSION_VALUE},
			{0, 0, 0, 0}
		};

		int c = getopt_long(argc, argv, "cCdst", long_options, NULL);

		if(c == -1)
			break;

		switch(c) {
		case 's':
			opt_squeeze = 1;
			
			break;
		case 'c':
		case 'C':
			opt_complement = 1;
			
			break;
		case 'd':
			opt_delete = 1;
		
			break;
		case 't':
			opt_truncate_set1 = 1;

			break;
		case GETOPT_HELP_VALUE:
			print_help();
			exit(0);
			
			break;
		case GETOPT_VERSION_VALUE:
			print_version();
			exit(0);
			
			break;
		case '?':
		default:
			fprintf(stderr, "Unknown option: -%c\n", optopt);
			print_usage();
			exit(1);
		}
	}

	if(option_index != NULL)
		*option_index = optind;
}

int main(int argc, char** argv)
{
	int last_option_index = 0,
	    remaining_args = 0,
	    set2_necessary = 0;

	const char* string1, *string2;
	char_vector_t *set1 = NULL,
		          *set2 = NULL;
	tr_parser_error_t parser_error = {0, NULL, NULL, 0};

	//	

	get_options(argc, argv, &last_option_index);
	remaining_args = argc - last_option_index;

	if(remaining_args <= 0) {
		print_usage();
		exit(1);
	}

	if(opt_delete) {
		if(opt_squeeze) {
			set2_necessary = 1;

			if(remaining_args < 2) {
				tr_fatal_error("Two strings must be given when deleting and "
					           "squeezing repeats");
			}
		} else if(remaining_args >= 2) {
			tr_fatal_error("Only one string must be given when deleting "
				            "without squeezing repeats");
		}
	} else {
		if(remaining_args < 2) {
			if(!opt_squeeze) {
				tr_fatal_error("Two strings must be given when translating.");
			}
		} else {
			opt_translate = 1;
			set2_necessary = 1;
		}
	}

	
	tr_parser_error_reset(&parser_error, NULL);

	string1 = argv[last_option_index];
	set1 = tr_parser_parse(string1, 0, &parser_error);

	if(tr_parser_error_check(&parser_error)) {
		tr_fatal_error("Error parsing set1: %s at index %lu",
			           parser_error.msg, parser_error.err_pos);
	}

	if(set2_necessary) {
		string2 = argv[last_option_index + 1];
		set2 = tr_parser_parse(string2, set1->len, &parser_error);

		if(tr_parser_error_check(&parser_error)) {
			tr_fatal_error("Error parsing set2: %s at index %lu",
				           parser_error.msg, parser_error.err_pos);
		}

		if(opt_truncate_set1) {
			set1->len = set2->len;
		}
	}

	{
		size_t i;
		char* repr;

		fprintf(stderr, "set1(%lu):\n", set1 != NULL ? set1->len : 0);
		for(i = 0; i < set1->len; i++) {
			repr = tr_char_printable_repr(set1->vector[i]);
			fprintf(stderr, "%s", repr);
		}

		fprintf(stderr, "\n");

		fprintf(stderr, "set2(%lu):\n", set2 != NULL ? set2->len : 0);
		if(set2) {
			for(i = 0; i < set2->len; i++) {
				repr = tr_char_printable_repr(set2->vector[i]);
				fprintf(stderr, "%s", repr);
			}

			fprintf(stderr, "\n");
		}
	}

	// Set stdin to full buffering of 512 characters.
	setvbuf(stdin, NULL, _IOFBF, 512);
	
	if(opt_translate) {
		tr_process_translate(set1, set2, opt_complement, opt_squeeze);
	} else if(opt_delete) {
		tr_process_delete(set1, set2, opt_complement, opt_squeeze);
	} else { // squeeze only
		tr_process_squeeze(set1, opt_complement);
	}

	return 0;
}
