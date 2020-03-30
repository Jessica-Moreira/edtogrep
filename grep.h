#include <signal.h>
#include <setjmp.h>

/* make BLKSIZE and LBSIZE 512 for smaller machines */
#define	BLKSIZE	4096
#define	NBLK	2047

#define	NULL	0
#define	FNSIZE	128
#define	LBSIZE	4096
#define	ESIZE	256
#define	GBSIZE	256
#define	NBRA	5
#define	EOF		-1
#define	KSIZE	9

#define	CBRA	1
#define	CCHR	2
#define	CDOT	4
#define	CCL		6
#define	NCCL	8
#define	CDOL	10
#define	CEOF	11
#define	CKET	12
#define	CBACK	14
#define	CCIRC	15

#define	STAR	01

char	Q[]	= "";
char	T[]	= "TMP";
#define	READ	0
#define	WRITE	1

int	peekc, lastc, given, ninbuf, io, pflag;
unsigned int *addr1, *addr2, *dot, *dol, *zero;
char	genbuf[LBSIZE], savedfile[FNSIZE], file[FNSIZE], linebuf[LBSIZE], expbuf[ESIZE+4], inputarry[128], *nextip, *input;
long	count;

int	tfile	= -1;
int	oblock	= -1;
char obuff[BLKSIZE], *tfname, *loc1, *loc2, *globp;
int	nbra, names[26], nleft, tline, oflag;
unsigned nlall = 128;

void	*malloc(unsigned long);
char	*mkstemp(char *);
char	tmpXXXXX[50] = "/tmp/eXXXXX";

jmp_buf	savej;

typedef void	(*SIG_TYP)(int);
SIG_TYP	oldhup;
SIG_TYP	oldquit;
/* these two are not in ansi, but we need them */
#define	SIGHUP	1	/* hangup */
#define	SIGQUIT	3	/* quit (ASCII FS) */

int	open(char *, int);
int	read(int, char*, int);
int	write(int, char*, int);
void	exit(int);

char	*braelist[NBRA];
char	*braslist[NBRA];

int cclass(char *set, int c, int af);
int backref(int i, char *lp);
char *getblock(unsigned int atl, int iof);
char *getline(unsigned int tl);
int advance(char *lp, char *ep);
int append(int (*f)(void), unsigned int *a);
void commands(void);
void compile(int eof);
int execute(unsigned int *addr);
void filename(int comm);
int getchr(void);
int getfile(void);
void global(int k);
void init(void);
unsigned int *address(void);
void newline(void);
void print(void);
void putchr(int ac);
int putline(void);
void puts(char *sp);
void quit(int n);
void setwide(void);
int sprintf(char *,const char *, ...);