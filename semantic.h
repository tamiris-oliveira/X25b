/*
 * Analisador Semântico para a linguagem X25b
 * Avaliação Parcial 2 - Compiladores
 */

#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"

/* ========== Tabela de Símbolos ========== */

#define TAB_SIMBOLOS_TAM 256

/* Entrada na tabela de símbolos */
typedef struct EntradaSimbolo {
    char *nome;
    TipoDado tipo;
    int tamanho_array;      /* 0 para variáveis simples */
    int linha_declaracao;
    int inicializada;       /* Flag para verificar se foi inicializada */
    struct EntradaSimbolo *prox;  /* Para tratamento de colisões */
} EntradaSimbolo;

/* Tabela de símbolos */
typedef struct TabelaSimbolos {
    EntradaSimbolo *entradas[TAB_SIMBOLOS_TAM];
    int num_simbolos;
} TabelaSimbolos;

/* ========== Funções da Tabela de Símbolos ========== */

/* Inicializa a tabela de símbolos */
void inicializar_tabela(void);

/* Insere um símbolo na tabela */
int inserir_simbolo(const char *nome, TipoDado tipo, int tamanho, int linha);

/* Busca um símbolo na tabela */
EntradaSimbolo *buscar_simbolo(const char *nome);

/* Marca um símbolo como inicializado */
void marcar_inicializado(const char *nome);

/* Imprime a tabela de símbolos */
void imprimir_tabela_simbolos(void);

/* Libera a tabela de símbolos */
void liberar_tabela_simbolos(void);

/* ========== Análise Semântica ========== */

/* Contador de erros semânticos */
extern int erros_semanticos;

/* Analisa semanticamente o programa completo */
int analisar_semantica(NoPrograma *prog);

/* Analisa as declarações */
int analisar_declaracoes(NoDecl *decl);

/* Analisa os comandos */
int analisar_comandos(NoCmd *cmd);

/* Analisa uma expressão e retorna seu tipo */
TipoDado analisar_expressao(NoExpr *expr);

/* Verifica se uma variável foi declarada */
int verificar_variavel(NoVar *var);

/* Verifica compatibilidade de tipos */
int tipos_compativeis(TipoDado t1, TipoDado t2);

/* Retorna o tipo resultante de uma operação */
TipoDado tipo_resultante(TipoDado t1, TipoDado t2);

/* ========== Mensagens de Erro ========== */

void erro_semantico(int linha, const char *formato, ...);
void aviso_semantico(int linha, const char *formato, ...);

#endif /* SEMANTIC_H */

