#include <ctype.h>
#include <locale.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

WINDOW *calculatorCase;

// Prototipos
void removeSpaces(char *originalString, int length);

void topRow(int width, int height, int startY, int startX);
void middleRow(int width, int height, char *character, int startY, int startX);
void loweRow(int width, int height, int startY, int startX);
void createDisplay(int width, int height, int startY, int startX);
void createBox(int width, int height, char *character, int startY, int startX);
void menuWindow();

int main(int argc, char const *argv[])
{
    setlocale(LC_ALL, "");

    // Iniciando a Tela (terminal)
    initscr();
    cbreak();
    curs_set(FALSE); // Não mostra o cursor

    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax); // Recebendo o tamanho do terminal aberta

    if (!has_colors()) // Verificando se o terminal do Usuario tem suporte para cores
    {
        printw("Seu terminal não suporta cores, sua experiência não será completa. [:-(");
        mvprintw(1, 0, "Pressione qualquer tecla para continuar...");
        getch();
        clear();
    }
    start_color(); // Iniciando as cores

    // Criando a interface da calculadora
    int start_Y = (yMax / 2) - HEIGHT_DEFAULT / 2;
    int start_X = (xMax / 2) - WIDITH_DEFAULT / 2;

    calculatorCase = newwin(HEIGHT_DEFAULT, WIDITH_DEFAULT + 1, start_Y, start_X);
    box(calculatorCase, 0, 0);
    refresh();
    wrefresh(calculatorCase);

    keypad(calculatorCase, true);

    menuWindow();
    // Aguardando proximo caracter
    char number = getch();
    endwin();

    return 0;
}

char characterOperator[1];

void menuWindow()
{
    char *buttons[5][4] =
    {
        {"0", ",", "=", ""},
        {"1", "2", "3", "+"},
        {"4", "5", "6", "-"},
        {"7", "8", "9", "x"},
        {"C", "<", "%", "/"}
    };

    int keyPressed, line = 0, column = 0;

    int startX_enter = WIDITH_DEFAULT - 4;

    int startY_enter = 3, countCharacters = 0, result = 0, countCharactersDisplay = 0, defaultLoop = 0;

    char charactersTEMP[1];

    char *characters = malloc(MAX_CHARACTERS * sizeof(char));
    char *charactersDisplay = malloc(MAX_CHARACTERS * sizeof(char));

    while (1)
    {
        int startY = (HEIGHT_DEFAULT - 4), startX = 2;
        int widthButton;

        for (int i = 0; i < 5; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                if (!STR_CMP(buttons, ""))
                {
                    if (column == 3 && line == 0)
                        column = 2;
                    widthButton = (STR_CMP(buttons, "0")) ? (WIDITH_DEFAULT - 3) / 2 : (WIDITH_DEFAULT - 3) / 4;

                    if (i == line && j == column)
                        wattron(calculatorCase, A_REVERSE);
                    createBox(widthButton, HEIGHT_BUTTON, buttons[i][j], startY, startX);
                    if (i == line && j == column)
                        wattroff(calculatorCase, A_REVERSE);
                    startX += widthButton + 1;
                }
            }
            startY -= 3;
            startX = 2;
        }
        startY--;
        int lengthDisplay = WIDITH_DEFAULT - 3;
        createDisplay(lengthDisplay, HEIGHT_BUTTON, startY, startX);
        wrefresh(calculatorCase);

        keyPressed = wgetch(calculatorCase);
        switch (keyPressed)
        {
            case KEY_RIGHT:
                column++;
                if (line == 0 && column == 3)
                    column = 2;
                if (column == 4)
                    column = 3;
                break;

            case KEY_LEFT:
                column--;
                if (column == -1)
                    column = 0;
                break;

            case KEY_UP:
                if (column == 2 && line == 0)
                    column = 3;
                if (column == 1 && line == 0)
                    column = 2;
                line++;

                if (line == 5)
                    line = 4;
                break;

            case KEY_DOWN:
                if (column == 1 && line == 1)
                    column = 0;
                if (column == 2 && line == 1)
                    column = 1;
                line--;
                if (line == -1)
                    line = 0;
                break;

            default:
                break;
        }

        charactersTEMP[0] = buttons[line][column][0];
        charactersTEMP[1] = '\0';

        int lengthCharacters = strlen(charactersDisplay);
        removeSpaces(charactersDisplay, lengthCharacters);

        /* 10 = ENTER */
        if (keyPressed == 10)
        {

            if (charactersTEMP[0] != '=' && charactersTEMP[0] != 'C')
                strcat(charactersDisplay, charactersTEMP); // Concatenando os caracteres

            if (defaultLoop > 0)
            {
                startX_enter = 30;           /**/
                startX_enter -= defaultLoop; /**/
                defaultLoop++;
            } // Configurando a posição de X

            if (charactersTEMP[0] == 'C')
            {
                startX_enter = 30;
                defaultLoop = 0;
            }

            mvwprintw(calculatorCase, 1, 1, "startX - %i", startX_enter);

            // Lógica para os digitos aparecerem na tela.
            if (isdigit(charactersTEMP[0]))
            {
                characters[countCharacters] = *charactersTEMP;
                mvwprintw(calculatorCase, startY_enter, startX_enter, "%s", charactersDisplay);
                countCharacters++;
                countCharactersDisplay++;
                startX_enter--;
            }

            // Lógica para os caracteres de operação aparecerem na tela.
            else if (!isdigit(charactersTEMP[0]) && charactersTEMP[0] != '=' && charactersTEMP[0] != 'C')
            {
                mvwprintw(calculatorCase, startY_enter, startX_enter, "%s", charactersDisplay);
                countCharacters++;
                countCharactersDisplay++;
                startX_enter--;
            }
        }

        //
        if (keyPressed == 10 && !isdigit(charactersTEMP[0]))
        {
            // Lógica quando o operador '+' é pressionado
            if (charactersTEMP[0] == '+')
            {
                int number = atoi(characters);
                result += number;

                characterOperator[0] = '+';
                for (int i = 0; i < strlen(characters); i++) {characters[i] = ' ';}
                countCharacters = 0;
            }

            // Lógica quando o operador '-' é pressionado
            if (charactersTEMP[0] == '-')
            {
                int number = atoi(characters);
                result = number;

                characterOperator[0] = '-';
                for (int i = 0; i < strlen(characters); i++){characters[i] = ' ';}
                countCharacters = 0;
            }

            // Lógica quando o operador 'x' é pressionado
            if (charactersTEMP[0] == 'x')
            {
                int number = atoi(characters);
                mvwprintw(calculatorCase, 5, 1, "%i", number);
                if (number != 0) {result = number;}

                characterOperator[0] = '*';
                for (int i = 0; i < strlen(characters); i++) {characters[i] = ' ';}
                countCharacters = 0;
            }

            // Lógica quando o operador '/' é pressionado
            if (charactersTEMP[0] == '/')
            {
                int number = atoi(characters);
                if (number != 0){result = number;}

                characterOperator[0] = '/';
                for (int i = 0; i < strlen(characters); i++) {characters[i] = ' ';}
                countCharacters = 0;
            }

            if (charactersTEMP[0] == 'C')
            {
                int count = 2;
                for (int i = 0; i < 30; i++)
                {
                    mvwprintw(calculatorCase, startY_enter, count, " ");
                    count++;
                }

                for (int i = 0; i < strlen(charactersDisplay); i++) {charactersDisplay[i] = ' ';}
                for (int i = 0; i < strlen(characters); i++) {characters[i] = ' ';}
                countCharacters = 0;
            }

            // Lógica quando o operador '=' é pressionado
            else if (charactersTEMP[0] == '=')
            {
                mvwprintw(calculatorCase, 5, 15, "op test - %s", characterOperator);

            
                int count = 2;
                // Operação de soma
                if (characterOperator[0] == '+')
                {
                    int number = atoi(characters);
                    result += number;

                    for (int i = 0; i < strlen(characters); i++){characters[i] = ' ';}
                    countCharacters = 0;
                }

                // Operação de Subtração
                else if (characterOperator[0] == '-')
                {
                    int number = atoi(characters);
                    result = result - number;
                    for (int i = ; i < strlen(characters); i++){characters[i] = ' ';}
                    countCharacters = 0;
                }

                // Operação de Multiplicação
                else if (characterOperator[0] == '*')
                {
                    int number = atoi(characters);
                    result = number * result;
                    for (int i = 0; i < strlen(characters); i++){characters[i] = ' ';}
                    countCharacters = 0;
                }

                // Operação de Divisão
                else if (characterOperator[0] == '/')
                {
                    int number = atoi(characters);
                    result = result / number;
                    for (int i = 0; i < strlen(characters); i++){characters[i] = ' ';}
                    countCharacters = 0;
                }

                int arr[] = {9, 99, 999, 9999, 99999, 999999, 9999999, 99999999, 999999999}, numberOptions = sizeof(arr) / sizeof(arr[0]);

                // Configuração da posição do print (não mexa)
                defaultLoop = 1;
                for (int i = 0; i < numberOptions; i++) if (result > arr[i]) defaultLoop++;

                // Limpando o display e zerando a variavel.
                for (int i = 0; i < 30; i++)
                {
                    mvwprintw(calculatorCase, startY_enter, count, " ");
                    count++;
                }

                for (int i = 0; i < strlen(charactersDisplay); i++) {charactersDisplay[i] = ' ';}

                // Determinando o tamanho do espaço do resultado (ex: strlen(resultado)), para poder pegar o valor correto
                int defaultPrint = 1;
                for (int i = 0; i < numberOptions; i++) if (result > arr[i]) defaultPrint++;

                // Adicionando a variavel display, o resultado.
                sprintf(charactersDisplay + (strlen(charactersDisplay) - defaultPrint), "%d", result); countCharacters = 0;

                // Determinando o tamanho do espaço do resultado (ex: strlen(resultado)), para poder pegar o valor correto
                int defaultX = WIDITH_DEFAULT - 4;
                for (int i = 0; i < numberOptions; i++) if (result > arr[i]) defaultX--;

                mvwprintw(calculatorCase, startY_enter, defaultX, "%i", result);
            }
        }
    }
}


/* Recomendavel não alterar nada nas funções. */
void removeSpaces(char *originalString, int length)
{
    char *source = originalString;
    char *destination = originalString;
    while (*source && length > 0)
    {
        if (*source != ' ')
        {
            *destination = *source;
            destination++;
        }
        source++;
        length--;
    }
    *destination = '\0';
}

void createDisplay(int width, int height, int startY, int startX)
{
    topRow(width, height, startY, startX);
    mvwprintw(calculatorCase, startY + 1, startX, "x");
    mvwprintw(calculatorCase, startY + 1, WIDITH_DEFAULT - 2, "x");
    loweRow(width, height, startY, startX);
}

void createBox(int width, int height, char *character, int startY, int startX)
{
    topRow(width, height, startY, startX);
    middleRow(width, height, character, startY, startX);
    loweRow(width, height, startY, startX);
}

void topRow(int width, int height, int startY, int startX)
{
    int count = 0;
    mvwprintw(calculatorCase, startY, startX, "%s", TOP_LEFT);
    for (int i = 0; i < (width - 1) - 1; i++)
    {
        count++, mvwprintw(calculatorCase, startY, startX + count, "%s", HORIZONTAL_LINE);
    }
    count++;
    mvwprintw(calculatorCase, startY, startX + count, "%s", TOP_RIGHT);
}

void middleRow(int width, int height, char *character, int startY, int startX)
{
    int count = 0;
    startY++;
    if (height == 1)
    {
        mvwprintw(calculatorCase, startY, startX, "%s", VERTICAL_LINE);
        for (int i = 0; i < ((width - 1) / 2) - 1; i++)
        {
            count++, mvwprintw(calculatorCase, startY, startX + count, " ");
        }
        count++;
        mvwprintw(calculatorCase, startY, startX + count, "%s", character);
        for (int i = 0; i < ((width - 1) / 2) - 1; i++)
        {
            count++, mvwprintw(calculatorCase, startY, startX + count, " ");
        }
        count++;
        mvwprintw(calculatorCase, startY, startX + count, "%s", VERTICAL_LINE);
    }
    else
    {
        for (int i = 0; i < height; i++)
        {
            mvwprintw(calculatorCase, startY, startX, "%s", VERTICAL_LINE);
            for (int j = 0; j < width; j++)
            {
                count++, mvwprintw(calculatorCase, startY, startX + count, " ");
            }
            count++;
            mvwprintw(calculatorCase, startY, startX + count, "%s", VERTICAL_LINE);
            startY++;
        }
    }
}

void loweRow(int width, int height, int startY, int startX)
{
    int count = 0;
    startY += 2;
    mvwprintw(calculatorCase, startY, startX, "%s", LOWER_LEFT);
    for (int i = 0; i < (width - 1) - 1; i++)
    {
        count++, mvwprintw(calculatorCase, startY, startX + count, "%s", HORIZONTAL_LINE);
    }
    count++;
    mvwprintw(calculatorCase, startY, startX + count, "%s", LOWER_RIGHT);
}