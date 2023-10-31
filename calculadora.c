#include "calculadora.h"
#include <ctype.h>
#include <locale.h>
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdbool.h>

// Variaveis para manipulaçação das operações
char currentSelectedCharacterOperator;
char tempCurrentSelectedCharacterOperator;
char *currentNumberBeingTyped = NULL;
char *charactersDisplay = NULL;
int countNumberBeingTyped = 0;
int countCharactersDisplay = 0;

int result = 0;
int defaultLoop = 0;

int number = 0;
int countOperations = 0;
int countEquals = 0;

// Definição dos botões da calculadora
char *buttons[BUTTON_ROWS][BUTTON_COLUMNS] = {
    {"0", ",", "=", ""}, //
    {"1", "2", "3", "+"}, //
    {"4", "5", "6", "-"}, //
    {"7", "8", "9", "x"}, //
    {"C", "<", "%", "/"} //
};

// Variaveis principais
WINDOW *calculatorCase;

int main(int argc, char const *argv[])
{
    setlocale(LC_ALL, "");
    initialize();

    currentNumberBeingTyped = allocateMemory();
    charactersDisplay = allocateMemory();

    menuWindow();
    endwin();

    return 0;
}

void showTerminalColorError()
{
    printw("Seu terminal não suporta cores, sua experiência não será completa. [:-(");
    mvprintw(1, 0, "Pressione qualquer tecla para continuar...");
    getch();
    clear();
}

// ASSETS
void initialize()
{
    initscr();
    cbreak();
    curs_set(FALSE); // Não mostra o cursor
    if (!has_colors()) showTerminalColorError();
    start_color();

    // Tamanho do console, quando aberto pelo usuario
    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);

    // Criando a borda da calculadora
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
    if (temp == NULL)
        showErrors("Memory allocation failure.");
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

void replace(char *characterDisplay)
{
    int lengthChar = strlen(charactersDisplay);
    char *temp = malloc(lengthChar * sizeof(char));

    for (int i = 0; i < lengthChar - 1; i++)
    {
        temp[i] = charactersDisplay[i];
    }
    temp[lengthChar - 1] = '\0';
    mvwprintw(calculatorCase, 1, 1, "%s", temp);
    strcpy(characterDisplay, temp);
    mvwprintw(calculatorCase, 5, 1, "%s", characterDisplay);
}

// Operações matematicas
void sum()
{
    int number = atoi(currentNumberBeingTyped);
    result += number;

    clearVariable(currentNumberBeingTyped);

    countNumberBeingTyped = 0;
}

void subtraction()
{
    int number = atoi(currentNumberBeingTyped);
    result = result - number;
    clearVariable(currentNumberBeingTyped);
    countNumberBeingTyped = 0;
}

void multiplication()
{
    int number = atoi(currentNumberBeingTyped);
    result = number * result;
    clearVariable(currentNumberBeingTyped);
    countNumberBeingTyped = 0;
}

void division()
{
    int number = atoi(currentNumberBeingTyped);
    result = result / number;
    clearVariable(currentNumberBeingTyped);
    countNumberBeingTyped = 0;
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
                if ((*column) == 3 && (*line) == 0)
                    (*column) = 2;
                widthButton = (STR_CMP(buttons, "0")) ? (WIDITH_DEFAULT - 3) / 2 : (WIDITH_DEFAULT - 3) / 4;

                if (i == (*line) && j == (*column))
                    wattron(calculatorCase, A_REVERSE);

                createBox(widthButton, HEIGHT_BUTTON, buttons[i][j], (*startY), (*startX));

                if (i == (*line) && j == (*column))
                    wattroff(calculatorCase, A_REVERSE);
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
    char charactersTEMP[1];
    bool verification = true;

    while (1)
    {
        // Visual da calculadora
        int startY = LENGTH_DISPLAY_Y, startX = 2;
        int lengthDisplay = LENGTH_DISPLAY_X + 1;
        drawCalculatorButtons(&startY, &startX, &line, &column);
        createDisplay(lengthDisplay, HEIGHT_BUTTON, startY, startX);

        keyPressed = wgetch(calculatorCase);
        processKeypress(keyPressed, &line, &column);

        // Guarda o character selecionado (não confirmado)
        charactersTEMP[0] = buttons[line][column][0];
        charactersTEMP[1] = '\0';

        removeSpaces(charactersDisplay);

        // Debug
        int count = 2;
        for (int i = 0; i < LENGTH_DISPLAY_X; i++)
        {
            mvwprintw(calculatorCase, 1, count, " ");
            count++;
        }
        //--------//

        if (keyPressed != 10) continue;

        if (verification == false && charactersTEMP[0] == 'C') verification = true;
        if (verification == false) continue;

        // Ignore '=' 'C' '<' na hora de concatenar para mostrar na tela
        if (IS_CLEAR_CHARACTER(charactersTEMP[0]) == false)
        {
            strcat(charactersDisplay, charactersTEMP);
        }

        // Limpe a tela e a variavel, caso o primeiro caracter seja um operador
        if (ALL_CHARACTERS_NOT_NUMBERS(charactersDisplay[0]))
        {
            clearVariable(charactersDisplay);
            continue;
        }

        // ? Misterios da meia noite ?
        if (defaultLoop > 0)
        {
            defineLoop(&startX_enter);
        }

        processKeyClear(charactersTEMP[0], &startX_enter);

        // Atualizar numero sendo digitado
        if (strlen(currentNumberBeingTyped) < 9)
        {
            if (isdigit(charactersTEMP[0]))
            {
                currentNumberBeingTyped[countNumberBeingTyped] = *charactersTEMP;
                drawDisplayResults(&startX_enter);
                indexPosition(&startX_enter);
            }
            else
            {
                if ( IS_CLEAR_CHARACTER(charactersTEMP[0]) == false )
                {
                    drawDisplayResults(&startX_enter);
                    indexPosition(&startX_enter);
                }
                processOperations(charactersTEMP[0], &startX_enter);
            }
        }

        else
        {
            verification = errorMessageOfLength(&startX_enter);
        }
    }
}


bool errorMessageOfLength(int *startX_enter)
{
    drawClearDisplay();
    clearVariable(charactersDisplay);
    clearVariable(currentNumberBeingTyped);
    for (int i = 0; i < 7; i++)
    {
        currentNumberBeingTyped[i] = '0';
    }
    currentNumberBeingTyped[8] = '\0';
    countNumberBeingTyped = 0;
    countCharactersDisplay = 0;
    (*startX_enter) = 30;
    mvwprintw(calculatorCase, 3, 5, "LIMITE EXCEDIDO, APERTE C ");
    return false;
}

void drawDisplayResults(int *startX_enter)
{
    mvwprintw(calculatorCase, START_Y_DISPLAY, (*startX_enter), "%s", charactersDisplay);
}

void indexPosition(int *startX_enter)
{
    countNumberBeingTyped++;
    countCharactersDisplay++;
    (*startX_enter)--;
}

void defineLoop(int *startX_enter)
{
    (*startX_enter) = 30;
    (*startX_enter) -= defaultLoop;
    defaultLoop++;
}

void configPositionX(int *defaultX, int *defaultPrint, int *result)
{
    int arrayPositive[] = {9, 99, 999, 9999, 99999, 999999, 9999999, 99999999, 999999999};
    int arrayNegative[] = {0, -9, -99, -999, -9999, -99999, -999999, -9999999, -99999999, -999999999};
    int numberOptionsPositive = sizeof(arrayPositive) / sizeof(arrayPositive[0]);
    int numberOptionsNegative = sizeof(arrayNegative) / sizeof(arrayNegative[0]);

    for (int i = 0; i < numberOptionsPositive; i++)
        if ((*result) > arrayPositive[i])
            (*defaultPrint)++;
    for (int i = 0; i < numberOptionsPositive; i++)
        if ((*result) > arrayPositive[i])
            (*defaultX)--;

    for (int i = 0; i < numberOptionsNegative; i++)
        if ((*result) < arrayNegative[i])
            (*defaultX)--;

    for (int i = 0; i < numberOptionsPositive; i++)
        if ((*result) > arrayPositive[i])
            defaultLoop++;
}

void drawClearDisplay()
{
    int count = 2;
    for (int i = 0; i < LENGTH_DISPLAY_X; i++)
    {
        mvwprintw(calculatorCase, START_Y_DISPLAY, count, " ");
        count++;
    }
}

void clearVariable(char *display)
{
    for (int i = 0; i < strlen(display); i++)
    {
        display[i] = ' ';
    }
}

void handleC(int *startX_enter, int *countNumberBeingTyped, int *result)
{
    drawClearDisplay();
    clearVariable(charactersDisplay);
    clearVariable(currentNumberBeingTyped);
    *countNumberBeingTyped = 0;
    *startX_enter = 30;
    defaultLoop = 0;
    *result = 0;
}

void processKeypress(int keyPressed, int *line, int *column)
{
    switch (keyPressed)
    {
        case KEY_RIGHT:
            (*column)++;
            if ((*line) == 0 && (*column) == 3)
                (*column) = 2;
            if ((*column) == 4)
                (*column) = 3;
            break;

        case KEY_LEFT:
            (*column)--;
            if ((*column) == -1)
                (*column) = 0;
            break;

        case KEY_UP:
            if ((*column) == 2 && (*line) == 0)
                (*column) = 3;
            if ((*column) == 1 && (*line) == 0)
                (*column) = 2;

            (*line)++;

            if ((*line) == 5)
                (*line) = 4;
            break;

        case KEY_DOWN:
            if ((*column) == 1 && (*line) == 1)
                (*column) = 0;
            if ((*column) == 2 && (*line) == 1)
                (*column) = 1;

            (*line)--;

            if ((*line) == -1)
                (*line) = 0;
            break;
        default:
            break;
    }
}

void processKeyClear(char character, int *startX_enter)
{
    switch (character)
    {
        case 'C':
            handleC(startX_enter, &countNumberBeingTyped, &result); // Limpar tela caso seja C
            break;

        case '<':
            if (strlen(charactersDisplay) != 0)
            {
                replace(charactersDisplay);
                drawClearDisplay();
                (*startX_enter) += 2;
                mvwprintw(calculatorCase, START_Y_DISPLAY, (*startX_enter), "%s", charactersDisplay);
                (*startX_enter)--;
            }
            break;

        default:
            break;
    }
}

void processOperationEquals(char operator)
{
    switch (currentSelectedCharacterOperator)
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
    currentSelectedCharacterOperator = '\0';
}

void equal()
{
    defaultLoop = 1;
    int defaultX = LENGTH_DISPLAY_X;
    int defaultPrint = 1;
    processOperationEquals(currentSelectedCharacterOperator);
    configPositionX(&defaultX, &defaultPrint, &result);
    drawClearDisplay();
    clearVariable(currentNumberBeingTyped);
    clearVariable(charactersDisplay);
    sprintf(charactersDisplay + (strlen(charactersDisplay) - defaultPrint), "%d", result);

    sprintf(currentNumberBeingTyped + 0, "%d", result);

    countNumberBeingTyped = 1;
    mvwprintw(calculatorCase, START_Y_DISPLAY, defaultX, "%i", result);
}

void equalRepeat(char tempCurrentSelectedCharacterOperator)
{
    defaultLoop = 2;
    int defaultX = LENGTH_DISPLAY_X;
    int defaultPrint = 1;
    processOperationEquals(currentSelectedCharacterOperator);
    configPositionX(&defaultX, &defaultPrint, &result);
    drawClearDisplay();
    clearVariable(currentNumberBeingTyped);
    clearVariable(charactersDisplay);
    sprintf(charactersDisplay + (strlen(charactersDisplay) - defaultPrint), "%d", result);
    countNumberBeingTyped = 1;

    sprintf(charactersDisplay + strlen(charactersDisplay), "%c", tempCurrentSelectedCharacterOperator);
    mvwprintw(calculatorCase, START_Y_DISPLAY, defaultX-4, "%s", charactersDisplay);
}


void processOperations(char currentCharacter, int *startX_enter)
{
    int temp = strlen(charactersDisplay);
    for (int i = 0; i < temp; i++)
    {
        if (IS_OPERATOR(charactersDisplay[i]))
        {
            countOperations++;
        }
        if (charactersDisplay[i] == '=')
        {
            countEquals++;
        }
    }

    if (IS_OPERATOR(currentCharacter) || currentCharacter == '=')
    {
        if(countOperations == 2 || currentCharacter == '=')
        {
            char tempCurrentSelectedCharacterOperator;
            if (currentCharacter != '=')
            {
                countEquals = 0;
                switch (currentCharacter)
                {
                    case '+':
                        tempCurrentSelectedCharacterOperator = '+';
                        currentSelectedCharacterOperator = '+';
                        break;

                    case '-':
                        tempCurrentSelectedCharacterOperator = '-';
                        currentSelectedCharacterOperator = '-';
                        break;

                    case 'x':
                        tempCurrentSelectedCharacterOperator = 'x';
                        currentSelectedCharacterOperator = '*';
                        break;

                    case '/':
                        tempCurrentSelectedCharacterOperator = '/';
                        currentSelectedCharacterOperator = '/';
                        break;
                }
                equalRepeat(tempCurrentSelectedCharacterOperator);
                countOperations = 0;
            }
            else
            {
                if (countEquals == 0)
                {
                    switch (currentCharacter)
                    {
                        case '+':
                            currentSelectedCharacterOperator = '+';
                            break;

                        case '-':
                            currentSelectedCharacterOperator = '-';
                            break;

                        case 'x':
                            currentSelectedCharacterOperator = '*';
                            break;

                        case '/':
                            currentSelectedCharacterOperator = '/';
                            break;
                    }
                }
                equal();
                countEquals = 1;
                countOperations = 0;
            }
            return;
        }
    }


    switch (currentCharacter)
    {
        case '+':
            countOperations = 0;
            number = atoi(currentNumberBeingTyped);
            result = number;
            currentSelectedCharacterOperator = '+';
            clearVariable(currentNumberBeingTyped);
            countNumberBeingTyped = 0;
            break;

        case '-':
            countOperations = 0;
            number = atoi(currentNumberBeingTyped);
            result = number;
            currentSelectedCharacterOperator = '-';
            clearVariable(currentNumberBeingTyped);
            countNumberBeingTyped = 0;
            break;

        case 'x':
            countOperations = 0;
            number = atoi(currentNumberBeingTyped);
            if (number != 0) result = number;
            currentSelectedCharacterOperator = '*';
            clearVariable(currentNumberBeingTyped);
            countNumberBeingTyped = 0;
            break;

        case '/':
            countOperations = 0;
            number = atoi(currentNumberBeingTyped);
            if (number != 0)  result = number;
            currentSelectedCharacterOperator = '/';
            clearVariable(currentNumberBeingTyped);
            countNumberBeingTyped = 0;
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