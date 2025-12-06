/*
 * Implementação do Analisador Semântico para X25b
 * Avaliação Parcial 2 - Compiladores
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "semantic.h"

/* Declaração explícita para evitar warnings */
extern char *strdup(const char *s);

/* Tabela de símbolos global */
static TabelaSimbolos tabela;

/* Contador de erros semânticos */
int erros_semanticos = 0;

/* ========== Funções Hash ========== */

static unsigned int hash(const char *str) {
    unsigned int h = 0;
    while (*str) {
        h = h * 31 + (unsigned char)*str++;
    }
    return h % TAB_SIMBOLOS_TAM;
}

/* ========== Implementação da Tabela de Símbolos ========== */

void inicializar_tabela(void) {
    for (int i = 0; i < TAB_SIMBOLOS_TAM; i++) {
        tabela.entradas[i] = NULL;
    }
    tabela.num_simbolos = 0;
}

int inserir_simbolo(const char *nome, TipoDado tipo, int tamanho, int linha) {
    /* Verifica se já existe */
    if (buscar_simbolo(nome) != NULL) {
        erro_semantico(linha, "Variavel '%s' ja foi declarada", nome);
        return 0;
    }
    
    /* Cria nova entrada */
    EntradaSimbolo *nova = (EntradaSimbolo *)malloc(sizeof(EntradaSimbolo));
    nova->nome = strdup(nome);
    nova->tipo = tipo;
    nova->tamanho_array = tamanho;
    nova->linha_declaracao = linha;
    nova->inicializada = 0;
    
    /* Insere na tabela */
    unsigned int h = hash(nome);
    nova->prox = tabela.entradas[h];
    tabela.entradas[h] = nova;
    tabela.num_simbolos++;
    
    return 1;
}

EntradaSimbolo *buscar_simbolo(const char *nome) {
    unsigned int h = hash(nome);
    EntradaSimbolo *atual = tabela.entradas[h];
    
    while (atual != NULL) {
        if (strcmp(atual->nome, nome) == 0) {
            return atual;
        }
        atual = atual->prox;
    }
    
    return NULL;
}

void marcar_inicializado(const char *nome) {
    EntradaSimbolo *s = buscar_simbolo(nome);
    if (s != NULL) {
        s->inicializada = 1;
    }
}

void imprimir_tabela_simbolos(void) {
    printf("\n=== TABELA DE SIMBOLOS ===\n");
    printf("%-15s %-12s %-10s %-8s\n", "Nome", "Tipo", "Tamanho", "Linha");
    printf("----------------------------------------------\n");
    
    for (int i = 0; i < TAB_SIMBOLOS_TAM; i++) {
        EntradaSimbolo *atual = tabela.entradas[i];
        while (atual != NULL) {
            const char *tipo_str;
            switch (atual->tipo) {
                case TIPO_INTEIRO: tipo_str = "INTEIRO"; break;
                case TIPO_REAL: tipo_str = "REAL"; break;
                case TIPO_LISTAINT: tipo_str = "LISTAINT"; break;
                case TIPO_LISTAREAL: tipo_str = "LISTAREAL"; break;
                default: tipo_str = "???"; break;
            }
            
            printf("%-15s %-12s %-10d %-8d\n", 
                   atual->nome, 
                   tipo_str,
                   atual->tamanho_array,
                   atual->linha_declaracao);
            atual = atual->prox;
        }
    }
    printf("==========================\n\n");
}

void liberar_tabela_simbolos(void) {
    for (int i = 0; i < TAB_SIMBOLOS_TAM; i++) {
        EntradaSimbolo *atual = tabela.entradas[i];
        while (atual != NULL) {
            EntradaSimbolo *prox = atual->prox;
            free(atual->nome);
            free(atual);
            atual = prox;
        }
        tabela.entradas[i] = NULL;
    }
    tabela.num_simbolos = 0;
}

/* ========== Mensagens de Erro ========== */

void erro_semantico(int linha, const char *formato, ...) {
    va_list args;
    fprintf(stderr, "ERRO SEMANTICO na linha %d: ", linha);
    va_start(args, formato);
    vfprintf(stderr, formato, args);
    va_end(args);
    fprintf(stderr, "\n");
    erros_semanticos++;
}

void aviso_semantico(int linha, const char *formato, ...) {
    va_list args;
    fprintf(stderr, "AVISO na linha %d: ", linha);
    va_start(args, formato);
    vfprintf(stderr, formato, args);
    va_end(args);
    fprintf(stderr, "\n");
}

/* ========== Verificação de Tipos ========== */

int tipos_compativeis(TipoDado t1, TipoDado t2) {
    if (t1 == t2) return 1;
    
    /* INTEIRO e REAL são compatíveis (com conversão implícita) */
    if ((t1 == TIPO_INTEIRO || t1 == TIPO_REAL) &&
        (t2 == TIPO_INTEIRO || t2 == TIPO_REAL)) {
        return 1;
    }
    
    return 0;
}

TipoDado tipo_resultante(TipoDado t1, TipoDado t2) {
    /* Se um dos tipos é REAL, o resultado é REAL */
    if (t1 == TIPO_REAL || t2 == TIPO_REAL) {
        return TIPO_REAL;
    }
    return TIPO_INTEIRO;
}

/* ========== Análise de Declarações ========== */

int analisar_declaracoes(NoDecl *decl) {
    int ok = 1;
    
    while (decl != NULL) {
        /* Verifica o nome da variável */
        if (strlen(decl->nome) > 8) {
            erro_semantico(decl->linha, "Nome de variavel '%s' excede 8 caracteres", decl->nome);
            ok = 0;
        }
        
        /* Verifica tamanho do array */
        if (decl->tamanho_array > 0) {
            if (decl->tamanho_array < 10 || decl->tamanho_array > 40) {
                erro_semantico(decl->linha, "Tamanho do array '%s' deve ser entre 10 e 40", decl->nome);
                ok = 0;
            }
            
            /* Verifica se o tipo é compatível com array */
            if (decl->tipo != TIPO_LISTAINT && decl->tipo != TIPO_LISTAREAL) {
                erro_semantico(decl->linha, "Tipo '%s' nao pode ser usado para arrays", decl->nome);
                ok = 0;
            }
        }
        
        /* Insere na tabela de símbolos */
        if (!inserir_simbolo(decl->nome, decl->tipo, decl->tamanho_array, decl->linha)) {
            ok = 0;
        }
        
        decl = decl->prox;
    }
    
    return ok;
}

/* ========== Análise de Variáveis ========== */

int verificar_variavel(NoVar *var) {
    EntradaSimbolo *s = buscar_simbolo(var->nome);
    
    if (s == NULL) {
        erro_semantico(var->linha, "Variavel '%s' nao foi declarada", var->nome);
        return 0;
    }
    
    /* Verifica uso de índice */
    if (var->indice != NULL) {
        /* Usando como array */
        if (s->tamanho_array == 0) {
            erro_semantico(var->linha, "Variavel '%s' nao e um array", var->nome);
            return 0;
        }
        
        /* Verifica tipo do índice */
        TipoDado tipo_indice = analisar_expressao(var->indice);
        if (tipo_indice != TIPO_INTEIRO) {
            erro_semantico(var->linha, "Indice do array '%s' deve ser inteiro", var->nome);
            return 0;
        }
    } else {
        /* Usando como variável simples */
        if (s->tamanho_array > 0) {
            erro_semantico(var->linha, "Array '%s' requer indice", var->nome);
            return 0;
        }
    }
    
    return 1;
}

/* ========== Análise de Expressões ========== */

TipoDado analisar_expressao(NoExpr *expr) {
    if (expr == NULL) return TIPO_INDEFINIDO;
    
    switch (expr->tipo) {
        case EXPR_CONST_INT:
            expr->tipo_dado = TIPO_INTEIRO;
            return TIPO_INTEIRO;
            
        case EXPR_CONST_REAL:
            expr->tipo_dado = TIPO_REAL;
            return TIPO_REAL;
            
        case EXPR_VAR:
        case EXPR_VAR_ARRAY:
            {
                if (!verificar_variavel(expr->dado.var)) {
                    expr->tipo_dado = TIPO_INDEFINIDO;
                    return TIPO_INDEFINIDO;
                }
                
                EntradaSimbolo *s = buscar_simbolo(expr->dado.var->nome);
                if (s != NULL) {
                    /* Para arrays, o tipo do elemento */
                    if (s->tipo == TIPO_LISTAINT) {
                        expr->tipo_dado = TIPO_INTEIRO;
                        return TIPO_INTEIRO;
                    } else if (s->tipo == TIPO_LISTAREAL) {
                        expr->tipo_dado = TIPO_REAL;
                        return TIPO_REAL;
                    }
                    expr->tipo_dado = s->tipo;
                    return s->tipo;
                }
                return TIPO_INDEFINIDO;
            }
            
        case EXPR_ARITMETICA:
            {
                TipoDado t1 = analisar_expressao(expr->dado.aritmetica.esq);
                TipoDado t2 = analisar_expressao(expr->dado.aritmetica.dir);
                
                if (!tipos_compativeis(t1, t2)) {
                    erro_semantico(expr->linha, "Tipos incompativeis em operacao aritmetica");
                    expr->tipo_dado = TIPO_INDEFINIDO;
                    return TIPO_INDEFINIDO;
                }
                
                /* Verifica divisão por zero (constantes) */
                if (expr->dado.aritmetica.op == ARIT_DIV) {
                    NoExpr *dir = expr->dado.aritmetica.dir;
                    if (dir->tipo == EXPR_CONST_INT && dir->dado.const_int == 0) {
                        aviso_semantico(expr->linha, "Possivel divisao por zero");
                    }
                    if (dir->tipo == EXPR_CONST_REAL && dir->dado.const_real == 0.0) {
                        aviso_semantico(expr->linha, "Possivel divisao por zero");
                    }
                }
                
                expr->tipo_dado = tipo_resultante(t1, t2);
                return expr->tipo_dado;
            }
            
        case EXPR_RELACIONAL:
            {
                TipoDado t1 = analisar_expressao(expr->dado.relacional.esq);
                TipoDado t2 = analisar_expressao(expr->dado.relacional.dir);
                
                if (!tipos_compativeis(t1, t2)) {
                    erro_semantico(expr->linha, "Tipos incompativeis em comparacao");
                }
                
                expr->tipo_dado = TIPO_INTEIRO;  /* Booleano representado como inteiro */
                return TIPO_INTEIRO;
            }
            
        case EXPR_LOGICA:
            {
                /* Analisa operandos lógicos */
                analisar_expressao(expr->dado.logica.esq);
                analisar_expressao(expr->dado.logica.dir);
                
                /* Operandos lógicos devem ser "booleanos" (resultado de expressões relacionais) */
                /* Por simplificação, aceitamos inteiros */
                
                expr->tipo_dado = TIPO_INTEIRO;
                return TIPO_INTEIRO;
            }
            
        case EXPR_NAO:
            {
                analisar_expressao(expr->dado.negacao);
                expr->tipo_dado = TIPO_INTEIRO;
                return TIPO_INTEIRO;
            }
    }
    
    return TIPO_INDEFINIDO;
}

/* ========== Análise de Comandos ========== */

int analisar_comandos(NoCmd *cmd) {
    int ok = 1;
    
    while (cmd != NULL) {
        switch (cmd->tipo) {
            case CMD_ATRIB:
                {
                    /* Verifica a variável destino */
                    if (!verificar_variavel(cmd->dado.atrib.var)) {
                        ok = 0;
                    } else {
                        /* Verifica tipos */
                        EntradaSimbolo *s = buscar_simbolo(cmd->dado.atrib.var->nome);
                        TipoDado tipo_expr = analisar_expressao(cmd->dado.atrib.expr);
                        
                        if (s != NULL) {
                            TipoDado tipo_var = s->tipo;
                            /* Para arrays, considera o tipo do elemento */
                            if (tipo_var == TIPO_LISTAINT) tipo_var = TIPO_INTEIRO;
                            if (tipo_var == TIPO_LISTAREAL) tipo_var = TIPO_REAL;
                            
                            if (!tipos_compativeis(tipo_var, tipo_expr)) {
                                erro_semantico(cmd->linha, 
                                    "Tipo incompativel na atribuicao a '%s'", 
                                    cmd->dado.atrib.var->nome);
                                ok = 0;
                            }
                            
                            /* Marca como inicializada */
                            marcar_inicializado(cmd->dado.atrib.var->nome);
                        }
                    }
                }
                break;
                
            case CMD_LEIA:
                {
                    ListaVar *v = cmd->dado.leia;
                    while (v != NULL) {
                        if (!verificar_variavel(v->var)) {
                            ok = 0;
                        } else {
                            marcar_inicializado(v->var->nome);
                        }
                        v = v->prox;
                    }
                }
                break;
                
            case CMD_ESCREVA:
                {
                    ListaEscreva *e = cmd->dado.escreva;
                    while (e != NULL) {
                        if (!e->is_cadeia) {
                            analisar_expressao(e->item.expr);
                        }
                        e = e->prox;
                    }
                }
                break;
                
            case CMD_SE:
                {
                    /* Analisa condição */
                    analisar_expressao(cmd->dado.se.condicao);
                    
                    /* Analisa blocos */
                    if (!analisar_comandos(cmd->dado.se.entao)) {
                        ok = 0;
                    }
                    if (cmd->dado.se.senao != NULL) {
                        if (!analisar_comandos(cmd->dado.se.senao)) {
                            ok = 0;
                        }
                    }
                }
                break;
                
            case CMD_ENQUANTO:
                {
                    /* Analisa condição */
                    analisar_expressao(cmd->dado.enquanto.condicao);
                    
                    /* Analisa corpo */
                    if (!analisar_comandos(cmd->dado.enquanto.corpo)) {
                        ok = 0;
                    }
                }
                break;
                
            case CMD_BLOCO:
                if (!analisar_comandos(cmd->dado.bloco.cmd)) {
                    ok = 0;
                }
                break;
        }
        
        cmd = cmd->prox;
    }
    
    return ok;
}

/* ========== Análise Principal ========== */

int analisar_semantica(NoPrograma *prog) {
    if (prog == NULL) {
        fprintf(stderr, "Programa vazio!\n");
        return 0;
    }
    
    printf("\n>>> Iniciando analise semantica...\n");
    
    /* Inicializa tabela de símbolos */
    inicializar_tabela();
    
    /* Analisa declarações */
    if (!analisar_declaracoes(prog->declaracoes)) {
        /* Continua mesmo com erros nas declarações */
    }
    
    /* Analisa algoritmo */
    if (!analisar_comandos(prog->algoritmo)) {
        /* Continua mesmo com erros nos comandos */
    }
    
    /* Imprime tabela de símbolos */
    imprimir_tabela_simbolos();
    
    /* Retorna sucesso se não houve erros */
    if (erros_semanticos == 0) {
        printf(">>> Analise semantica concluida com sucesso!\n");
        return 1;
    } else {
        printf(">>> Analise semantica encontrou %d erro(s).\n", erros_semanticos);
        return 0;
    }
}

