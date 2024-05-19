#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./frontend/lexer.h"
#include "./frontend/ast.h"
#include "./frontend/parser.h"

void run(char[]);
extern Program *produceAst(char *);
int main()
{
  FILE *file = fopen("./main.mawl", "r");
  if (file == NULL)
  {
    printf("Failed to open main.mawl.\n");
    exit(EXIT_FAILURE);
  }
  fseek(file, 0, SEEK_END);
  long length = ftell(file);
  rewind(file);
  char *input = malloc(length * sizeof(char));
  if (input == NULL)
  {
    printf("Memory allocation failed.\n");
    fclose(file);
    exit(EXIT_FAILURE);
  }

  size_t readLen = fread(input, 1, length, file);
  input[readLen] = '\0';

  fclose(file);

  run(input);

  free(input);

  return 0;
}

void run(char *input)
{
  Program *program = produceAst(input);
}