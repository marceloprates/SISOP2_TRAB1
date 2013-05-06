target: mmthreads

make_all: mmthreads geraMatriz

mmthreads: main_threads.o parserArquivo.o writerArquivo.o
	gcc -pthread -o mmthreads main_threads.o parserArquivo.o writerArquivo.o

geraMatriz: geraMatriz.o writerArquivo.o
	gcc -o geraMatriz geraMatriz.o writerArquivo.o

main_threads.o: main_threads.c
	gcc -c -x c main_threads.c

parserArquivo.o: parserArquivo.c
	gcc -c -x c parserArquivo.c

geraMatriz.o: geraMatriz.c
	gcc -c -x c geraMatriz.c

writerArquivo.o: writerArquivo.c
	gcc -c -x c writerArquivo.c


######## clean

clean:
	rm -f *.o mmthreads geraMatriz out1.txt
