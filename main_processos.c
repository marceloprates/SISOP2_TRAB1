#include <stdio.h>

#include "parserArquivo.h"

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
    printf("Uso: ./mmprocessos numprocessos\nnumprocessos: numero de processos para executar a multiplicacao.\nExemplo: ./mmprocessos 4\n\n");
    return 0;
  }
  
  numProcessos = atoi(argv[1]);
  
  if (numProcessos == 0)
  {
    printf("Uso: ./mmprocessos numprocessos\nnumprocessos: numero de processos para executar a multiplicacao.\nExemplo: ./mmprocessos 4\n\n");
    return 0;
  }
  
  //le os arquivos de entrada
  if (abreArquivoMatriz("in1.txt", &linhas1, &colunas1, &matriz1) == 0)
  {
    printf("Erro ao abrir \"in1.txt\", processo abortado.\n\n");
    return 0;
  }
  
  if (abreArquivoMatriz("in2.txt", &linhas2, &colunas2, &matriz2) == 0)
  {
    printf("Erro ao abrir \"in2.txt\", processo abortado.\n\n");
    return 0;
  }
  
  if (colunas1 != linhas2)
  {
    printf("Numero de colunas da matriz 1 diferente do numero de linhas da matriz 2, processo abortado.\n\n");
    return 0;
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
  
  
  
}