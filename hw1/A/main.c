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

int to_low(char ** str_mas, int str_num, char *** result) {

    int str_res = 0;
    *result = (char **) malloc(sizeof(char*) * str_num);
    char last_el = 'a';

    for (int i = 0; i < str_num; i++) {   
        if (last_el == EOF || last_el == '\0') {
            puts("[error]");
        } else {
            int n = strlen(str_mas[i]) + 1;
            (*result)[i] = (char *) calloc(sizeof(char), n + 1);

            for (int j = 0; j < n; j++) {
                if (str_mas[i][j] >= 'A' && str_mas[i][j] <= 'Z') {
                    (*result)[i][j] = (char)(str_mas[i][j] - 'A' + 'a');
                } else {
                    (*result)[i][j] = str_mas[i][j];
                }
            }
            last_el = str_mas[i][n - 1];
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

void free_mas(char ** mas, int n) {
    for (int i = 0; i < n; i++) {
        free(mas[i]);
    }
    free(mas);
}


int main(void) {
    int str_buf = 1;
    int chr_buf = 20;

    char ** res = NULL;   
    char ** str_mas = (char **)malloc(sizeof(char*) * str_buf);
    for (int i = 0; i < str_buf; i++) {
        str_mas[i] = (char *)malloc(sizeof(char) * chr_buf);
    }

    char curc = 0;
    int str_num = 0;
    int chr_num = 0;

    while (!feof(stdin)) {
        curc = getchar();
        if (curc == '\n' || curc == EOF) {
            str_mas[str_num][chr_num] = 0;
            to_low(str_mas, 1, &res);
            print_mas(res, 1);
            free_mas(res, 1);
            chr_num = 0;
        } else {
            if (chr_num == chr_buf - 2) {
                chr_buf *= 2;
                str_mas[str_num] = (char *)realloc(str_mas[str_num], sizeof(char) * chr_buf);
            }
            str_mas[str_num][chr_num] = curc;
            chr_num++;
        }
    }
    free_mas(str_mas, 1);        
    return 0;
}

