#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

typedef struct Program Program;
typedef struct ReturnStat ReturnStat;
typedef struct VarDeclaration VarDeclaration;
typedef struct FunctionDeclaration FunctionDeclaration;
typedef struct IfStatement IfStatement;
typedef struct WhileStatement WhileStatement;
typedef struct ForStatement ForStatement;
typedef struct AssignmentExpr AssignmentExpr;
typedef struct BinaryExpr BinaryExpr;
typedef struct CallExpr CallExpr;
typedef struct MemberExpr MemberExpr;
typedef struct Identifier Identifier;
typedef struct NumericLiteral NumericLiteral;
typedef struct StringLiteral StringLiteral;
typedef struct Property Property;
typedef struct ObjectLiteral ObjectLiteral;
typedef struct ArrProperty ArrProperty;
typedef struct ArrLiteral ArrLiteral;
typedef struct Expr Expr;
typedef struct Stat Stat;
typedef union ExprData ExprData;
typedef union StatData StatData;

typedef enum NodeType
{
  // Statements
  Kind_Program,
  Kind_VarDeclaration,
  Kind_FunctionDeclaration,
  Kind_IfStatement,
  Kind_WhileStatement,
  Kind_ForStatement,
  // Expressions
  Kind_AssignmentExpr,
  Kind_MemberExpr,
  Kind_CallExpr,
  // Literals
  Kind_Property,
  Kind_ArrProperty,
  Kind_ObjectLiteral,
  Kind_ArrLiteral,
  Kind_NumericLiteral,
  Kind_StringLiteral,
  Kind_NullLiteral,
  Kind_Identifier,
  Kind_BinaryExpr,
  Kind_ConditionalExpr,
  Kind_ReturnStat
} NodeType;

union ExprData
{
  AssignmentExpr *AssignmentExpr;
  BinaryExpr *BinaryExpr;
  CallExpr *CallExpr;
  MemberExpr *MemberExpr;
  Identifier *Identifier;
  NumericLiteral *NumericLiteral;
  StringLiteral *StringLiteral;
  Property *Property;
  ObjectLiteral *ObjectLiteral;
  ArrProperty *ArrProperty;
  ArrLiteral *ArrLiteral;
};

struct Expr
{
  NodeType kind;
  ExprData *data;
};

union StatData
{
  Program *Program;
  ReturnStat *ReturnStat;
  VarDeclaration *VarDeclaration;
  FunctionDeclaration *FunctionDeclaration;
  IfStatement *IfStatement;
  WhileStatement *WhileStatement;
  ForStatement *ForStatement;
  ExprData *ExprData;
};

struct Stat
{
  NodeType kind;
  StatData *data;
};

struct Program
{
  Stat *body;
  size_t bodyCount;
};

struct ReturnStat
{
  Stat right;
};

struct VarDeclaration
{
  bool constant;
  char *identifier;
  Stat value;
};

struct FunctionDeclaration
{
  char **parameters;
  size_t paramCount;
  char *name;
  Stat *body;
  size_t bodyCount;
  Stat ret;
};

struct IfStatement
{
  Stat comparison;
  Stat *body;
  size_t bodyCount;
  Stat *elseBody;
  size_t elseBodyCount;
};

struct WhileStatement
{
  Stat comparison;
  Stat *body;
  size_t bodyCount;
};

struct ForStatement
{
  VarDeclaration variable;
  Stat increment;
  Stat comparison;
  Stat *body;
  size_t bodyCount;
};

struct AssignmentExpr
{
  Stat assignee;
  Stat value;
};

struct BinaryExpr
{
  Stat *left;
  Stat *right;
  char *binaryOperator;
};

struct CallExpr
{
  Stat *args;
  size_t argCount;
  Stat *caller;
};

struct MemberExpr
{
  Stat object;
  Stat property;
  bool computed;
};

struct Identifier
{
  char *symbol;
};

struct NumericLiteral
{
  double value;
};

struct StringLiteral
{
  char *value;
};

struct Property
{
  char *key;
  Stat value;
};

struct ObjectLiteral
{
  Property *properties;
  size_t propertyCount;
};

struct ArrProperty
{
  size_t key;
  Stat value;
};

struct ArrLiteral
{
  ArrProperty *properties;
  size_t propertyCount;
};

#endif