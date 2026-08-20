CC = gcc 
CFLAGS = -Wall -Wextra

mp3out : main.o view.o edit.o
	$(CC) $(CFLAGS) main.o view.o edit.o -o mp3out
main.o : main.c All_type.h view.h edit.h
	$(CC) $(CFLAGS) -c main.c
view.o : view.c view.h All_type.h
	$(CC) $(CFLAGS) -c view.c
edit.o : edit.c All_type.h edit.h
	$(CC) $(CFLAGS) -c edit.c

clean :
	rm -f *.o mp3out
