#if !defined(__STDC_VERSION__) || __STDC_VERSION__ < 199901L // check for C99
	int isblank(int ch) {
		if(ch == ' ' || ch == '\t')
			return 1;
		
		return 0;
	}
#endif