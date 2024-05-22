#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"
#include "ast.h"

Stat *parse_stat(Token *, int *);
Stat *parse_expr(Token *, int *);
Stat *parse_string(Token *tk, int *index);
Stat *parse_additive_expr(Token *tk, int *index);
Stat *parse_and_or_expr(Token *tk, int *index);
Stat *parse_object_arr_expr(Token *tk, int *index);
Stat *parse_assignmnet_expr(Token *tk, int *index);
Stat *parse_primary_expr(Token *tk, int *index);
Stat *parse_member_expr(Token *tk, int *index);
Stat *parse_arguments_list(Token *tk, int *index);
Stat *parse_call_expr(Token *tk, int *index, Stat *caller);
Stat *parse_multiplicitave_expr(Token *tk, int *index);
Stat *parse_call_member_expr(Token *tk, int *index);
Stat *parse_var_declaration(Token *tk, int *index);
Stat *parse_funct_declaration(Token *tk, int *index);
Stat *parse_if_stat(Token *tk, int *index);
Stat *parse_args(Token *tk, int *index);
Stat *produceAst(char *src);
double string_to_double(const char *str);

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
  printf("Type: %d, Value: %s\n", tokens[j].type, tokens[j].value);
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

  while (tokens[i - 1].type != ENDFILE && tokens[i].type != ENDFILE)
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
  case Var:
    return parse_var_declaration(tk, index);
  case Const:
    return parse_var_declaration(tk, index);
  case If:
    return parse_if_stat(tk, index);
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

Stat *parse_if_stat(Token *tk, int *index)
{
  int bodycount = 0;
  int elsecount = 0;
  eat(tk, index);
  expect(
      OpenParen,
      "Expected open parenthesis during if statement.", tk, index);
  Stat *comparison = parse_expr(tk, index);
  expect(
      ClosedParen,
      "Expected closing parenthesis during if statment.", tk, index);

  int ifBodyBuff = 10;
  Stat **body = malloc(ifBodyBuff * sizeof(Stat *));
  if (body == NULL)
  {
    printf("Memory allocation failure when parsing if statement.");
    exit(EXIT_FAILURE);
  }
  expect(OpenBrace, "Expected open brace in if statement", tk, index);
  int i = 0;
  while (
      tk[*index].type != ENDFILE &&
      tk[*index].type != ClosedBrace)
  {
    body[i] = parse_stat(tk, index);
    i++;
    if (i >= ifBodyBuff)
    {
      ifBodyBuff *= 2;
      body = realloc(body, ifBodyBuff * sizeof(Stat *));
      if (body == NULL)
      {
        printf("Memory reallocation failure when parsing if statement.");
        exit(EXIT_FAILURE);
      }
    }
  }
  bodycount = i;

  expect(ClosedBrace, "Closing brace expected after if statement", tk, index);

  if (tk[*index].type == Else)
  {

    eat(tk, index);

    int elseBodyBuff = 10;
    Stat **elseBody = NULL;

    if (tk[*index].type == If)
    {

      Stat **elseBody = malloc(1 * sizeof(Stat *));
      if (body == NULL)
      {
        printf("Memory allocation failure when parsing else statement.");
        exit(EXIT_FAILURE);
      }
      elseBody[0] = parse_stat(tk, index);

      elsecount = 1;
    }
    else
    {
      expect(OpenBrace, "Expected open brace in if statement", tk, index);
      Stat **elseBody = malloc(elseBodyBuff * sizeof(Stat *));
      if (body == NULL)
      {
        printf("Memory allocation failure when parsing else statement.");
        exit(EXIT_FAILURE);
      }
      int i = 0;
      while (
          tk[*index].type != ENDFILE &&
          tk[*index].type != ClosedBrace)
      {
        elseBody[i] = parse_stat(tk, index);
        i++;
        if (i >= elseBodyBuff)
        {
          elseBodyBuff *= 2;
          elseBody = realloc(elseBody, elseBodyBuff * sizeof(Stat *));
          if (elseBody == NULL)
          {
            printf("Memory reallocation failure when parsing else statement.");
            exit(EXIT_FAILURE);
          }
        }
      }
      elsecount = i;
      expect(
          ClosedBrace,
          "Closing brace expected after if statement", tk, index);
    }

    Stat *ifElseStat = malloc(sizeof(Stat));
    if (ifElseStat == NULL)
    {
      printf("Memory allocation failed when parsing if statement.\n");
      exit(EXIT_FAILURE);
    }

    ifElseStat->kind = Kind_IfStatement;
    ifElseStat->data = (StatData *)malloc(sizeof(StatData));
    if (ifElseStat->data == NULL)
    {
      printf("Memory allocation failed when parsing if statement.\n");
      exit(EXIT_FAILURE);
    }
    ifElseStat->data->IfStatement = (IfStatement *)malloc(sizeof(IfStatement));
    if (ifElseStat->data->IfStatement == NULL)
    {
      printf("Memory allocation failed when parsing if statement.\n");
      exit(EXIT_FAILURE);
    }
    ifElseStat->data->IfStatement->comparison = comparison;
    ifElseStat->data->IfStatement->body = body;
    ifElseStat->data->IfStatement->elseBody = elseBody;
    ifElseStat->data->IfStatement->bodyCount = bodycount;
    ifElseStat->data->IfStatement->elseBodyCount = elsecount;

    return ifElseStat;
  }
  else
  {
    Stat *ifStat = malloc(sizeof(Stat));
    if (ifStat == NULL)
    {
      printf("Memory allocation failed when parsing if statement.\n");
      exit(EXIT_FAILURE);
    }

    ifStat->kind = Kind_IfStatement;
    ifStat->data = (StatData *)malloc(sizeof(StatData));
    if (ifStat->data == NULL)
    {
      printf("Memory allocation failed when parsing if statement.\n");
      exit(EXIT_FAILURE);
    }
    ifStat->data->IfStatement = (IfStatement *)malloc(sizeof(IfStatement));
    if (ifStat->data->IfStatement == NULL)
    {
      printf("Memory allocation failed when parsing if statement.\n");
      exit(EXIT_FAILURE);
    }
    ifStat->data->IfStatement->comparison = comparison;
    ifStat->data->IfStatement->body = body;
    ifStat->data->IfStatement->elseBody = NULL;
    ifStat->data->IfStatement->bodyCount = bodycount;
    ifStat->data->IfStatement->elseBodyCount = 0;

    return ifStat;
  }
}

Stat *parse_var_declaration(Token *tk, int *index)
{
  bool isConstant = (eat(tk, index).type == Const);
  char *identifier = expect(Ident, "Expected identifier name following var or const keyword.", tk, index).value;

  if (tk[*index].type == Semicolon)
  {
    eat(tk, index);
    if (isConstant)
    {
      printf("Must assign value to constant expression. No value provided.");
      exit(EXIT_FAILURE);
    }

    Stat *varDec = malloc(sizeof(Stat));
    if (varDec == NULL)
    {
      printf("Memory allocation failed when parsing variable declaration.\n");
      exit(EXIT_FAILURE);
    }

    varDec->kind = Kind_VarDeclaration;
    varDec->data = (StatData *)malloc(sizeof(StatData));
    if (varDec->data == NULL)
    {
      printf("Memory allocation failed when parsing variable declaration.\n");
      exit(EXIT_FAILURE);
    }
    varDec->data->VarDeclaration = (VarDeclaration *)malloc(sizeof(VarDeclaration));
    if (varDec->data->VarDeclaration == NULL)
    {
      printf("Memory allocation failed when parsing variable declaration.\n");
      exit(EXIT_FAILURE);
    }

    varDec->data->VarDeclaration->identifier = identifier;
    varDec->data->VarDeclaration->constant = false;
    return varDec;
  }

  expect(
      Equals,
      "Exprected equals sign when declaring variable.",
      tk,
      index);

  Stat *varDec = malloc(sizeof(Stat));
  if (varDec == NULL)
  {
    printf("Memory allocation failed when parsing variable declaration.\n");
    exit(EXIT_FAILURE);
  }

  varDec->kind = Kind_VarDeclaration;
  varDec->data = (StatData *)malloc(sizeof(StatData));
  if (varDec->data == NULL)
  {
    printf("Memory allocation failed when parsing variable declaration.\n");
    exit(EXIT_FAILURE);
  }
  varDec->data->VarDeclaration = (VarDeclaration *)malloc(sizeof(VarDeclaration));
  if (varDec->data->VarDeclaration == NULL)
  {
    printf("Memory allocation failed when parsing variable declaration.\n");
    exit(EXIT_FAILURE);
  }
  varDec->data->VarDeclaration->identifier = identifier;
  varDec->data->VarDeclaration->constant = isConstant;
  varDec->data->VarDeclaration->value = parse_expr(tk, index);

  if (tk[*index].type == Semicolon)
  {
    eat(tk, index);
  }

  return varDec;
}

Stat *parse_expr(Token *tk, int *index)
{
  switch (tk[*index].type)
  {
  case DQuote:
    return parse_string(tk, index);
  case SQuote:
    return parse_string(tk, index);
  case Funct:
    return parse_funct_declaration(tk, index);
  default:
    return parse_assignmnet_expr(tk, index);
  }
  exit(EXIT_FAILURE);
}

Stat *parse_funct_declaration(Token *tk, int *index)
{
  eat(tk, index);

  char *name = expect(Ident, "Expected function name following funct keyword.", tk, index).value;
  Stat *args = parse_args(tk, index);
  int agrs_size = sizeof(args) / sizeof(args[0]);
  int paramBuff = 5;
  char **paramaters = malloc(paramBuff * sizeof(char *));
  if (paramaters == NULL)
  {
    printf("Memory allocation failed when parsing paramaters inside function declaration.\n");
    exit(EXIT_FAILURE);
  }
  int i;
  for (i = 0; i < agrs_size; i)
  {
    if (args[0].kind != Ident)
    {
      printf("Expected identifier pared into agrs of functions.");
      exit(EXIT_FAILURE);
    }
    paramaters[i] = args[i].data->ExprData->Identifier->symbol;

    i++;
    if (i >= paramBuff)
    {
      char **paramaters = realloc(paramaters, paramBuff * sizeof(char *));
      if (paramaters == NULL)
      {
        printf("Memory reallocation failed when parsing paramaters inside function declaration.\n");
        exit(EXIT_FAILURE);
      }
    }
  }
  int paramcount = i;
  expect(OpenBrace, "Expected function body following declaration.", tk, index);
  int bodyBuff = 10;
  Stat **body = malloc(bodyBuff * sizeof(Stat *));
  if (body == NULL)
  {
    printf("Memory allocation failure when parsing if statement.");
    exit(EXIT_FAILURE);
  }
  int j = 0;
  while (
      tk[*index].type != ENDFILE &&
      tk[*index].type != ClosedBrace)
  {
    body[j] = parse_stat(tk, index);
    j++;
    if (j >= bodyBuff)
    {
      bodyBuff *= 2;
      body = realloc(body, bodyBuff * sizeof(Stat *));
      if (body == NULL)
      {
        printf("Memory reallocation failure when parsing if statement.");
        exit(EXIT_FAILURE);
      }
    }
  }
  int bodycount = j;

  expect(ClosedBrace, "Closing brace expected following function paramaters.", tk, index);

  Stat *functDec = malloc(sizeof(Stat));
  if (functDec == NULL)
  {
    printf("Memory allocation failed when parsing variable declaration.\n");
    exit(EXIT_FAILURE);
  }

  functDec->kind = Kind_FunctionDeclaration;
  functDec->data = (StatData *)malloc(sizeof(StatData));
  if (functDec->data == NULL)
  {
    printf("Memory allocation failed when parsing variable declaration.\n");
    exit(EXIT_FAILURE);
  }
  functDec->data->FunctionDeclaration = (FunctionDeclaration *)malloc(sizeof(FunctionDeclaration));
  if (functDec->data->FunctionDeclaration == NULL)
  {
    printf("Memory allocation failed when parsing variable declaration.\n");
    exit(EXIT_FAILURE);
  }
  functDec->data->FunctionDeclaration->name = name;
  functDec->data->FunctionDeclaration->body = body;
  functDec->data->FunctionDeclaration->bodyCount = bodycount;
  functDec->data->FunctionDeclaration->parameters = paramaters;
  functDec->data->FunctionDeclaration->paramCount = paramcount;

  return functDec;
}

Stat *parse_assignmnet_expr(Token *tk, int *index)
{
  Stat *left = parse_object_arr_expr(tk, index);

  if (tk[*index].type == Equals)
  {
    eat(tk, index);
    Stat *value = parse_expr(tk, index);
    if (tk[*index].type == Semicolon)
    {
      eat(tk, index);
    }

    Stat *assignExpr = malloc(sizeof(Stat));
    if (assignExpr == NULL)
    {
      printf("Memory allocation failed when parsing assignment expression.\n");
      exit(EXIT_FAILURE);
    }

    assignExpr->kind = Kind_AssignmentExpr;
    assignExpr->data = (StatData *)malloc(sizeof(StatData));
    if (assignExpr->data == NULL)
    {
      printf("Memory allocation failed when parsing assignment expression.\n");
      exit(EXIT_FAILURE);
    }
    assignExpr->data->ExprData = (ExprData *)malloc(sizeof(ExprData));
    if (assignExpr->data->ExprData == NULL)
    {
      printf("Memory allocation failed when parsing assignment expression.\n");
      exit(EXIT_FAILURE);
    }
    assignExpr->data->ExprData->AssignmentExpr = (AssignmentExpr *)malloc(sizeof(AssignmentExpr));
    if (assignExpr->data->ExprData->AssignmentExpr == NULL)
    {
      printf("Memory allocation failed when parsing assignment expression.\n");
      exit(EXIT_FAILURE);
    }

    assignExpr->data->ExprData->AssignmentExpr->assignee = left;
    assignExpr->data->ExprData->AssignmentExpr->value = value;

    retrun assignExpr;
  }
  else if (tk[*index].type == Inc)
  {
    eat(tk, index);

    Stat *numLit = malloc(sizeof(Stat));
    if (numLit == NULL)
    {
      printf("Memory allocation failed when parsing increment expression.\n");
      exit(EXIT_FAILURE);
    }

    numLit->kind = Kind_NumericLiteral;
    numLit->data = (StatData *)malloc(sizeof(StatData));
    if (numLit->data == NULL)
    {
      printf("Memory allocation failed when parsing increment expression.\n");
      exit(EXIT_FAILURE);
    }
    numLit->data->ExprData = (ExprData *)malloc(sizeof(ExprData));
    if (numLit->data->ExprData == NULL)
    {
      printf("Memory allocation failed when parsing increment expression.\n");
      exit(EXIT_FAILURE);
    }
    numLit->data->ExprData->NumericLiteral = (NumericLiteral *)malloc(sizeof(NumericLiteral));
    if (numLit->data->ExprData->NumericLiteral == NULL)
    {
      printf("Memory allocation failed when parsing increment expression.\n");
      exit(EXIT_FAILURE);
    }

    Stat *iden = malloc(sizeof(Stat));
    if (iden == NULL)
    {
      printf("Memory allocation failed when parsing increment expression.\n");
      exit(EXIT_FAILURE);
    }

    iden->kind = Kind_Identifier;
    iden->data = (StatData *)malloc(sizeof(StatData));
    if (iden->data == NULL)
    {
      printf("Memory allocation failed when parsing increment expression.\n");
      exit(EXIT_FAILURE);
    }
    iden->data->ExprData = (ExprData *)malloc(sizeof(ExprData));
    if (iden->data->ExprData == NULL)
    {
      printf("Memory allocation failed when parsing increment expression.\n");
      exit(EXIT_FAILURE);
    }
    iden->data->ExprData->Identifier = (Identifier *)malloc(sizeof(Identifier));
    if (iden->data->ExprData->Identifier == NULL)
    {
      printf("Memory allocation failed when parsing increment expression.\n");
      exit(EXIT_FAILURE);
    }

    iden->data->ExprData->NumericLiteral->value = 1;
    iden->data->ExprData->Identifier->symbol = left->data->ExprData->Identifier->symbol;

    Stat *inc = malloc(sizeof(Stat));
    if (inc == NULL)
    {
      printf("Memory allocation failed when parsing increment expression.\n");
      exit(EXIT_FAILURE);
    }

    inc->kind = Kind_BinaryExpr;
    inc->data = (StatData *)malloc(sizeof(StatData));
    if (inc->data == NULL)
    {
      printf("Memory allocation failed when parsing increment expression.\n");
      exit(EXIT_FAILURE);
    }
    inc->data->ExprData = (ExprData *)malloc(sizeof(ExprData));
    if (inc->data->ExprData == NULL)
    {
      printf("Memory allocation failed when parsing increment expression.\n");
      exit(EXIT_FAILURE);
    }
    inc->data->ExprData->BinaryExpr = (BinaryExpr *)malloc(sizeof(BinaryExpr));
    if (inc->data->ExprData->BinaryExpr == NULL)
    {
      printf("Memory allocation failed when parsing increment expression.\n");
      exit(EXIT_FAILURE);
    }
    inc->data->ExprData->BinaryExpr->binaryOperator = "+";
    inc->data->ExprData->BinaryExpr->left = iden;
    inc->data->ExprData->BinaryExpr->right = numLit;

    Stat *assign = malloc(sizeof(Stat));
    if (assign == NULL)
    {
      printf("Memory allocation failed when parsing increment expression.\n");
      exit(EXIT_FAILURE);
    }

    assign->kind = Kind_AssignmentExpr;
    assign->data = (StatData *)malloc(sizeof(StatData));
    if (assign->data == NULL)
    {
      printf("Memory allocation failed when parsing increment expression.\n");
      exit(EXIT_FAILURE);
    }
    assign->data->ExprData = (ExprData *)malloc(sizeof(ExprData));
    if (assign->data->ExprData == NULL)
    {
      printf("Memory allocation failed when parsing increment expression.\n");
      exit(EXIT_FAILURE);
    }
    assign->data->ExprData->AssignmentExpr = (AssignmentExpr *)malloc(sizeof(AssignmentExpr));
    if (assign->data->ExprData->AssignmentExpr == NULL)
    {
      printf("Memory allocation failed when parsing increment expression.\n");
      exit(EXIT_FAILURE);
    }

    assign->data->ExprData->AssignmentExpr->assignee = left;
    assign->data->ExprData->AssignmentExpr->value = inc;

    return assign;
  }
  else if (tk[*index].type == Inc)
  {
    eat(tk, index);

    Stat *numLit = malloc(sizeof(Stat));
    if (numLit == NULL)
    {
      printf("Memory allocation failed when parsing decrement expression.\n");
      exit(EXIT_FAILURE);
    }

    numLit->kind = Kind_NumericLiteral;
    numLit->data = (StatData *)malloc(sizeof(StatData));
    if (numLit->data == NULL)
    {
      printf("Memory allocation failed when parsing decrement expression.\n");
      exit(EXIT_FAILURE);
    }
    numLit->data->ExprData = (ExprData *)malloc(sizeof(ExprData));
    if (numLit->data->ExprData == NULL)
    {
      printf("Memory allocation failed when parsing decrement expression.\n");
      exit(EXIT_FAILURE);
    }
    numLit->data->ExprData->NumericLiteral = (NumericLiteral *)malloc(sizeof(NumericLiteral));
    if (numLit->data->ExprData->NumericLiteral == NULL)
    {
      printf("Memory allocation failed when parsing decrement expression.\n");
      exit(EXIT_FAILURE);
    }

    Stat *iden = malloc(sizeof(Stat));
    if (iden == NULL)
    {
      printf("Memory allocation failed when parsing decrement expression.\n");
      exit(EXIT_FAILURE);
    }

    iden->kind = Kind_Identifier;
    iden->data = (StatData *)malloc(sizeof(StatData));
    if (iden->data == NULL)
    {
      printf("Memory allocation failed when parsing decrement expression.\n");
      exit(EXIT_FAILURE);
    }
    iden->data->ExprData = (ExprData *)malloc(sizeof(ExprData));
    if (iden->data->ExprData == NULL)
    {
      printf("Memory allocation failed when parsing decrement expression.\n");
      exit(EXIT_FAILURE);
    }
    iden->data->ExprData->Identifier = (Identifier *)malloc(sizeof(Identifier));
    if (iden->data->ExprData->Identifier == NULL)
    {
      printf("Memory allocation failed when parsing decrement expression.\n");
      exit(EXIT_FAILURE);
    }

    iden->data->ExprData->NumericLiteral->value = 1;
    iden->data->ExprData->Identifier->symbol = left->data->ExprData->Identifier->symbol;

    Stat *dec = malloc(sizeof(Stat));
    if (dec == NULL)
    {
      printf("Memory allocation failed when parsing decrement expression.\n");
      exit(EXIT_FAILURE);
    }

    dec->kind = Kind_BinaryExpr;
    dec->data = (StatData *)malloc(sizeof(StatData));
    if (dec->data == NULL)
    {
      printf("Memory allocation failed when parsing decrement expression.\n");
      exit(EXIT_FAILURE);
    }
    dec->data->ExprData = (ExprData *)malloc(sizeof(ExprData));
    if (dec->data->ExprData == NULL)
    {
      printf("Memory allocation failed when parsing decrement expression.\n");
      exit(EXIT_FAILURE);
    }
    dec->data->ExprData->BinaryExpr = (BinaryExpr *)malloc(sizeof(BinaryExpr));
    if (dec->data->ExprData->BinaryExpr == NULL)
    {
      printf("Memory allocation failed when parsing decrement expression.\n");
      exit(EXIT_FAILURE);
    }
    dec->data->ExprData->BinaryExpr->binaryOperator = "-";
    dec->data->ExprData->BinaryExpr->left = iden;
    dec->data->ExprData->BinaryExpr->right = numLit;

    Stat *assign = malloc(sizeof(Stat));
    if (assign == NULL)
    {
      printf("Memory allocation failed when parsing decrement expression.\n");
      exit(EXIT_FAILURE);
    }

    assign->kind = Kind_AssignmentExpr;
    assign->data = (StatData *)malloc(sizeof(StatData));
    if (assign->data == NULL)
    {
      printf("Memory allocation failed when parsing decrement expression.\n");
      exit(EXIT_FAILURE);
    }
    assign->data->ExprData = (ExprData *)malloc(sizeof(ExprData));
    if (assign->data->ExprData == NULL)
    {
      printf("Memory allocation failed when parsing decrement expression.\n");
      exit(EXIT_FAILURE);
    }
    assign->data->ExprData->AssignmentExpr = (AssignmentExpr *)malloc(sizeof(AssignmentExpr));
    if (assign->data->ExprData->AssignmentExpr == NULL)
    {
      printf("Memory allocation failed when parsing decrement expression.\n");
      exit(EXIT_FAILURE);
    }

    assign->data->ExprData->AssignmentExpr->assignee = left;
    assign->data->ExprData->AssignmentExpr->value = dec;

    return assign;
  }
  return left;
}

Stat *parse_object_arr_expr(Token *tk, int *index)
{
  switch (tk[*index].type)
  {
  // case OpenBracket:
  // return parse_arr_expr();
  // case OpenBrace:
  // return parse_object_expr();
  default:
    return parse_and_or_expr(tk, index);
  }
}

Stat *parse_and_or_expr(Token *tk, int *index)
{
  Stat *left = parse_additive_expr(tk, index);

  while (
      tk[*index].value == "and" ||
      tk[*index].value == "And" ||
      tk[*index].value == "or" ||
      tk[*index].value == "Or")
  {
    char *operator= eat(tk, index).value;
    Stat *right = parse_additive_expr(tk, index);
    Stat *andOrExpr = malloc(sizeof(Stat));
    if (andOrExpr == NULL)
    {
      printf("Memory allocation failed when parsing and/or token.\n");
      exit(EXIT_FAILURE);
    }

    andOrExpr->kind = Kind_BinaryExpr;
    andOrExpr->data = (StatData *)malloc(sizeof(StatData));
    if (andOrExpr->data == NULL)
    {
      printf("Memory allocation failed when parsing and/or token.\n");
      exit(EXIT_FAILURE);
    }
    andOrExpr->data->ExprData = (ExprData *)malloc(sizeof(ExprData));
    if (andOrExpr->data->ExprData == NULL)
    {
      printf("Memory allocation failed when parsing and/or token.\n");
      exit(EXIT_FAILURE);
    }
    andOrExpr->data->ExprData->BinaryExpr = (BinaryExpr *)malloc(sizeof(BinaryExpr));
    if (andOrExpr->data->ExprData->BinaryExpr == NULL)
    {
      printf("Memory allocation failed when parsing and/or token.\n");
      exit(EXIT_FAILURE);
    }
    andOrExpr->data->ExprData->BinaryExpr->left = left;
    andOrExpr->data->ExprData->BinaryExpr->right = right;
    andOrExpr->data->ExprData->BinaryExpr->binaryOperator = operator;
    if (!(tk[*index].value == "and" ||
          tk[*index].value == "And" ||
          tk[*index].value == "or" ||
          tk[*index].value == "Or"))
    {
      return andOrExpr;
    }
  }

  return left;
}

Stat *parse_additive_expr(Token *tk, int *index)
{
  Stat *left = parse_multiplicitave_expr(tk, index);

  while (
      tk[*index].value[0] == '+' ||
      tk[*index].value[0] == '-' ||
      (tk[*index].value[0] == '=' && tk[*index].value[1] == '=') ||
      (tk[*index].value[0] == '>' && tk[*index].value[1] == '=') ||
      (tk[*index].value[0] == '<' && tk[*index].value[1] == '=') ||
      (tk[*index].value[0] == '!' && tk[*index].value[1] == '=') ||
      tk[*index].value[0] == '>' ||
      tk[*index].value[0] == '<')
  {
    char *operator= eat(tk, index).value;
    Stat *right = parse_multiplicitave_expr(tk, index);
    Stat *additiveExpr = malloc(sizeof(Stat));
    if (additiveExpr == NULL)
    {
      printf("Memory allocation failed when parsing additive token.\n");
      exit(EXIT_FAILURE);
    }

    additiveExpr->kind = Kind_BinaryExpr;
    additiveExpr->data = (StatData *)malloc(sizeof(StatData));
    if (additiveExpr->data == NULL)
    {
      printf("Memory allocation failed when parsing additive token.\n");
      exit(EXIT_FAILURE);
    }
    additiveExpr->data->ExprData = (ExprData *)malloc(sizeof(ExprData));
    if (additiveExpr->data->ExprData == NULL)
    {
      printf("Memory allocation failed when parsing additive token.\n");
      exit(EXIT_FAILURE);
    }
    additiveExpr->data->ExprData->BinaryExpr = (BinaryExpr *)malloc(sizeof(BinaryExpr));
    if (additiveExpr->data->ExprData->BinaryExpr == NULL)
    {
      printf("Memory allocation failed when parsing additive token.\n");
      exit(EXIT_FAILURE);
    }
    additiveExpr->data->ExprData->BinaryExpr->left = left;
    additiveExpr->data->ExprData->BinaryExpr->right = right;
    additiveExpr->data->ExprData->BinaryExpr->binaryOperator = operator;
    if (!(
            tk[*index].value[0] == '+' ||
            tk[*index].value[0] == '-' ||
            (tk[*index].value[0] == '=' && tk[*index].value[1] == '=') ||
            (tk[*index].value[0] == '>' && tk[*index].value[1] == '=') ||
            (tk[*index].value[0] == '<' && tk[*index].value[1] == '=') ||
            (tk[*index].value[0] == '!' && tk[*index].value[1] == '=') ||
            tk[*index].value[0] == '>' ||
            tk[*index].value[0] == '<'))
    {
      return additiveExpr;
    }
  }

  return left;
}

Stat *parse_multiplicitave_expr(Token *tk, int *index)
{
  Stat *left = parse_call_member_expr(tk, index);
  while (
      tk[*index].value[0] == '/' ||
      tk[*index].value[0] == '*' ||
      tk[*index].value[0] == '%')
  {
    char *operator= eat(tk, index).value;
    Stat *right = parse_call_member_expr(tk, index);
    Stat *multiplicitaveExpr = malloc(sizeof(Stat));
    if (multiplicitaveExpr == NULL)
    {
      printf("Memory allocation failed when parsing multiplicitave token.\n");
      exit(EXIT_FAILURE);
    }

    multiplicitaveExpr->kind = Kind_BinaryExpr;
    multiplicitaveExpr->data = (StatData *)malloc(sizeof(StatData));
    if (multiplicitaveExpr->data == NULL)
    {
      printf("Memory allocation failed when parsing multiplicitave token.\n");
      exit(EXIT_FAILURE);
    }
    multiplicitaveExpr->data->ExprData = (ExprData *)malloc(sizeof(ExprData));
    if (multiplicitaveExpr->data->ExprData == NULL)
    {
      printf("Memory allocation failed when parsing multiplicitave token.\n");
      exit(EXIT_FAILURE);
    }
    multiplicitaveExpr->data->ExprData->BinaryExpr = (BinaryExpr *)malloc(sizeof(BinaryExpr));
    if (multiplicitaveExpr->data->ExprData->BinaryExpr == NULL)
    {
      printf("Memory allocation failed when parsing multiplicitave token.\n");
      exit(EXIT_FAILURE);
    }
    multiplicitaveExpr->data->ExprData->BinaryExpr->left = left;
    multiplicitaveExpr->data->ExprData->BinaryExpr->right = right;
    multiplicitaveExpr->data->ExprData->BinaryExpr->binaryOperator = operator;
    if (!(
            tk[*index].value[0] == '/' ||
            tk[*index].value[0] == '*' ||
            tk[*index].value[0] == '%'))
    {
      return multiplicitaveExpr;
    }
  }

  return left;
}

Stat *parse_call_member_expr(Token *tk, int *index)
{
  Stat *member = parse_member_expr(tk, index);
  if (tk[*index].type == OpenParen)
  {
    return parse_call_expr(tk, index, member);
  }
  return member;
}

Stat *parse_call_expr(Token *tk, int *index, Stat *caller)
{
  Stat *call_expr = malloc(sizeof(Stat));
  if (call_expr == NULL)
  {
    printf("Memory allocation failed when parsing call expression.\n");
    exit(EXIT_FAILURE);
  }

  call_expr->kind = Kind_CallExpr;
  call_expr->data = (StatData *)malloc(sizeof(StatData));
  if (call_expr->data == NULL)
  {
    printf("Memory allocation failed when parsing call expression.\n");
    exit(EXIT_FAILURE);
  }
  call_expr->data->ExprData = (ExprData *)malloc(sizeof(ExprData));
  if (call_expr->data->ExprData == NULL)
  {
    printf("Memory allocation failed when parsing call expression.\n");
    exit(EXIT_FAILURE);
  }
  call_expr->data->ExprData->CallExpr = (CallExpr *)malloc(sizeof(CallExpr));
  if (call_expr->data->ExprData->CallExpr == NULL)
  {
    printf("Memory allocation failed when parsing call expression.\n");
    exit(EXIT_FAILURE);
  }
  call_expr->data->ExprData->CallExpr->args = parse_args(tk, index);
  size_t size = sizeof(call_expr->data->ExprData->CallExpr->args) / sizeof(call_expr->data->ExprData->CallExpr->args[0]);
  call_expr->data->ExprData->CallExpr->argCount = size;
  call_expr->data->ExprData->CallExpr->caller = caller;
  if (tk[*index].type == OpenParen)
  {
    call_expr = parse_call_expr(tk, index, call_expr);
  }

  return call_expr;
}

Stat *parse_args(Token *tk, int *index)
{
  expect(OpenParen, "Expected open parenthesis when parsing args.", tk, index);

  Stat *args = NULL;
  if (tk[*index].type != ClosedParen)
  {
    args = parse_arguments_list(tk, index);
  }
  expect(ClosedParen, "Expected closing parenthesis when parsing args.", tk, index);
  if (tk[*index].type == Semicolon)
  {
    eat(tk, index);
  }
  return args;
}

Stat *parse_arguments_list(Token *tk, int *index)
{
  int argsBuffSize = 5;
  Stat **args = malloc(argsBuffSize * sizeof(Stat));
  if (args == NULL)
  {
    printf("Memory allocation failed when reallocating args.");
    exit(EXIT_FAILURE);
  }
  int i = 0;
  args[i] = parse_expr(tk, index);
  i++;
  while (((tk[*index].type != ENDFILE) && (tk[*index].type == Comma)))
  {
    eat(tk, index);
    args[i] = parse_expr(tk, index);
    i++;
    if (i >= argsBuffSize)
    {
      argsBuffSize *= 2;
      args = realloc(args, argsBuffSize * sizeof(Stat));
      if (args == NULL)
      {
        printf("Memory reallocation failed when reallocating args.");
        exit(EXIT_FAILURE);
      }
    }
  }

  return *args;
}

Stat *parse_member_expr(Token *tk, int *index)
{
  return parse_primary_expr(tk, index);
}

Stat *parse_primary_expr(Token *tk, int *index)
{
  switch (tk[*index].type)
  {
  case Ident:
    Stat *ident_expr = malloc(sizeof(Stat));
    if (ident_expr == NULL)
    {
      printf("Memory allocation failed when parsing primary identifier expression.\n");
      exit(EXIT_FAILURE);
    }

    ident_expr->kind = Kind_Identifier;
    ident_expr->data = (StatData *)malloc(sizeof(StatData));
    if (ident_expr->data == NULL)
    {
      printf("Memory allocation failed when parsing primary identifier expression.\n");
      exit(EXIT_FAILURE);
    }
    ident_expr->data->ExprData = (ExprData *)malloc(sizeof(ExprData));
    if (ident_expr->data->ExprData == NULL)
    {
      printf("Memory allocation failed when parsing primary identifier expression.\n");
      exit(EXIT_FAILURE);
    }
    ident_expr->data->ExprData->Identifier = (Identifier *)malloc(sizeof(Identifier));
    if (ident_expr->data->ExprData->Identifier == NULL)
    {
      printf("Memory allocation failed when parsing primary identifier expression.\n");
      exit(EXIT_FAILURE);
    }
    ident_expr->data->ExprData->Identifier->symbol = eat(tk, index).value;
    return ident_expr;
  case Number:
    Stat *number_expr = malloc(sizeof(Stat));
    if (number_expr == NULL)
    {
      printf("Memory allocation failed when parsing primary numeric expression.\n");
      exit(EXIT_FAILURE);
    }

    number_expr->kind = Kind_NumericLiteral;
    number_expr->data = (StatData *)malloc(sizeof(StatData));
    if (number_expr->data == NULL)
    {
      printf("Memory allocation failed when parsing primary numeric expression.\n");
      exit(EXIT_FAILURE);
    }
    number_expr->data->ExprData = (ExprData *)malloc(sizeof(ExprData));
    if (number_expr->data->ExprData == NULL)
    {
      printf("Memory allocation failed when parsing primary numeric expression.\n");
      exit(EXIT_FAILURE);
    }
    number_expr->data->ExprData->NumericLiteral = (NumericLiteral *)malloc(sizeof(NumericLiteral));
    if (number_expr->data->ExprData->NumericLiteral == NULL)
    {
      printf("Memory allocation failed when parsing primary numeric expression.\n");
      exit(EXIT_FAILURE);
    }
    number_expr->data->ExprData->NumericLiteral->value = string_to_double(eat(tk, index).value);
    return number_expr;
  case OpenParen:
  {
    eat(tk, index);
    Stat *value = parse_expr(tk, index);

    expect(ClosedParen, "Expected closed parenthasis when parsing primary expressions.", tk, index);
    return value;
  }
  default:
    printf("Unexpected token found during parsing! Type: %d", tk[*index].type);
    exit(EXIT_FAILURE);
  }
}

double string_to_double(const char *str)
{
  double result = 0.0;
  bool first_digit = true;
  bool after_decimal = false;

  for (; *str != '\0'; ++str)
  {
    if (isdigit(*str))
    {
      if (first_digit)
      {
        result = *str - '0';
        first_digit = false;
      }
      else if (after_decimal)
      {
        result = result * 10.0 + ((*str - '0') / 10.0);
      }
      else
      {
        result = result * 10.0 + (*str - '0');
      }
    }
    else if (*str == '.')
    {
      after_decimal = true;
    }
    else
    {
      printf("String value assigned as number.");
      exit(EXIT_FAILURE);
    }
  }

  return result;
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

  expect(quoteType, "Expected end quote at end of string.", tk, index);

  return str;
}