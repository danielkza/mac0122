#if !defined(STDC_VERSION) || STDC_VERSION < 199901L // check for C99
	int isblank(int ch) {
		if(ch == ' ' || ch == '\t')
			return 1;
		else
			return 0;
	}
#endif