#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "char_classes.h"

int main(int argc, char* argv[])
{
	int len;
	char *expanded;

	expanded = tr_char_class_expand(tr_char_class_get_by_name("alpha"), &len);
	if(expanded)
		printf("%s\n", expanded);

	system("pause");

	return 0;
}