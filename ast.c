/*
 * Implementação da Árvore Sintática Abstrata (AST) para X25b
 * Avaliação Parcial 2 - Compiladores
 */

#define _GNU_SOURCE
#include "ast.h"

/* A variável programa_raiz é definida em parser.y */

/* ========== Funções auxiliares ========== */

static void imprimir_indent(int nivel) {
    for (int i = 0; i < nivel; i++) {
        printf("  ");
    }
}

static const char *tipo_para_string(TipoDado tipo) {
    switch (tipo) {
        case TIPO_INTEIRO: return "INTEIRO";
        case TIPO_REAL: return "REAL";
        case TIPO_LISTAINT: return "LISTAINT";
        case TIPO_LISTAREAL: return "LISTAREAL";
        case TIPO_CADEIA: return "CADEIA";
        default: return "INDEFINIDO";
    }
}

static const char *op_arit_para_string(OpAritmetico op) {
    switch (op) {
        case ARIT_SOMA: return "+";
        case ARIT_SUB: return "-";
        case ARIT_MULT: return "*";
        case ARIT_DIV: return "/";
        default: return "?";
    }
}

static const char *op_rel_para_string(OpRelacional op) {
    switch (op) {
        case REL_MAQ: return ".MAQ.";
        case REL_MAI: return ".MAI.";
        case REL_MEQ: return ".MEQ.";
        case REL_MEI: return ".MEI.";
        case REL_IGU: return ".IGU.";
        case REL_DIF: return ".DIF.";
        default: return "?";
    }
}

static const char *op_log_para_string(OpLogico op) {
    switch (op) {
        case LOG_OU: return ".OU.";
        case LOG_E: return ".E.";
        case LOG_NAO: return ".NAO.";
        default: return "?";
    }
}

/* ========== Criação de nós - Programa ========== */

NoPrograma *criar_programa(char *nome, NoDecl *decl, NoCmd *algo) {
    NoPrograma *prog = (NoPrograma *)malloc(sizeof(NoPrograma));
    prog->nome = nome;
    prog->declaracoes = decl;
    prog->algoritmo = algo;
    return prog;
}

/* ========== Criação de nós - Declarações ========== */

NoDecl *criar_declaracao(TipoDado tipo, char *nome, int tamanho) {
    NoDecl *decl = (NoDecl *)malloc(sizeof(NoDecl));
    decl->tipo = tipo;
    decl->nome = nome;
    decl->tamanho_array = tamanho;
    decl->linha = linha;
    decl->coluna = coluna;
    decl->prox = NULL;
    return decl;
}

NoDecl *concat_declaracoes(NoDecl *lista, NoDecl *nova) {
    if (lista == NULL) return nova;
    NoDecl *atual = lista;
    while (atual->prox != NULL) {
        atual = atual->prox;
    }
    atual->prox = nova;
    return lista;
}

/* ========== Criação de nós - Variáveis ========== */

NoVar *criar_var_simples(char *nome) {
    NoVar *var = (NoVar *)malloc(sizeof(NoVar));
    var->nome = nome;
    var->indice = NULL;
    var->linha = linha;
    var->coluna = coluna;
    return var;
}

NoVar *criar_var_array(char *nome, NoExpr *indice) {
    NoVar *var = (NoVar *)malloc(sizeof(NoVar));
    var->nome = nome;
    var->indice = indice;
    var->linha = linha;
    var->coluna = coluna;
    return var;
}

ListaVar *criar_lista_var(NoVar *var) {
    ListaVar *lista = (ListaVar *)malloc(sizeof(ListaVar));
    lista->var = var;
    lista->prox = NULL;
    return lista;
}

ListaVar *concat_lista_var(ListaVar *lista, NoVar *var) {
    ListaVar *novo = criar_lista_var(var);
    if (lista == NULL) return novo;
    ListaVar *atual = lista;
    while (atual->prox != NULL) {
        atual = atual->prox;
    }
    atual->prox = novo;
    return lista;
}

/* ========== Criação de nós - Expressões ========== */

NoExpr *criar_expr_const_int(int valor) {
    NoExpr *expr = (NoExpr *)malloc(sizeof(NoExpr));
    expr->tipo = EXPR_CONST_INT;
    expr->tipo_dado = TIPO_INTEIRO;
    expr->linha = linha;
    expr->coluna = coluna;
    expr->dado.const_int = valor;
    return expr;
}

NoExpr *criar_expr_const_real(double valor) {
    NoExpr *expr = (NoExpr *)malloc(sizeof(NoExpr));
    expr->tipo = EXPR_CONST_REAL;
    expr->tipo_dado = TIPO_REAL;
    expr->linha = linha;
    expr->coluna = coluna;
    expr->dado.const_real = valor;
    return expr;
}

NoExpr *criar_expr_var(NoVar *var) {
    NoExpr *expr = (NoExpr *)malloc(sizeof(NoExpr));
    if (var->indice != NULL) {
        expr->tipo = EXPR_VAR_ARRAY;
    } else {
        expr->tipo = EXPR_VAR;
    }
    expr->tipo_dado = TIPO_INDEFINIDO;  /* Será definido na análise semântica */
    expr->linha = linha;
    expr->coluna = coluna;
    expr->dado.var = var;
    return expr;
}

NoExpr *criar_expr_aritmetica(OpAritmetico op, NoExpr *esq, NoExpr *dir) {
    NoExpr *expr = (NoExpr *)malloc(sizeof(NoExpr));
    expr->tipo = EXPR_ARITMETICA;
    expr->tipo_dado = TIPO_INDEFINIDO;  /* Será definido na análise semântica */
    expr->linha = linha;
    expr->coluna = coluna;
    expr->dado.aritmetica.op = op;
    expr->dado.aritmetica.esq = esq;
    expr->dado.aritmetica.dir = dir;
    return expr;
}

NoExpr *criar_expr_relacional(OpRelacional op, NoExpr *esq, NoExpr *dir) {
    NoExpr *expr = (NoExpr *)malloc(sizeof(NoExpr));
    expr->tipo = EXPR_RELACIONAL;
    expr->tipo_dado = TIPO_INTEIRO;  /* Resultado booleano (0 ou 1) */
    expr->linha = linha;
    expr->coluna = coluna;
    expr->dado.relacional.op = op;
    expr->dado.relacional.esq = esq;
    expr->dado.relacional.dir = dir;
    return expr;
}

NoExpr *criar_expr_logica(OpLogico op, NoExpr *esq, NoExpr *dir) {
    NoExpr *expr = (NoExpr *)malloc(sizeof(NoExpr));
    expr->tipo = EXPR_LOGICA;
    expr->tipo_dado = TIPO_INTEIRO;  /* Resultado booleano (0 ou 1) */
    expr->linha = linha;
    expr->coluna = coluna;
    expr->dado.logica.op = op;
    expr->dado.logica.esq = esq;
    expr->dado.logica.dir = dir;
    return expr;
}

NoExpr *criar_expr_nao(NoExpr *expr_interna) {
    NoExpr *expr = (NoExpr *)malloc(sizeof(NoExpr));
    expr->tipo = EXPR_NAO;
    expr->tipo_dado = TIPO_INTEIRO;  /* Resultado booleano (0 ou 1) */
    expr->linha = linha;
    expr->coluna = coluna;
    expr->dado.negacao = expr_interna;
    return expr;
}

/* ========== Criação de nós - Comandos ========== */

NoCmd *criar_cmd_atrib(NoVar *var, NoExpr *expr) {
    NoCmd *cmd = (NoCmd *)malloc(sizeof(NoCmd));
    cmd->tipo = CMD_ATRIB;
    cmd->linha = linha;
    cmd->coluna = coluna;
    cmd->dado.atrib.var = var;
    cmd->dado.atrib.expr = expr;
    cmd->prox = NULL;
    return cmd;
}

NoCmd *criar_cmd_leia(ListaVar *vars) {
    NoCmd *cmd = (NoCmd *)malloc(sizeof(NoCmd));
    cmd->tipo = CMD_LEIA;
    cmd->linha = linha;
    cmd->coluna = coluna;
    cmd->dado.leia = vars;
    cmd->prox = NULL;
    return cmd;
}

NoCmd *criar_cmd_escreva(ListaEscreva *itens) {
    NoCmd *cmd = (NoCmd *)malloc(sizeof(NoCmd));
    cmd->tipo = CMD_ESCREVA;
    cmd->linha = linha;
    cmd->coluna = coluna;
    cmd->dado.escreva = itens;
    cmd->prox = NULL;
    return cmd;
}

NoCmd *criar_cmd_se(NoExpr *cond, NoCmd *entao, NoCmd *senao) {
    NoCmd *cmd = (NoCmd *)malloc(sizeof(NoCmd));
    cmd->tipo = CMD_SE;
    cmd->linha = linha;
    cmd->coluna = coluna;
    cmd->dado.se.condicao = cond;
    cmd->dado.se.entao = entao;
    cmd->dado.se.senao = senao;
    cmd->prox = NULL;
    return cmd;
}

NoCmd *criar_cmd_enquanto(NoExpr *cond, NoCmd *corpo) {
    NoCmd *cmd = (NoCmd *)malloc(sizeof(NoCmd));
    cmd->tipo = CMD_ENQUANTO;
    cmd->linha = linha;
    cmd->coluna = coluna;
    cmd->dado.enquanto.condicao = cond;
    cmd->dado.enquanto.corpo = corpo;
    cmd->prox = NULL;
    return cmd;
}

NoCmd *concat_comandos(NoCmd *lista, NoCmd *novo) {
    if (lista == NULL) return novo;
    NoCmd *atual = lista;
    while (atual->prox != NULL) {
        atual = atual->prox;
    }
    atual->prox = novo;
    return lista;
}

/* ========== Lista para ESCREVA ========== */

ListaEscreva *criar_item_cadeia(char *cadeia) {
    ListaEscreva *item = (ListaEscreva *)malloc(sizeof(ListaEscreva));
    item->is_cadeia = 1;
    item->item.cadeia = cadeia;
    item->prox = NULL;
    return item;
}

ListaEscreva *criar_item_expr(NoExpr *expr) {
    ListaEscreva *item = (ListaEscreva *)malloc(sizeof(ListaEscreva));
    item->is_cadeia = 0;
    item->item.expr = expr;
    item->prox = NULL;
    return item;
}

ListaEscreva *concat_lista_escreva(ListaEscreva *lista, ListaEscreva *item) {
    if (lista == NULL) return item;
    ListaEscreva *atual = lista;
    while (atual->prox != NULL) {
        atual = atual->prox;
    }
    atual->prox = item;
    return lista;
}

/* ========== Impressão da AST ========== */

void imprimir_expressao(NoExpr *expr) {
    if (expr == NULL) {
        printf("NULL");
        return;
    }
    
    switch (expr->tipo) {
        case EXPR_CONST_INT:
            printf("%d", expr->dado.const_int);
            break;
            
        case EXPR_CONST_REAL:
            printf("%.2f", expr->dado.const_real);
            break;
            
        case EXPR_VAR:
            printf("%s", expr->dado.var->nome);
            break;
            
        case EXPR_VAR_ARRAY:
            printf("%s[", expr->dado.var->nome);
            imprimir_expressao(expr->dado.var->indice);
            printf("]");
            break;
            
        case EXPR_ARITMETICA:
            printf("(");
            imprimir_expressao(expr->dado.aritmetica.esq);
            printf(" %s ", op_arit_para_string(expr->dado.aritmetica.op));
            imprimir_expressao(expr->dado.aritmetica.dir);
            printf(")");
            break;
            
        case EXPR_RELACIONAL:
            printf("(");
            imprimir_expressao(expr->dado.relacional.esq);
            printf(" %s ", op_rel_para_string(expr->dado.relacional.op));
            imprimir_expressao(expr->dado.relacional.dir);
            printf(")");
            break;
            
        case EXPR_LOGICA:
            printf("(");
            imprimir_expressao(expr->dado.logica.esq);
            printf(" %s ", op_log_para_string(expr->dado.logica.op));
            imprimir_expressao(expr->dado.logica.dir);
            printf(")");
            break;
            
        case EXPR_NAO:
            printf(".NAO. (");
            imprimir_expressao(expr->dado.negacao);
            printf(")");
            break;
    }
}

void imprimir_declaracoes(NoDecl *decl, int nivel) {
    while (decl != NULL) {
        imprimir_indent(nivel);
        printf("%s %s", tipo_para_string(decl->tipo), decl->nome);
        if (decl->tamanho_array > 0) {
            printf("[%d]", decl->tamanho_array);
        }
        printf("\n");
        decl = decl->prox;
    }
}

void imprimir_comandos(NoCmd *cmd, int nivel) {
    while (cmd != NULL) {
        imprimir_indent(nivel);
        
        switch (cmd->tipo) {
            case CMD_ATRIB:
                if (cmd->dado.atrib.var->indice != NULL) {
                    printf("%s[", cmd->dado.atrib.var->nome);
                    imprimir_expressao(cmd->dado.atrib.var->indice);
                    printf("]");
                } else {
                    printf("%s", cmd->dado.atrib.var->nome);
                }
                printf(" := ");
                imprimir_expressao(cmd->dado.atrib.expr);
                printf("\n");
                break;
                
            case CMD_LEIA:
                printf("LEIA ");
                {
                    ListaVar *v = cmd->dado.leia;
                    while (v != NULL) {
                        if (v->var->indice != NULL) {
                            printf("%s[", v->var->nome);
                            imprimir_expressao(v->var->indice);
                            printf("]");
                        } else {
                            printf("%s", v->var->nome);
                        }
                        if (v->prox != NULL) printf(", ");
                        v = v->prox;
                    }
                }
                printf("\n");
                break;
                
            case CMD_ESCREVA:
                printf("ESCREVA ");
                {
                    ListaEscreva *e = cmd->dado.escreva;
                    while (e != NULL) {
                        if (e->is_cadeia) {
                            printf("'%s'", e->item.cadeia);
                        } else {
                            imprimir_expressao(e->item.expr);
                        }
                        if (e->prox != NULL) printf(", ");
                        e = e->prox;
                    }
                }
                printf("\n");
                break;
                
            case CMD_SE:
                printf("SE ");
                imprimir_expressao(cmd->dado.se.condicao);
                printf("\n");
                imprimir_indent(nivel);
                printf("ENTAO\n");
                imprimir_comandos(cmd->dado.se.entao, nivel + 1);
                if (cmd->dado.se.senao != NULL) {
                    imprimir_indent(nivel);
                    printf("SENAO\n");
                    imprimir_comandos(cmd->dado.se.senao, nivel + 1);
                }
                imprimir_indent(nivel);
                printf("FIMSE\n");
                break;
                
            case CMD_ENQUANTO:
                printf("ENQUANTO ");
                imprimir_expressao(cmd->dado.enquanto.condicao);
                printf(" FACA\n");
                imprimir_comandos(cmd->dado.enquanto.corpo, nivel + 1);
                imprimir_indent(nivel);
                printf("FIMENQ\n");
                break;
                
            case CMD_BLOCO:
                imprimir_comandos(cmd->dado.bloco.cmd, nivel);
                break;
        }
        
        cmd = cmd->prox;
    }
}

void imprimir_ast(NoPrograma *prog) {
    if (prog == NULL) {
        printf("Programa vazio!\n");
        return;
    }
    
    printf("=== ARVORE SINTATICA ABSTRATA ===\n\n");
    printf("PROGRAMA %s\n\n", prog->nome ? prog->nome : "(sem nome)");
    
    printf("DECLARACOES:\n");
    imprimir_declaracoes(prog->declaracoes, 1);
    printf("\n");
    
    printf("ALGORITMO:\n");
    imprimir_comandos(prog->algoritmo, 1);
    
    printf("\n=================================\n");
}

/* ========== Liberação de memória ========== */

void liberar_var(NoVar *var) {
    if (var == NULL) return;
    free(var->nome);
    liberar_expressao(var->indice);
    free(var);
}

void liberar_lista_var(ListaVar *lista) {
    while (lista != NULL) {
        ListaVar *prox = lista->prox;
        liberar_var(lista->var);
        free(lista);
        lista = prox;
    }
}

void liberar_expressao(NoExpr *expr) {
    if (expr == NULL) return;
    
    switch (expr->tipo) {
        case EXPR_CONST_INT:
        case EXPR_CONST_REAL:
            break;
            
        case EXPR_VAR:
        case EXPR_VAR_ARRAY:
            liberar_var(expr->dado.var);
            break;
            
        case EXPR_ARITMETICA:
            liberar_expressao(expr->dado.aritmetica.esq);
            liberar_expressao(expr->dado.aritmetica.dir);
            break;
            
        case EXPR_RELACIONAL:
            liberar_expressao(expr->dado.relacional.esq);
            liberar_expressao(expr->dado.relacional.dir);
            break;
            
        case EXPR_LOGICA:
            liberar_expressao(expr->dado.logica.esq);
            liberar_expressao(expr->dado.logica.dir);
            break;
            
        case EXPR_NAO:
            liberar_expressao(expr->dado.negacao);
            break;
    }
    
    free(expr);
}

void liberar_lista_escreva(ListaEscreva *lista) {
    while (lista != NULL) {
        ListaEscreva *prox = lista->prox;
        if (lista->is_cadeia) {
            free(lista->item.cadeia);
        } else {
            liberar_expressao(lista->item.expr);
        }
        free(lista);
        lista = prox;
    }
}

void liberar_comandos(NoCmd *cmd) {
    while (cmd != NULL) {
        NoCmd *prox = cmd->prox;
        
        switch (cmd->tipo) {
            case CMD_ATRIB:
                liberar_var(cmd->dado.atrib.var);
                liberar_expressao(cmd->dado.atrib.expr);
                break;
                
            case CMD_LEIA:
                liberar_lista_var(cmd->dado.leia);
                break;
                
            case CMD_ESCREVA:
                liberar_lista_escreva(cmd->dado.escreva);
                break;
                
            case CMD_SE:
                liberar_expressao(cmd->dado.se.condicao);
                liberar_comandos(cmd->dado.se.entao);
                liberar_comandos(cmd->dado.se.senao);
                break;
                
            case CMD_ENQUANTO:
                liberar_expressao(cmd->dado.enquanto.condicao);
                liberar_comandos(cmd->dado.enquanto.corpo);
                break;
                
            case CMD_BLOCO:
                liberar_comandos(cmd->dado.bloco.cmd);
                break;
        }
        
        free(cmd);
        cmd = prox;
    }
}

void liberar_declaracoes(NoDecl *decl) {
    while (decl != NULL) {
        NoDecl *prox = decl->prox;
        free(decl->nome);
        free(decl);
        decl = prox;
    }
}

void liberar_programa(NoPrograma *prog) {
    if (prog == NULL) return;
    free(prog->nome);
    liberar_declaracoes(prog->declaracoes);
    liberar_comandos(prog->algoritmo);
    free(prog);
}

