#include <stdio.h>
#include <stdlib.h>
#include "fat.h"
#include "validate.h"
#include <assert.h>

#define FAT_START_ENTRY_NUM		(NUM_FAT_BLOCK)
#define FAT_END_ENTRY_NUM		(NUM_FAT_BLOCK*BLOCK_SIZE/sizeof(int) - 1)

void testcase1(void)
{
	int lblkno = 0; 
	int j = 0;
	int i = 0;

	FatInit();
	FatAdd(-1, FAT_START_ENTRY_NUM);
	for(i = FAT_START_ENTRY_NUM;i <= FAT_END_ENTRY_NUM - 2;i += 2) 
	{
		lblkno++;
		j = i + 2;	
		if (FatAdd(i, j) < 0) 
		{
			printf("testcase1: fail O.O\n");
			return ;
		}
		if (j != FatGetBlockNum(FAT_START_ENTRY_NUM, lblkno))
		{
			printf("testcase1: fail O.O\n");
			return ;
		}
		if (validateFatAdd(i, j) < 0)
		{
			printf("testcase1: fail O.O\n");
			return ;
		}
	}

	j = 0;
	lblkno = 0;
	FatAdd(-1, FAT_START_ENTRY_NUM + 1);
	for(i = FAT_START_ENTRY_NUM + 1;i <= FAT_END_ENTRY_NUM - 2;i += 2) 
	{
		lblkno++;
		j = i + 2;	
		if (FatAdd(i, j) < 0)
		{
			printf("testcase1: fail O.O\n");
			return ;
		}		
		if (j != FatGetBlockNum(FAT_START_ENTRY_NUM + 1, lblkno))
		{
			printf("testcase1: fail O.O\n");
			return ;
		}

		if (validateFatAdd(i, j) < 0)
		{
			printf("testcase1: fail O.O\n");
			return ;
		}
	}
	printf("testcase1: success!!!!!\n");
	return ;
}

void testFatRemove(int firstBlkNum, int startBlkNum)
{
	putFatRemoveArg(firstBlkNum, startBlkNum);
	if (FatRemove(firstBlkNum, startBlkNum) < 0)
	{
		printf("testcase1: first blk no:%d, start blk no:%d fail O.O\n", 
				firstBlkNum, startBlkNum);
		assert(0);
	}
	if (validateFatRemove(firstBlkNum, startBlkNum) < 0)
	{
		printf("testcase1: first blk no:%d, start blk no:%d fail O.O\n", 
				firstBlkNum, startBlkNum);
		assert(0);
	}

}


void testcase2(void)
{
	int lblkno = 0; 
	int j = 0;
	int i = 0;

	/* test 1*/
	FatInit();
	FatAdd(-1, FAT_START_ENTRY_NUM);
	for(i = FAT_START_ENTRY_NUM;i <= FAT_END_ENTRY_NUM - 2;i += 2) 
	{
		lblkno++;
		j = i + 2;	
		if (FatAdd(i, j) < 0) 
		{
			printf("testcase2: fail O.O\n");
			return ;
		}
		if (j != FatGetBlockNum(FAT_START_ENTRY_NUM, lblkno))
		{
			printf("testcase2: fail O.O\n");
			return ;
		}
		if (validateFatAdd(i, j) < 0)
		{
			printf("testcase2: fail O.O\n");
			return ;
		}
	}
	testFatRemove(16, 124);
	testFatRemove(16, 100);
	testFatRemove(16, 50);
	testFatRemove(16, 32);
	testFatRemove(16, 20);
	testFatRemove(16, 18);
	
	/* test2 */
	FatInit();
	lblkno = 0;
	FatAdd(-1, FAT_START_ENTRY_NUM);
	for(i = FAT_START_ENTRY_NUM;i <= FAT_END_ENTRY_NUM - 2;i += 2) 
	{
		lblkno++;
		j = i + 2;	
		if (FatAdd(i, j) < 0) 
		{
			printf("testcase2: fail O.O\n");
			return ;
		}
		if (j != FatGetBlockNum(FAT_START_ENTRY_NUM, lblkno))
		{
			printf("testcase2: fail O.O\n");
			return ;
		}
		if (validateFatAdd(i, j) < 0)
		{
			printf("testcase2: fail O.O\n");
			return ;
		}
	}
	testFatRemove(16, 18);


	/* test 3 */
	FatAdd(-1, 18);

	FatAdd(18, 30);
	if (validateFatAdd(18, 30) < 0)
	{
		printf("testcase2: fail O.O\n");
		return ;
	}

	FatAdd(30, 35);
	if (validateFatAdd(30, 35) < 0)
	{
		printf("testcase2: fail O.O\n");
		return ;
	}

	FatAdd(35, 75);
	if (validateFatAdd(35, 75) < 0)
	{
		printf("testcase2: fail O.O\n");
		return ;
	}

	FatAdd(75, 90);
	if (validateFatAdd(75, 90) < 0)
	{
		printf("testcase2: fail O.O\n");
		return ;
	}

	FatAdd(90, 127);
	if (validateFatAdd(90, 127) < 0)
	{
		printf("testcase2: fail O.O\n");
		return ;
	}

	testFatRemove(18, 30);

	printf("testcase2: success!!!!!\n");
	return ;
	
}

int main(int argc, char* argv[])
{
	int tcNum;

	if (argc != 2) 
	{
		perror("Input TestCase Number!");
		exit(0);
	}
	tcNum = atoi(argv[1]);
	
	switch(tcNum)
	{
		case 1:
			testcase1();
			break;
		case 2:
			testcase2();
		    break;
		
	}

	return 0;
}
