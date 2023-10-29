#pragma once

// Caracteres que formam uma caixa (linha 10 até a linha 17)
// Quinas
#define TOP_RIGHT "\u2510"
#define TOP_LEFT "\u250C"
#define LOWER_LEFT "\u2514"
#define LOWER_RIGHT "\u2518"

// Prenchemento
#define VERTICAL_LINE "\u2502"
#define HORIZONTAL_LINE "\u2500"

// Quantidade de colunas da calculadora (y)
#define CALCULATOR_COLUMN 5
// Quantidade de linhas da calculadora  (x)
#define CALCULATOR_LINE 4


// Tamanho do botão (minimo)
#define HEIGHT_BUTTON 1

// Tamanho da calculadora (minimo)
#define HEIGHT_DEFAULT 22
#define WIDITH_DEFAULT 34

#define STR_CMP(x, y) (strcmp(x[i][j], y) == 0) // Macro de comparação
#define MAX_CHARACTERS 100
#define LENGTH_DISPLAY_X (WIDITH_DEFAULT - 4)

#define LENGTH_DISPLAY_Y (HEIGHT_DEFAULT - 4)

#define START_Y_DISPLAY 3

//
#define BUTTON_ROWS 5
#define BUTTON_COLUMNS 4

// Assets
void removeSpaces(char *originalString);
void showErrors(const char *message);
char *allocateMemory();
void initialize();

// Criação das caixas
void topRow(int width, int height, int startY, int startX);
void middleRow(int width, int height, char *character, int startY, int startX);
void lowerRow(int width, int height, int startY, int startX);
void createDisplay(int width, int height, int startY, int startX);
void createBox(int width, int height, char *character, int startY, int startX);

// Criação da interface
void menuWindow();
void drawCalculatorCase(int yMax, int xMax);
void drawCalculatorButtons(int *startY, int *startX, int *line, int *column);

// Operações matematicas
void sum();
void subtraction();
void division();
void multiplication();

// Processamento
void processKeypress(int keyPressed, int *line, int *column);
void processOperationEquals(char operator);

void drawDisplayResults(int *startX_enter);
void configPositionX(int *defaultLoop, int *defaultX, int *defaultPrint, int *result);
void defineLoop(int *startX_enter, int *defaultLoop);


// Funções de clear
void drawClearDisplay();
void clearVariable(char *display);
void handleC(int *startX_enter, int *defaultLoop, int *countCharacters, int *result);