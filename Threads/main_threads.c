#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <pthread.h>
#include <time.h>

#include "parserArquivo.h"
#include "writerArquivo.h"

typedef struct {
	int id;
} parm;

//espaço de memória compartilhado:

int ** matriz1;         //matriz entrada 1
int linhas1, colunas1;  // tamanho da matriz1

int ** matriz2;         //matriz entrada 2
int linhas2, colunas2;  // tamanho da matriz2

int ** matrizR;          //matriz Resultado
int linhasR, colunasR;  // tamanho da matriz resultado

int numThreads;

void* hello(void* arg);
void* worker(void* args);
void Imprime(int** mat, int n, int m);
int  ProdutoEscalar(int* a, int* b, int n);
void GetLinha(int** mat, int numLinhas, int numColunas, int indiceLinha, int* out);
void GetColuna(int** mat, int numLinhas, int numColunas, int indiceColuna, int* out);
void ProcessaEntrada(int argc, char** argv);
void MultiplicaSequencial();

int main(int argc, char** argv)
{
	int i,j;
	pthread_t* threads;
	pthread_attr_t pthread_custom_attr;
	parm* p;
	time_t start, end;

	ProcessaEntrada(argc,argv);

	threads = (pthread_t*)malloc(numThreads*sizeof(*threads));
	pthread_attr_init(&pthread_custom_attr);

	//começando o processamento paralelo: armazena o tempo para calcular o tempo gasto
	fprintf(stderr,"Iniciando o processamento paralelo. Aguarde...\n");
	start = time(NULL);

	for(i = 0; i < 10; i++) //rodando 10 vezes, como especificado
	{
		p = (parm*)malloc(numThreads*sizeof(parm));
		
		for(j = 0; j < numThreads; j++)
		{
			p[j].id = j;
	
			pthread_create(&threads[j],&pthread_custom_attr,worker,(void*)&p[j]);
		}
	
		for(j = 0; j < numThreads; j++) // espera todas as threads terminarem
		{
			pthread_join(threads[j],NULL);
		}
	
		free(p);
	}

	end = time(NULL);

	fprintf(stderr,"Processamento paralelo encerrado. Tempo total gasto: %f.\n\n",(double)difftime(end,start));

	//começando o processamento sequencial: armazena o tempo para calcular o tempo gasto
	fprintf(stderr,"Iniciando o processamento sequencial. Aguarde...\n");
	start = time(NULL);

	for(i = 0; i < 10; i++)
		MultiplicaSequencial();

	end = time(NULL);
	fprintf(stderr,"Processamento sequencial encerrado. Tempo total gasto: %f.\n\n",(double)difftime(end,start));

	fprintf(stderr,"Matriz1: \n");
  	Imprime(matriz1,linhas1,colunas1);
  
  	fprintf(stderr,"\nMatriz2: \n");
  	Imprime(matriz2,linhas2,colunas2);
	
	fprintf(stderr,"\nMatriz Resultado: \n");
	Imprime(matrizR,linhasR,colunasR);

	//escreve resultado no arquivo
  	escreveArquivoMatriz("out1.txt",matrizR,linhasR,colunasR);

	return 1;
}

void* hello(void *arg)
{
	parm *p=(parm *)arg;
	fprintf(stderr,"Hello from node %d\n", p->id);
	return (NULL);
}

void* worker(void *args)
{
	int i,j;
	parm *p=(parm *)args; int indiceThread = p->id;

	//fprintf(stderr,"Hello from thread %d\n",indiceThread);

  	for(i = 0; i < linhas1; i++)
  	{
  	  if(i % numThreads == indiceThread)
  	  {
  	  	//fprintf(stderr,"Thread %d will process line %d\n",indiceThread,i);

  	    int* linha = (int*)malloc(colunas1*sizeof(int));
  	    GetLinha(matriz1,linhas1,colunas1,i,linha);
	
  	    for(j = 0; j < colunas2; j++)
  	    {
  	      int* coluna = (int*)malloc(linhas2*sizeof(int));
  	      GetColuna(matriz2,linhas2,colunas2,j,coluna);
	
  	      matrizR[i][j] = ProdutoEscalar(linha,coluna,colunas1);
  	    }
  	  }
  	}

  	//fprintf(stderr,"Thread %d exited\n",indiceThread);

	return (NULL);
}

void Imprime(int** mat, int n, int m)
{
  int i, j;

  for(i = 0; i < n; i++)
  {
    for(j = 0; j < m; j++)
    {
      fprintf(stderr,"%d ", mat[i][j]);
    }

    fprintf(stderr,"\n");
  }

  fprintf(stderr,"\n");
}

int ProdutoEscalar(int* a, int* b, int n) // input: a: n, b: n; output: sum[i=0 until n](a[i]*b[i])
{
  int produtoEscalar = 0;

  int i;

  for(i = 0; i < n; i++)
  {
    produtoEscalar += a[i]*b[i];
  }

  return produtoEscalar;
}

void GetLinha(int** mat, int numLinhas, int numColunas, int indiceLinha, int* out)
{
  int i;

  for(i = 0; i < numColunas; i++)
  {
    out[i] = mat[indiceLinha][i];
  }
}

void GetColuna(int** mat, int numLinhas, int numColunas, int indiceColuna, int* out)
{
  int i;

  for(i = 0; i < numLinhas; i++)
  {
    out[i] = mat[i][indiceColuna];
  }
}

void ProcessaEntrada(int argc, char** argv)
{
  int i; 
  
  if (argc != 2)
  {
    fprintf(stderr,"Uso: ./mmthreads numThreads\nnumThreads: numero de threads para executar a multiplicacao.\nExemplo: ./mmprocessos 4\n\n");
    exit(1);
  }
  
  numThreads = atoi(argv[1]);
  
  if (numThreads == 0)
  {
    fprintf(stderr,"Uso: ./mmthreads numThreads\nnumThreads: numero de threads para executar a multiplicacao.\nExemplo: ./mmprocessos 4\n\n");
    exit(1);
  }
  
  //---------------le os arquivos de entrada--------------
  if (abreArquivoMatriz("in1.txt", &linhas1, &colunas1, &matriz1) == 0)
  {
    fprintf(stderr,"Erro ao abrir \"in1.txt\", processo abortado.\n\n");
    exit(1);
  }
  
  if (abreArquivoMatriz("in2.txt", &linhas2, &colunas2, &matriz2) == 0)
  {
    fprintf(stderr,"Erro ao abrir \"in2.txt\", processo abortado.\n\n");
    exit(1);
  }
  
  //---------- Verifica se o número de linhas e colunas é compatível
  if (colunas1 != linhas2)
  {
    fprintf(stderr,"Numero de colunas da matriz 1 diferente do numero de linhas da matriz 2, processo abortado.\n\n");
    exit(1);
  }

  linhasR = linhas1;
  colunasR = colunas2;

  matrizR = (int**)malloc(linhasR*sizeof(int*));

  for(i = 0; i < linhasR; i++)
  {
  	matrizR[i] = (int*)malloc(colunasR*sizeof(int));
  }

  //--- Professor avisou no email que o numero maximo de threads é o numero de linhas da matriz resultado
  if (numThreads > linhasR)
  {
    fprintf(stderr,"Numero de threads desejado eh maior que o numero de linhas. Usaremos o numero maximo (%d) ao inves.\n", linhasR);
    numThreads = linhasR;
  }
}

void MultiplicaSequencial()
{
  int i, j;

  for(i = 0; i < linhas1; i++)
  {
    for(j = 0; j < colunas2; j++)
    {
      int* linha = (int*)malloc(colunas1*sizeof(int)); GetLinha(matriz1,linhas1,colunas1,i,linha);
      int* coluna = (int*)malloc(linhas2*sizeof(int)); GetColuna(matriz2,linhas2,colunas2,j,coluna);

      matrizR[i][j] = ProdutoEscalar(linha,coluna,colunas1);
    }
  }
}