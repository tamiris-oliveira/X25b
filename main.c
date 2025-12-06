/*
 * Programa Principal - Compilador X25b
 * Avaliação Parcial 2 - Compiladores
 * 
 * Front-end do compilador integrando:
 * - Analisador Léxico (FLEX)
 * - Analisador Sintático LALR(1) (Bison)
 * - Analisador Semântico
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "semantic.h"

/* Declarações externas */
extern FILE *yyin;
extern int yyparse(void);
extern NoPrograma *programa_raiz;
extern int erros_sintaticos;
extern int erros_semanticos;
extern int linha;
extern int coluna;

/* Flags de execução */
int mostrar_ast = 0;
int mostrar_tabela = 1;
int modo_verbose = 0;

void imprimir_cabecalho(void) {
    printf("\n");
    printf("╔══════════════════════════════════════════════════════════════╗\n");
    printf("║           COMPILADOR X25b - Avaliação Parcial 2              ║\n");
    printf("║                    Compiladores 2025                         ║\n");
    printf("╠══════════════════════════════════════════════════════════════╣\n");
    printf("║  Fases implementadas:                                        ║\n");
    printf("║    [✓] Análise Léxica (FLEX)                                 ║\n");
    printf("║    [✓] Análise Sintática LALR(1) (Bison)                     ║\n");
    printf("║    [✓] Análise Semântica                                     ║\n");
    printf("╚══════════════════════════════════════════════════════════════╝\n");
    printf("\n");
}

void imprimir_uso(const char *programa) {
    printf("Uso: %s [opcoes] <arquivo.x25b>\n\n", programa);
    printf("Opcoes:\n");
    printf("  -a, --ast      Mostra a arvore sintatica abstrata\n");
    printf("  -t, --tabela   Mostra a tabela de simbolos (padrao: ativado)\n");
    printf("  -v, --verbose  Modo verbose\n");
    printf("  -h, --help     Mostra esta mensagem de ajuda\n");
    printf("\n");
}

void imprimir_resultado(int sucesso) {
    printf("\n");
    printf("══════════════════════════════════════════════════════════════════\n");
    if (sucesso) {
        printf("  ✓ COMPILACAO CONCLUIDA COM SUCESSO!\n");
        printf("  O programa em X25b foi reconhecido sem erros.\n");
    } else {
        printf("  ✗ COMPILACAO FALHOU!\n");
        if (erros_sintaticos > 0) {
            printf("  Erros sintaticos encontrados: %d\n", erros_sintaticos);
        }
        if (erros_semanticos > 0) {
            printf("  Erros semanticos encontrados: %d\n", erros_semanticos);
        }
    }
    printf("══════════════════════════════════════════════════════════════════\n");
    printf("\n");
}

int main(int argc, char *argv[]) {
    char *arquivo_entrada = NULL;
    int i;
    
    /* Processa argumentos */
    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-a") == 0 || strcmp(argv[i], "--ast") == 0) {
            mostrar_ast = 1;
        } else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--tabela") == 0) {
            mostrar_tabela = 1;
        } else if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            modo_verbose = 1;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            imprimir_cabecalho();
            imprimir_uso(argv[0]);
            return 0;
        } else if (argv[i][0] != '-') {
            arquivo_entrada = argv[i];
        } else {
            fprintf(stderr, "Opcao desconhecida: %s\n", argv[i]);
            imprimir_uso(argv[0]);
            return 1;
        }
    }
    
    imprimir_cabecalho();
    
    /* Verifica se foi fornecido arquivo de entrada */
    if (arquivo_entrada == NULL) {
        fprintf(stderr, "Erro: Nenhum arquivo de entrada especificado.\n\n");
        imprimir_uso(argv[0]);
        return 1;
    }
    
    /* Abre arquivo de entrada */
    yyin = fopen(arquivo_entrada, "r");
    if (yyin == NULL) {
        fprintf(stderr, "Erro: Nao foi possivel abrir o arquivo '%s'\n", arquivo_entrada);
        return 1;
    }
    
    printf(">>> Processando arquivo: %s\n\n", arquivo_entrada);
    
    /* Fase 1: Análise Léxica e Sintática */
    printf(">>> Fase 1: Analise Lexica e Sintatica\n");
    
    linha = 1;
    coluna = 1;
    erros_sintaticos = 0;
    erros_semanticos = 0;
    
    int resultado_parse = yyparse();
    
    fclose(yyin);
    
    if (resultado_parse != 0 || erros_sintaticos > 0) {
        printf(">>> Analise sintatica encontrou erros.\n");
        imprimir_resultado(0);
        return 1;
    }
    
    printf(">>> Analise lexica e sintatica concluidas com sucesso!\n");
    
    /* Mostra AST se solicitado */
    if (mostrar_ast && programa_raiz != NULL) {
        printf("\n");
        imprimir_ast(programa_raiz);
    }
    
    /* Fase 2: Análise Semântica */
    printf("\n>>> Fase 2: Analise Semantica\n");
    
    analisar_semantica(programa_raiz);
    
    /* Resultado final */
    int sucesso = (erros_sintaticos == 0 && erros_semanticos == 0);
    imprimir_resultado(sucesso);
    
    /* Libera memória */
    if (programa_raiz != NULL) {
        liberar_programa(programa_raiz);
    }
    liberar_tabela_simbolos();
    
    return sucesso ? 0 : 1;
}

