/*
 * Jessica Moreira
 */
#include "grep.h"

int main(int argc, char *argv[]) {
	char *p1, *p2;
	SIG_TYP oldintr;
	oldquit = signal(SIGQUIT, SIG_IGN);
	oldhup = signal(SIGHUP, SIG_IGN);
	oldintr = signal(SIGINT, SIG_IGN);
	if (signal(SIGTERM, SIG_IGN) == SIG_DFL) signal(SIGTERM, quit); argv++;
	if (argc>1) {
		sprintf(inputarry, "g/%s\nq\n", *argv);
		input = inputarry;
		argv++;
		argc--;
		while(argc>0){
			p1 = *argv; p2 = savedfile;
			while ((*p2++ = *p1++)){
				if (p2 >= &savedfile[sizeof(savedfile)]) p2--;
			}
			globp = "r";
			zero = (unsigned *)malloc(nlall*sizeof(unsigned));
			tfname = mkstemp(tmpXXXXX);
			init();
			setjmp(savej);
			commands();
			argv++;
			argc--;
		}
	}
	quit(0);
	return 0;
}

void commands(void) {
	unsigned int *a1;
	int c, temp;
	char lastsep;
	for (;;) { 
		if (pflag) { pflag = 0; addr1 = addr2 = dot; print();} c = '\n';
		for (addr1 = 0;;) {
			lastsep = c; a1 = address(); c = getchr();
			if (c!=',' && c!=';') break;
			if (a1==0) { a1 = zero+1; if (a1>dol) a1--; }
			addr1 = a1;
			if (c==';') dot = a1;
		}
		if (lastsep!='\n' && a1==0) a1 = dol;
		if ((addr2=a1)==0) { given = 0; addr2 = dot; } else given = 1;
		if (addr1==0) addr1 = addr2;
		switch(c) {
			case 'g':
				global(1);
				continue;
			case 'p':
				newline();
				print();
				continue;
			case 'q':
				newline();
				quit(0);
			case 'r':
				filename(c);
			caseread:
				if ((io = open(file, 0)) < 0) { lastc = '\n'; }
				setwide();
				ninbuf = 0; c = zero != dol;
				append(getfile, addr2);
				continue;
			case EOF:
				return;
		}
	}
}

void print(void) {
	unsigned int *a1;
	a1 = addr1;
	do { puts(getline(*a1++)); } while (a1 <= addr2);
	dot = addr2;
}

unsigned int * address(void) {
	unsigned int *a, *b;
	int opcnt, nextopand, c, sign;
	nextopand = -1; sign = 1; opcnt = 0; a = dot;
	do {
		do c = getchr(); while (c==' ' || c=='\t');
		if ('0'<=c && c<='9') { peekc = c; } else switch (c) {
		case '?':
			sign = -sign;
			/* fall through */
		case '/':
			compile(c);
			b = a;
			for (;;) {
				a += sign;
				if (a<=zero)
					a = dol;
				if (a>dol)
					a = zero;
				if (execute(a))
					break;
			}
			break;
			default:
			if (c!='+' && c!='-' && c!='^') {
				peekc = c;
				if (opcnt==0)
					a = 0;
				return (a);
			}
			sign = 1;
			if (c!='+')
				sign = -sign;
			nextopand = ++opcnt;
			continue;
		 }
		sign = 1;
		opcnt++;
	} while (zero<=a && a<=dol);
	/*NOTREACHED*/
	return 0;
}

void setwide(void) { if (!given) { addr1 = zero + (dol>zero); addr2 = dol;} }

void newline(void) { int c; if ((c = getchr()) == '\n' || c == EOF) return; }

void filename(int comm) {
	char *p1, *p2;
	int c;
	count = 0;
	c = getchr();
	if (c=='\n' || c==EOF) {
		p1 = savedfile;
		p2 = file;
		while ((*p2++ = *p1++)) ;
		return;
	}
	if (savedfile[0]==0 || comm=='e' || comm=='f') {
		p1 = savedfile;
		p2 = file;
		while ((*p1++ = *p2++)) ;
	}
}

int getchr(void) {
	char c;
	if ((lastc = peekc)) {
		peekc = 0;
		return(lastc);
	}
	if (globp) {
		if ((lastc = *globp++) != 0) return(lastc);
		globp = 0;
		return(EOF);
	}
	if ((c = *input++) == EOF) return(lastc = EOF);
	lastc = c&0177;
	return(lastc);
}

int getfile(void) {
	int c;
	char *lp, *fp;
	lp = linebuf; fp = nextip;
	do {
		if (--ninbuf < 0) {
			if ((ninbuf = read(io, genbuf, LBSIZE)-1) < 0){
				if (lp>linebuf) {
					puts("'\\n' appended");
					*genbuf = '\n';
				}
				else return(EOF);
			}
			fp = genbuf;
			while(fp < &genbuf[ninbuf]) { if (*fp++ & 0200) break; }
			fp = genbuf;
		}
		c = *fp++;
		if (c=='\0') continue;
		*lp++ = c;
		count++;
	} while (c != '\n');
	*--lp = 0;
	nextip = fp;
	return(0);
}

int append(int (*f)(void), unsigned int *a) {
	unsigned int *a1, *a2, *rdot;
	int nline, tl;
	nline = 0; dot = a;
	while ((*f)() == 0) {
		if ((dol-zero)+1 >= nlall) {
			unsigned *ozero = zero;
			nlall += 1024;
			dot += zero - ozero;
			dol += zero - ozero;
		}
		tl = putline();
		nline++;
		a1 = ++dol;
		a2 = a1+1;
		rdot = ++dot;
		while (a1 > rdot) *--a2 = *--a1;
		*rdot = tl;
	}
	return(nline);
}

void quit(int n) { exit(0);}

char * getline(unsigned int tl) {
	char *bp, *lp;
	int nl;
	lp = linebuf; bp = getblock(tl, READ); nl = nleft; tl &= ~((BLKSIZE/2)-1);
	while ((*lp++ = *bp++))
		if (--nl == 0) {
			bp = getblock(tl+=(BLKSIZE/2), READ);
			nl = nleft;
		}
	return(linebuf);
}

int putline(void) {
	char *bp, *lp;
	int nl;
	unsigned int tl;
	lp = linebuf; tl = tline; bp = getblock(tl, WRITE); nl = nleft; tl &= ~((BLKSIZE/2)-1);
	while ((*bp = *lp++)) {
		if (*bp++ == '\n') { *--bp = 0; break; }
		if (--nl == 0) { bp = getblock(tl+=(BLKSIZE/2), WRITE); nl = nleft; }
	}
	nl = tline;
	tline += (((lp-linebuf)+03)>>1)&077776;
	return(nl);
}

char * getblock(unsigned int atl, int iof) {
	int bno, off;
	bno = (atl/(BLKSIZE/2));
	off = (atl<<1) & (BLKSIZE-1) & ~03;
	nleft = BLKSIZE - off;
	if (bno==oblock) return(obuff+off);
	oblock = bno;
	return(obuff+off);
}

void init(void) {
	int *markp;
	for (markp = names; markp < &names[26]; ) *markp++ = 0;
	oblock = -1;
	tfile = open(tfname, 2);
	dot = dol = zero;
}

void global(int k) {
	char *gp, globuf[GBSIZE];
	int c;
	unsigned int *a1;
	setwide();
	c = getchr();
	compile(c);
	gp = globuf;
	while ((c = getchr()) != '\n') {
		if (c=='\\') {
			c = getchr();
			if (c!='\n') *gp++ = '\\';
		}
		*gp++ = c;
	}
	if (gp == globuf) *gp++ = 'p';
	*gp++ = '\n';
	*gp++ = 0;
	for (a1=zero; a1<=dol; a1++) {
		*a1 &= ~01;
		if (a1>=addr1 && a1<=addr2 && execute(a1)==k) *a1 |= 01;
	}
	/*
	 * Special case: g/.../d (avoid n^2 algorithm)
	 */
	for (a1=zero; a1<=dol; a1++) {
		if (*a1 & 01) {
			*a1 &= ~01;
			dot = a1;
			globp = globuf;
			commands();
			a1 = zero;
		}
	}
}

void compile(int eof) {
	int c, cclcnt;
	char *ep, *lastep, bracket[NBRA], *bracketp;
	ep = expbuf; bracketp = bracket;
	if ((c = getchr()) == '\n') { peekc = c; c = eof; }
	peekc = c;
	lastep = 0;
	for (;;) {
		if (ep >= &expbuf[ESIZE]) goto cerror;
		c = getchr();
		if (c == '\n') { peekc = c; c = eof;}
		if (c==eof) {
			if (bracketp != bracket) goto cerror;
			*ep++ = CEOF;
			return;
		}
		if (c!='*') lastep = ep;
		switch (c) {
		case '\\':
			if ((c = getchr())=='(') {
				if (nbra >= NBRA) goto cerror;
				*bracketp++ = nbra;
				*ep++ = CBRA;
				*ep++ = nbra++;
				continue;
			}
			if (c == ')') {
				if (bracketp <= bracket) goto cerror;
				*ep++ = CKET;
				*ep++ = *--bracketp;
				continue;
			}
			if (c>='1' && c<'1'+NBRA) { *ep++ = CBACK; *ep++ = c-'1'; continue; }
			*ep++ = CCHR;
			if (c=='\n') goto cerror;
			*ep++ = c;
			continue;
		case '.': *ep++ = CDOT; continue;
		case '\n': goto cerror;
		case '*':
			if (lastep==0 || *lastep==CBRA || *lastep==CKET) goto defchar;
			*lastep |= STAR;
			continue;
		case '$':
			if ((peekc=getchr()) != eof && peekc!='\n') goto defchar;
			*ep++ = CDOL;
			continue;
		case '[':
			*ep++ = CCL;
			*ep++ = 0;
			cclcnt = 1;
			if ((c=getchr()) == '^') {c = getchr(); ep[-2] = NCCL; }
			do {
				if (c=='\n') goto cerror;
				if (c=='-' && ep[-1]!=0) {
					if ((c=getchr())==']') { *ep++ = '-'; cclcnt++; break; }
					while (ep[-1]<c) {
						*ep = ep[-1]+1;
						ep++;
						cclcnt++;
						if (ep>=&expbuf[ESIZE]) goto cerror;
					}
				}
				*ep++ = c;
				cclcnt++;
				if (ep >= &expbuf[ESIZE]) goto cerror;
			} while ((c = getchr()) != ']');
			lastep[1] = cclcnt;
			continue;
		defchar: default: *ep++ = CCHR; *ep++ = c;
		}
	}
   cerror: expbuf[0] = 0; nbra = 0;
}

int execute(unsigned int *addr) {
	char *p1, *p2 = expbuf;
	int c;
	if (addr == (unsigned *)0) {
		if (*p2==CCIRC) return(0);
		p1 = loc2;
	} else if (addr==zero) return(0);
	else p1 = getline(*addr);
	if (*p2==CCIRC) { loc1 = p1; return(advance(p1, p2+1));}
	/* fast check for first character */
	if (*p2==CCHR) {
		c = p2[1];
		do {
			if (*p1!=c) continue;
			if (advance(p1, p2)) { loc1 = p1; return(1); }
		} while (*p1++);
		return(0);
	}
	/* regular algorithm */
	do {
		if (advance(p1, p2)) { loc1 = p1; return(1); }
	} while (*p1++);
	return(0);
}

int advance(char *lp, char *ep) {
	char *curlp;
	int i;

	for (;;) switch (*ep++) {

	case CCHR:
		if (*ep++ == *lp++)
			continue;
		return(0);

	case CDOT:
		if (*lp++)
			continue;
		return(0);

	case CDOL:
		if (*lp==0)
			continue;
		return(0);

	case CEOF:
		loc2 = lp;
		return(1);

	case CCL:
		if (cclass(ep, *lp++, 1)) {
			ep += *ep;
			continue;
		}
		return(0);

	case NCCL:
		if (cclass(ep, *lp++, 0)) {
			ep += *ep;
			continue;
		}
		return(0);

	case CBRA:
		braslist[*ep++] = lp;
		continue;

	case CKET:
		braelist[*ep++] = lp;
		continue;

	case CBACK:
		if (braelist[i = *ep++]==0)
		if (backref(i, lp)) {
			lp += braelist[i] - braslist[i];
			continue;
		}
		return(0);

	case CBACK|STAR:
		if (braelist[i = *ep++] == 0)
		curlp = lp;
		while (backref(i, lp))
			lp += braelist[i] - braslist[i];
		while (lp >= curlp) {
			if (advance(lp, ep))
				return(1);
			lp -= braelist[i] - braslist[i];
		}
		continue;

	case CDOT|STAR:
		curlp = lp;
		while (*lp++)
			;
		goto star;

	case CCHR|STAR:
		curlp = lp;
		while (*lp++ == *ep)
			;
		ep++;
		goto star;

	case CCL|STAR:
	case NCCL|STAR:
		curlp = lp;
		while (cclass(ep, *lp++, ep[-1]==(CCL|STAR)))
			;
		ep += *ep;
		goto star;

	star:
		do {
			lp--;
			if (advance(lp, ep))
				return(1);
		} while (lp > curlp);
		return(0);
	}
}


void puts(char *sp) { while (*sp) putchr(*sp++); putchr('\n'); }

char line[70], *linp	= line;

void putchr(int ac) {
	char *lp;
	int c;
	lp = linp; c = ac; *lp++ = c;
	if(c == '\n' || lp >= &line[64]) { linp = line; write(oflag?2:1, line, lp-line); return;}
	linp = lp;
}

int backref(int i, char *lp) {
	char *bp;

	bp = braslist[i];
	while (*bp++ == *lp++)
		if (bp >= braelist[i])
			return(1);
	return(0);
}

int cclass(char *set, int c, int af) {
	int n;

	if (c==0)
		return(0);
	n = *set++;
	while (--n)
		if (*set++ == c)
			return(af);
	return(!af);
}