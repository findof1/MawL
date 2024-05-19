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

// Define Stat struct and StatData union first
union StatData
{
  Program *Program;
  ReturnStat *ReturnStat;
  VarDeclaration *VarDeclaration;
  FunctionDeclaration *FunctionDeclaration;
  IfStatement *IfStatement;
  WhileStatement *WhileStatement;
  ForStatement *ForStatement;
  Expr *Expr;
};

struct Stat
{
  NodeType kind;
  StatData *data;
};

// Define other structs that depend on Expr and Stat
struct Program
{
  Stat *body;
  size_t bodyCount;
};

struct ReturnStat
{
  Expr right;
};

struct VarDeclaration
{
  bool constant;
  char *identifier;
  Expr value;
};

struct FunctionDeclaration
{
  char **parameters;
  size_t paramCount;
  char *name;
  Stat *body;
  size_t bodyCount;
  Expr ret;
};

struct IfStatement
{
  Expr comparison;
  Stat *body;
  size_t bodyCount;
  Stat *elseBody;
  size_t elseBodyCount;
};

struct WhileStatement
{
  Expr comparison;
  Stat *body;
  size_t bodyCount;
};

struct ForStatement
{
  VarDeclaration variable;
  Expr increment;
  Expr comparison;
  Stat *body;
  size_t bodyCount;
};

struct AssignmentExpr
{
  Expr assignee;
  Expr value;
};

struct BinaryExpr
{
  Expr left;
  Expr right;
  char *binaryOperator;
};

struct CallExpr
{
  Expr *args;
  size_t argCount;
  Expr caller;
};

struct MemberExpr
{
  Expr object;
  Expr property;
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
  Expr value;
};

struct ObjectLiteral
{
  Property *properties;
  size_t propertyCount;
};

struct ArrProperty
{
  size_t key;
  Expr value;
};

struct ArrLiteral
{
  ArrProperty *properties;
  size_t propertyCount;
};
