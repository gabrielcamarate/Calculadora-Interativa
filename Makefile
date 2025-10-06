# Makefile para Calculadora Interativa
# Compilador e flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99
LDFLAGS = `pkg-config --libs ncursesw`

# Diretórios
SRCDIR = src
INCDIR = include
BUILDDIR = build
SCRIPTSDIR = scripts

# Arquivos
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)
TARGET = calculadora

# Regra principal
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "Compilação concluída: $(TARGET)"

# Compilar objetos
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

# Limpar arquivos de build
clean:
	rm -rf $(BUILDDIR)
	rm -f $(TARGET)
	@echo "Arquivos de build removidos"

# Instalar dependências (Ubuntu)
install-deps:
	@echo "Instalando dependências..."
	chmod +x $(SCRIPTSDIR)/requirements-ubuntu.sh
	./$(SCRIPTSDIR)/requirements-ubuntu.sh

# Executar a calculadora
run: $(TARGET)
	./$(TARGET)

# Mostrar ajuda
help:
	@echo "Comandos disponíveis:"
	@echo "  make          - Compilar o projeto"
	@echo "  make clean    - Limpar arquivos de build"
	@echo "  make run      - Compilar e executar"
	@echo "  make install-deps - Instalar dependências"
	@echo "  make help     - Mostrar esta ajuda"

.PHONY: clean install-deps run help