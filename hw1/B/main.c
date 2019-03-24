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
#include <stdbool.h>

#define INIT_BUF_SIZE (20)

#define INTERSECTION ('^')
#define DIFFERENCE ('\\')
#define UNION ('U')
#define BUFF_INC_COEF (2)
#define CHAR_NUM_SET_TO_STR (4)
#define CHAR_NUM_CALC_SET (3)

typedef struct  node_t {
    struct node_t * next;
    struct node_t * prev;
    char * str;
} Node;

typedef struct set_t {
    Node * head;
    Node * tail;
    int length;
} Set;

enum Status {
    OK = 0,
    ERROR = 1
};

Set * Set_new();
Node * Node_new(const int len);
void node_free(Node * node);
void set_free(Set * set);
bool is_elem_in_set(const Set * set, const char * elem);
void push(Set * set, const char * elem);
char * pop(Set * set);
void add_elem_sort(Set * set, const char * elem);
void add_elem_to_mid(Set * set, Node * node, Node * current);
void add_elem_to_tail(Set * set, Node * node, Node * current);
Set * copy_set(const Set * set);
Set * my_union(const Set * l, const Set * r);
Set * my_difference(const Set * l, const Set * r);
Set * my_intersection(const Set * l, const Set * r);
void print_set(const Set * set);
int str_to_polish_notation(const char * cmd, char ** result_str);
char * set_to_str(const Set * set);
Set * set_from_str(const char * str);
int calc_set(const char * cmd, Set ** result);
int calculate_sets(FILE * stream);
int resize_str(char ** str, int * buf_size);
bool is_operational_symbol(char c);

int main(){
    int err = calculate_sets(stdin);
    if (err)
        puts("[error]");
    return OK;
}

Set * Set_new() {
    Set * set = (Set *)malloc(sizeof(Set));
    if (!set) {
        return NULL;
    }
    set->head = NULL;
    set->tail = NULL;
    set->length = 0;
    return set;
}

Node * Node_new(const int len) {
    if (len <= 0) {
        return NULL;
    }
    Node * node = (Node *)malloc(sizeof(Node));
    if (!node) {
        return NULL;
    }

    node->str = (char *)calloc(len, sizeof(char));
    if (!node->str) {
        free(node);
        return NULL;
    }
    return node;
}

void node_free(Node * node) {
    if (!node) {
        return;
    }

    if (!node->str) {
        free(node);
        return;
    }

    free(node->str);
    free(node);
}

void set_free(Set * set) {
    if (!set) {
        return;
    }
    Node * current = set->head;
    while (current) {
        Node * node = current;
        current = current->next;
        node_free(node);
    }

    free(set);
}

bool is_elem_in_set(const Set * set, const char * elem) {
    int result = false;
    if (!set || !elem) {
        return result;
    }
    int elen = strlen(elem);
    Node * current = set->head;
    while (current) {
        int len = strlen(current->str);
        if (elen == len && !strncmp(elem, current->str, len)) {
            result = true;
            break;
        }
        current = current->next;
    }
    return result;
}

// push elem to the end of list
void push(Set * set, const char * elem) {
    if (!set || !elem) {
        return;
    }
    
    int len = strlen(elem) + 1;
    Node * node = Node_new(len);
    if (!node) {
        return;
    }

    memcpy(node->str, elem, len);
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

// extract elem from the end of list
char * pop(Set * set) {
    if (!set && set->length <= 0) {
        return NULL;
    }
    int len = strlen(set->tail->str) + 1;
    char * result = (char *)calloc(len, sizeof(char));
    if (!result) {
        return NULL;
    }

    memcpy(result, set->tail->str, len);
    Node * node = set->tail;
    if (set->length == 1) {
        set->head = NULL;
        set->tail = NULL;
    } else {
        set->tail = set->tail->prev;
        set->tail->next = NULL;
    }        
    set->length--;
    node_free(node);
    return result;
}

void add_elem_to_mid(Set * set, Node * node, Node * current) {
    if (!set || !node || !current) {
        return;
    }

    node->prev = current->prev;
    node->next = current;

    if (current->prev) {
        current->prev->next = node;
    } else {
        set->head = node;
    }
    current->prev = node;
    set->length++;
}

void add_elem_to_tail(Set * set, Node * node, Node * current) {
    if (!set || !node) {
        return;
    }
    if (!current) {
        node->prev = NULL;
        set->head = node;
    } else {
        node->prev = current;
        current->next = node;
    }
    node->next = NULL;
    set->tail = node;
    set->length++;
}

// add elem in sorted order
void add_elem_sort(Set * set, const char * elem) {
    if (!set || !elem) {
        return;
    }
    int len = strlen(elem) + 1;
    Node * node = Node_new(len);
    if (!node) {
        return;
    }
    memcpy(node->str, elem, len);            
    Node * current = set->head;
    for (int i = 0; i < set->length && current; i++){

        int cur_len = strlen(current->str);
        int max = (cur_len > len) ? cur_len : len;
        // stop calculations when current elem was found in set 
        if (strncmp(elem, current->str, max) == 0) {
            node_free(node);
            return;
        }
        if (strncmp(elem, current->str, max) < 0) {
            add_elem_to_mid(set, node, current);
            return;
        }
        if (i == set->length - 1 && strncmp(elem, current->str, max) > 0) {
            add_elem_to_tail(set, node, current);
            return;
        }
        current = current->next;
    }
    // add first elem
    if (set->length == 0) {
        add_elem_to_tail(set, node, NULL);
    }
}

Set * copy_set(const Set * set) {
    if (!set) {
        return NULL;
    }
    Set * result = Set_new();
    if (!result) {
        return NULL;
    }

    Node * current = set->head;
    while (current) {
        add_elem_sort(result, current->str);
        current = current->next;
    }
    return result;
}

Set * my_union(const Set * left_op, const Set * right_op) {
    if (!left_op || !right_op) {
        return NULL;
    }
    Set * result = NULL;
    if (!left_op->length) {
        result = copy_set(right_op);
    } else if (!right_op->length) {
        result = copy_set(left_op);
    } else {
        result = copy_set(left_op);
        if (!result) {
            return NULL;
        }
        Node * current = right_op->head;
        while (current) {
            if (!is_elem_in_set(left_op, current->str)) {
                add_elem_sort(result, current->str);
            }
            current = current->next;
        }
    }
    return result;
}

Set * my_difference(const Set * left_op, const Set * right_op) {
    if (!left_op || !right_op) {
        return NULL;
    }
    Set * result = NULL;
    if (!right_op->length) {
        result = copy_set(left_op);
    } else {
        result = Set_new();
        if (!result) {
            return NULL;
        }
        Node * current = left_op->head;
        while (current) {
            if (!is_elem_in_set(right_op, current->str)) {
                add_elem_sort(result, current->str);
            }
            current = current->next;
        }
    }
    return result;
}

Set * my_intersection(const Set * left_op, const Set * right_op) {
    if (!left_op || !right_op) {
        return NULL;
    }
    Set * result = Set_new();
    if (!result) {
        return NULL;
    }
    Node * current = right_op->head;
    while (current) {
        if (is_elem_in_set(left_op, current->str))
            add_elem_sort(result, current->str);
        current = current->next;
    }
    return result;
}

void print_set(const Set * set) {
    if (!set) {
        return;
    }
    char * result = set_to_str(set);
    printf("%s", result);
    free(result);
}


int pop_operations_to_str(Set * stack, int * n, char ** result_str) {
    if (!n || !stack || !result_str) {
        return ERROR;
    }
    char * chr_pop = pop(stack);
    while (chr_pop && chr_pop[0] != '(') {
        (*result_str)[(*n)++] = chr_pop[0];
        free(chr_pop);
        chr_pop = pop(stack);
    }
    if (chr_pop) {
        free(chr_pop);
    }
    return OK;
}


char * char_to_str(char c) {
    char * str = (char *)calloc(2, sizeof(char));
    if (!str) {
        return NULL;
    }
    str[0] = c;
    return str;
}

bool is_operational_symbol(char c) {
    if (c == UNION || c == INTERSECTION || c == DIFFERENCE) {
        return true;
    }
    return false;
}

int pop_stack_write_to_str(Set * stack, char ** result_str, int * n) {
    if (!n || !result_str || !(*result_str) || !stack) {
        return ERROR;
    }
    char * chr_pop = pop(stack);
    if (!chr_pop) {
        return ERROR;
    }
    (*result_str)[(*n)++] = chr_pop[0];
    free(chr_pop);
    return OK;
}

int chr_by_chr_to_polish_notation(char cmd, char * last_op, Set * stack, char ** result_str, int * n) {
    int err = OK;
    if (!cmd || !stack || !result_str || !n || !last_op) {
        return ERROR;
    }

    char * chr_push = char_to_str(cmd);
    if (!chr_push) {
        return ERROR;
    }
    if (is_operational_symbol(cmd) || cmd == '(') {
        if (!(cmd == INTERSECTION && *last_op != INTERSECTION) 
            && *last_op != '(' 
            && stack->length) {
            err = pop_stack_write_to_str(stack, result_str, n); 
        }
        push(stack, chr_push);
        *last_op = chr_push[0];
    } else if (cmd == ')') {
        err = pop_operations_to_str(stack, n, result_str);
    } else if (cmd != '\n') {
        (*result_str)[(*n)++] = cmd;
    }
    free(chr_push);
    return err;
}


int str_to_polish_notation(const char * cmd, char ** result_str) {
    if (!result_str || !cmd) {
        return ERROR;
    }
    Set * stack = Set_new();
    if (!stack) {
        return ERROR;
    }
    int len = strlen(cmd);
    * result_str = (char *)calloc((len + 1), sizeof(char));
    if (!(* result_str)) {
        set_free(stack);
        return ERROR;
    }
    int err = OK;
    int n = 0;
    int inside_quotes = 0;
    int inside_parentheses = 0;
    char last_op = 0;
    for (int i = 0; i < len && !err; i++) {
        if (cmd[i] == '(') {
            inside_parentheses++; 
        } else if (cmd[i] == ')') {
            inside_parentheses--;
        } else if (cmd[i] == '\"') {
            inside_quotes = !inside_quotes;
        } else if ((cmd[i] == ' ') && (inside_quotes == 0)) {
            err = 1;
        }
        if (chr_by_chr_to_polish_notation(cmd[i], &last_op, stack, result_str, &n))
            err = 1;
    }
    if (inside_parentheses)
        err = 1;
    while (!err && stack->length) {
        if (pop_stack_write_to_str(stack, result_str, &n))
            err = 1;
    }
    set_free(stack);
    return err;
}

char * set_to_str(const Set * set) {
    if (!set) {
        return NULL;
    }
    int buf_size = INIT_BUF_SIZE;
    char * str = (char *)calloc(buf_size, sizeof(char));
    if (!str) {
        return NULL;
    }
    int n = 0;
    str[n++] = '[';
    if (set->length) {
        Node * current = set->head;
        for (int i = 0; i < set->length && current && str; i++) {
            int len = strlen(current->str);
            // buf_size - 4, because may we add 3 symbols to str and last '\0' 
            while (len + n + CHAR_NUM_SET_TO_STR >= buf_size) {
                if (resize_str(&str, &buf_size)) {
                    free(str);
                    str = NULL;
                    break;
                }
            }
            str[n++] = '\"';
            for (int i = 0; i < len; i++) {
                str[n++] = current->str[i];
            }
            str[n++] = '\"';
            if (i == set->length - 1) {
                str[n++] = ']';
            } else {
                str[n++] = ',';
            }
            str[n] = 0;
            current = current->next;
        }
    } else {
        str[n] = ']';
    }
    return str;
}


Set * set_from_str(const char * str) {
    if (!str) {
        return NULL;
    }
    Set * set = Set_new();
    if (!set) {
        return NULL;
    }
    if (!strcmp(str, "[]")) {
        return set;
    }

    int len = strlen(str) + 1;  
    int mem_error = 0;
    int new_node = 0;

    char * node_str = (char *)calloc(len, sizeof(char));
    if (!node_str) {
        set_free(set);
        return NULL;
    }

    int n = 0;
    for (int i = 0; i < len; i++) {
        if (str[i] == '\"') {
            if (new_node) {
                int node_len = strlen(node_str) + 1;
                char * elem = (char *)calloc(node_len, sizeof(char *));
                if (!elem) {
                    mem_error = 1;
                    break;
                }
                strncpy(elem, node_str, node_len);
                add_elem_sort(set, elem);
                free(elem);
            }
            new_node = !new_node;
            memset(node_str, 0, sizeof(char) * len);
            n = 0;
        } else if (new_node) {
            node_str[n] = str[i];
            n++;
        }
    }

    if (mem_error) {
        set_free(set);
        return NULL;
    }
    free(node_str);
    return set;
}

int resize_str(char ** str, int * buf_size) {
    if (!str || !(*str) || !buf_size) {
        return ERROR;
    }
    (*buf_size) *= BUFF_INC_COEF;
    char * tmp = (char *)realloc(*str, sizeof(char) * (*buf_size));
    if (tmp) {
        (*str) = tmp;
        return OK;
    }
    return ERROR;
}

Set * calculate_set_by_command(char cmd, Set * stack, Set * left_op, Set ** right_op) {    
    if (!left_op || !stack) {
        return NULL;
    }
    Set * result;
    if (!(*right_op)) {
        char * prev_res = pop(stack);
        *right_op = set_from_str(prev_res);
        free(prev_res);
    }
    switch (cmd) {
        case UNION:
            result = my_union(left_op, *right_op);
            break;
        case DIFFERENCE:
            result = my_difference(left_op, *right_op);
            break;
        case INTERSECTION:
            result = my_intersection(left_op, *right_op);
            break;
    }
    return result;
}

int get_operand(char * set, Set ** stack, Set ** left_op, Set ** right_op) {
    int err = OK;
    if (!(*left_op)) {
        *left_op = set_from_str(set);
    } else if (!(*right_op)) {
        *right_op = set_from_str(set);
    } else if (*left_op && *right_op) {
        char * result_str = set_to_str(*left_op);
        if (!result_str) {
            err = ERROR;
            set_free(*left_op);
            return err;
        }
        push(*stack, result_str);
        free(result_str);
        set_free(*left_op);

        *left_op = *right_op;
        *right_op = set_from_str(set);          
    }
    return err;
}

int calc_set(const char * cmd, Set ** result) {
    if (!cmd || !result)
        return ERROR;

    int buf_size = INIT_BUF_SIZE;
    Set * left_op = NULL;
    Set * right_op = NULL;
    Set * temp_result = NULL;
    Set * stack = Set_new(); // stack - temp_result storage
    if (!stack) {
        return ERROR;
    }
    char * set = (char *)calloc(buf_size, sizeof(char));
    if (!set) {
        set_free(stack);
        return ERROR;
    }

    int err = OK;
    int len = strlen(cmd);
    for (int i = 0; i < len; i++) {
        if (err)
            break;
        if (cmd[i] == '[') {
            int n = 0;
            memset(set, 0, sizeof(char) * buf_size);
            while (cmd[i] != ']' && n < len) {
                if (n >= buf_size - CHAR_NUM_CALC_SET) {
                    if (resize_str(&set, &buf_size)) {
                        err = 1;
                        break;
                    }
                }
                set[n++] = cmd[i++];
            }
            set[n++] = ']';
            set[n] = 0;

            if (get_operand(set, &stack, &left_op, &right_op)) {
                err = ERROR;
                break;
            }
        } else if (is_operational_symbol(cmd[i]) && left_op) {
            temp_result = calculate_set_by_command(cmd[i], stack, left_op, &right_op);
            set_free(left_op);
            set_free(right_op);
            left_op = temp_result;
            temp_result = right_op = NULL;
        }
    }
    if (!err) {
        *result = left_op;
    } else {
        set_free(left_op);
        *result = NULL;
    }

    if (set)
        free(set);
    set_free(right_op);
    set_free(stack);
    set_free(temp_result);
    return err;
}


int calculate_sets(FILE * stream) {
    if (!stream) {
        return ERROR;
    }

    char * cmd = NULL;
    size_t i = 0;
    if (!getline(&cmd, &i, stream)) {
        return ERROR;
    }

    char * not = NULL;
    int err = str_to_polish_notation(cmd, &not);
    if (err) {
        if (not)
            free(not);
        free(cmd);
        return ERROR;
    }

    Set * set = NULL;
    err = calc_set(not, &set);
    if (err) {
        set_free(set);
        if (not)
            free(not);
        free(cmd);
        return ERROR;
    }

    print_set(set);
    set_free(set);
    if (not) {
        free(not);
    }
    free(cmd);
    return OK;
}

