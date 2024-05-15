#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "frontend/lexer.h"

void run(char[]);

int main()
{
  FILE *file = fopen("./main.mawl", "r");
  if (file == NULL)
  {
    printf("Failed to open main.mawl.\n");
    return 1;
  }
  fseek(file, 0, SEEK_END);
  long length = ftell(file);
  rewind(file);
  char *input = malloc(length * sizeof(char));
  if (input == NULL)
  {
    printf("Memory allocation failed.\n");
    fclose(file);
    return 1;
  }

  size_t readLen = fread(input, 1, length, file);
  input[readLen] = '\0';

  fclose(file);
  Token *tokens = tokenize(input);

  if (tokens == NULL)
  {
    printf("Failed to tokenize input.\n");
    free(input);
    return 1;
  }

  int i = 0;
  while (tokens[i].type != ENDFILE)
  {
    printf("Type: %d, Value: %s\n", tokens[i].type, tokens[i].value);
    i++;
  }

  free(tokens);
  free(input);

  return 0;
}

void run(char input[])
{
}