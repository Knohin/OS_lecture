main: Disk.o buf.o main.o
		gcc -o main main.o Disk.o buf.o
Disk.o: Disk.c Disk.h
		gcc -c Disk.c
buf.o: buf.c buf.h queue.h
		gcc -c buf.c
main.o: main.c buf.h
		gcc -c main.c
clean:
		rm main main.o Disk.o buf.o
