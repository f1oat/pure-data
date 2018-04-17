%{
#include <math.h>   /* For math functions: cos(), sin(), etc. */
#include <string.h>
#include <stdio.h>

#include "m_pd.h"
#include "math_expr_calc.h" /* Contains definition of 'symrec'        */
#include "lex.math_expr.h"
#include "math_expr_ast.h"

static double fn_plus(double d0, double d1) { return d0 + d1; }
static double fn_minus(double d0, double d1) { return d0 - d1; }
static double fn_mul(double d0, double d1) { return d0 * d1; }
static double fn_div(double d0, double d1) { return d0 / d1; }
static double fn_mod(double d0, double d1) { return (long)d0 % (long)d1; }
static double fn_pow(double d0, double d1) { return pow(d0, d1); }
static double fn_neg(double d0) { return -d0; }

static double fn_eq(double d0, double d1) { return d0 == d1; }
static double fn_ne(double d0, double d1) { return d0 != d1; }
static double fn_le(double d0, double d1) { return d0 <= d1; }
static double fn_lt(double d0, double d1) { return d0 < d1; }
static double fn_ge(double d0, double d1) { return d0 >= d1; }
static double fn_gt(double d0, double d1) { return d0 > d1; }

static void print_error(ast* tree, const char* msg, int c) {
    char buf[100];

    switch(c) {
    case ERR_UNKNOWN_FUNC:
        snprintf(buf, 100, "unknown function \"%s\"", msg);
        break;
    default:
        sprintf(buf, "???");
        break;
    }

    math_expr_error(tree, buf);
}

%}

%define api.prefix {math_expr_}
%define api.pure false
%parse-param {ast *ast}

%union {
  double val;   /* for returning numbers                  */
  Node* node;
}

%token <val> T_NUM T_REF
%token <val> T_EQ T_NOT_EQ
%token <val> T_UFUNC T_BFUNC T_ERROR

%type  <node>  exp input

%left T_EQ T_NOT_EQ T_LE T_LT T_GT T_GE
%left  '-' '+'
%left  '*' '/' '%'
%left  T_NEG     /* unary minus    */
%right '^'     /* exponentiation */

/* Grammar follows */

%%

input : exp { node_add_cont(ast_root(ast), $1); }
;

exp : T_NUM                 { $$ = node_create_value_float($1);                         }
      | T_ERROR             { print_error(ast, math_expr_text, $1); YYERROR;            }
      | T_UFUNC '(' exp ')' { $$ = node_create_ufunc(ufnNameToPtr($1), $3);             }
      | T_BFUNC '(' exp ',' exp ')' { $$ = node_create_bfunc(bfnNameToPtr($1), $3, $5); }
      | T_REF               { $$ = node_create_ref_float(ast_ref(ast, $1));             }
      | exp T_EQ exp        { $$ = node_create_bfunc(fn_eq, $1, $3);                    }
      | exp T_NOT_EQ exp    { $$ = node_create_bfunc(fn_ne, $1, $3);                    }
      | exp T_LT exp        { $$ = node_create_bfunc(fn_lt, $1, $3);                    }
      | exp T_LE exp        { $$ = node_create_bfunc(fn_le, $1, $3);                    }
      | exp T_GT exp        { $$ = node_create_bfunc(fn_gt, $1, $3);                    }
      | exp T_GE exp        { $$ = node_create_bfunc(fn_ge, $1, $3);                    }
      | exp '+' exp         { $$ = node_create_bfunc(fn_plus, $1, $3);                  }
      | exp '-' exp         { $$ = node_create_bfunc(fn_minus, $1, $3);                 }
      | exp '*' exp         { $$ = node_create_bfunc(fn_mul, $1, $3);                   }
      | exp '/' exp         { $$ = node_create_bfunc(fn_div, $1, $3);                   }
      | exp '%' exp         { $$ = node_create_bfunc(fn_mod, $1, $3);                   }
      | '-' exp %prec T_NEG { $$ = node_create_ufunc(fn_neg, $2);                       }
      | exp '^' exp         { $$ = node_create_bfunc(fn_pow, $1, $3);                   }
      | '(' exp ')'         { $$ = node_create_cont($2);                                }
;

/* End of Grammar */

%%

void math_expr_error(ast* ast, const char* s)
{
    ast_invalidate(ast);
    pd_error(0, "[math.expr] parse error: %s", s);
}

int math_expr_parse_ast(ast* ast, const char* s)
{
    post("parse: %s", s);

    YY_BUFFER_STATE b;
    b = math_expr__scan_string(s);

    int ok = yyparse(ast);

    math_expr__delete_buffer(b);
    return ok;
}

