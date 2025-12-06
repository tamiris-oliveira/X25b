/*
 * Definição da Árvore Sintática Abstrata (AST) para X25b
 * Avaliação Parcial 2 - Compiladores
 */

#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ========== Tipos de Nós da AST ========== */

/* Tipos de dados da linguagem */
typedef enum {
    TIPO_INTEIRO,
    TIPO_REAL,
    TIPO_LISTAINT,
    TIPO_LISTAREAL,
    TIPO_CADEIA,
    TIPO_INDEFINIDO
} TipoDado;

/* Tipos de operadores relacionais */
typedef enum {
    REL_MAQ,    /* > */
    REL_MAI,    /* >= */
    REL_MEQ,    /* < */
    REL_MEI,    /* <= */
    REL_IGU,    /* = */
    REL_DIF     /* != */
} OpRelacional;

/* Tipos de operadores lógicos */
typedef enum {
    LOG_OU,
    LOG_E,
    LOG_NAO
} OpLogico;

/* Tipos de operadores aritméticos */
typedef enum {
    ARIT_SOMA,
    ARIT_SUB,
    ARIT_MULT,
    ARIT_DIV
} OpAritmetico;

/* Tipos de nós de expressão */
typedef enum {
    EXPR_CONST_INT,
    EXPR_CONST_REAL,
    EXPR_VAR,
    EXPR_VAR_ARRAY,
    EXPR_ARITMETICA,
    EXPR_RELACIONAL,
    EXPR_LOGICA,
    EXPR_NAO
} TipoExpr;

/* Tipos de nós de comando */
typedef enum {
    CMD_ATRIB,
    CMD_LEIA,
    CMD_ESCREVA,
    CMD_SE,
    CMD_ENQUANTO,
    CMD_BLOCO
} TipoCmd;

/* ========== Estruturas da AST ========== */

/* Forward declarations */
struct NoExpr;
struct NoCmd;
struct NoDecl;
struct NoVar;
struct NoPrograma;

/* Nó de variável (para referência) */
typedef struct NoVar {
    char *nome;
    struct NoExpr *indice;  /* NULL para variáveis simples, expressão para arrays */
    int linha;
    int coluna;
} NoVar;

/* Lista de variáveis (para LEIA) */
typedef struct ListaVar {
    NoVar *var;
    struct ListaVar *prox;
} ListaVar;

/* Nó de expressão */
typedef struct NoExpr {
    TipoExpr tipo;
    TipoDado tipo_dado;  /* Tipo resultante da expressão */
    int linha;
    int coluna;
    
    union {
        /* Constante inteira */
        int const_int;
        
        /* Constante real */
        double const_real;
        
        /* Variável */
        NoVar *var;
        
        /* Operação aritmética */
        struct {
            OpAritmetico op;
            struct NoExpr *esq;
            struct NoExpr *dir;
        } aritmetica;
        
        /* Operação relacional */
        struct {
            OpRelacional op;
            struct NoExpr *esq;
            struct NoExpr *dir;
        } relacional;
        
        /* Operação lógica */
        struct {
            OpLogico op;
            struct NoExpr *esq;
            struct NoExpr *dir;
        } logica;
        
        /* Negação lógica */
        struct NoExpr *negacao;
    } dado;
} NoExpr;

/* Lista de itens para ESCREVA */
typedef struct ListaEscreva {
    int is_cadeia;
    union {
        char *cadeia;
        NoExpr *expr;
    } item;
    struct ListaEscreva *prox;
} ListaEscreva;

/* Nó de comando */
typedef struct NoCmd {
    TipoCmd tipo;
    int linha;
    int coluna;
    
    union {
        /* Atribuição */
        struct {
            NoVar *var;
            NoExpr *expr;
        } atrib;
        
        /* LEIA */
        ListaVar *leia;
        
        /* ESCREVA */
        ListaEscreva *escreva;
        
        /* SE */
        struct {
            NoExpr *condicao;
            struct NoCmd *entao;
            struct NoCmd *senao;  /* NULL se não houver SENAO */
        } se;
        
        /* ENQUANTO */
        struct {
            NoExpr *condicao;
            struct NoCmd *corpo;
        } enquanto;
        
        /* Bloco de comandos */
        struct {
            struct NoCmd *cmd;
            struct NoCmd *prox;
        } bloco;
    } dado;
    
    struct NoCmd *prox;  /* Próximo comando na sequência */
} NoCmd;

/* Nó de declaração */
typedef struct NoDecl {
    TipoDado tipo;
    char *nome;
    int tamanho_array;  /* 0 para variáveis simples */
    int linha;
    int coluna;
    struct NoDecl *prox;
} NoDecl;

/* Nó raiz do programa */
typedef struct NoPrograma {
    char *nome;
    NoDecl *declaracoes;
    NoCmd *algoritmo;
} NoPrograma;

/* ========== Funções de criação de nós ========== */

/* Programa */
NoPrograma *criar_programa(char *nome, NoDecl *decl, NoCmd *algo);

/* Declarações */
NoDecl *criar_declaracao(TipoDado tipo, char *nome, int tamanho);
NoDecl *concat_declaracoes(NoDecl *lista, NoDecl *nova);

/* Variáveis */
NoVar *criar_var_simples(char *nome);
NoVar *criar_var_array(char *nome, NoExpr *indice);
ListaVar *criar_lista_var(NoVar *var);
ListaVar *concat_lista_var(ListaVar *lista, NoVar *var);

/* Expressões */
NoExpr *criar_expr_const_int(int valor);
NoExpr *criar_expr_const_real(double valor);
NoExpr *criar_expr_var(NoVar *var);
NoExpr *criar_expr_aritmetica(OpAritmetico op, NoExpr *esq, NoExpr *dir);
NoExpr *criar_expr_relacional(OpRelacional op, NoExpr *esq, NoExpr *dir);
NoExpr *criar_expr_logica(OpLogico op, NoExpr *esq, NoExpr *dir);
NoExpr *criar_expr_nao(NoExpr *expr);

/* Comandos */
NoCmd *criar_cmd_atrib(NoVar *var, NoExpr *expr);
NoCmd *criar_cmd_leia(ListaVar *vars);
NoCmd *criar_cmd_escreva(ListaEscreva *itens);
NoCmd *criar_cmd_se(NoExpr *cond, NoCmd *entao, NoCmd *senao);
NoCmd *criar_cmd_enquanto(NoExpr *cond, NoCmd *corpo);
NoCmd *concat_comandos(NoCmd *lista, NoCmd *novo);

/* Lista de itens para ESCREVA */
ListaEscreva *criar_item_cadeia(char *cadeia);
ListaEscreva *criar_item_expr(NoExpr *expr);
ListaEscreva *concat_lista_escreva(ListaEscreva *lista, ListaEscreva *item);

/* ========== Funções de impressão da AST ========== */
void imprimir_ast(NoPrograma *prog);
void imprimir_declaracoes(NoDecl *decl, int nivel);
void imprimir_comandos(NoCmd *cmd, int nivel);
void imprimir_expressao(NoExpr *expr);

/* ========== Funções de liberação de memória ========== */
void liberar_programa(NoPrograma *prog);
void liberar_declaracoes(NoDecl *decl);
void liberar_comandos(NoCmd *cmd);
void liberar_expressao(NoExpr *expr);
void liberar_var(NoVar *var);
void liberar_lista_var(ListaVar *lista);
void liberar_lista_escreva(ListaEscreva *lista);

/* ========== Variáveis globais ========== */
extern int linha;
extern int coluna;

#endif /* AST_H */

