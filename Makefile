CC   = gcc -o
RM   = rm -f

all: 
	$(CC) falsh falsh.c

clean:
	$(RM) falsh
