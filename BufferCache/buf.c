#include <stdio.h>
#include <stdlib.h>
#include "buf.h"
#include "queue.h" // <sys/queue.h>로 수정해서 실행 해야 되는데 헤더파일에도 이게 있는데?
#include "Disk.h"
#include <string.h> // memcpy

int gBufNumInCache = 0;

void BufInit(void)
{
	// 리스트 헤드 초기화
	gBufNumInCache = 0;
	TAILQ_INIT(&pBufList);
	for (int i = 0; i < MAX_BUFLIST_NUM; i++)
		TAILQ_INIT(&ppStateListHead[i]);
	TAILQ_INIT(&pLruListHead);

	// Disk.h 파일 초기화
	DevCreateDisk();
	DevOpenDisk();
}

Buf* getNewBuffer(int blkno) /// 이름이...
{
	Buf* newBuf;
	if (gBufNumInCache >= MAX_BUF_NUM)	// IF.  캐시 꽉차면,  (즉, Buffer replacement)
	{
		Buf *victim = TAILQ_FIRST(&pLruListHead); // victim 설정
		if (victim->state == BUF_STATE_DIRTY)	// IF. victim이 dirty면,
			DevWriteBlock(blkno, (char*)victim->pMem);// disk에 저장하고

		TAILQ_REMOVE(&pBufList, victim, blist);
		TAILQ_REMOVE(&ppStateListHead[victim->state], victim, slist);
		TAILQ_REMOVE(&pLruListHead, victim, llist);

		newBuf = victim;
	}
	else {	// 캐시 널널하면, Buf구조체 새로 할당
		newBuf = (Buf*)malloc(sizeof(Buf));
		newBuf->pMem = malloc(BLOCK_SIZE);
		gBufNumInCache++;
	}

	return newBuf;
}

void BufRead(int blkno, char* pData)
{
	Buf *targetBuf;	// Buffer to Read

	// find blkno in buffer list
	TAILQ_FOREACH(targetBuf, &pBufList, blist)
		if (targetBuf->blkno == blkno)
			break;

	if (targetBuf == NULL)	// blkno가 buffer list에 없다면
	{
		targetBuf = getNewBuffer(blkno);

		// targetBuf 세팅
		targetBuf->blkno = blkno;
		DevReadBlock(blkno, (char*)targetBuf->pMem);
		targetBuf->state = BUF_STATE_CLEAN;
		TAILQ_INSERT_HEAD(&pBufList, targetBuf, blist);
		TAILQ_INSERT_TAIL(&ppStateListHead[BUF_LIST_CLEAN], targetBuf, slist);
	}
	else
		TAILQ_REMOVE(&pLruListHead, targetBuf, llist);

	// 해당하는 blk의 pMem을 pData에 '복사한다.'(즉, 포인터 넘기기 ㄴㄴ)
	memcpy(pData, targetBuf->pMem, BLOCK_SIZE);
	TAILQ_INSERT_TAIL(&pLruListHead, targetBuf, llist);

	return;
}

void BufWrite(int blkno, char* pData)
{
	Buf *targetBuf;	// Buffer to Write

	// find blkno in buffer list
	TAILQ_FOREACH(targetBuf, &pBufList, blist)
		if (targetBuf->blkno == blkno)
			break;

	if (targetBuf == NULL)	// blkno가 buffer list에 없다면
	{
		targetBuf = getNewBuffer(blkno);

		// targetBuf 세팅
		targetBuf->blkno = blkno;
		DevReadBlock(blkno, (char*)targetBuf->pMem);
		targetBuf->state = BUF_STATE_DIRTY;
		TAILQ_INSERT_HEAD(&pBufList, targetBuf, blist);
		TAILQ_INSERT_TAIL(&ppStateListHead[BUF_LIST_DIRTY], targetBuf, slist);
	}
	else
		TAILQ_REMOVE(&pLruListHead, targetBuf, llist);

	// pData를 blkno의 pMem에 '복사한다' (즉, 포인터 넘기기 ㄴㄴ)
	memcpy(targetBuf->pMem, pData, BLOCK_SIZE);
	if (targetBuf->state == BUF_STATE_CLEAN) // IF. clean list에 있다면?
	{
		targetBuf->state = BUF_STATE_DIRTY;
		TAILQ_REMOVE(&ppStateListHead[BUF_LIST_CLEAN], targetBuf, slist);
		TAILQ_INSERT_TAIL(&ppStateListHead[BUF_LIST_DIRTY], targetBuf, slist);
	}
	TAILQ_INSERT_TAIL(&pLruListHead, targetBuf, llist);

	return;
}

// dirty list에 연결된 buffer의 블록들을 디스크로 저장한다. 단, HEAD부터
// 저장 후에, buffer는 dirty list에서 clean list의 tail로 이동한다.
void BufSync(void)
{
	Buf *item;
	// WHILE. dirty list의 head가 null이 아니면
	TAILQ_FOREACH(item, &ppStateListHead[BUF_LIST_DIRTY], slist)
	{
		// buffer를  디스크에 저장
		DevWriteBlock(item->blkno, item->pMem);

		// buffer를 clean list의 TAIL로 이동
		TAILQ_REMOVE(&ppStateListHead[BUF_LIST_DIRTY], item, slist);
		item->state = BUF_STATE_CLEAN;
		TAILQ_INSERT_TAIL(&ppStateListHead[BUF_LIST_CLEAN], item, slist);
	}
	return;
}



/*
* GetBufInfoByListNum: Get all buffers in a list specified by listnum.
*                      This function receives a memory pointer to "ppBufInfo" that can contain the buffers.
*/
void GetBufInfoByListNum(StateList listnum, Buf** ppBufInfo, int* pNumBuf)
{// MYprecondition : ppBufInfo is NOT allocated. pNumBUf IS allocated
	Buf *item;
	int bufNum, i;

	TAILQ_FOREACH(item, &ppStateListHead[listnum], slist)
		bufNum++;
	*ppBufInfo = (Buf*)malloc(sizeof(Buf)*bufNum);

	*pNumBuf = bufNum;
	i = 0;
	TAILQ_FOREACH(item, &ppStateListHead[listnum], slist)
	{
		/// 다 복사해서 넘겨?
		(*ppBufInfo)[i++].blkno = item->blkno;
	}

	return;
}



/*
* GetBufInfoInLruList: Get all buffers in a list specified at the LRU list.
*                         This function receives a memory pointer to "ppBufInfo" that can contain the buffers.
*/
void GetBufInfoInLruList(Buf** ppBufInfo, int* pNumBuf)
{// MYprecondition : ppBufInfo is NOT allocated. pNumBUf IS allocated
	Buf *item;
	int i;

	*ppBufInfo = (Buf*)malloc(sizeof(Buf)*gBufNumInCache);

	*pNumBuf = gBufNumInCache;
	i = 0;
	TAILQ_FOREACH(item, &pLruListHead, llist)
	{
		/// 다 복사해서 넘겨?
		(*ppBufInfo)[i++].blkno = item->blkno;
	}

	return;
}

