#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>

#include "parserArquivo.h"
#include "writerArquivo.h"

//espaço de memoria compartilhado:

int ** matriz1;         //matriz entrada 1
int linhas1, colunas1;  // tamanho da matriz1

int ** matriz2;         //matriz entrada 2
int linhas2, colunas2;  // tamanho da matriz2

int * matrizR;          //matriz Resultado
int linhasR, colunasR;  // tamanho da matriz resultado

int numProcessos;

void Imprime(int** mat, int n, int m);
void ImprimeResultado();
int  ProdutoEscalar(int* a, int* b, int n);
void GetLinha(int** mat, int numLinhas, int numColunas, int indiceLinha, int* out);
void GetColuna(int** mat, int numLinhas, int numColunas, int indiceColuna, int* out);
void ProcessaEntrada(int argc, char** argv);
void worker(int indiceProcesso);
void MultiplicaSequencial();
unsigned int getTickCount();

int main (int argc, char ** argv)
{
  int i,j;
  pid_t pid;
  pid_t * filhos; //array de pids de processos filhos
  int status;
  unsigned int start, end;

  ProcessaEntrada(argc,argv);
  
  //aloca espaço para a matriz resultado
  //a matriz resultado deve ser allocada em espaço de memória compartilhado, pois será acessada
  //por diversos processos.

  matrizR = (int*) mmap(0, linhasR * colunasR * sizeof(int), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANON, -1, 0);
  
  filhos = (pid_t *) malloc(sizeof(pid_t) * (numProcessos - 1));
  if (filhos == NULL)
  {
    fprintf(stderr,"Erro de alocacao de memoria.\n");
    exit(1);
  }
  
  //começando o processamento paralelo: armazena o tempo para calcular o tempo gasto
  fprintf(stderr, "Iniciando o processamento. Aguarde...\n");
  start = getTickCount();
  
  for(i = 0; i < 10; i++) //rodando 10 vezes, como especificado
  {
    for (j = 1; j < numProcessos; j++) //abre somente numProcessos -1: o processo pai é o zero
    {
      pid = fork();
      
      if (pid == 0) //processo filho
      {
        worker(j); //chama a funcao que vai fazer o processamento
        exit(1); //encerra o processamento
      }
      
      filhos[j-1] = pid;
    }
    
    //processo pai tem que trabalhar também:
    worker(0);
  
    //espera todos os filhos terminarem
    for (j = 0; j < (numProcessos-1); j++);
      waitpid(filhos[j], &status, 0);
  }
  
  end = getTickCount();
  
  fprintf(stderr,"Processamento encerrado. Tempo total gasto: %u ms.\n\n", (end - start));

  //escreve resultado no arquivo
  escreveArquivoMatriz("out1.txt",matrizR,linhasR,colunasR);  
}

//função retorna a hora do dia em millisegundos
unsigned int getTickCount()
{
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
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

  fprintf(stderr, "\n");
}

void ImprimeResultado()
{
  int i, j;

  for(i = 0; i < linhasR; i++)
  {
    for(j = 0; j < colunasR; j++)
    {
      fprintf(stderr,"%d ", matrizR[i*colunasR + j]);
    }

    fprintf(stderr,"\n");
  }

  fprintf(stderr, "\n");
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
  if (argc != 2)
  {
    fprintf(stderr,"Uso: ./mmprocessos numprocessos\nnumprocessos: numero de processos para executar a multiplicacao.\nExemplo: ./mmprocessos 4\n\n");
    exit(1);
  }
  
  numProcessos = atoi(argv[1]);
  
  if (numProcessos == 0)
  {
    fprintf(stderr,"Uso: ./mmprocessos numprocessos\nnumprocessos: numero de processos para executar a multiplicacao.\nExemplo: ./mmprocessos 4\n\n");
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

  //--- Professor avisou no email que o numero maximo de processos é o numero de linhas da matriz resultado
  if (numProcessos > linhasR)
  {
    fprintf(stderr,"Numero de processos desejado eh maior que o numero de linhas. Usaremos o numero maximo (%d) ao inves.\n", linhasR);
    numProcessos = linhasR;
  }
}

void worker(int indiceProcesso) //recebe um numero de 0 a numProcessos-1 para determinar qual(is) linha(s) deve processar
{
  int i,j;

  for(i = indiceProcesso; i < linhas1; i = i += numProcessos)
  {
    int* linha = (int*)malloc(colunas1*sizeof(int));
    GetLinha(matriz1,linhas1,colunas1,i,linha);
    for(j = 0; j < colunas2; j++)
    {
      int* coluna = (int*)malloc(linhas2*sizeof(int));
      GetColuna(matriz2,linhas2,colunas2,j,coluna);
      matrizR[i*colunasR + j] = ProdutoEscalar(linha,coluna,colunas1);
    }
  }
}
