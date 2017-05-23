#include <stdio.h>
#include <stdlib.h>
#include "buf.h"

int main(void) 
{
	char* cbuf;
	cbuf = (char*)malloc(32);
	for (int i = 0; i < 31; i++)
		cbuf[i] = 'a'+ (char)i;
	cbuf[32] = '\0';
	printf("cbuf: %s\n", cbuf);

	BufInit();
	BufWrite(0, cbuf);
	BufWrite(2, cbuf);

	BufRead(2, cbuf);
	printf("2: %s\n", cbuf);
	BufRead(1, cbuf);
	printf("1: %s\n", cbuf);
	BufRead(0, cbuf);
	printf("0: %s\n", cbuf);

	int bufNum;
	Buf* pBuf;

	GetBufInfoInLruList(&pBuf, &bufNum);
	printf("bufNum : %d\n", bufNum);
	for (int i = 0; i < bufNum; i++)
		printf(" %d ->", pBuf[i].blkno);
	printf("\n");

	return 0;
}
