#include <stdio.h>
#include <stdlib.h>
#include "buf.h"
#include "queue.h"
#include "DIsk.h"

int gBufCount = 0;

void BufInit(void)
{
	// ����Ʈ ��� �ʱ�ȭ
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
	// IF. blkno�� buffer list�� ���ٸ�
		// IF. Buffer replacement /// ĳ�� ����
			// victim = LRU list�� HEAD;
			// IF. victim�� dirty��,
				// disk�� �����ϰ�

			// (������ ����)
			// bufferlist���� victim ����
			// clean/dirty list���� victim ����
			// LRU list���� victim ����

		// ELSE. /// ĳ�� �γ�
			// Buf����ü �Ҵ�
			// pMem �Ҵ�
		// blkno ����
		// DevReadBlock(blkno, pMem);

		// buffer list�� HEAD�� �ִ´�
		// clean list�� TAIL�� �ִ´�
	
	// �ش��ϴ� blkno�� pMem�� pData�� '�����Ѵ�.'(��, ������ �ѱ�� ����)
	// ���� ���ٵǴ� buffer�� LRU list�� tail�� �ִ´�
	return;
}

// pMem�� ���ְ� CleanList��������, DirtyList�� �߰� (�̹� ��Ƽ�鹹 ���ΰ�)
void BufWrite(int blkno, char* pData)
{
	// IF. blkno�� buffer list�� ���ٸ�
		// IF. Buffer replacement /// ĳ�� ����
			// victim = LRU list�� HEAD;
			// IF. victim�� dirty��,
				// disk�� �����ϰ�

			// (������ ����)
			// bufferlist���� victim ����
			// clean/dirty list���� victim ����
			// LRU list���� victim ����

		// ELSE. /// ĳ�� �γ�
			// Buf����ü �Ҵ�
			// pMem �Ҵ�
		// blkno ����
		// pData�� blkno�� pMem�� '�����Ѵ�' (��, ������ �ѱ�� ����)

		// buffer list�� HEAD�� �ִ´�
		// dirty list�� TAIL�� �ִ´�

	// ELSE
		// pData�� blkno�� pMem�� '�����Ѵ�' (��, ������ �ѱ�� ����)
		// LRU list���� ����
		// IF clean list�� �ִٸ�?
			// buffer�� clean list���� ����
			// buffer�� dirty list�� TAIL�� �߰�

	// ���� ���ٵǴ� buffer�� LRU list�� tail�� �ִ´�
	return;
}

// dirty list�� ����� buffer�� ��ϵ��� ��ũ�� �����Ѵ�. ��, HEAD����
// ���� �Ŀ�, buffer�� dirty list���� clean list�� tail�� �̵��Ѵ�.
void BufSync(void)
{
	// WHILE. dirty list�� head�� null�� �ƴϸ�
		// buffer�� DevWriteBlock(..)���� ��ũ�� ����
		// buffer�� clean list�� TAIL�� �̵�
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

