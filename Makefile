target: mmprocessos

make_all: mmprocessos mmthreads

mmprocessos: main_processos.o parserArquivo.o
	gcc -o mmprocessos main_processos.o parserArquivo.o


main_processos.o: main_processos.c
	gcc -c -x c main_processos.c

parserArquivo.o: parserArquivo.c
	gcc -c -x c parserArquivo.c


######## clean

clean:
	rm -f *.o mmprocessos
