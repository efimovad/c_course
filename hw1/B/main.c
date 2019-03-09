/*Задача B-2. Калькулятор для строковых множеств
Time limit:	14 s
Memory limit:	64 M
Разработайте программу-калькулятор, умеющую вычислять арифметические выражения 
над множествами строк. Входные данные (выражение с описанием множеств и 
производимых над ними операций) подаются на стандартный поток ввода программы, 
результат вычислений должен подаваться на стандартный поток вывода. 

Поддерживаемые операции: 'U' - объединение, '^' - пересечение,
'\' - разность множеств, '()' - задание приоритета вычислений.

Множества задаются в виде [el1,el2,...], где элементами являются строки в 
формате "abc def" (могут содержать пробелы).

Множества могут быть пустыми. Между операциями, множествами и элементами 
множеств пробелы запрещены.

При вычислениях должны учитываться приоритеты операций (в том числе, заданные 
вручную при помощи круглых скобочек).

Элементы результирующего множества должны выводиться в лексикографическом 
порядке.
В случае, если введенное выражение содержит ошибку, необходимо вывести в поток 
стандартного вывода сообщение "[error]" и завершить выполнение программы.*/

#define _GNU_SOURCE
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


typedef struct  _node {
    struct _node * next;
    struct _node * prev;
    char * mas;
} Node;

typedef struct _set {
    Node * head;
    Node * tail;
    int length;
} Set;


Set * Set_new() {
    Set * set = (Set *)malloc(sizeof(Set));
    set->head = NULL;
    set->tail = NULL;
    set->length = 0;
    return set;
}

void free_node(Node * node) {
    if (node) {
        if (node->mas) {
            free(node->mas);
        }
        free(node);
    }
}

void free_set(Set * set) {
    if (set) {
        Node * current = set->head;
        for (int i = 0; i < set->length; i++) {
            Node * node = current;
            if (i < set->length - 1) {
                current = current->next;
            }
            free_node(node);
        }
        free(set);
    }
}

int haveElem(Set * set, char * elem) {
    int result = 0;
    if (set) {
        int elen = strlen(elem);
        Node * current = set->head;
        while (current) {
            int len = strlen(current->mas);
            if (elen == len && !strncmp(elem, current->mas, len)) {
                result = 1;
                break;
            }
            current = current->next;
        }
    }
    return result;
}

// push elem to the end of list
void push(Set * set, char * elem) {
    if (set) {
        int len = strlen(elem);
        Node * node = (Node *)malloc(sizeof(Node));
        node->mas = (char *)malloc(sizeof(char) * (len + 1));
        memcpy(node->mas, elem, len + 1);

        node->prev = set->tail;
        node->next = NULL;
        
        if (set->tail) {
            set->tail->next = node;
        } else {
            set->head = node;
        }
        set->tail = node;
        set->length++;
    }
}

// extract elem from the end of list
char * pop(Set * set) {
    char * result = NULL;
    if (set && set->length > 0) {
        int len = strlen(set->tail->mas);
        result = (char *)calloc((len + 1), sizeof(char));
        memcpy(result, set->tail->mas, len + 1);

        Node * node = set->tail;
        if (set->length == 1) {
            set->head = NULL;
            set->tail = NULL;
        } else {
            set->tail = set->tail->prev;
            set->tail->next = NULL;
        }        
        set->length--;
        free_node(node);
    }
    return result;
}

// add elem in sorted order
void addElem_sort(Set * set, char * elem) {
    if (set) {
        int len = strlen(elem);
        Node * node = (Node *)malloc(sizeof(Node));
        node->mas = (char *)malloc(sizeof(char) * (len + 1));
        memcpy(node->mas, elem, len + 1);
        
        Node * current = set->head;
        for (int i = 0; i < set->length; i++){

            int cur_len = strlen(current->mas);
            int max = (cur_len > len) ? cur_len : len;

            // stop calculations when current elem was found in set 
            if (strncmp(elem, current->mas, max) == 0) {
                free_node(node);
                return;
            }

            // add elem between two elems in set or to the start of list
            if (strncmp(elem, current->mas, max) < 0) {
                node->prev = current->prev;
                node->next = current;

                if (current->prev) {
                    current->prev->next = node;
                } else {
                    set->head = node;
                }
                current->prev = node;
                break;
            }

            // add elem to the end of list
            if (i == set->length - 1 && strncmp(elem, current->mas, max) > 0) {
                node->prev = current;
                node->next = NULL;
                current->next = node;
                
                set->tail = node;
            }
            current = current->next;
        }

        // add first elem
        if (set->length == 0) {
            set->head = node;
            set->head->next = NULL;
            set->head->prev = NULL;
            set->tail = set->head;
        }
        set->length++;
    }
}

Set * copy_set(Set * set) {
    if (set) {
        Set * result = Set_new();
        Node * current = set->head;

        for (int i = 0; i < set->length; i++) {
            addElem_sort(result, current->mas);
            current = current->next;
        }
        return result;
    }
    return NULL;
}

Set * my_union(Set * l, Set * r) {
    Set * result = NULL;
    if (l && r) {
        if (!l->length) {
            result = copy_set(r);
        } else if (!r->length) {
            result = copy_set(l);
        } else {
            result = copy_set(l);
            Node * current = r->head;
            while (current) {
                if (!haveElem(l, current->mas)) {
                    addElem_sort(result, current->mas);
                }
                current = current->next;
            }
        }
    }
    return result;
}

Set * my_difference(Set * l, Set * r) {
    Set * result = NULL;
    if (l && r) {
        if (!r->length) {
            result = copy_set(l);
        } else {
            result = Set_new();

            Node * current = l->head;
            for (int i = 0; i < l->length; i++) {
                if (!haveElem(r, current->mas))
                    addElem_sort(result, current->mas);
                current = current->next;
            }
        }
    }
    return result;
}

Set * my_intersection(Set * l, Set * r) {
    Set * result = NULL;
    if (l && r) {
        result = Set_new();
        Node * current = r->head;
        for (int i = 0; i < r->length; i++) {
            if (haveElem(l, current->mas))
                addElem_sort(result, current->mas);
            current = current->next;
        }
    }
    return result;
}

void print_set(Set * set) {
    if (set) {
        if (set->length) {
            Node * current = set->head;
            printf("[");
            for (int i = 0; i < set->length; i++) {
                if (i == set->length - 1) {
                    printf("\"%s\"]", current->mas);
                } else {
                    printf("\"%s\",", current->mas);
                }
                current = current->next;
            }
        } else printf("[]");
    }
}

int pol_notation(char * cmd, char ** pol_not) {
    Set * stack = Set_new();
    int n = 0;
    int prev = 0;
    int result = 0;
    int word_wrt = 0; // to check odd num of " or spases inside ""

    char chr_push[2] = {'\0'};
    char * chr_pop;
    char last_op = 0;

    int len = strlen(cmd);
    * pol_not = (char *)calloc((len + 1), sizeof(char));
    for (int i = 0; i < len; i++) {                
        chr_push[0] = cmd[i];
        // ^ operation has higher priority, but
        // first ^ higher than second ^
        if (cmd[i] == 'U' || cmd[i] == '\\' || (cmd[i] == '^' && last_op == '^')) {
            if (stack->length && !prev) {
                chr_pop = pop(stack);
                (*pol_not)[n] = chr_pop[0];
                n++;
                free(chr_pop);
            }
            push(stack, chr_push);
            last_op = chr_push[0];
            prev = 0;
        // ^ higher than \ or ^
        } else if (cmd[i] == '^') {
            push(stack, chr_push);
            last_op = chr_push[0];
            prev = 0;
        } else if (cmd[i] == '(') {
            push(stack, chr_push);
            last_op = chr_push[0];
            prev = 1;
            result = 1; 
        } else if (cmd[i] == ')') {
            result = 0;
            chr_pop = pop(stack);
            while (chr_pop && chr_pop[0] != '(') {
                (*pol_not)[n] = chr_pop[0];
                n++;
                free(chr_pop);
                chr_pop = pop(stack);
            }
            free(chr_pop);
        } else if (cmd[i] != '\n') {
            if (cmd[i] == '\"') {
                word_wrt = !word_wrt;
            }
            if ((cmd[i] == ' ') && (word_wrt == 0)) {
                result = 1;
            }
            (*pol_not)[n] = cmd[i];
            n++;
        }
    }

    while (stack->length) {
        chr_pop = pop(stack);
        (*pol_not)[n] = chr_pop[0];
        n++;
        free(chr_pop);        
    }

    if (result) {
        puts("[error]");
    }
    free_set(stack);
    return result;
}

char * set_to_str(Set * set) {
    char * str = NULL;
    if (set) {
        int buf_size = 20;
        str = (char *)calloc(buf_size + 1, sizeof(char));
        str[0] = '[';
        int n = 1; // char index in str

        if (set->length) {
            Node * current = set->head;
            for (int i = 0; i < set->length; i++) {
                int len = strlen(current->mas);
                // buf_size - 4, because may we add 3 symbols to str and '\0' 
                while (len + n >= buf_size - 4) {
                    buf_size *= 2;
                    str = (char *) realloc(str, sizeof(char) * buf_size);
                }
                if (i == set->length - 1) {
                    str[n++] = '\"';
                    for (int i = 0; i < len; i++) {
                        str[n++] = current->mas[i];
                    }
                    str[n++] = '\"';
                    str[n++] = ']';
                } else {
                    str[n++] = '\"';
                    for (int i = 0; i < len; i++) {
                        str[n++] = current->mas[i];
                    }
                    str[n++] = '\"';
                    str[n++] = ',';
                }
                str[n] = 0;
                current = current->next;
            }
        } else {
            str[1] = ']';
        }
    }
    return str;
}


Set * set_from_str(char * str) {
    Set * set = NULL;
    if (str && !strcmp(str, "[]")) {
        set = Set_new();
    } else if (str) {
        int len = strlen(str);
        int new_node = 0;
        int n = 0;
        char * node_str = (char*)calloc(len + 1, sizeof(char));
        set = Set_new();

        for (int i = 0; i < len; i++) {
            if (str[i] == '\"') {
                if (new_node) {
                    int node_len = strlen(node_str);
                    char * elem = (char *)calloc(node_len + 1, sizeof(char *));
                    strncpy(elem, node_str, node_len + 1);
                    addElem_sort(set, elem);
                    free(elem);
                }
                new_node = !new_node;
                memset(node_str, 0, sizeof(char) * (len + 1));
                n = 0;
            } else {
                if (new_node) {
                    node_str[n] = str[i];
                    n++;
                }
            }
        }
        free(node_str);
    }
    return set;
}

void calc_set(char * cmd) {
    if (cmd) {
        int buf_size = 10;

        Set * l = NULL; //left operand
        Set * r = NULL; //right operand
        Set * result = NULL; // result
        Set * stack = Set_new(); // stack - for result storage
   
        char * set = (char *)calloc(buf_size, sizeof(char));
        int n = 0;

        int len = strlen(cmd);
        for (int i = 0; i < len; i++) {

            if (cmd[i] == '[') {
                memset(set, 0, sizeof(char) * buf_size);
                n = 0;
                while (cmd[i] != ']') {
                    if (n >= buf_size - 3) {
                        buf_size *= 2;
                        set = (char *)realloc(set, sizeof(char) * buf_size);
                    }
                    set[n++] = cmd[i++];
                }
                set[n] = ']';
                set[n + 1] = 0;

                if (!l) {
                    l = set_from_str(set);
                } else if (!r) {
                    r = set_from_str(set);
                } else if (l && r) {
                    char * result_str = set_to_str(l);
                    push(stack, result_str);
                    free(result_str);
                    free_set(l);

                    l = r;
                    r = set_from_str(set);          
                }
            } else if (cmd[i] == 'U' || cmd[i] == '\\' || cmd[i] == '^') {
                if (l) {
                    if (!r) {
                        char * prev_res = pop(stack);
                        r = set_from_str(prev_res);
                        free(prev_res);
                    }
                    switch (cmd[i]) {
                        case 'U':
                            result = my_union(l,r);
                            break;
                        case '\\':
                            result = my_difference(l,r);
                            break;
                        case '^':
                            result = my_intersection(l,r);
                            break;
                    }
                    free_set(l);
                    free_set(r);
                    l = result;
                    result = r = NULL;
                }
            }
        }
        print_set(l);
        free(set);
        free_set(r);
        free_set(l);
        free_set(stack);
        free_set(result);   
    }
}

int main(){
    char * cmd = NULL;
    size_t i = 0;
    if (getline(&cmd, &i, stdin)) {
        char * not;
        int err = pol_notation(cmd, &not);
        if (!err) {
            calc_set(not);
        }
        free(not);
    }
    free(cmd);
    return 0;
}
