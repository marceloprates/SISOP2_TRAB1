#include <stdlib.h>
#include <stdio.h>

#include "writerArquivo.h"

int escreveArquivoMatriz(char* path, int* matriz, int numLinhas, int numColunas)
{
	FILE* file;
	int i, j;

	file = fopen(path, "w");

	if(file == NULL)
	{
		fprintf(stderr,"Erro na abertura do arquivo de output\n");
		exit(1);
	}

	fprintf(file, "LINHAS = %d\n", numLinhas);
	fprintf(file, "COLUNAS = %d\n", numColunas);

	for(i = 0; i < numLinhas; i++)
	{
		for(j = 0; j < numColunas-1; j++)
		{
			fprintf(file, "%d ", matriz[i*numColunas + j]);
		}

		fprintf(file, "%d\n", matriz[i*numColunas + numColunas-1]);
	}

	fclose(file);

	return 0;
}