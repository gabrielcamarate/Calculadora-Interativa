# Estrutura do Projeto

## Visão Geral

Este projeto segue uma estrutura organizada para facilitar manutenção e escalabilidade.

## Diretórios

### `src/`
Contém o código-fonte principal (.c)
- `calculadora.c` - Implementação principal da calculadora

### `include/`
Contém os headers públicos (.h)
- `calculadora.h` - Declarações de funções e constantes

### `build/`
Arquivos gerados durante a compilação (não versionados)
- Objetos (.o)
- Executáveis

### `scripts/`
Scripts auxiliares
- `requirements-ubuntu.sh` - Instalação de dependências

### `docs/`
Documentação adicional
- `ESTRUTURA.md` - Este arquivo

## Compilação

```bash
make          # Compilar
make clean    # Limpar build
make run      # Compilar e executar
make help     # Ver comandos disponíveis
```

## Benefícios

- **Organização**: Código separado por responsabilidade
- **Escalabilidade**: Fácil adicionar novos módulos
- **Manutenção**: Estrutura clara e intuitiva
- **Build**: Sistema de compilação robusto 