#include <stdio.h>
#include <stdlib.h>
#include "buf.h"
#include "queue.h" // <sys/queue.h>�� �����ؼ� ���� �ؾ� �Ǵµ� ������Ͽ��� �̰� �ִµ�?

#include "Disk.h"
#include <string.h> // memcpy

int gBufNumInCache = 0;

void BufInit(void)
{
	// ����Ʈ ��� �ʱ�ȭ
	gBufNumInCache = 0;
	TAILQ_INIT(&pBufList);
	for (int i = 0; i < MAX_BUFLIST_NUM; i++)
		TAILQ_INIT(&ppStateListHead[i]);
	TAILQ_INIT(&pLruListHead);

	// Disk.h ���� �ʱ�ȭ
	DevCreateDisk();
	DevOpenDisk();
}

Buf* getNewBuffer(int blkno) /// �̸���...
{
	Buf* newBuf;
	if (gBufNumInCache >= MAX_BUF_NUM)	// IF.  ĳ�� ������,  (��, Buffer replacement)
	{
		Buf *victim = TAILQ_FIRST(&pLruListHead); // victim ����
		if (victim->state == BUF_STATE_DIRTY)	// IF. victim�� dirty��,
			DevWriteBlock(victim->blkno, (char*)victim->pMem);// disk�� �����ϰ�

		TAILQ_REMOVE(&pBufList, victim, blist);
		TAILQ_REMOVE(&ppStateListHead[victim->state], victim, slist);
		TAILQ_REMOVE(&pLruListHead, victim, llist);

		newBuf = victim;
	}
	else {	// ĳ�� �γ��ϸ�, Buf����ü ���� �Ҵ�
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

	if (targetBuf == NULL)	// blkno�� buffer list�� ���ٸ�
	{
		targetBuf = getNewBuffer(blkno);

		// targetBuf ����
		targetBuf->blkno = blkno;
		DevReadBlock(blkno, (char*)targetBuf->pMem);
		targetBuf->state = BUF_STATE_CLEAN;
		TAILQ_INSERT_HEAD(&pBufList, targetBuf, blist);
		TAILQ_INSERT_TAIL(&ppStateListHead[BUF_LIST_CLEAN], targetBuf, slist);
	}
	else
		TAILQ_REMOVE(&pLruListHead, targetBuf, llist);
	TAILQ_INSERT_TAIL(&pLruListHead, targetBuf, llist);

	// �ش��ϴ� blk�� pMem�� pData�� '�����Ѵ�.'(��, ������ �ѱ�� ����)
	memcpy(pData, targetBuf->pMem, BLOCK_SIZE);

	return;
}

void BufWrite(int blkno, char* pData)
{
	Buf *targetBuf;	// Buffer to Write

	// find blkno in buffer list
	TAILQ_FOREACH(targetBuf, &pBufList, blist)
		if (targetBuf->blkno == blkno)
			break;

	if (targetBuf == NULL)	// blkno�� buffer list�� ���ٸ�
	{
		targetBuf = getNewBuffer(blkno);

		// targetBuf ����
		targetBuf->blkno = blkno;
		targetBuf->state = BUF_STATE_DIRTY;
		TAILQ_INSERT_HEAD(&pBufList, targetBuf, blist);
		TAILQ_INSERT_TAIL(&ppStateListHead[BUF_LIST_DIRTY], targetBuf, slist);
	}
	else
		TAILQ_REMOVE(&pLruListHead, targetBuf, llist);
	TAILQ_INSERT_TAIL(&pLruListHead, targetBuf, llist);

	if (targetBuf->state == BUF_STATE_CLEAN) // IF. clean list�� �ִٸ�?
	{
		targetBuf->state = BUF_STATE_DIRTY;
		TAILQ_REMOVE(&ppStateListHead[BUF_LIST_CLEAN], targetBuf, slist);
		TAILQ_INSERT_TAIL(&ppStateListHead[BUF_LIST_DIRTY], targetBuf, slist);
	}

	// pData�� blkno�� pMem�� '�����Ѵ�' (��, ������ �ѱ�� ����)
	memcpy(targetBuf->pMem, pData, BLOCK_SIZE);

	return;
}

// dirty list�� ����� buffer�� ��ϵ��� ��ũ�� �����Ѵ�. ��, HEAD����
// ���� �Ŀ�, buffer�� dirty list���� clean list�� tail�� �̵��Ѵ�.
void BufSync(void)
{
	Buf *item;
	// WHILE. dirty list�� head�� null�� �ƴϸ�
	while ((item = TAILQ_FIRST(&ppStateListHead[BUF_LIST_DIRTY])) != NULL)
	{
		// buffer��  ��ũ�� ����
		DevWriteBlock(item->blkno, item->pMem);

		// buffer�� clean list�� TAIL�� �̵�
		TAILQ_REMOVE(&ppStateListHead[BUF_LIST_DIRTY], item, slist);
		item->state = BUF_STATE_CLEAN;
		TAILQ_INSERT_TAIL(&ppStateListHead[BUF_LIST_CLEAN], item, slist);
	}
}


/*
* GetBufInfoByListNum: Get all buffers in a list specified by listnum.
*                      This function receives a memory pointer to "ppBufInfo" that can contain the buffers.
*/
void GetBufInfoByListNum(StateList listnum, Buf** ppBufInfo, int* pNumBuf)
{
	Buf *item;
	int bufNum = 0, i = 0;

	TAILQ_FOREACH(item, &ppStateListHead[listnum], slist)
	{
		ppBufInfo[i++] = item;
		bufNum++;
	}
	*pNumBuf = bufNum;
}
/*
* GetBufInfoInLruList: Get all buffers in a list specified at the LRU list.
*                         This function receives a memory pointer to "ppBufInfo" that can contain the buffers.
*/
void GetBufInfoInLruList(Buf** ppBufInfo, int* pNumBuf)
{
	Buf *item;
	int i = 0;

	*pNumBuf = gBufNumInCache;
	TAILQ_FOREACH(item, &pLruListHead, llist)
		ppBufInfo[i++] = item;
}
/*
* GetBufInfoInBufferList: Get all buffers in the buffer list.
*                         This function receives a memory pointer to "ppBufInfo" that can contain the buffers.
*/
void GetBufInfoInBufferList(Buf** ppBufInfo, int* pNumBuf)
{
	Buf *item;
	int i = 0;

	*pNumBuf = gBufNumInCache;
	TAILQ_FOREACH(item, &pBufList, blist)
		ppBufInfo[i++] = item;
}