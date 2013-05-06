#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <pthread.h>
#include <time.h>

#include "parserArquivo.h"
#include "writerArquivo.h"

#define tamanhoAmostra 10

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

void Imprime(int** mat, int n, int m);
int  ProdutoEscalar(int* a, int* b, int n);
void GetLinha(int** mat, int numLinhas, int numColunas, int indiceLinha, int* out);
void GetColuna(int** mat, int numLinhas, int numColunas, int indiceColuna, int* out);
void ProcessaEntrada(int argc, char** argv);
void* worker(void* args);
unsigned int getTickCount();

int main(int argc, char** argv)
{
	int i,j;
	pthread_t* threads;
	pthread_attr_t pthread_custom_attr;
	parm* p;

	ProcessaEntrada(argc,argv);

	//começando o processamento paralelo: armazena o tempo para calcular o tempo gasto
	fprintf(stderr,"Iniciando o processamento. Aguarde...\n");	
	
	threads = (pthread_t*)malloc(numThreads*sizeof(*threads));
	pthread_attr_init(&pthread_custom_attr);

  p = (parm*)malloc(numThreads*sizeof(parm));

	for(i = 0; i < tamanhoAmostra; i++) //cálculo da média
	{		
		for(j = 0; j < numThreads; j++)
		{
			p[j].id = j;
	
			pthread_create(&threads[j],&pthread_custom_attr,worker,(void*)&p[j]);
		}
	
		for(j = 0; j < numThreads; j++) // espera todas as threads terminarem
		{
			pthread_join(threads[j],NULL);
		}
	}

  free(p);

	fprintf(stderr,"Processamento encerrado.\n");

	//escreve matriz resultado no arquivo
  escreveArquivoMatriz("out1.txt",matrizR,linhasR,colunasR);

	return 1;
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

void* worker(void *args)
{
  int i,j;
  parm *p=(parm *)args; int indiceThread = p->id;

  for(i = indiceThread; i < linhas1; i += numThreads)
  {
     //fprintf(stderr,"Thread %d multiplica linha %d\n",indiceThread,i);

     int* linha = (int*)malloc(colunas1*sizeof(int));
     GetLinha(matriz1,linhas1,colunas1,i,linha);
     for(j = 0; j < colunas2; j++)
     {
       int* coluna = (int*)malloc(linhas2*sizeof(int));
       GetColuna(matriz2,linhas2,colunas2,j,coluna);
       matrizR[i][j] = ProdutoEscalar(linha,coluna,colunas1);
     }
  }

  return (NULL);
}

unsigned int getTickCount() //função retorna a hora do dia em millisegundos
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}
