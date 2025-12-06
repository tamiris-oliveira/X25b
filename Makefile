#
# Makefile para o Compilador X25b
# Avaliação Parcial 2 - Compiladores
#

# Compilador e flags
CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99
LDFLAGS = -lfl

# Ferramentas
FLEX = flex
BISON = bison

# Arquivos fonte
LEXER = lexer.l
PARSER = parser.y
AST_SRC = ast.c
SEMANTIC_SRC = semantic.c
MAIN_SRC = main.c

# Arquivos gerados
LEX_C = lex.yy.c
PARSER_C = parser.tab.c
PARSER_H = parser.tab.h

# Arquivos objeto
OBJS = $(LEX_C:.c=.o) $(PARSER_C:.c=.o) ast.o semantic.o main.o

# Executável
TARGET = x25b

# Regra principal
all: $(TARGET)

# Gera o executável
$(TARGET): $(OBJS)
	@echo ">>> Linkando $(TARGET)..."
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo ">>> Compilador X25b criado com sucesso!"
	@echo ""

# Compila arquivos objeto
lex.yy.o: $(LEX_C) $(PARSER_H) ast.h
	@echo ">>> Compilando analisador lexico..."
	$(CC) $(CFLAGS) -c -o $@ $(LEX_C)

parser.tab.o: $(PARSER_C) ast.h semantic.h
	@echo ">>> Compilando analisador sintatico..."
	$(CC) $(CFLAGS) -c -o $@ $(PARSER_C)

ast.o: $(AST_SRC) ast.h
	@echo ">>> Compilando modulo AST..."
	$(CC) $(CFLAGS) -c -o $@ $(AST_SRC)

semantic.o: $(SEMANTIC_SRC) semantic.h ast.h
	@echo ">>> Compilando analisador semantico..."
	$(CC) $(CFLAGS) -c -o $@ $(SEMANTIC_SRC)

main.o: $(MAIN_SRC) ast.h semantic.h
	@echo ">>> Compilando programa principal..."
	$(CC) $(CFLAGS) -c -o $@ $(MAIN_SRC)

# Gera o analisador léxico
$(LEX_C): $(LEXER)
	@echo ">>> Gerando analisador lexico com FLEX..."
	$(FLEX) $(LEXER)

# Gera o analisador sintático
$(PARSER_C) $(PARSER_H): $(PARSER)
	@echo ">>> Gerando analisador sintatico com BISON..."
	$(BISON) -d -v $(PARSER)

# Limpeza
clean:
	@echo ">>> Limpando arquivos gerados..."
	rm -f $(TARGET) $(OBJS)
	rm -f $(LEX_C) $(PARSER_C) $(PARSER_H)
	rm -f parser.output
	@echo ">>> Limpeza concluida."

# Limpeza completa
distclean: clean
	rm -f *.x25b.out

# Teste com arquivo de exemplo
test: $(TARGET)
	@echo ""
	@echo ">>> Executando teste com programa de exemplo..."
	@echo ""
	./$(TARGET) -a teste.x25b

# Teste do fatorial
test-fatorial: $(TARGET)
	@echo ""
	@echo ">>> Testando programa fatorial..."
	./$(TARGET) fatorial.x25b

# Ajuda
help:
	@echo ""
	@echo "Makefile do Compilador X25b"
	@echo ""
	@echo "Comandos disponiveis:"
	@echo "  make          - Compila o projeto"
	@echo "  make clean    - Remove arquivos objeto e executavel"
	@echo "  make test     - Executa teste com arquivo de exemplo"
	@echo "  make help     - Mostra esta mensagem"
	@echo ""

.PHONY: all clean distclean test test-fatorial help

