#include <stdio.h>
#include <stdlib.h>
#include "buf.h"
#include "queue.h"
#include "DIsk.h"

int gBufCount = 0;

void BufInit(void)
{
	// 리스트 헤드 초기화
	gBufCount = 0;
	TAILQ_INIT(pBufList);
	for (int i = 0; i < MAX_BUFLIST_NUM; i++)
		TAILQ_INIT(ppStateListHead[i]);
	TAILQ_INIT(pLruListHead);
}

void BufRead(int blkno, char* pData)
{
	Buf *targetBuf;
	TAILQ_FOREACH(targetBuf, pBufList, blist)
		if (targetBuf->blkno == blkno)
			break;
	// IF. blkno가 buffer list에 없다면
		// IF. Buffer replacement /// 캐시 꽉참
			// victim = LRU list의 HEAD;
			// IF. victim이 dirty면,
				// disk에 저장하고

			// (재사용을 위해)
			// bufferlist에서 victim 삭제
			// clean/dirty list에서 victim 삭제
			// LRU list에서 victim 삭제

		// ELSE. /// 캐시 널널
			// Buf구조체 할당
			// pMem 할당
		// blkno 세팅
		// DevReadBlock(blkno, pMem);

		// buffer list의 HEAD에 넣는다
		// clean list의 TAIL에 넣는다
	
	// 해당하는 blkno의 pMem을 pData에 '복사한다.'(즉, 포인터 넘기기 ㄴㄴ)
	// 현재 접근되는 buffer는 LRU list의 tail에 넣는다
	return;
}

// pMem에 써주고 CleanList에서제외, DirtyList에 추가 (이미 더티면뭐 냅두고)
void BufWrite(int blkno, char* pData)
{
	// IF. blkno가 buffer list에 없다면
		// IF. Buffer replacement /// 캐시 꽉참
			// victim = LRU list의 HEAD;
			// IF. victim이 dirty면,
				// disk에 저장하고

			// (재사용을 위해)
			// bufferlist에서 victim 삭제
			// clean/dirty list에서 victim 삭제
			// LRU list에서 victim 삭제

		// ELSE. /// 캐시 널널
			// Buf구조체 할당
			// pMem 할당
		// blkno 세팅
		// pData를 blkno의 pMem에 '복사한다' (즉, 포인터 넘기기 ㄴㄴ)

		// buffer list의 HEAD에 넣는다
		// dirty list의 TAIL에 넣는다

	// ELSE
		// pData를 blkno의 pMem에 '복사한다' (즉, 포인터 넘기기 ㄴㄴ)
		// LRU list에서 삭제
		// IF clean list에 있다면?
			// buffer를 clean list에서 삭제
			// buffer를 dirty list의 TAIL에 추가

	// 현재 접근되는 buffer는 LRU list의 tail에 넣는다
	return;
}

// dirty list에 연결된 buffer의 블록들을 디스크로 저장한다. 단, HEAD부터
// 저장 후에, buffer는 dirty list에서 clean list의 tail로 이동한다.
void BufSync(void)
{
	// WHILE. dirty list의 head가 null이 아니면
		// buffer를 DevWriteBlock(..)으로 디스크에 저장
		// buffer를 clean list의 TAIL로 이동
}



/*
 * GetBufInfoByListNum: Get all buffers in a list specified by listnum.
 *                      This function receives a memory pointer to "ppBufInfo" that can contain the buffers.
 */
void GetBufInfoByListNum(StateList listnum, Buf** ppBufInfo, int* pNumBuf)
{

}



/*
 * GetBufInfoInLruList: Get all buffers in a list specified at the LRU list.
 *                         This function receives a memory pointer to "ppBufInfo" that can contain the buffers.
 */
void GetBufInfoInLruList(Buf** ppBufInfo, int* pNumBuf)
{

}

