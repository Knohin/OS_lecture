#include <stdio.h>
#include <stdlib.h>
#include "buf.h"
#include "queue.h"
#include "Disk.h"
#include <string.h> // memcpy

// DIsk.h : #define BLOCK_SIZE (32)

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

void BufRead(int blkno, char* pData)
{
	Buf *targetBuf;	// Buffer to Read

	// find blkno in buffer list
	TAILQ_FOREACH(targetBuf, &pBufList, blist)
		if (targetBuf->blkno == blkno)
			break;

	if (targetBuf == NULL)	// blkno�� buffer list�� ���ٸ�
	{
		if (gBufNumInCache >= MAX_BUF_NUM)	// IF.  ĳ�� ������,  (��, Buffer replacement)
		{
			Buf *victim = TAILQ_FIRST(&pLruListHead); // victim ����
			if (victim->state == BUF_STATE_DIRTY)	// IF. victim�� dirty��,
				DevWriteBlock(blkno, (char*)targetBuf->pMem);// disk�� �����ϰ�

			 // bufferlist���� victim ����
			TAILQ_REMOVE(&pBufList, victim, blist);
			// clean/dirty list���� victim ����
			TAILQ_REMOVE(&ppStateListHead[victim->state], victim, slist);
			// LRU list���� victim ����
			TAILQ_REMOVE(&pLruListHead, victim, llist);
			// (������ ���� free ����, target���� ����)
			targetBuf = victim;
		}
		else {	// ĳ�� �γ��ϸ�, Buf����ü �Ҵ�
			targetBuf = (Buf*)malloc(sizeof(Buf));
			targetBuf->pMem = malloc(BLOCK_SIZE);
			gBufNumInCache++;
		}
		// blkno ����
		targetBuf->blkno = blkno;
		// ��� �о����;
		DevReadBlock(blkno, (char*)targetBuf->pMem);

		// buffer list�� HEAD�� �ִ´�
		TAILQ_INSERT_HEAD(&pBufList, targetBuf, blist);
		// clean list�� TAIL�� �ִ´�
		targetBuf->state = BUF_STATE_CLEAN;
		TAILQ_INSERT_TAIL(&ppStateListHead[BUF_LIST_CLEAN], targetBuf, slist);
	}
	else
		TAILQ_REMOVE(&pLruListHead, targetBuf, llist);

	// �ش��ϴ� blk�� pMem�� pData�� '�����Ѵ�.'(��, ������ �ѱ�� ����)
	memcpy(pData, targetBuf->pMem, BLOCK_SIZE);
	// ���� ���ٵǴ� buffer�� LRU list�� tail�� �ִ´�
	TAILQ_INSERT_TAIL(&pLruListHead, targetBuf, llist);

	return;
}

// pMem�� ���ְ� CleanList��������, DirtyList�� �߰� (�̹� ��Ƽ�鹹 ���ΰ�)
void BufWrite(int blkno, char* pData)
{
	Buf *targetBuf;	// Buffer to Write

	// find blkno in buffer list
	TAILQ_FOREACH(targetBuf, &pBufList, blist)
		if (targetBuf->blkno == blkno)
			break;

	if (targetBuf == NULL)	// blkno�� buffer list�� ���ٸ�
	{
		if (gBufNumInCache >= MAX_BUF_NUM)	// IF.  ĳ�� ������,  (��, Buffer replacement)
		{
			Buf *victim = TAILQ_FIRST(&pLruListHead); // victim ����
			if (victim->state == BUF_STATE_DIRTY)	// IF. victim�� dirty��,
				DevWriteBlock(blkno, (char*)targetBuf->pMem);// disk�� �����ϰ�

			// bufferlist���� victim ����
			TAILQ_REMOVE(&pBufList, victim, blist);
			// clean/dirty list���� victim ����
			TAILQ_REMOVE(&ppStateListHead[victim->state], victim, slist);
			// LRU list���� victim ����
			TAILQ_REMOVE(&pLruListHead, victim, llist);
			// (������ ���� free ����, target���� ����)
			targetBuf = victim;
		}
		else {	// ĳ�� �γ��ϸ�, Buf����ü �Ҵ�
			targetBuf = (Buf*)malloc(sizeof(Buf));
			targetBuf->pMem = malloc(BLOCK_SIZE);
			gBufNumInCache++;
		}

		// blkno ����
		targetBuf->blkno = blkno;
		// ��� �о����;
		DevReadBlock(blkno, (char*)targetBuf->pMem);

		// buffer list�� HEAD�� �ִ´�
		TAILQ_INSERT_HEAD(&pBufList, targetBuf, blist);
		// dirty list�� TAIL�� �ִ´�
		targetBuf->state = BUF_STATE_DIRTY;
		TAILQ_INSERT_TAIL(&ppStateListHead[BUF_LIST_DIRTY], targetBuf, slist);
	}
	else
		TAILQ_REMOVE(&pLruListHead, targetBuf, llist);

	// pData�� blkno�� pMem�� '�����Ѵ�' (��, ������ �ѱ�� ����)
	memcpy(targetBuf->pMem, pData, BLOCK_SIZE);
	if (targetBuf->state == BUF_STATE_CLEAN) // IF. clean list�� �ִٸ�?
	{
		targetBuf->state = BUF_STATE_DIRTY;
		// buffer�� clean list���� ����
		TAILQ_REMOVE(&ppStateListHead[BUF_LIST_CLEAN], targetBuf, slist);
		// buffer�� dirty list�� TAIL�� �߰�
		TAILQ_INSERT_TAIL(&ppStateListHead[BUF_LIST_DIRTY], targetBuf, slist);
	}

	// ���� ���ٵǴ� buffer�� LRU list�� tail�� �ִ´�
	TAILQ_INSERT_TAIL(&pLruListHead, targetBuf, llist);

	return;
}

// dirty list�� ����� buffer�� ��ϵ��� ��ũ�� �����Ѵ�. ��, HEAD����
// ���� �Ŀ�, buffer�� dirty list���� clean list�� tail�� �̵��Ѵ�.
void BufSync(void)
{
	Buf *item;
	// WHILE. dirty list�� head�� null�� �ƴϸ�
	TAILQ_FOREACH(item, &ppStateListHead[BUF_LIST_DIRTY], slist)
	{
		// buffer�� DevWriteBlock(..)���� ��ũ�� ����
		DevWriteBlock(item->blkno, item->pMem);

		// buffer�� clean list�� TAIL�� �̵�
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
		/// �� �����ؼ� �Ѱ�?
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
		/// �� �����ؼ� �Ѱ�?
		(*ppBufInfo)[i++].blkno = item->blkno;
	}

	return;
}

