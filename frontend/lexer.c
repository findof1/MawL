#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

typedef enum TokenType
{
  Number,
  Identifier,
  Var,
  Print,
  Const,
  Comma,
  Colon,
  Dot,
  SQuote,
  DQuote,
  Funct,
  Return,
  If,
  And,
  Or,
  Inc,
  Dec,
  For,
  Else,
  While,
  BinaryOperator,
  Equals,
  DEquals,
  GrEq,
  LsEq,
  Gr,
  Ls,
  NotEqual,
  Semicolon,
  OpenParen,
  ClosedParen,
  OpenBrace,
  ClosedBrace,
  OpenBracket,
  ClosedBracket,
  ENDFILE,
} TokenType;

typedef struct
{
  char *value;
  TokenType type;
} Token;

Token createToken(const char *value, TokenType type)
{

  // test print printf("OG: %s", value);
  bool isEmpty = true;

  if (*value != '\0')
  {
    isEmpty = false;
  }

  size_t len = strlen(value);

  char *valueCopy = malloc((len + 1) * sizeof(char));
  if (valueCopy == NULL)
  {
    printf("Memory allocation failed when creating token. Exiting...");
    exit(1);
  }

  strcpy(valueCopy, value);
  // test print printf("Val: %s", valueCopy);

  Token ret = {valueCopy, type};
  return ret;
}

bool isSkippable(char c)
{
  if (c == '\n' || c == '\r' || c == '\t' || c == ' ' || c == '\0' || c == '\\' || c == 10)
  {
    return true;
  }
  return false;
}

bool isAlpha(char c)
{

  if (isupper(c) || islower(c))
  {
    return true;
  }
  return false;
}

#define TOKENS_BUFF 64

Token *tokenize(char *src)
{
  int buffsize = TOKENS_BUFF;
  Token *tokens = malloc(buffsize * sizeof(Token));

  if (tokens == NULL)
  {
    printf("Memory allocation failed when tokenizing source code. Exiting...");
    return NULL;
  }
  bool insideStr = false;
  bool doubleQuotesForStr = false;

  int pos = 0;
  int tokenPos = 0;

  while (src[pos] != '\0')
  {

    if (src[pos] == '(' && !insideStr)
    {
      const char val[] = {src[pos], '\0'};
      tokens[tokenPos] = createToken(val, OpenParen);
      pos++;
      tokenPos++;
    }
    else if (src[pos] == ')' && !insideStr)
    {
      const char val[] = {src[pos], '\0'};
      tokens[tokenPos] = createToken(val, ClosedParen);
      pos++;
      tokenPos++;
    }
    else if (src[pos] == '{' && !insideStr)
    {
      const char val[] = {src[pos], '\0'};
      tokens[tokenPos] = createToken(val, OpenBrace);
      pos++;
      tokenPos++;
    }
    else if (src[pos] == '}' && !insideStr)
    {
      const char val[] = {src[pos], '\0'};
      tokens[tokenPos] = createToken(val, ClosedBrace);
      pos++;
      tokenPos++;
    }
    else if (src[pos] == '[' && !insideStr)
    {
      const char val[] = {src[pos], '\0'};
      tokens[tokenPos] = createToken(val, OpenBracket);
      pos++;
      tokenPos++;
    }
    else if (src[pos] == ']' && !insideStr)
    {
      const char val[] = {src[pos], '\0'};
      tokens[tokenPos] = createToken(val, ClosedBracket);
      pos++;
      tokenPos++;
    }
    else if (src[pos] == '/' && src[pos + 1] == '/' && !insideStr)
    {
      pos += 2;
      while (src[pos] != '\0' && (src[pos] != '\n' && src[pos] != '\r'))
      {
        pos++;
      }
    }
    else if (src[pos] == '&' && src[pos + 1] == '&' && !insideStr)
    {
      pos += 2;
      char val[] = "and";
      tokens[tokenPos] = createToken(val, And);
      tokenPos++;
    }
    else if (src[pos] == '+' && src[pos + 1] == '+' && !insideStr)
    {
      pos += 2;
      char val[] = "++";
      tokens[tokenPos] = createToken(val, Inc);
      tokenPos++;
    }
    else if (src[pos] == '-' && src[pos + 1] == '-' && !insideStr)
    {
      pos += 2;
      char val[] = "--";
      tokens[tokenPos] = createToken(val, Dec);
      tokenPos++;
    }
    else if (src[pos] == '|' && src[pos + 1] == '|' && !insideStr)
    {
      pos += 2;
      char val[] = "or";
      tokens[tokenPos] = createToken(val, Dec);
      tokenPos++;
    }
    else if (src[pos] == '.' && !insideStr)
    {
      const char val[] = {src[pos], '\0'};
      tokens[tokenPos] = createToken(val, Dot);
      pos++;
      tokenPos++;
    }
    else if (
        (src[pos] == '+' ||
         src[pos] == '-' ||
         src[pos] == '*' ||
         src[pos] == '/' ||
         src[pos] == '%') &&
        !insideStr)
    {
      const char val[] = {src[pos], '\0'};
      tokens[tokenPos] = createToken(val, BinaryOperator);
      pos++;
      tokenPos++;
    }
    else if (src[pos] == '=' && src[pos + 1] == '=' && !insideStr)
    {
      pos += 2;
      char val[] = "==";
      tokens[tokenPos] = createToken(val, DEquals);
      tokenPos++;
    }
    else if (src[pos] == '>' && src[pos + 1] == '=' && !insideStr)
    {
      pos += 2;
      char val[] = ">=";
      tokens[tokenPos] = createToken(val, GrEq);
      tokenPos++;
    }
    else if (src[pos] == '<' && src[pos + 1] == '=' && !insideStr)
    {
      pos += 2;
      char val[] = "<=";
      tokens[tokenPos] = createToken(val, LsEq);
      tokenPos++;
    }
    else if (src[pos] == '!' && src[pos + 1] == '=' && !insideStr)
    {
      pos += 2;
      char val[] = "!=";
      tokens[tokenPos] = createToken(val, NotEqual);
      tokenPos++;
    }
    else if (src[pos] == '=' && !insideStr)
    {
      const char val[] = {src[pos], '\0'};
      tokens[tokenPos] = createToken(val, Equals);
      pos++;

      tokenPos++;
    }
    else if (src[pos] == '>' && !insideStr)
    {
      const char val[] = {src[pos], '\0'};
      tokens[tokenPos] = createToken(val, Gr);
      pos++;
      tokenPos++;
    }
    else if (src[pos] == '<' && !insideStr)
    {
      const char val[] = {src[pos], '\0'};
      tokens[tokenPos] = createToken(val, Ls);
      pos++;
      tokenPos++;
    }
    else if (src[pos] == ';' && !insideStr)
    {
      const char val[] = {src[pos], '\0'};
      tokens[tokenPos] = createToken(val, Semicolon);
      pos++;
      tokenPos++;
    }
    else if (src[pos] == ':' && !insideStr)
    {
      const char val[] = {src[pos], '\0'};
      tokens[tokenPos] = createToken(val, Colon);
      pos++;
      tokenPos++;
    }
    else if (src[pos] == ',' && !insideStr)
    {
      const char val[] = {src[pos], '\0'};
      tokens[tokenPos] = createToken(val, Comma);
      pos++;
      tokenPos++;
    }
    else if (src[pos] == '\'')
    {
      const char val[] = {src[pos], '\0'};
      tokens[tokenPos] = createToken(val, SQuote);
      pos++;
      tokenPos++;
      insideStr = !insideStr;
      doubleQuotesForStr = false;
    }
    else if (src[pos] == '\"')
    {
      const char val[] = {src[pos], '\0'};
      tokens[tokenPos] = createToken(val, DQuote);
      pos++;
      tokenPos++;
      insideStr = !insideStr;
      doubleQuotesForStr = true;
    }
    else if (isdigit(src[pos]) && !insideStr)
    {
      int buffsize = 64;
      char *num = malloc(buffsize + 1 * sizeof(char));
      if (num == NULL)
      {
        printf("Memory allocation failed when creating number. Exiting...");
        return NULL;
      }
      int loops = 0;
      while (src[pos] != '\0' && isdigit(src[pos]))
      {
        num[loops] = src[pos];

        pos++;
        loops++;
        if (loops >= buffsize)
        {
          buffsize *= 2;
          num = realloc(num, buffsize + 1 * sizeof(Token));
          if (num == NULL)
          {
            printf("Memory reallocation failed when creating number. Exiting...");
            return NULL;
          }
        }
      }
      num[loops] = '\0';
      tokens[tokenPos] = createToken(num, Number);
      tokenPos++;
    }
    else if (insideStr)
    {
      int buffsize = 64;
      char *ident = malloc(buffsize * sizeof(char));
      if (ident == NULL)
      {
        printf("Memory allocation failed when creating string ident. Exiting...");
        return NULL;
      }
      int loops = 0;
      while (src[pos] != '\0' && src[pos] != (doubleQuotesForStr ? '\"' : '\''))
      {
        ident[loops] = src[pos];
        pos++;
        loops++;
        if (loops >= buffsize)
        {
          buffsize *= 2;
          ident = realloc(ident, buffsize * sizeof(Token));
          if (ident == NULL)
          {
            printf("Memory reallocation failed when creating string ident. Exiting...");
            return NULL;
          }
        }
      }

      ident[loops] = '\0';
      tokens[tokenPos] = createToken(ident, Identifier);
      tokenPos++;
    }
    else if (isAlpha(src[pos]))
    {

      int buffsize = 64;
      char *ident = malloc(buffsize * sizeof(char));
      if (ident == NULL)
      {
        printf("Memory allocation failed when creating ident. Exiting...");
        return NULL;
      }
      int loops = 0;
      while (src[pos] != '\0' && src[pos] != '\n' && isAlpha(src[pos]))
      {
        ident[loops] = src[pos];
        pos++;
        loops++;
        if (loops >= buffsize)
        {
          buffsize *= 2;
          ident = realloc(ident, buffsize * sizeof(Token));
          if (ident == NULL)
          {
            printf("Memory reallocation failed when creating ident. Exiting...");
            return NULL;
          }
        }
      }

      ident[loops] = '\0';

      if (strcmp(ident, "var") == 0 || strcmp(ident, "Var") == 0)
      {

        tokens[tokenPos] = createToken(ident, Var);
        tokenPos++;
      }
      else if (strcmp(ident, "if") == 0 || strcmp(ident, "If") == 0)
      {
        tokens[tokenPos] = createToken(ident, If);
        tokenPos++;
      }
      else if (strcmp(ident, "const") == 0 || strcmp(ident, "Const") == 0)
      {
        tokens[tokenPos] = createToken(ident, Const);
        tokenPos++;
      }
      else if (strcmp(ident, "return") == 0 || strcmp(ident, "Return") == 0)
      {
        tokens[tokenPos] = createToken(ident, Return);
        tokenPos++;
      }
      else if (strcmp(ident, "funct") == 0 || strcmp(ident, "Funct") == 0)
      {
        tokens[tokenPos] = createToken(ident, Funct);
        tokenPos++;
      }
      else if (strcmp(ident, "else") == 0 || strcmp(ident, "Else") == 0)
      {
        tokens[tokenPos] = createToken(ident, Else);
        tokenPos++;
      }
      else if (strcmp(ident, "ifelse") == 0 || strcmp(ident, "Ifelse") == 0 || strcmp(ident, "IfElse") == 0 || strcmp(ident, "ifElse") == 0)
      {
        tokens[tokenPos] = createToken(ident, If);
        tokenPos++;
        tokens[tokenPos] = createToken(ident, Else);
        tokenPos++;
      }
      else if (strcmp(ident, "while") == 0 || strcmp(ident, "While") == 0)
      {
        tokens[tokenPos] = createToken(ident, While);
        tokenPos++;
      }
      else if (strcmp(ident, "for") == 0 || strcmp(ident, "For") == 0)
      {
        tokens[tokenPos] = createToken(ident, For);
        tokenPos++;
      }
      else if (strcmp(ident, "or") == 0 || strcmp(ident, "Or") == 0)
      {
        tokens[tokenPos] = createToken(ident, Or);
        tokenPos++;
      }
      else if (strcmp(ident, "and") == 0 || strcmp(ident, "And") == 0)
      {
        tokens[tokenPos] = createToken(ident, And);
        tokenPos++;
      }
      else if (strcmp(ident, "var") == 0 || strcmp(ident, "Var") == 0)
      {
        tokens[tokenPos] = createToken(ident, Var);
        tokenPos++;
      }
      else
      {

        tokens[tokenPos] = createToken(ident, Identifier);
        tokenPos++;
      }
    }
    else if (isSkippable(src[pos]))
    {
      pos++;
    }
    else
    {
      printf("Character not recognised when parsing: %c. Code: %d. ", src[pos], (int)src[pos]);
      return NULL;
    }

    if (tokenPos + 2 >= buffsize)
    {
      buffsize *= 2;
      tokens = realloc(tokens, buffsize * sizeof(Token));
      if (tokens == NULL)
      {
        printf("Memory reallocation failed when tokenizing source code. Exiting...");
        return NULL;
      }
    }
  }

  char endfileVal[] = "ENDFILE";
  tokens[tokenPos] = createToken(endfileVal, ENDFILE);
  return tokens;
}