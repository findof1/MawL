#ifndef LEXER_H
#define LEXER_H

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
Token createToken(char value[], TokenType type);
Token *tokenize(char *src);
#endif