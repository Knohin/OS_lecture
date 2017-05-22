#include <stdio.h>
#include "fat.h"
#include "Disk.h"

// lastBlkNum�� �ش��ϴ� ��ũ �����
int getBlockNum(int blockIndex)
{
	return blockIndex / (BLOCK_SIZE / 4);
}

void FatInit(void)
{
	// �Ѻ� ũ���� �޸𸮸� �Ҵ�
	int bufBlock[BLOCK_SIZE / 4];

	DevCreateDisk();
	
	DevOpenDisk();
	
	for (int i = 0; i < NUM_FAT_BLOCK; i++)
	{
		// 0���� ä���.
		for (int j = 0; j < BLOCK_SIZE / 4; j++)
			bufBlock[j] = 0;

		// ��ũ�� ����
		DevWriteBlock(i, (char*)bufBlock);
	}
}


/* newBlkNum�� �����ϴ� FAT entry�� value�� 0�� �ƴϸ� -1�� ������.
lastBlkNum�� �����ϴ� FAT entry�� ���� -1�� �ƴϸ� -1�� ������. */
int FatAdd(int lastBlkNum, int newBlkNum)
{	
	// BLock ũ���� �޸� �Ҵ�
	int bufBlockOfLast[BLOCK_SIZE / 4];
	int bufBlockOfNew[BLOCK_SIZE / 4];

	// lastBlkNum�� �ش��ϴ� ��ũ �����
	int lastEntryBlkNum = getBlockNum(lastBlkNum);
	int newEntryBlkNum = getBlockNum(newBlkNum);
	int lastBlkBufIdx = lastBlkNum % (BLOCK_SIZE / 4);
	int newBlkBufIdx = newBlkNum % (BLOCK_SIZE / 4);

	/// TODO : lastBLkNum==-1 �̸� �� ���� ��Ʈ�� �߰�
	if (lastBlkNum == -1)
	{
		DevReadBlock(newEntryBlkNum, (char *)bufBlockOfNew);
		if (bufBlockOfNew[newBlkBufIdx] != 0)
			return -1;
		bufBlockOfNew[newBlkBufIdx] = -1;
		DevWriteBlock(newEntryBlkNum, (char *)bufBlockOfNew);
		return 0;
	}

	// ReadBlock();
	DevReadBlock(lastEntryBlkNum, (char*)bufBlockOfLast);

	// if, lastBlkNum�� �����ϴ� FAT entry�� ���� -1�� �ƴϸ� -1�� ������.
	if (bufBlockOfLast[lastBlkBufIdx] != -1)
		return -1;

	bufBlockOfLast[lastBlkBufIdx] = newBlkNum;

	// if, newBlkNum�� �ش��ϴ� ��ũ����� �ٸ� ��ũ ����̶��
	if (lastEntryBlkNum != newEntryBlkNum)
	{
		DevReadBlock(newEntryBlkNum, (char*)bufBlockOfNew);
		// if, newBlkNum�� �����ϴ� FAT entry�� value�� 0�� �ƴϸ� -1�� ������
		if (bufBlockOfNew[newBlkBufIdx] != 0)
			return -1;
		bufBlockOfNew[newBlkBufIdx] = -1;
	}
	else
	{
		if (bufBlockOfLast[newBlkBufIdx] != 0)
			return -1;
		bufBlockOfLast[newBlkBufIdx] = -1;
	}

	// ���ۿ� �ִ� ���� ����
	if (lastEntryBlkNum != newEntryBlkNum)
	{
		DevWriteBlock(newEntryBlkNum, (char*)bufBlockOfNew);
		DevWriteBlock(lastEntryBlkNum, (char*)bufBlockOfLast);
	}
	else
		DevWriteBlock(newEntryBlkNum, (char*)bufBlockOfLast);

	return 0;
}

/* firstBlkNum�� �����ϴ� FAT entry�� value�� 0�̰ų�
   logicalBlkNum�� �����ϴ� physical block ��ȣ�� -1�̰ų� 0�� ���, -1�� ������ */
int FatGetBlockNum(int firstBlkNum, int logicalBlkNum)
{
	// Blockũ���� �޸� �Ҵ�
	int bufBlock[BLOCK_SIZE / 4];

	// firstBlkNum�� �ִ� ��ũ ��� �ѹ� ���
	int curEntry= firstBlkNum;
	int curBlockNum = getBlockNum(curEntry);
	int bufIdxOfCurEntry = curEntry % (BLOCK_SIZE / 4);

	int nextEntry;
	int nextBlockNum;
	int bufIdxOfNextEntry;

	int resultBlkNum = curEntry;

	// ReadBlock();
	DevReadBlock(curBlockNum, (char*)bufBlock);
	// if, firstBlkNum�� �����ϴ� FAT entry�� value�� 0�ϋ�
	if (bufBlock[bufIdxOfCurEntry] == 0)
		return -1;

	nextEntry = bufBlock[bufIdxOfCurEntry];

	for (int i = 0; i < logicalBlkNum; i++)
	{
		// if, logicalBlkNum�� �����ϴ� physical block ��ȣ�� -1�̰ų� 0�� ���
		if ((nextEntry == -1) || (nextEntry == 0))
			return -1;

		nextBlockNum = getBlockNum(nextEntry);
		// if, ���� ��� ��ġ != ���� ��ũ ���
		if (curBlockNum != nextBlockNum)
			DevReadBlock(nextBlockNum, (char*)bufBlock);

		curEntry = nextEntry;
		bufIdxOfCurEntry = curEntry % (BLOCK_SIZE / 4);
		nextEntry = bufBlock[bufIdxOfCurEntry];
		resultBlkNum = curEntry;
	}

	return resultBlkNum;
}

/* firstBlkNum�� �����ϴ� FAT entry�� value�� 0�̰ų�
   startBlkNm�� �����ϴ� FAT entry�� value�� 0�� ���, -1�� ������.*/
int FatRemove(int firstBlkNum, int startBlkNum)
{
	// Blockũ���� �޸� �Ҵ�
	int bufBlock[BLOCK_SIZE / 4];

	int deletedEntryCount = 0;

	int curEntry = firstBlkNum;
	int curBlockNum = getBlockNum(curEntry);
	int bufIdxOfCurEntry = curEntry % (BLOCK_SIZE / 4);

	int nextEntry;
//	int nextBlockNum;
//	int bufIdxOfNextEntry;

	DevReadBlock(curBlockNum, (char*)bufBlock);
	// if, firstBlkNum�� �����ϴ� FAT entry�� value�� 0�ΰ�� -1����
	if (bufBlock[bufIdxOfCurEntry] == 0)
		return -1;

	nextEntry = bufBlock[bufIdxOfCurEntry];
//	nextBlockNum = getBlockNum(nextEntry);
//	bufIdxOfNextEntry = nextEntry % (BLOCK_SIZE / 4);


	if (firstBlkNum == startBlkNum)
	{
		bufBlock[bufIdxOfCurEntry] = 0;
		DevWriteBlock(curBlockNum, (char*)bufBlock);
		return 0;
	}

	// loop1 : startBlkNum���� ã�ư�
	while (nextEntry != startBlkNum)
	{
		curEntry = nextEntry;
		curBlockNum = getBlockNum(curEntry);
		bufIdxOfCurEntry = curEntry % (BLOCK_SIZE / 4);

		DevReadBlock(curBlockNum, (char*)bufBlock);

		nextEntry = bufBlock[bufIdxOfCurEntry];
//		nextBlockNum = getBlockNum(nextEntry);
//		bufIdxOfNextEntry = nextEntry % (BLOCK_SIZE / 4);

		if (nextEntry == -1) // startEntry�� ã�� �� ���� ���
			return -1;
	}

	bufBlock[bufIdxOfCurEntry] = -1;
	DevWriteBlock(curBlockNum, (char*)bufBlock);

	// loop2 : startBlkNum �ڿ��͵� �� ����
	do
	{
		curEntry = nextEntry;
		curBlockNum = getBlockNum(curEntry);
		bufIdxOfCurEntry = curEntry % (BLOCK_SIZE / 4);

		DevReadBlock(curBlockNum, (char*)bufBlock);

		nextEntry = bufBlock[bufIdxOfCurEntry];
//		nextBlockNum = getBlockNum(nextEntry);
//		bufIdxOfNextEntry = nextEntry % (BLOCK_SIZE / 4);

		bufBlock[bufIdxOfCurEntry] = 0;
		DevWriteBlock(curBlockNum, (char*)bufBlock);
		deletedEntryCount++;
	} while (nextEntry != -1);

	return deletedEntryCount;
}