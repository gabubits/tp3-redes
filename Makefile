all: tp3

tp3: servidor.o cliente.o
	gcc servidor.o -o servidor
	gcc cliente.o -o cliente

servidor.o: servidor.c
	gcc -c servidor.c

cliente.o: cliente.c
	gcc -c cliente.c

c:
	rm -f *.o cliente servidor cliente.exe servidor.exe