// Bruno Gabriel Fernandes 2013/1
// Arquivo com funções responsáveis pela leitura e escrita do arquivo de entrada

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parserArquivo.h"


/* abre o arquivo no formato

LINHAS = m
COLUNAS = n
M11 M12 M13 ... M1n
M21 M22 M23 ... M2n
...
Mm1 Mm2 Mm3 ... Mmn


retorna 1 sucesso, 0 erro ao abrir arquivo */
int abreArquivoMatriz(const char * nomeArquivo, int * numLinhas, int * numColunas, int *** pMatriz)
{
  FILE * pFile = NULL;
  char strLinha[2048];
  char strNumero[512]; 
  int i,j,k,l;
  
  int ** matriz;
  
  //tenta abrir o arquivo
  pFile = fopen(nomeArquivo, "r");
  if (pFile == NULL)
  {
    printf("\nArquivo nao encontrado.\n");
    return 0;
  }
  
  //-------------le a primeira linha, deverá ser LINHAS = algo--------------
  if (fgets(strLinha, sizeof(strLinha), pFile) == NULL)
  {
    printf("\nErro ao ler primeira linha.\n");
    return 0;
  }
  if (strncmp(strLinha, "LINHAS = ", 9) != 0)
  {
    fclose(pFile);
    printf("\nPrimeira linha do arquivo diferente de \"LINHAS = ...\".\n");
    return 0;
  }
  
  //copia o numero para strNumero
  for (i = 0; strLinha[i] != '='; i++); 
  i+=2;//pula o espaço depois de =
  for (j = 0; ((strLinha[i] != '\n') && (strLinha[i] != ' ') && (strLinha[i] != '\t')); strNumero[j++] = strLinha[i++]);
  strNumero[j] = '\0';
  
  *numLinhas = atoi(strNumero);
  if (*numLinhas == 0)
  {
    printf("\nNumero de linhas lido \"%s\" invalido.\n", strNumero);
    return 0;
  }
  
  //-------------le a segunda linha, deverá ser COLUNAS = algo--------------
  if (fgets(strLinha, sizeof(strLinha), pFile) == NULL)
  {
    printf("\nErro ao ler segunda linha.\n");
    return 0;
  }
  if (strncmp(strLinha, "COLUNAS = ", 9) != 0)
  {
    fclose(pFile);
    printf("\nSegunda linha do arquivo diferente de \"COLUNAS = ...\".\n");
    return 0;
  }
  
  //copia o numero para strNumero
  for (i = 0; strLinha[i] != '='; i++);
  i+=2;//pula o espaço depois de =
  for (j = 0; ((strLinha[i] != '\n') && (strLinha[i] != ' ') && (strLinha[i] != '\t')); strNumero[j++] = strLinha[i++]);
  strNumero[j] = '\0';
  
  *numColunas = atoi(strNumero);
  if (*numColunas == 0)
  {
    printf("\nNumero de colunas lido \"%s\" invalido.\n", strNumero);
    return 0;
  }
      
  //-----------Aloca espaço para matriz ----------------
  matriz = NULL;
  matriz = (int **) malloc(sizeof(int *) * (*numLinhas));
  if (matriz == NULL)
  {
    printf("\nErro de alocacao de memoria.\n");
    return 0;
  }
  for (i = 0; i < *numLinhas; i++)
  {
    matriz[i] = (int *) malloc (sizeof(int) * (*numColunas));
    if (matriz[i] == NULL)
    {
      printf("\nErro de alocacao de memoria.\n");
      return 0;
    }
  }
  
  //---------Le os numeros da matriz ---
  i = 0; //linha
  j = 0; //coluna
  k = 0; //iterador da string linha
  l = 0; //iterador da string numero
  
  while ((fgets(strLinha,sizeof(strLinha),pFile) != NULL) && (i < *numLinhas))
  {
    j = 0;
    k = 0;
    
    //le numero a numero da linha
    while ((k < strlen(strLinha)) && (j < (*numColunas)))
    {
      for (l = 0; ((strLinha[k] != '\n') && (strLinha[k] != ' ') && (strLinha[k] != '\t')); strNumero[l++] = strLinha[k++]);
      strNumero[l] = '\0';
      k++; //pula o espaço
      
      matriz[i][j] = atoi(strNumero);
      j++;
    }
    
    if (j != *numColunas)
    {
      printf("Numero de inteiros na linha %d insuficiente. Foram lidos %d inteiros, necessario: %d. \n", i+1, j, *numColunas);
      return 0;
    }
    
    i++;
  }
  
  if (i != *numLinhas)
  {
    printf("Numero de linhas insuficiente. Foram lidas %d linhas, necessario: %d. \n", i, *numLinhas);
    return 0;
  }
  
  *pMatriz = matriz;
  
  return 1;
  
}
