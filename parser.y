%{
/*
 * Analisador Sintático LALR(1) para a linguagem X25b
 * Avaliação Parcial 2 - Compiladores
 * Implementado usando GNU Bison
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "semantic.h"

/* Funções externas */
extern int yylex(void);
extern FILE *yyin;
extern int linha;
extern int coluna;
extern char *yytext;

/* Variável para raiz do programa (declarada aqui, extern em main.c) */
NoPrograma *programa_raiz = NULL;

/* Contadores de erros */
int erros_sintaticos = 0;

/* Função de tratamento de erros */
void yyerror(const char *s);

%}

/* União para valores semânticos */
%union {
    int ival;
    double fval;
    char *sval;
    struct NoPrograma *programa;
    struct NoDecl *declaracao;
    struct NoCmd *comando;
    struct NoExpr *expressao;
    struct NoVar *variavel;
    struct ListaVar *lista_var;
    struct ListaEscreva *lista_escreva;
    int tipo_dado;
}

/* Tokens */
%token PROGRAMA FIMPROG
%token DECLARACOES ALGORITMO
%token INTEIRO REAL LISTAINT LISTAREAL
%token LEIA ESCREVA
%token SE ENTAO SENAO FIMSE
%token ENQUANTO FACA FIMENQ
%token ATRIB

/* Operadores relacionais */
%token OP_MAQ OP_MAI OP_MEQ OP_MEI OP_IGU OP_DIF

/* Operadores lógicos */
%token OP_OU OP_E OP_NAO

/* Literais */
%token <ival> CONST_INT
%token <fval> CONST_REAL
%token <sval> CADEIA_LIT
%token <sval> ID

/* Tipos dos não-terminais */
%type <programa> programa
%type <declaracao> area_declaracoes lista_declaracoes declaracao
%type <comando> area_algoritmo lista_comandos comando cmd_atrib cmd_leia cmd_escreva cmd_se cmd_enquanto
%type <expressao> expressao expr_aritmetica expr_relacional expr_logica termo fator
%type <variavel> variavel
%type <lista_var> lista_variaveis
%type <lista_escreva> lista_escreva item_escreva
%type <tipo_dado> tipo

/* Precedência e associatividade */
%left OP_OU
%left OP_E
%right OP_NAO
%left OP_MAQ OP_MAI OP_MEQ OP_MEI OP_IGU OP_DIF
%left '+' '-'
%left '*' '/'
%right UMINUS

%%

/* ========== Regras da Gramática ========== */

programa
    : PROGRAMA area_declaracoes area_algoritmo FIMPROG
        {
            $$ = criar_programa(NULL, $2, $3);
            programa_raiz = $$;
        }
    ;

area_declaracoes
    : DECLARACOES lista_declaracoes
        { $$ = $2; }
    | DECLARACOES
        { $$ = NULL; }
    | /* vazio */
        { $$ = NULL; }
    ;

lista_declaracoes
    : lista_declaracoes declaracao
        { $$ = concat_declaracoes($1, $2); }
    | declaracao
        { $$ = $1; }
    ;

declaracao
    : tipo ID
        { 
            $$ = criar_declaracao($1, $2, 0); 
        }
    | tipo ID '[' CONST_INT ']'
        { 
            if ($1 == TIPO_LISTAINT || $1 == TIPO_LISTAREAL) {
                $$ = criar_declaracao($1, $2, $4);
            } else {
                yyerror("Array deve ser declarado com LISTAINT ou LISTAREAL");
                $$ = criar_declaracao($1, $2, $4);
            }
        }
    ;

tipo
    : INTEIRO
        { $$ = TIPO_INTEIRO; }
    | REAL
        { $$ = TIPO_REAL; }
    | LISTAINT
        { $$ = TIPO_LISTAINT; }
    | LISTAREAL
        { $$ = TIPO_LISTAREAL; }
    ;

area_algoritmo
    : ALGORITMO lista_comandos
        { $$ = $2; }
    | ALGORITMO
        { $$ = NULL; }
    ;

lista_comandos
    : lista_comandos comando
        { $$ = concat_comandos($1, $2); }
    | comando
        { $$ = $1; }
    ;

comando
    : cmd_atrib
        { $$ = $1; }
    | cmd_leia
        { $$ = $1; }
    | cmd_escreva
        { $$ = $1; }
    | cmd_se
        { $$ = $1; }
    | cmd_enquanto
        { $$ = $1; }
    ;

cmd_atrib
    : variavel ATRIB expressao
        { $$ = criar_cmd_atrib($1, $3); }
    ;

cmd_leia
    : LEIA lista_variaveis
        { $$ = criar_cmd_leia($2); }
    ;

lista_variaveis
    : lista_variaveis ',' variavel
        { $$ = concat_lista_var($1, $3); }
    | variavel
        { $$ = criar_lista_var($1); }
    ;

cmd_escreva
    : ESCREVA lista_escreva
        { $$ = criar_cmd_escreva($2); }
    ;

lista_escreva
    : lista_escreva ',' item_escreva
        { $$ = concat_lista_escreva($1, $3); }
    | item_escreva
        { $$ = $1; }
    ;

item_escreva
    : CADEIA_LIT
        { $$ = criar_item_cadeia($1); }
    | expressao
        { $$ = criar_item_expr($1); }
    ;

cmd_se
    : SE expr_relacional ENTAO lista_comandos FIMSE
        { $$ = criar_cmd_se($2, $4, NULL); }
    | SE expr_relacional ENTAO lista_comandos SENAO lista_comandos FIMSE
        { $$ = criar_cmd_se($2, $4, $6); }
    | SE expr_relacional ENTAO comando FIMSE
        { $$ = criar_cmd_se($2, $4, NULL); }
    | SE expr_relacional ENTAO comando SENAO comando FIMSE
        { $$ = criar_cmd_se($2, $4, $6); }
    ;

cmd_enquanto
    : ENQUANTO expr_relacional FACA lista_comandos FIMENQ
        { $$ = criar_cmd_enquanto($2, $4); }
    | ENQUANTO expr_relacional FACA comando FIMENQ
        { $$ = criar_cmd_enquanto($2, $4); }
    ;

variavel
    : ID
        { $$ = criar_var_simples($1); }
    | ID '[' expressao ']'
        { $$ = criar_var_array($1, $3); }
    ;

/* Expressões */

expressao
    : expr_aritmetica
        { $$ = $1; }
    | expr_relacional
        { $$ = $1; }
    ;

expr_relacional
    : expr_logica
        { $$ = $1; }
    ;

expr_logica
    : expr_logica OP_OU expr_logica
        { $$ = criar_expr_logica(LOG_OU, $1, $3); }
    | expr_logica OP_E expr_logica
        { $$ = criar_expr_logica(LOG_E, $1, $3); }
    | OP_NAO '(' expr_logica ')'
        { $$ = criar_expr_nao($3); }
    | '(' expr_logica ')'
        { $$ = $2; }
    | expr_aritmetica OP_MAQ expr_aritmetica
        { $$ = criar_expr_relacional(REL_MAQ, $1, $3); }
    | expr_aritmetica OP_MAI expr_aritmetica
        { $$ = criar_expr_relacional(REL_MAI, $1, $3); }
    | expr_aritmetica OP_MEQ expr_aritmetica
        { $$ = criar_expr_relacional(REL_MEQ, $1, $3); }
    | expr_aritmetica OP_MEI expr_aritmetica
        { $$ = criar_expr_relacional(REL_MEI, $1, $3); }
    | expr_aritmetica OP_IGU expr_aritmetica
        { $$ = criar_expr_relacional(REL_IGU, $1, $3); }
    | expr_aritmetica OP_DIF expr_aritmetica
        { $$ = criar_expr_relacional(REL_DIF, $1, $3); }
    ;

expr_aritmetica
    : expr_aritmetica '+' termo
        { $$ = criar_expr_aritmetica(ARIT_SOMA, $1, $3); }
    | expr_aritmetica '-' termo
        { $$ = criar_expr_aritmetica(ARIT_SUB, $1, $3); }
    | termo
        { $$ = $1; }
    ;

termo
    : termo '*' fator
        { $$ = criar_expr_aritmetica(ARIT_MULT, $1, $3); }
    | termo '/' fator
        { $$ = criar_expr_aritmetica(ARIT_DIV, $1, $3); }
    | fator
        { $$ = $1; }
    ;

fator
    : '(' expr_aritmetica ')'
        { $$ = $2; }
    | CONST_INT
        { $$ = criar_expr_const_int($1); }
    | CONST_REAL
        { $$ = criar_expr_const_real($1); }
    | variavel
        { $$ = criar_expr_var($1); }
    | '-' fator %prec UMINUS
        { 
            /* Unário negativo: 0 - fator */
            NoExpr *zero = criar_expr_const_int(0);
            $$ = criar_expr_aritmetica(ARIT_SUB, zero, $2);
        }
    ;

%%

/* ========== Tratamento de Erros ========== */

void yyerror(const char *s) {
    fprintf(stderr, "ERRO SINTATICO na linha %d, coluna %d: %s\n", linha, coluna, s);
    erros_sintaticos++;
}

