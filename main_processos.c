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
  int ** matriz3;
  int linhas3, colunas3;
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
  
  // Lê os arquivos de entrada
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

  matriz3 = (int**)malloc(linhas1*sizeof(int*));
  for(i = 0; i < linhas1; i++)
  {
    matriz3[i] = (int*)malloc(colunas2*sizeof(int));
  }

  //MultiplicaMatrizes(matriz1,matriz2,matriz3,linhas1,colunas1,colunas2,numProcessos);

  printf("Matriz1: \n");
  Imprime(matriz1,linhas1,colunas1);
  
  printf("\nMatriz2: \n");
  Imprime(matriz2,linhas2,colunas2);

  printf("\nMatriz3: \n");
  Imprime(matriz3,linhas3,colunas3);

  ShmTest();
}

int bkpoint(int* i)
{
  fprintf(stderr,"BKPOINT %d\n",(*i)++);
}

int Imprime(int** mat, int n, int m)
{
  int i, j;

  for (i = 0; i < n; i++)
  {
    for (j = 0; j < m; j++)
      printf("%d ", mat[i][j]);
    
    printf("\n");
  }

  return 1;
}

int MultiplicaMatrizes(int** mat1, int** mat2, int** mat3, int n, int m, int p, int numProcessos) // input: mat1: n x m , mat2: m x p ; output: mat3: n x p
{
  int* bk; *bk = 1;

  int numCelulas = n*p;
  celula* celulas = (celula*)malloc(numCelulas*sizeof(celula));

  int i; int j;

  for(i = 0; i < n; i++)
  {
    for(j = 0; j < p; j++)
    {
      celula c; c.i = i; c.j = j;
      celulas[i*p + j] = c;
    }
  }

  // Criação da área de memória compartilhada

  int shmid; size_t shm_size; key_t key; int** shm;
  shm_size = 1024; key = ftok("somefile",42);

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

  bkpoint(bk);

  //int** s = shm;

  for(i = 0; i < n; i++)
  {
    bkpoint(bk);
    for(j = 0; j < p; j++)
    {
      bkpoint(bk);
      shm[i][j] = 1;
    }
  }

  bkpoint(bk);

  for(i = 0; i < n; i++)
  {
    for(j = 0; j < n; j++)
    {
      fprintf(stderr,"%d,",shm[i][j]);
    }

    fprintf(stderr,"\n");
  }

  /*
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
      if ((shmid = shmget(key, shm_size, 0666)) < 0) 
      {
        perror("shmget");
        abort();
      }
    
      if ((shm = shmat(shmid, NULL, 0)) == (int**) -1) 
      {
        perror("shmat");
        abort();
      }

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

          shm[minha_celula.i][minha_celula.j] = ProdutoEscalar(linha,coluna,m);

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

  for(i = 0; i < n; i++)
  {
    for(j = 0; j < p; j++)
    {
      mat3[i][j] = shm[i][j];
    }
  }
  */
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

int ShmTest()
{
    char c;
    int shmid;
    key_t key;
    char *shm, *s;
    int SHMSZ = 27;

    /*
     * We'll name our shared memory segment
     * "5678".
     */
    key = 5678;

    /*
     * Create the segment.
     */
    if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
        perror("shmget");
        exit(1);
    }

    /*
     * Now we attach the segment to our data space.
     */
    if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
        perror("shmat");
        exit(1);
    }

    if(fork() != 0) // father
    {
        /*
       * Now put some things into the memory for the
       * other process to read.
       */
      s = shm;
  
      for (c = 'a'; c <= 'z'; c++)
          *s++ = c;
      *s = NULL;
  
      /*
       * Finally, we wait until the other process 
       * changes the first character of our memory
       * to '*', indicating that it has read what 
       * we put there.
       */
      while (*shm != '*')
          sleep(1);
  
      exit(0);
    }
    else // child
    {
      int shmid;
      key_t key;
      char *shm, *s;
  
      /*
       * We need to get the segment named
       * "5678", created by the server.
       */
      key = 5678;
  
      /*
       * Locate the segment.
       */
      if ((shmid = shmget(key, SHMSZ, 0666)) < 0) {
          perror("shmget");
          exit(1);
      }
  
      /*
       * Now we attach the segment to our data space.
       */
      if ((shm = shmat(shmid, NULL, 0)) == (char *) -1) {
          perror("shmat");
          exit(1);
      }
  
      /*
       * Now read what the server put in the memory.
       */
      for (s = shm; *s != NULL; s++)
          putchar(*s);
      putchar('\n');
  
      /*
       * Finally, change the first character of the 
       * segment to '*', indicating we have read 
       * the segment.
       */
      *shm = '*';
  
      exit(0);
    }
}