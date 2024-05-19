#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "ast.h"

Stat *parse_stat(Token *, int *);
Stat *parse_expr(Token *, int *);
Stat *parse_string(Token *tk, int *index);
Stat *produceAst(char *src);

Token eat(Token *tk, int *index)
{
  Token indexTk = tk[*index];
  (*index)++;
  return indexTk;
}

Token expect(TokenType type, char *err, Token *tk, int *index)
{
  Token prev = tk[*index];
  (*index)++;
  if (prev.type != type)
  {
    printf("Parser Error: %s\nExprecting: %d- Got: %d", err, type, prev.type);
    exit(EXIT_FAILURE);
  }

  return prev;
}

Stat *produceAst(char *src)
{

  Token *tokens = tokenize(src);
  free(src);

  if (tokens == NULL)
  {
    printf("Failed to tokenize input.\n");
    exit(EXIT_FAILURE);
  }

  int j = 0;

  while (tokens[j].type != ENDFILE)
  {
    printf("Type: %d, Value: %s\n", tokens[j].type, tokens[j].value);
    j++;
  }

  int arr_buffsize = 20;
  void *arr = malloc(arr_buffsize * sizeof(Stat));

  Stat *program = malloc(sizeof(Stat));
  program->kind = Kind_Program;
  program->data = malloc(sizeof(StatData));
  if (program->data == NULL)
  {
    printf("Memory allocation failed for StatData.\n");
    exit(EXIT_FAILURE);
  }

  program->data->Program = (Program *)malloc(sizeof(Program));

  if (program->data->Program == NULL)
  {
    printf("Memory allocation for StatData failed.\n");
    exit(EXIT_FAILURE);
  }

  int i = 0;

  while (tokens[i].type != ENDFILE)
  {

    Stat *stat = parse_stat(tokens, &i);

    memcpy(arr + i * sizeof(Stat), stat, sizeof(Stat));
    free(stat);
    i++;
    if (i >= arr_buffsize)
    {
      arr_buffsize *= 2;
      arr = realloc(arr, arr_buffsize * sizeof(Stat));
      if (arr == NULL)
      {
        printf("Memory reallocation failed when parsing tokens. Exiting...");
        exit(EXIT_FAILURE);
      }
    }
  }
  program->data->Program->body = arr;
  program->data->Program->bodyCount = i;
  free(tokens);
  return program;
}

Stat *parse_stat(Token *tk, int *index)
{
  switch (tk[*index].type)
  {
    // case Var:
    // return parse_var_declaration(tk, index);
  // case Const:
  // return parse_var_declaration(tk, index);
  // case If:
  //  return parse_if_stat(tk, index);
  // case While:
  //  return parse_while_stat(tk, index);
  // case Return:
  //  return parse_return_stat(tk, index);
  // case For:
  // return parse_for_stat(tk, index);
  default:
    return parse_expr(tk, index);
  }
}

Stat *parse_expr(Token *tk, int *index)
{
  switch (tk[*index].type)
  {
  case DQuote:
    return parse_string(tk, index);
  case SQuote:
    return parse_string(tk, index);
    // case Funct:
    //  return parse_funct_declaration(tk, index);
    // default:
    //  return parse_assignmnet_expr(tk, index);
  }
  exit(EXIT_FAILURE);
}

Stat *parse_string(Token *tk, int *index)
{
  int quoteType;
  if (tk[*index].type == DQuote)
  {
    quoteType = DQuote;
    eat(tk, index);
  }
  else if (tk[*index].type == SQuote)
  {
    quoteType = SQuote;
    eat(tk, index);
  }
  else
  {
    printf("Expected quote at start of string");
    exit(EXIT_FAILURE);
  }

  if (tk[*index].type == quoteType)
  {
    eat(tk, index);
    Stat *emptyStrLit = malloc(sizeof(Stat));
    if (emptyStrLit == NULL)
    {
      printf("Memory allocation failed when parsing empty string token.\n");
      exit(EXIT_FAILURE);
    }

    emptyStrLit->kind = Kind_StringLiteral;
    emptyStrLit->data = (StatData *)malloc(sizeof(StatData));
    if (emptyStrLit->data == NULL)
    {
      printf("Memory allocation failed when parsing empty string token.\n");
      exit(EXIT_FAILURE);
    }
    emptyStrLit->data->ExprData = (ExprData *)malloc(sizeof(ExprData));
    if (emptyStrLit->data->ExprData == NULL)
    {
      printf("Memory allocation failed when parsing empty string token.\n");
      exit(EXIT_FAILURE);
    }
    emptyStrLit->data->ExprData->StringLiteral = (StringLiteral *)malloc(sizeof(StringLiteral));
    if (emptyStrLit->data->ExprData->StringLiteral == NULL)
    {
      printf("Memory allocation failed when parsing empty string token.\n");
      exit(EXIT_FAILURE);
    }
    emptyStrLit->data->ExprData->StringLiteral->value = "";

    return emptyStrLit;
  }

  char err[] = "Expected Identifier inside string";
  char *value = expect(Ident, err, tk, index).value;
  Stat *str = malloc(sizeof(Stat));
  if (str == NULL)
  {
    printf("Memory allocation failed when parsing string token.\n");
    exit(EXIT_FAILURE);
  }

  str->kind = Kind_StringLiteral;
  str->data = (StatData *)malloc(sizeof(StatData));
  if (str->data == NULL)
  {
    printf("Memory allocation failed when parsing empty string token.\n");
    exit(EXIT_FAILURE);
  }
  str->data->ExprData = (ExprData *)malloc(sizeof(ExprData));
  if (str->data->ExprData == NULL)
  {
    printf("Memory allocation failed when parsing empty string token.\n");
    exit(EXIT_FAILURE);
  }
  str->data->ExprData->StringLiteral = (StringLiteral *)malloc(sizeof(StringLiteral));
  if (str->data->ExprData->StringLiteral == NULL)
  {
    printf("Memory allocation failed when parsing empty string token.\n");
    exit(EXIT_FAILURE);
  }

  str->data->ExprData->StringLiteral->value = value;

  if (tk[*index].type != quoteType)
  {
    printf("Expected corresponding end quote at the end of a string.");
    exit(EXIT_FAILURE);
  }
  eat(tk, index);

  return str;
}