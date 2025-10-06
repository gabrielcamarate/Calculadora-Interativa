#!/bin/bash

echo "Atualizando lista de pacotes..."
sudo apt update

echo "Instalando compilador GCC, Make, pkg-config e bibliotecas ncurses..."
sudo apt install -y build-essential pkg-config libncurses5-dev libncursesw5-dev

echo "Pronto! Todas as dependências essenciais foram instaladas." 