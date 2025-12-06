# Compilador X25b - Avaliação Parcial 2

## Descrição

Compilador front-end para a linguagem X25b, uma linguagem de algoritmos hipotética utilizada para demonstração de conceitos de compiladores.

## Estrutura do Projeto

```
ava2/
├── lexer.l          # Analisador Léxico (FLEX)
├── parser.y         # Analisador Sintático LALR(1) (Bison)
├── ast.h            # Definição da Árvore Sintática Abstrata
├── ast.c            # Implementação da AST
├── semantic.h       # Cabeçalho do Analisador Semântico
├── semantic.c       # Implementação do Analisador Semântico
├── main.c           # Programa Principal
├── Makefile         # Script de compilação
├── teste.x25b       # Programa de teste (item f)
├── fatorial.x25b    # Exemplo de fatorial
└── README.md        # Este arquivo
```

## Requisitos

- GCC (GNU Compiler Collection)
- Flex (Fast Lexical Analyzer Generator)
- Bison (GNU Parser Generator)

### Instalação dos requisitos (Ubuntu/Debian):

```bash
sudo apt-get install build-essential flex bison
```

## Compilação

```bash
make
```

## Uso

```bash
./x25b [opcoes] <arquivo.x25b>
```

### Opções:
- `-a, --ast` - Mostra a árvore sintática abstrata
- `-t, --tabela` - Mostra a tabela de símbolos
- `-v, --verbose` - Modo verbose
- `-h, --help` - Mostra ajuda

### Exemplos:

```bash
# Compilar o programa de teste
./x25b teste.x25b

# Compilar mostrando a AST
./x25b -a fatorial.x25b

# Usar o make para testes
make test
```

## Características da Linguagem X25b

### Estrutura do Programa
```
PROGRAMA {nome_do_programa}
{DECLARACOES}
<declarações de variáveis>
{ALGORITMO}
<comandos>
FIMPROG
```

### Tipos de Dados
- `INTEIRO` - Números inteiros
- `REAL` - Números reais (usar vírgula como separador decimal)
- `LISTAINT` - Array de inteiros (tamanho 10-40)
- `LISTAREAL` - Array de reais (tamanho 10-40)

### Operadores Relacionais
- `.MAQ.` - Maior que (>)
- `.MAI.` - Maior ou igual (>=)
- `.MEQ.` - Menor que (<)
- `.MEI.` - Menor ou igual (<=)
- `.IGU.` - Igual (=)
- `.DIF.` - Diferente (≠)

### Operadores Lógicos
- `.OU.` - Ou lógico
- `.E.` - E lógico
- `.NAO.` - Negação

### Comandos
- Atribuição: `variavel := expressao`
- Entrada: `LEIA variavel` ou `LEIA var1, var2`
- Saída: `ESCREVA expressao` ou `ESCREVA 'texto'`
- Seleção: `SE condição ENTAO comando [SENAO comando] FIMSE`
- Repetição: `ENQUANTO condição FACA comando FIMENQ`

## Fases do Compilador

### 1. Análise Léxica (FLEX)
- Reconhece tokens da linguagem
- Identifica palavras reservadas
- Processa literais e identificadores
- Trata comentários

### 2. Análise Sintática (Bison - LALR(1))
- Gramática livre de contexto
- Constrói árvore sintática abstrata
- Reporta erros sintáticos

### 3. Análise Semântica
- Tabela de símbolos
- Verificação de declaração de variáveis
- Verificação de tipos
- Compatibilidade de operações

## Saída do Compilador

O compilador reporta:
- Sucesso: "COMPILACAO CONCLUIDA COM SUCESSO!"
- Erros léxicos com linha e coluna
- Erros sintáticos com localização
- Erros semânticos (variáveis não declaradas, tipos incompatíveis, etc.)

## Autor

Avaliação Parcial 2 - Disciplina de Compiladores

