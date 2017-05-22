#include <stdio.h>
#include "fat.h"
#include "Disk.h"

// lastBlkNum에 해당하는 디스크 블럭계산
int getBlockNum(int blockIndex)
{
	return blockIndex / (BLOCK_SIZE / 4);
}

void FatInit(void)
{
	// 한블럭 크기의 메모리를 할당
	int bufBlock[BLOCK_SIZE / 4];

	DevCreateDisk();
	
	DevOpenDisk();
	
	for (int i = 0; i < NUM_FAT_BLOCK; i++)
	{
		// 0으로 채운다.
		for (int j = 0; j < BLOCK_SIZE / 4; j++)
			bufBlock[j] = 0;

		// 디스크에 저장
		DevWriteBlock(i, (char*)bufBlock);
	}
}


/* newBlkNum이 지정하는 FAT entry의 value가 0이 아니면 -1을 리턴함.
lastBlkNum이 지정하는 FAT entry의 값이 -1이 아니면 -1을 리턴함. */
int FatAdd(int lastBlkNum, int newBlkNum)
{	
	// BLock 크기의 메모리 할당
	int bufBlockOfLast[BLOCK_SIZE / 4];
	int bufBlockOfNew[BLOCK_SIZE / 4];

	// lastBlkNum에 해당하는 디스크 블럭계산
	int lastEntryBlkNum = getBlockNum(lastBlkNum);
	int newEntryBlkNum = getBlockNum(newBlkNum);
	int lastBlkBufIdx = lastBlkNum % (BLOCK_SIZE / 4);
	int newBlkBufIdx = newBlkNum % (BLOCK_SIZE / 4);

	/// TODO : lastBLkNum==-1 이면 빈 곳에 엔트리 추가
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

	// if, lastBlkNum이 지정하는 FAT entry의 값이 -1이 아니면 -1을 리턴함.
	if (bufBlockOfLast[lastBlkBufIdx] != -1)
		return -1;

	bufBlockOfLast[lastBlkBufIdx] = newBlkNum;

	// if, newBlkNum에 해당하는 디스크즐록이 다른 디스크 블록이라면
	if (lastEntryBlkNum != newEntryBlkNum)
	{
		DevReadBlock(newEntryBlkNum, (char*)bufBlockOfNew);
		// if, newBlkNum이 지정하는 FAT entry의 value가 0이 아니면 -1을 리턴함
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

	// 버퍼에 있는 내용 쓰기
	if (lastEntryBlkNum != newEntryBlkNum)
	{
		DevWriteBlock(newEntryBlkNum, (char*)bufBlockOfNew);
		DevWriteBlock(lastEntryBlkNum, (char*)bufBlockOfLast);
	}
	else
		DevWriteBlock(newEntryBlkNum, (char*)bufBlockOfLast);

	return 0;
}

/* firstBlkNum이 지정하는 FAT entry의 value가 0이거나
   logicalBlkNum에 대응하는 physical block 번호가 -1이거나 0인 경우, -1을 리턴함 */
int FatGetBlockNum(int firstBlkNum, int logicalBlkNum)
{
	// Block크기의 메모리 할당
	int bufBlock[BLOCK_SIZE / 4];

	// firstBlkNum이 있는 디스크 블록 넘버 계산
	int curEntry= firstBlkNum;
	int curBlockNum = getBlockNum(curEntry);
	int bufIdxOfCurEntry = curEntry % (BLOCK_SIZE / 4);

	int nextEntry;
	int nextBlockNum;
	int bufIdxOfNextEntry;

	int resultBlkNum = curEntry;

	// ReadBlock();
	DevReadBlock(curBlockNum, (char*)bufBlock);
	// if, firstBlkNum이 지정하는 FAT entry의 value가 0일떄
	if (bufBlock[bufIdxOfCurEntry] == 0)
		return -1;

	nextEntry = bufBlock[bufIdxOfCurEntry];

	for (int i = 0; i < logicalBlkNum; i++)
	{
		// if, logicalBlkNum에 대응하는 physical block 번호가 -1이거나 0인 경우
		if ((nextEntry == -1) || (nextEntry == 0))
			return -1;

		nextBlockNum = getBlockNum(nextEntry);
		// if, 다음 블록 위치 != 같은 디스크 블록
		if (curBlockNum != nextBlockNum)
			DevReadBlock(nextBlockNum, (char*)bufBlock);

		curEntry = nextEntry;
		bufIdxOfCurEntry = curEntry % (BLOCK_SIZE / 4);
		nextEntry = bufBlock[bufIdxOfCurEntry];
		resultBlkNum = curEntry;
	}

	return resultBlkNum;
}

/* firstBlkNum이 지정하는 FAT entry의 value가 0이거나
   startBlkNm이 지정하는 FAT entry의 value가 0인 경우, -1을 리턴함.*/
int FatRemove(int firstBlkNum, int startBlkNum)
{
	// Block크기의 메모리 할당
	int bufBlock[BLOCK_SIZE / 4];

	int deletedEntryCount = 0;

	int curEntry = firstBlkNum;
	int curBlockNum = getBlockNum(curEntry);
	int bufIdxOfCurEntry = curEntry % (BLOCK_SIZE / 4);

	int nextEntry;
//	int nextBlockNum;
//	int bufIdxOfNextEntry;

	DevReadBlock(curBlockNum, (char*)bufBlock);
	// if, firstBlkNum이 지정하는 FAT entry의 value가 0인경우 -1리턴
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

	// loop1 : startBlkNum까지 찾아감
	while (nextEntry != startBlkNum)
	{
		curEntry = nextEntry;
		curBlockNum = getBlockNum(curEntry);
		bufIdxOfCurEntry = curEntry % (BLOCK_SIZE / 4);

		DevReadBlock(curBlockNum, (char*)bufBlock);

		nextEntry = bufBlock[bufIdxOfCurEntry];
//		nextBlockNum = getBlockNum(nextEntry);
//		bufIdxOfNextEntry = nextEntry % (BLOCK_SIZE / 4);

		if (nextEntry == -1) // startEntry를 찾을 수 없는 경우
			return -1;
	}

	bufBlock[bufIdxOfCurEntry] = -1;
	DevWriteBlock(curBlockNum, (char*)bufBlock);

	// loop2 : startBlkNum 뒤에것들 다 제거
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