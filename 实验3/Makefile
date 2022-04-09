CC = gcc
INCLUDE = /home/zhuoyunpeng/实验/实验3/Inc
CFLAGS = -g -Wall -ansi
MYLIB = /home/zhuoyunpeng/实验/实验3/mylib.a

sortseat : main.o outseat.o
	$(CC) -o sortseat main.o outseat.o
	mv main.o ./mylib.a
	mv outseat.o ./mylib.a
main.o : ./Src/main.c ./Inc/outseat.h
	$(CC) -c ./Src/main.c
outseat.o : ./Src/outseat.c ./Inc/outseat.h
	$(CC) -c ./Src/outseat.c
clean : 
	rm  sortseat ./mylib.a/main.o ./mylib.a/outseat.o
