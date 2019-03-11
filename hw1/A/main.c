/*Задача A-6. Задача о приведении строк к нижнему регистру символов
Time limit:	14 s
Memory limit:	64 M
Составить программу построчной обработки текста. Суть обработки - приведение
каждого символа в каждой строке к нижнему регистру.

Программа считывает входные данные со стандартного ввода, и печатает результат
в стандартный вывод.

Процедура приведения к нижнему регистру должна быть оформлена в виде отдельной
функции, которой на вход подается массив строк (который необходимо обработать),
количество переданных строк, а также указатель на переменную, в которой
необходимо разместить результат - массив уже обработанных строк.

В качестве возвращаемого значения функция должна возвращать количество строк,
содержащихся в результирующем массиве.

Программа должна уметь обрабатывать ошибки - такие как неверные входные
данные(отсутствие входных строк) или ошибки выделения памяти и т.п.

В случае возникновения ошибки нужно выводить об этом сообщение "[error]" и
завершать выполнение программы.*/

#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#define START_BUFFER_SIZE (20)
#define START_STR_NUM (1)
#define BUF_INC_SPEED (2)

int stream_to_low(FILE *);
int str_to_low(char **, int, char ***);
char char_to_low(char);

char **alloc_arr(int, int);
void free_arr(char **, int);

void print_mas(char **, int);
void show_error();


int main(void) {

    if (stream_to_low(stdin) < 0)
        show_error();

    return 0;
}

char **alloc_arr(int str_buf, int chr_buf) {
    if (str_buf <= 0 || chr_buf <= 0)
        return NULL;

    char ** str_arr = (char **)malloc(sizeof(char*) * str_buf);
    if (str_arr == NULL)
        return NULL;

    for (int i = 0; i < str_buf; i++) {
        str_arr[i] = (char *)malloc(chr_buf);

        if (str_arr[i] == NULL)
            return NULL;
    }

    return str_arr;
}

void show_error() {
    printf ("[error]");
}

char char_to_low(char symbol){
    if (symbol >= 'A' && symbol <= 'Z')
        return symbol - 'A' + 'a';
    return symbol;
}

int str_to_low(char ** str_arr, int str_num, char *** result) {

    int str_res = 0;

    if (str_num < 0 || result == NULL)
        return -1;

    *result = (char **) malloc(sizeof(char*) * str_num);
    if (*result == NULL)
        return -2;

    char last_el = 'a';

    for (int i = 0; i < str_num; i++) {
        if (last_el == EOF || last_el == '\0') {
            return -3;
        } else {
            int n = strlen(str_arr[i]) + 1;

            (*result)[i] = (char *) calloc(1, n + 1);
            if ((*result)[i] == NULL)
                return -2;

            for (int j = 0; j < n; j++) {
                (*result)[i][j] = char_to_low(str_arr[i][j]);
            }
            last_el = str_arr[i][n - 1];
            str_res++;
        }
    }

    return str_res;
}

void print_mas(char ** mas, int n) {
    for (int i = 0; i < n; i++) {
        int m = strlen(mas[i]);
        for (int j = 0; j < m; j++) {
            printf("%c", mas[i][j]);
        }

        if (i == n - 1)
            printf("\n");
    }
}

void free_arr(char ** mas, int n) {
    for (int i = 0; i < n; i++) {
        free(mas[i]);
    }

    free(mas);
}

int stream_to_low(FILE *stream) {
    int str_buf = START_STR_NUM;
    int chr_buf = START_BUFFER_SIZE;

    char ** res = NULL;

    char ** str_arr = alloc_arr(str_buf, chr_buf);
    if (str_arr == NULL)
        return -2;

    char curc = 0;
    int str_num = 0;
    int chr_num = 0;

     while (!feof(stream)) {
        curc = getchar();
        if (curc == '\n' || curc == EOF) {
            str_arr[str_num][chr_num] = 0;
            int status = str_to_low(str_arr, 1, &res);
            if (status < 0) {
                free_arr(str_arr, 1);
                free_arr(res, 1);
                return status;
            }
            print_mas(res, 1);
            free_arr(res, 1);
            chr_num = 0;
        } else {
            // chr_buf - 1 is last index
            // '\0' - last element
            if (chr_num == chr_buf - 2) {
                chr_buf *= BUF_INC_SPEED;
                str_arr[str_num] = (char *)realloc(str_arr[str_num],  chr_buf);

                if (str_arr[str_num] == NULL)
                    return -2;
                
            }
            str_arr[str_num][chr_num] = curc;
            chr_num++;
        }
    }

    free_arr(str_arr, 1);
    return 0;  
}   
