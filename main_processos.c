#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#include "parserArquivo.h"

typedef struct celula_struct
{

 int i;
 int j; 

} celula;

int main (int argc, char ** argv)
{
  int numProcessos;
  int ** matriz1;
  int linhas1, colunas1;
  int ** matriz2;
  int linhas2, colunas2;
  int i,j;
  
  if (argc != 2)
  {
    fprintf(stderr,"Uso: ./mmprocessos numprocessos\nnumprocessos: numero de processos para executar a multiplicacao.\nExemplo: ./mmprocessos 4\n\n");
    exit(1);
  }
  
  numProcessos = atoi(argv[1]);
  
  if (numProcessos == 0)
  {
    fprintf(stderr,"Uso: ./mmprocessos numprocessos\nnumprocessos: numero de processos para executar a multiplicacao.\nExemplo: ./mmprocessos 4\n\n");
    exit(2);
  }
  
  //le os arquivos de entrada
  if (abreArquivoMatriz("in1.txt", &linhas1, &colunas1, &matriz1) == 0)
  {
    fprintf(stderr,"Erro ao abrir \"in1.txt\", processo abortado.\n\n");
    exit(3);
  }
  
  if (abreArquivoMatriz("in2.txt", &linhas2, &colunas2, &matriz2) == 0)
  {
    fprintf(stderr,"Erro ao abrir \"in2.txt\", processo abortado.\n\n");
    exit(4);
  }
  
  if (colunas1 != linhas2)
  {
    fprintf(stderr,"Numero de colunas da matriz 1 diferente do numero de linhas da matriz 2, processo abortado.\n\n");
    exit(5);
  }

  printf("Matriz1: \n");
  for (i = 0; i < linhas1; i++)
  {
    for (j = 0; j < colunas1; j++)
      printf("%d ", matriz1[i][j]);
    
    printf("\n");
  }
  
  printf("\nMatriz2: \n");
  for (i = 0; i < linhas2; i++)
  {
    for (j = 0; j < colunas2; j++)
      printf("%d ", matriz2[i][j]);
    
    printf("\n");
  }

  int** matriz3;

  

  MultiplicaMatrizes(matriz1,matriz2,3,3,3);

  printf("\nMatriz3: \n");
  for (i = 0; i < linhas1; i++)
  {
    for (j = 0; j < colunas2; j++)
      printf("%d ", matriz3[i][j]);
    
    printf("\n");
  }

}

int MultiplicaMatrizes(int** mat1, int** mat2, int** mat3, int n, int m, int p, int numProcessos) // input: mat1: n x m , mat2: m x p ; output: mat3: n x p
{
  int numCelulas = n*p;
  celula* celulas = (celula*)malloc(numCelulas*sizeof(celula));

  int i; int j;

  for(i = 0; i < n; i++)
  {
    for(j = 0; j < p; j++)
    {
      celula c;
      c.i = i;
      c.j = j;

      celulas[i*p + j] = c;
    }
  }

  // Criação da área de memória compartilhada

  int shmid;
  int shm_size;
  key_t key;
  int** shm;

  shm_size = sizeof(mat3);
  key = 5678;

  if ((shmid = shmget(key, shm_size, IPC_CREAT | 0666)) < 0) 
  {
    perror("shmget");
    abort();
  }

  if ((shm = shmat(shmid, NULL, 0)) == (int**) -1) 
  {
    perror("shmat");
    abort();
  }

  // Criação dos processos filho
  pid_t* pids = (pid_t*)malloc(n*sizeof(pid_t));

  int indiceProcesso;

  for(indiceProcesso = 0; indiceProcesso < numProcessos; indiceProcesso++)
  {
    if((pids[indiceProcesso] = fork()) < 0) // Erro: abortar
    {
      perror("fork");
      abort();
    }
    else if(pids[indiceProcesso] == 0) // Processo filho rodando
    {
      int indiceCelula;

      for(indiceCelula = 0; indiceCelula < numCelulas; indiceCelula++) // Para todas as céulas
        if(indiceCelula % numProcessos == indiceProcesso)
        {
          // Processa célula

          celula minha_celula = celulas[indiceCelula];

          int* linha = (int*)malloc(m*sizeof(int));
          int* coluna = (int*)malloc(m*sizeof(int));

          for(i = 0; i < m; i++)
          {
            linha[i] = mat1[minha_celula.i][i];
            coluna[i] = mat2[i][minha_celula.j];
          }

          int** shm_pointer = shm;

          shm_pointer[minha_celula.i][minha_celula.j] = ProdutoEscalar(linha,coluna,m);

          exit(0);
        }
    }
  }

  // Esperando os processos filho finalizarem
  int status;
  pid_t pid;

  while (numProcessos > 0) 
  {
    pid = wait(&status);

    printf("Child with PID %ld exited with status 0x%x.\n", (long)pid, status);

    numProcessos--;  // TODO(pts): Remove pid from the pids array.
  }

  return 1;
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

int ForkTest(int n)
{
  pid_t* pids = (pid_t*)malloc(n*sizeof(pid_t));

  int i;
  
  /* Start children. */
  for (i = 0; i < n; ++i) 
  {
    if ((pids[i] = fork()) < 0) 
    {
      perror("fork");

      abort();
    } 
    else if (pids[i] == 0) 
    {
      fprintf(stderr,"Child %d says hi!\n",i);

      exit(0);
    }
  }
  
  /* Wait for children to exit. */
  int status;
  pid_t pid;

  while (n > 0) 
  {
    pid = wait(&status);

    printf("Child with PID %ld exited with status 0x%x.\n", (long)pid, status);

    --n;  // TODO(pts): Remove pid from the pids array.
  }

  return 1;
}