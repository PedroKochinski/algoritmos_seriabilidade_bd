# Nome do compilador C
CC = gcc

# Flags de compilação:
CFLAGS = -Wall

# Nome do executável final
PROG = escalona

# Lista de todos os arquivos .c
SOURCES = main.c algoritmos.c grafo.c

# Lista de arquivos objeto .o
OBJECTS = $(SOURCES:.c=.o)

# Arquivos a serem incluídos no pacote de distribuição
DISTFILES = $(SOURCES) algoritmos.h grafo.h Makefile

# Nome do diretório para o arquivo de distribuição
DISTDIR = ${USER}-$(PROG)

all: $(PROG)

$(PROG): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

%.o: %.c *.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f *~ *.bak

purge: clean
	@rm -f *.o core a.out @rm -f src/main.o
	@rm -f $(OBJS)
	@rm -f $(PROG)
	

dist: purge
	@echo "Gerando arquivo de distribuição ($(DISTDIR).tar.gz)..."
	@mkdir -p $(DISTDIR)
	@cp -p $(DISTFILES) $(DISTDIR)/
	@tar -czvf $(DISTDIR).tar.gz $(DISTDIR)
	@rm -rf $(DISTDIR)

.PHONY: all clean purge dist