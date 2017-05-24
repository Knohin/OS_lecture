testcase: Disk.o buf.o testcase.o
		gcc -o testcase testcase.o Disk.o buf.o
Disk.o: Disk.c Disk.h
		gcc -c Disk.c
buf.o: buf.c buf.h queue.h
		gcc -c buf.c
testcase.o: testcase.c buf.h Disk.h
		gcc -c testcase.c 
clean:
		rm testcase testcase.o Disk.o buf.o
