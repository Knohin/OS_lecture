testcase: Disk.o fat.o validate.o testcase.o
		gcc -o testcase Disk.o fat.o validate.o testcase.o
Disk.o: Disk.c Disk.h
		gcc -c Disk.c
fat.o: fat.c fat.h Disk.h
		gcc -c fat.c
testcase.o: testcase.c validate.h fat.h
		gcc -c testcase.c
clean:
		rm testcase testcase.o Disk.o fat.o
