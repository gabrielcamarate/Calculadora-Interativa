#include <ctype.h>
#include <locale.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "calculadora.h"


// Variaveis para manipulçação das operações
char characterOperator[1];
char *characters = NULL;
char *charactersDisplay = NULL;
int countCharacters = 0;
int countCharactersDisplay = 0;
int result = 0;

// Definição dos botões da calculadora
char *buttons[BUTTON_ROWS][BUTTON_COLUMNS] =
    {
        {"0", ",", "=", ""},
        {"1", "2", "3", "+"},
        {"4", "5", "6", "-"},
        {"7", "8", "9", "x"},
        {"C", "<", "%", "/"}
    };

// Variaveis principais
WINDOW *calculatorCase;


int main(int argc, char const *argv[])
{
    setlocale(LC_ALL, "");
    initialize();

    characters = allocateMemory();
    charactersDisplay = allocateMemory();

    menuWindow();
    endwin();

    return 0;
}

// ASSETS
void initialize()
{
    initscr();
    cbreak();
    curs_set(FALSE); // Não mostra o cursor
    if (!has_colors())
    {
        printw("Seu terminal não suporta cores, sua experiência não será completa. [:-(");
        mvprintw(1, 0, "Pressione qualquer tecla para continuar...");
        getch();
        clear();
    }
    start_color();

    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);
    drawCalculatorCase(yMax, xMax);
}

void showErrors(const char *message)
{
    system("clear");
    fprintf(stderr, "%s\n", message);
    exit(1);
}

char *allocateMemory()
{
    char *temp = (char *) malloc(MAX_CHARACTERS * sizeof(char));
    if (temp == NULL) showErrors("Memory allocation failure.");
    return temp;
}

void removeSpaces(char *originalString)
{
    int length = strlen(originalString);

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

// Operações matematicas
void sum()
{
    int number = atoi(characters);
    result += number;

    for (int i = 0; i < strlen(characters); i++){characters[i] = ' ';}
    countCharacters = 0;
}

void subtraction()
{
    int number = atoi(characters);
    result = result - number;
    for (int i = 0; i < strlen(characters); i++){characters[i] = ' ';}
    countCharacters = 0;
}

void multiplication()
{
    int number = atoi(characters);
    result = number * result;
    for (int i = 0; i < strlen(characters); i++){characters[i] = ' ';}
    countCharacters = 0;
}

void division()
{
    int number = atoi(characters);
    result = result / number;
    for (int i = 0; i < strlen(characters); i++){characters[i] = ' ';}
    countCharacters = 0;
}

// Criação da interface
void drawCalculatorCase(int yMax, int xMax)
{
    int start_Y = (yMax / 2) - HEIGHT_DEFAULT / 2;
    int start_X = (xMax / 2) - WIDITH_DEFAULT / 2;

    calculatorCase = newwin(HEIGHT_DEFAULT, WIDITH_DEFAULT + 1, start_Y, start_X);
    box(calculatorCase, 0, 0);
    refresh();
    wrefresh(calculatorCase);

    keypad(calculatorCase, true);
}

// Criação dos botões
void drawCalculatorButtons(int *startY, int *startX, int *line, int *column)
{
    int widthButton;
    for (int i = 0; i < BUTTON_ROWS; i++)
    {
        for (int j = 0; j < BUTTON_COLUMNS; j++)
        {
            if (!STR_CMP(buttons, ""))
            {
                if ((*column) == 3 && (*line) == 0) (*column) = 2;
                widthButton = (STR_CMP(buttons, "0")) ? (WIDITH_DEFAULT - 3) / 2 : (WIDITH_DEFAULT - 3) / 4;

                if (i == (*line) && j == (*column)) wattron(calculatorCase, A_REVERSE);

                createBox(widthButton, HEIGHT_BUTTON, buttons[i][j], (*startY), (*startX));

                if (i == (*line) && j == (*column)) wattroff(calculatorCase, A_REVERSE);
                (*startX) += widthButton + 1;
            }
        }
        (*startY) -= 3;
        (*startX) = 2;
    }
    (*startY)--;
}

void menuWindow()
{
    int keyPressed, line = 0, column = 0;
    int startX_enter = LENGTH_DISPLAY_X;
    int defaultLoop = 0;

    char charactersTEMP[1];

    while (1)
    {
        int startY = LENGTH_DISPLAY_Y, startX = 2;

        drawCalculatorButtons(&startY, &startX, &line, &column);

        int lengthDisplay = LENGTH_DISPLAY_X + 1;
        createDisplay(lengthDisplay, HEIGHT_BUTTON, startY, startX);

        keyPressed = wgetch(calculatorCase);
        processKeypress(keyPressed, &line, &column);

        charactersTEMP[0] = buttons[line][column][0]; charactersTEMP[1] = '\0';
        removeSpaces(charactersDisplay);

        /* 10 = ENTER */
        if (keyPressed == 10)
        {
            if (charactersTEMP[0] != '=' && charactersTEMP[0] != 'C') strcat(charactersDisplay, charactersTEMP); // Concatenando os caracteres
            if (defaultLoop > 0) defineLoop(&startX_enter, &defaultLoop);   // Modificando o loop
            if (charactersTEMP[0] == 'C') handleC(&startX_enter, &defaultLoop, &countCharacters, &result); // Limpar tela caso seja C

            // Lógica para os digitos aparecerem na tela.
            if (isdigit(charactersTEMP[0]))
            {
                characters[countCharacters] = *charactersTEMP;
                drawDisplayResults(&startX_enter);
            }

            // Lógica para os caracteres de operação aparecerem na tela.
            else if (!isdigit(charactersTEMP[0]) && charactersTEMP[0] != '=' && charactersTEMP[0] != 'C') drawDisplayResults(&startX_enter);
        }

        //
        if (keyPressed == 10 && !isdigit(charactersTEMP[0]))
        {
            // Lógica quando o operador '+' é pressionado
            if (charactersTEMP[0] == '+')
            {
                sum();
                characterOperator[0] = '+';
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

            // Lógica quando o operador '=' é pressionado
            else if (charactersTEMP[0] == '=')
            {
                defaultLoop = 1;
                int defaultX = LENGTH_DISPLAY_X;
                int defaultPrint = 1;

                processOperationEquals(characterOperator[0]);
                configPositionX(&defaultLoop, &defaultX, &defaultPrint, &result);


                drawClearDisplay();
                clearVariable(charactersDisplay);

                sprintf(charactersDisplay + (strlen(charactersDisplay) - defaultPrint), "%d", result); countCharacters = 0;


                mvwprintw(calculatorCase, START_Y_DISPLAY, defaultX, "%i", result);
            }
        }
    }
}


/**
 * @brief Configuração de exibição dos numeros e caracteres.
*/
void drawDisplayResults(int *startX_enter)
{
    mvwprintw(calculatorCase, START_Y_DISPLAY, (*startX_enter), "%s", charactersDisplay);
    countCharacters++;
    countCharactersDisplay++;
    (*startX_enter)--;
}

/**
 * @brief Configuração da posição de X de acordo com o número de loops do aplicativo.
*/
void defineLoop(int *startX_enter, int *defaultLoop)
{
    (*startX_enter) = 30;
    (*startX_enter) -= (*defaultLoop);
    (*defaultLoop)++;
}

/**
 * @brief Configuração da posição de X, para mostrar na tela.
*/
void configPositionX(int *defaultLoop, int *defaultX, int *defaultPrint, int *result)
{
    int arr[] = {9, 99, 999, 9999, 99999, 999999, 9999999, 99999999, 999999999};
    int numberOptions = sizeof(arr) / sizeof(arr[0]);
    for (int i = 0; i < numberOptions; i++) if ((*result) > arr[i]) (*defaultPrint)++;
    for (int i = 0; i < numberOptions; i++) if ((*result) > arr[i]) (*defaultX)--;
    for (int i = 0; i < numberOptions; i++) if ((*result) > arr[i]) (*defaultLoop)++;
}

/**
 * @brief Limpando a área do display.
*/
void drawClearDisplay()
{
    int count = 2;
    for (int i = 0; i < LENGTH_DISPLAY_X; i++)
    {
        mvwprintw(calculatorCase, START_Y_DISPLAY, count, " ");
        count++;
    }
}

/**
 * @brief Atribuindo espaço pra toda variavel.
*/
void clearVariable(char *display)
{
    for (int i = 0; i < strlen(display); i++)
    {
        display[i] = ' ';
    }
}

/**
 * @brief Modificações caso o character C seja pressionado.
*/
void handleC(int *startX_enter, int *defaultLoop, int *countCharacters, int *result)
{
    drawClearDisplay();
    clearVariable(charactersDisplay);
    clearVariable(characters);
    *countCharacters = 0;
    *startX_enter = 30;
    *defaultLoop = 0;
    *result = 0;
}


/**
 * Processamento
 * @brief Processando a tecla pressionada, e atribuindo suas devida coordenada.
*/
void processKeypress(int keyPressed, int *line, int *column)
{
    switch (keyPressed)
    {
        case KEY_RIGHT:
            (*column)++;
            if ((*line) == 0 && (*column) == 3) (*column) = 2;
            if ((*column) == 4) (*column) = 3;
            break;

        case KEY_LEFT:
            (*column)--;
            if ((*column) == -1) (*column) = 0;
            break;

        case KEY_UP:
            if ((*column) == 2 && (*line) == 0) (*column) = 3;
            if ((*column) == 1 && (*line) == 0) (*column) = 2;

            (*line)++;

            if ((*line) == 5) (*line) = 4;
            break;

        case KEY_DOWN:
            if ((*column) == 1 && (*line) == 1) (*column) = 0;
            if ((*column) == 2 && (*line) == 1) (*column) = 1;

            (*line)--;

            if ((*line) == -1) (*line) = 0;
            break;
        default:
            break;
    }
}

/**
 * @brief Processando a operação, e atribuindo ao resultado.
*/
void processOperationEquals(char operator)
{
    switch (characterOperator[0])
    {
        case '+':
            sum();
            break;

        case '-':
            subtraction();
            break;

        case '*':
            multiplication();
            break;

        case '/':
            division();
            break;
    }
}



// Criação das caixas
void createDisplay(int width, int height, int startY, int startX)
{
    topRow(width, height, startY, startX);
    mvwprintw(calculatorCase, startY + 1, startX, "x");
    mvwprintw(calculatorCase, startY + 1, WIDITH_DEFAULT - 2, "x");
    lowerRow(width, height, startY, startX);
    wrefresh(calculatorCase);
}

void createBox(int width, int height, char *character, int startY, int startX)
{
    topRow(width, height, startY, startX);
    middleRow(width, height, character, startY, startX);
    lowerRow(width, height, startY, startX);
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

void lowerRow(int width, int height, int startY, int startX)
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