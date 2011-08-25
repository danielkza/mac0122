#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef _MSC_VER
	#include "getopt/getopt.h"
#else
	#include <getopt.h>
#endif

#include "tr_parser.h"

static int opt_translate = 0,
           opt_delete = 0,
	       opt_complement = 0,
	       opt_squeeze = 0; 

int main(int argc, char* argv[])
{
	int option_index = 0;
	int remaining_options = 0;
	
	while(1) {
		static struct option long_options[] = {
			{"squeeze",         no_argument, NULL, 's'},
			{"squeeze-repeats", no_argument, NULL, 's'},
			{"delete",          no_argument, NULL, 'd'},
			{"complement",      no_argument, NULL, 'c'},
			{0, 0, 0, 0}
		};

		int c = getopt_long(argc, argv, "sdcC", long_options, &option_index);

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

		case '?':
		default:
			fprintf(stderr, "Unknown option: -%c", optopt);
			exit(1);
		}
	}

	// all options processed, check for the string arguments
	remaining_options = argc - option_index - 1;

	if(opt_delete) {
		if(remaining_options < 1) {
			fprintf(stderr, "At least one string must be given when deleting.");
			exit(1);
		} else if(opt_squeeze && remaining_options < 2) {
			fprintf(stderr, "Two strings must be given when deleting and squeezing");
			exit(1);
		}
	} else if(!opt_squeeze) {
		if(remaining_options < 2) {
			fprintf(stderr, "Two strings must be given when translating.");
			exit(1);
		}

		opt_translate = 1;
	} else if (remaining_options >= 2) {
		opt_translate = 1;
	}

	// Set stdin to full buffering of 512 characters.
	setvbuf(stdin, NULL, _IOFBF, 512);
	
	if(opt_translate) {
		tr_process_translate(opt_complement, opt_squeeze,
			                 argv[option_index + 1], argv[option_index + 2]);
	} else if(opt_delete) {
		tr_process_delete(opt_complement, opt_squeeze,
			              argv[option_index + 1], argv[option_index + 2]]);
	} else { // squeeze only
		tr_process_squeeze(opt_complement, argv[option_index + 1]);
	}

	return 0;
}

void tr_process_translate(int complement, int squeeze,
	                     const char* string1, const char* string2)
{
	int c, last;
	
	while((c = getchar()) != EOF) {
		c = tr_translate_char(buffer[i], set1, set2);
			
		if(squeeze 
		   && tr_check_char(c, set2) == (!complement ? 1 : 0)
		   && last != INVALID_CHAR && last == c)
		{
			continue;
		}

		putchar(c);
		last = c;
	}
}

void tr_process_delete(int complement, int squeeze,
	                  const char* string1, const char* string2)
{
	int c, last;

	while((c = getchar()) != EOF) {
		if(tr_check_char(c, set1))
			continue;

		if(squeeze
		   && tr_check_char(c, set2) == (!complement ? 1 : 0)
		   && last != INVALID_CHAR && last == c)
		{
			continue;
		}
	
		putchar(c);
		last = c;
	}
}

void tr_process_squeeze(int complement, const char* string1) {
	int c, last;

	while((c = getchar()) != EOF) {
		if(tr_check_char(c, set1) == (!complement ? 1 : 0)
		   && last != INVALID_CHAR && last == c)
		{
			continue;
		}
	
		putchar(c);
		last = c;
	}
}