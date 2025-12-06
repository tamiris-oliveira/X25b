%{
/*
 * Analisador Léxico para a linguagem X25b
 * Avaliação Parcial 2 - Compiladores
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
#include "parser.tab.h"

/* Declaração explícita para evitar warnings */
extern char *strdup(const char *s);
extern char *strndup(const char *s, size_t n);

int linha = 1;
int coluna = 1;

void atualiza_posicao(void);
void erro_lexico(const char *msg);

%}

%option noyywrap
%option nounput
%option noinput

DIGITO      [0-9]
LETRA       [a-zA-Z]
ESPACO      [ \t]+
NOVA_LINHA  \n

/* Identificador simples: até 8 caracteres, começa com letra */
ID_SIMPLES  {LETRA}({LETRA}|{DIGITO}){0,7}

/* Constante inteira */
INTEIRO_CONST   {DIGITO}+

/* Constante real (com vírgula como separador decimal) */
REAL_CONST      {DIGITO}+","{DIGITO}+

/* Cadeia de caracteres (aspas simples) */
CADEIA      \'[^\'\n]*\'

/* Cadeia de caracteres (aspas duplas - também aceito conforme exemplos) */
CADEIA_DUPLA \"[^\"\n]*\"

%%

"PROGRAMA"      { atualiza_posicao(); return PROGRAMA; }
"FIMPROG"       { atualiza_posicao(); return FIMPROG; }
"DECLARACOES"   { atualiza_posicao(); return DECLARACOES; }
"ALGORITMO"     { atualiza_posicao(); return ALGORITMO; }
"INTEIRO"       { atualiza_posicao(); return INTEIRO; }
"REAL"          { atualiza_posicao(); return REAL; }
"LISTAINT"      { atualiza_posicao(); return LISTAINT; }
"LISTAREAL"     { atualiza_posicao(); return LISTAREAL; }
"LEIA"          { atualiza_posicao(); return LEIA; }
"ESCREVA"       { atualiza_posicao(); return ESCREVA; }
"SE"            { atualiza_posicao(); return SE; }
"ENTAO"         { atualiza_posicao(); return ENTAO; }
"SENAO"         { atualiza_posicao(); return SENAO; }
"FIMSE"         { atualiza_posicao(); return FIMSE; }
"ENQUANTO"      { atualiza_posicao(); return ENQUANTO; }
"FACA"          { atualiza_posicao(); return FACA; }
"FIMENQ"        { atualiza_posicao(); return FIMENQ; }

".MAQ."         { atualiza_posicao(); return OP_MAQ; }
".MAI."         { atualiza_posicao(); return OP_MAI; }
".MEQ."         { atualiza_posicao(); return OP_MEQ; }
".MEI."         { atualiza_posicao(); return OP_MEI; }
".IGU."         { atualiza_posicao(); return OP_IGU; }
".DIF."         { atualiza_posicao(); return OP_DIF; }
".OU."          { atualiza_posicao(); return OP_OU; }
".E."           { atualiza_posicao(); return OP_E; }
".NAO."         { atualiza_posicao(); return OP_NAO; }

":="            { atualiza_posicao(); return ATRIB; }
"+"             { atualiza_posicao(); return '+'; }
"-"             { atualiza_posicao(); return '-'; }
"*"             { atualiza_posicao(); return '*'; }
"/"             { atualiza_posicao(); return '/'; }
"("             { atualiza_posicao(); return '('; }
")"             { atualiza_posicao(); return ')'; }
"["             { atualiza_posicao(); return '['; }
"]"             { atualiza_posicao(); return ']'; }
","             { atualiza_posicao(); return ','; }

"{"[^}]*"}"     { 
                  /* Comentário - ignora e conta linhas */
                  char *p = yytext;
                  while (*p) {
                      if (*p == '\n') {
                          linha++;
                          coluna = 1;
                      } else {
                          coluna++;
                      }
                      p++;
                  }
                }

{INTEIRO_CONST} { 
                  atualiza_posicao();
                  yylval.ival = atoi(yytext);
                  return CONST_INT;
                }

{REAL_CONST}    {
                  atualiza_posicao();
                  /* Substitui vírgula por ponto para conversão */
                  char *temp = strdup(yytext);
                  char *p = strchr(temp, ',');
                  if (p) *p = '.';
                  yylval.fval = atof(temp);
                  free(temp);
                  return CONST_REAL;
                }

{CADEIA}        {
                  atualiza_posicao();
                  /* Remove as aspas */
                  yylval.sval = strndup(yytext + 1, strlen(yytext) - 2);
                  return CADEIA_LIT;
                }

{CADEIA_DUPLA}  {
                  atualiza_posicao();
                  /* Remove as aspas */
                  yylval.sval = strndup(yytext + 1, strlen(yytext) - 2);
                  return CADEIA_LIT;
                }

{ID_SIMPLES}    {
                  atualiza_posicao();
                  if (strlen(yytext) > 8) {
                      erro_lexico("Identificador excede 8 caracteres");
                  }
                  yylval.sval = strdup(yytext);
                  return ID;
                }

{ESPACO}        { coluna += yyleng; }

{NOVA_LINHA}    { linha++; coluna = 1; }

.               { 
                  char msg[100];
                  sprintf(msg, "Caractere invalido: '%c'", yytext[0]);
                  erro_lexico(msg);
                  coluna++;
                }

%%

void atualiza_posicao(void) {
    coluna += yyleng;
}

void erro_lexico(const char *msg) {
    fprintf(stderr, "ERRO LEXICO na linha %d, coluna %d: %s\n", linha, coluna, msg);
}

