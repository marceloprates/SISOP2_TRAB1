#include <stdlib.h>
#include <stdio.h>

#include "writerArquivo.h"

#define MAX 1000

int geraMatriz(int n, int m)
{
	srand(time(NULL));

	int i, j;

	int** matriz = (int**)calloc(n,sizeof(int*));

	for(i = 0; i < n; i++)
	{
		matriz[i] = (int*)calloc(m,sizeof(int));

		for(j = 0; j < m; j++)
		{
			matriz[i][j] = rand()%MAX - MAX/2;
		}
	}

	escreveArquivoMatriz("in1.txt",matriz,n,m);
	escreveArquivoMatriz("in2.txt",matriz,n,m);

	return 0;
}

int main(int argc, char const *argv[])
{
	geraMatriz(atoi(argv[1]),atoi(argv[2]));

	return 0;
}