#include <curses.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define X 80  //длина поля
#define Y 25  //ширина поля

char **dynamic_alloc();  // функция выделения динамической памяти под масив

void run(char **data1, char **data2, char *key, int *speed);  // функция запуска игры

void init_field(char **data1, FILE *file);      // функция инициализация поля
void draw_field(char **data1, int speed);       // функция отрисовки поля
void update_field(char **data1, char **data2);  // функция обновления поля

int life(char **data2, int y, int x);  // функция реализация жизни-смерти

void menu();                             // функция вывода стартового меню
int main_menu(char **data1, int speed);  //функция выбора фигур и начальной точки(из 5 файлов)

int main() {
    initscr();  //начало работы библиотеки ncurses
    cbreak();   //дает возможность ввода с клавиатуры
    noecho();   //вводимые клавиши не отображаются
    nodelay(stdscr, TRUE);  //не требуется постоянное обновление пользователем,
                            //поле обновляется без задержек

    int speed = 100000;  // начальная скорость игры
    char key = '0';  //инициализация основной клавиши, в зависимости от которой будут выбор и
                     //изменения(например в стартовом меню)

    char **data1 = dynamic_alloc();  //инициализация массива data1, который заполняется полем из файла
    char **data2 = dynamic_alloc();  //инициализация массива data2, который заполняется полем из файла

    menu();  //вызов стартового меню

    if (main_menu(data1, speed)) {
        run(data1, data2, &key, &speed);
    } else {
        printw("\nError! File incorrect!\nPress q to exit...");
    }

    free(data2);  //устранение утечек
    free(data1);  //устранение утечек

    endwin();  //конечная точка работы библиотеки ncurses

    return 0;
}

char **dynamic_alloc() {
    char **data = malloc(Y * sizeof(char *) + Y * X * sizeof(int));
    char *ptr1 = (char *)(data + Y);
    for (int i = 0; i < Y; i++) {
        data[i] = ptr1 + X * i;
    }
    return data;
}

void run(char **data1, char **data2, char *key, int *speed) {
    while (*key != 'q' && *key != 'Q') {  //цикл работает пока не будет нажата клавиша q
        *key = getch();                   //ожидание нажатия клавиши
        update_field(data1, data2);
        draw_field(data1, *speed);
        if (*key == '+' && *speed >= 20000) {  //измениение скорости при нажатии на '+'
            *speed -= 10000;
        }
        if (*key == '-' && *speed <= 150000) {  //измениение скорости при нажатии на '-'
            *speed += 10000;
        }
    }
}

void init_field(char **data1, FILE *file) {
    char temp;
    for (int i = 0; i < Y; i++) {
        for (int j = 0; j < X; j++) {
            temp = getc(file);  //считывание файла
            if (temp == '.') {
                data1[i][j] = ' ';
            } else if (temp == '*') {
                data1[i][j] = '*';
            } else {
                j--;
            }
        }
    }
}

void draw_field(char **data1, int speed) {
    clear();
    usleep(speed);
    printw(">");
    for (int i = 0; i < X; i++) printw("~");
    printw("<\n");
    for (int i = 0; i < Y; i++) {
        printw(">");
        for (int j = 0; j < X; j++) {
            if (j == X - 1) {
                printw("%c", data1[i][j]);
                continue;
            }
            printw("%c", data1[i][j]);
        }
        printw("<\n");
    }
    printw(">");
    for (int i = 0; i < X; i++) printw("~");
    printw("<");
}

void update_field(char **data1, char **data2) {
    for (int i = 0; i < Y; i++) {
        for (int j = 0; j < X; j++) {
            data2[i][j] = data1[i][j];
        }
    }
    for (int i = 0; i < Y; i++) {
        for (int j = 0; j < X; j++) {
            if (life(data2, i, j))  //грубо говоря, если работа функции реализации игры равна
                                    // TRUE(мы в самой той функции ретерним 1 или 0. Это будет
                                    //работать если ретернится 1), то элемент массива
                                    //превращается в '*'
                data1[i][j] = '*';
            else
                data1[i][j] = ' ';
        }
    }
}

int life(char **data2, int y_point, int x_point) {
    int live_points = 0;
    if (data2[(y_point + 1 + Y) % Y][x_point] == '*') live_points++;
    if (data2[(y_point - 1 + Y) % Y][x_point] == '*') live_points++;
    if (data2[y_point][(x_point + 1 + X) % X] == '*') live_points++;
    if (data2[y_point][(x_point - 1 + X) % X] == '*') live_points++;
    if (data2[(y_point - 1 + Y) % Y][(x_point - 1 + X) % X] == '*') live_points++;
    if (data2[(y_point + 1 + Y) % Y][(x_point + 1 + X) % X] == '*') live_points++;
    if (data2[(y_point + 1 + Y) % Y][(x_point - 1 + X) % X] == '*') live_points++;
    if (data2[(y_point - 1 + Y) % Y][(x_point + 1 + X) % X] == '*') live_points++;
    if (data2[y_point][x_point] == '*' && (live_points == 2 || live_points == 3))
        return 1;
    else if (data2[y_point][x_point] == ' ' && live_points == 3)
        return 1;
    return 0;
}

void menu() {
    printw(
        "Choose shapes:\n1. Symmetric "
        "oscilator\n2. Rake\n3. Hive\n4. Diehard\n5. Gun\n[+]-speed up\n[-]-speed "
        "down\nPRESS Q TO EXIT!\n");
}

int main_menu(char **data1, int speed) {
    int flag = 0;
    char key = '0';
    while (key != '1' && key != '2' && key != '3' && key != '4' && key != '5' && key != 'q' && key != 'Q') {
        key = getch();
    }
    FILE *file;
    switch (key) {
        case '1':
            file = fopen("1.txt", "r");
            flag = 1;
            init_field(data1, file);
            draw_field(data1, speed);
            break;
        case '2':
            file = fopen("2.txt", "r");
            flag = 1;
            init_field(data1, file);
            draw_field(data1, speed);
            break;
        case '3':
            file = fopen("3.txt", "r");
            flag = 1;
            init_field(data1, file);
            draw_field(data1, speed);
            break;
        case '4':
            file = fopen("4.txt", "r");
            flag = 1;
            init_field(data1, file);
            draw_field(data1, speed);
            break;
        case '5':
            file = fopen("5.txt", "r");
            flag = 1;
            init_field(data1, file);
            draw_field(data1, speed);
            break;
        default:
            flag = 0;
            break;
    }
    fclose(file);
    return flag;
}