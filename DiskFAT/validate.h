#ifndef __VALIDATE_H__
#define __VALIDATE_H__


int validateFatAdd(int lastBlkNum, int newBlkNum);
void putFatRemoveArg(int firstBlkNum, int startBlkNum);
int validateFatRemove(int firstBlkNum, int startBlkNum);


#endif 
