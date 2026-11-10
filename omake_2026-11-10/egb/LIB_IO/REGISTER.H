/*
	header for registers 
*/

/* double word registers */
struct DWORDREGS {
	unsigned int eax;
	unsigned int ebx;
	unsigned int ecx;
	unsigned int edx;
	unsigned int esi;
	unsigned int edi;
	unsigned int eflag;
	};
/* word registers */
struct WORDREGS {
	unsigned short ax;
	unsigned short __ax;
	unsigned short bx;
	unsigned short __bx;
	unsigned short cx;
	unsigned short __cx;
	unsigned short dx;
	unsigned short __dx;
	unsigned short si;
	unsigned short __si;
	unsigned short di;
	unsigned short __di;
	unsigned short cflag;
	unsigned short __cflag;
	};
/* byte registers */
struct BYTEREGS {
	unsigned char al, ah;
	unsigned short __ax;
	unsigned char bl, bh;
	unsigned short __bx;
	unsigned char cl, ch;
	unsigned short __cx;
	unsigned char dl, dh;
	unsigned short __dx;
	};

union REGS {
	struct DWORDREGS	e;
	struct WORDREGS		x;
	struct BYTEREGS		h;
	};

/* segment registers */
struct SREGS {
	unsigned short es;
	unsigned short cs;
	unsigned short ss;
	unsigned short ds;
	unsigned short fs;
	unsigned short gs;
	};

