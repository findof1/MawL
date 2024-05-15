#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "frontend/lexer.h"

void run(char[]);

int main()
{
  char *input = "var x = 10; if (x > 5) { x++; }";

  Token *tokens = tokenize(input);

  if (tokens == NULL)
  {
    printf("Failed to tokenize input.\n");
    return 1;
  }

  int i = 0;
  while (tokens[i].type != ENDFILE)
  {
    printf("Type: %d, Value: %s\n", tokens[i].type, tokens[i].value);
    i++;
  }

  free(tokens);

  return 0;
}

void run(char input[])
{
}