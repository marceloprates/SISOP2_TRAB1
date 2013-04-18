target: mmprocessos

make_all: mmprocessos mmthreads

mmprocessos: main_processos.o parserArquivo.o writerArquivo.o
	gcc -o mmprocessos main_processos.o parserArquivo.o writerArquivo.o

main_processos.o: main_processos.c
	gcc -c -x c main_processos.c

parserArquivo.o: parserArquivo.c
	gcc -c -x c parserArquivo.c

writerArquivo.o: writerArquivo.c
	gcc -c -x c writerArquivo.c


######## clean

clean:
	rm -f *.o mmprocessos out1.txt
