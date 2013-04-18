target: mmthreads

make_all: mmthreads mmthreads

mmthreads: main_threads.o parserArquivo.o writerArquivo.o
	gcc -pthread -o mmthreads main_threads.o parserArquivo.o writerArquivo.o

main_threads.o: main_threads.c
	gcc -c -x c main_threads.c

parserArquivo.o: parserArquivo.c
	gcc -c -x c parserArquivo.c

writerArquivo.o: writerArquivo.c
	gcc -c -x c writerArquivo.c


######## clean

clean:
	rm -f *.o mmthreads out1.txt
