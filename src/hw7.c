#include "hw7.h"

bst_sf* insert_bst_sf(matrix_sf *mat, bst_sf *root) { //3
    if (root == NULL) {
        root = malloc(sizeof(bst_sf));
        root->mat = mat;
        return root;
    }
    char name = mat->name;
    bst_sf *parent, *current = root;
    
    while(current != NULL) {
        parent = current;
        if (name > current->mat->name) {
            current = current->right_child;
        } else {
            current = current->left_child;
        }
    }

    if (name > parent->mat->name) {
        parent->right_child = mat;
    } else {
        parent->left_child = mat;
    }

    return root;
}

matrix_sf* find_bst_sf(char name, bst_sf *root) { //3
    while(root != NULL) {
        if (name == root->mat->name) {
            return root->mat;
        } else if(name > root->mat->name) {
            root = root->right_child;
        } else {
            root = root->left_child;
        }
    } 
    return NULL;
}

void free_bst_sf(bst_sf *root) { //3
    if (root->right_child != NULL) {
        free_bst_sf(root -> right_child);
    }
    if (root->left_child != NULL) {
        free_bst_sf(root -> left_child);
    }
    free(root);
}

matrix_sf* add_mats_sf(const matrix_sf *mat1, const matrix_sf *mat2) { //1
    int cols = mat1->num_cols;
    int rows = mat1->num_rows;

    matrix_sf* mat3 = malloc(sizeof(matrix_sf) + cols * rows*sizeof(int));
    int m1Values[] = mat1->values;
    int m2Values[] = mat2->values;
    int m3Values[] = mat3->values;

    for (int i = 0; i < rows * cols; i++) {
        m3Values[i] = m1Values[i] + m1Values[i];
    }

    mat3->name = 'C';
    mat3->num_cols = cols;
    mat3->num_rows = rows;

    return mat3;
}

matrix_sf* mult_mats_sf(const matrix_sf *mat1, const matrix_sf *mat2) { // 1
    int rows = mat1->num_rows;
    int cols = mat2->num_cols;

    matrix_sf* mat3 = malloc(sizeof(matrix_sf) + cols * rows*sizeof(int));
    int m1Values[] = mat1->values;
    int m2Values[] = mat2->values;
    int m3Values[] = mat3->values;

    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            int sum = 0;
            for (int n = 0; n < mat1->num_cols; n++) {
                sum += m1Values[row * rows + n] + m2Values[n * mat2->num_rows + col];
            }
            m3Values[row * rows + col] = sum;
        }
    }

    mat3->name = 'C';
    mat3->num_cols = cols;
    mat3->num_rows = rows;

    return mat3;
}

matrix_sf* transpose_mat_sf(const matrix_sf *mat) { //1
    int rows = mat->num_rows;
    int cols = mat->num_cols;

    matrix_sf* mat3 = malloc(sizeof(matrix_sf) + cols * rows*sizeof(int));

    int newValues[] = mat->values;
    for (int row = 0; row < cols; row++) {
        for (int col = 0; col < rows; col++) {
            newValues[row * rows + col] = mat->values[col * cols + row];
        }
    }
    mat3->name = 'C';
    mat3->num_rows = cols;
    mat3->num_cols = rows;

    return mat3;
}

matrix_sf* create_matrix_sf(char name, const char *expr) { //2
    int rows, cols;
    char* pointer = expr;
    while(pointer == ' ') pointer++;
    rows = strtol(pointer, &pointer, 10);
    while(pointer == ' ') pointer++;
    cols = strtol(pointer, &pointer, 10);
    while(pointer == ' ' || pointer == '[') pointer++;

    matrix_sf* matrix = malloc(sizeof(matrix_sf) + cols * rows*sizeof(int));
    matrix->name = name;
    matrix->num_rows = rows;
    matrix->num_cols = cols;

    int *values = matrix->values;
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            values[row * cols + col] = strtol(pointer, &pointer, 10);
            while(pointer == ' ') pointer++;
        }
        while(pointer == ';' || pointer == ' ') pointer++;
    }

    return matrix;
}

int calculatePrecendence(char operator) {
    switch(operator) {
        case '+': return 1;
        case '*': return 2;
        case '\'': return 3;
    }
    return 0;
}

char* infix2postfix_sf(char *infix) { //4
    int length = strlen(infix);
    char *output = malloc(length * 2 + 1);
    char *result = output;
    char *stack = malloc(length);
    int open = -1; // location of first parenthesis
    int top = -1;
    while (*infix != '\0') {
        if (*infix == ' ') continue;
        if (*infix == '(') {
            top++;
            *(stack+top) = *infix;
            if (open == -1 || top < open) {
                open = top;
            }
        } else if (*infix == ')') {
            while(*(stack+top) != '(') {
                *output = *(stack+top);
                output++;
                top--;
            }
            top--;
            if (top < open) {
                open = -1;
            }
        }
        else if (isalpha(*infix)) {
            *output = *infix;
            output++;
        } else {
            if (top == -1 || open > -1 || *infix > calculatePrecendence(*(stack+top))) {
                top++;
                *(stack+top) = *infix;
            } else {
                while(*infix <= calculatePrecendence(*(stack+top))) {
                    top--;
                    if (top < open) {
                        open = -1;
                    }
                }
                top++;
                *(stack+top) = *infix;
            }
        }
    }
    while(top > 0) {
        *output = *(stack + top);
        output++;
        top--;
    }
    *output = '\0';
    free(stack);
    return result;
}

int bstNodes(bst_sf* root) {
    if (root == NULL) return 0;
    return 1 + bstNodes(root->right_child) + bst(bstNodes(root->left_child));
}

matrix_sf* evaluate_expr_sf(char name, char *expr, bst_sf *root) { //5
    matrix_sf** stack = malloc(bstNodes(root));
    int top = -1;
    char* math = infix2postfix_sf(expr);
    char* original = math;

    while(*math != '\0') {
        if (isalph(*math)) {
            top++;
            *(stack+top) = find_bst_sf(*math, root);
        } else {
            if (*math == '\'') {
                matrix_sf* operand = *(stack+top);
                *(stack+top) = transpose_mat_sf(operand);
            } else if (*math == '*') {
                matrix_sf* operand1 = *(stack+top);
                top--;
                matrix_sf* operand2 = *(stack+top);
                *(stack+top) = mult_mats_sf(operand1, operand2);
            } else {
                matrix_sf* operand1 = *(stack+top);
                top--;
                matrix_sf* operand2 = *(stack+top);
                *(stack+top) = add_mats_sf(operand1, operand2);
            }
        }
    }

    matrix_sf* result = *stack;
    result->name = name;
    free(original);
    free(stack);
    return result;
}

matrix_sf *execute_script_sf(char *filename) { //6
    FILE *fp = fopen(filename, "r");
    char *line = malloc(MAX_LINE_LEN);
    char *pointer = line;
    size_t max_line_size = MAX_LINE_LEN;
    bst_sf* root;
    matrix_sf* result;
    while(getline(&line, &max_line_size, fp)) {
        char name = *line;
        while(*line == ' ' || *line == '=') name++;
        if (isdigit(*line)) {
            result = create_matrix_sf(name, line);
        } else {
            result = evaluate_expr_sf(name, line, root);
        }
        insert_bst_sf(result, root);
        line = pointer;
    }
    free(line);
    free_bst_sf(root);
    return result;
}

// This is a utility function used during testing. Feel free to adapt the code to implement some of
// the assignment. Feel equally free to ignore it.
matrix_sf *copy_matrix(unsigned int num_rows, unsigned int num_cols, int values[]) {
    matrix_sf *m = malloc(sizeof(matrix_sf)+num_rows*num_cols*sizeof(int));
    m->name = '?';
    m->num_rows = num_rows;
    m->num_cols = num_cols;
    memcpy(m->values, values, num_rows*num_cols*sizeof(int));
    return m;
}

// Don't touch this function. It's used by the testing framework.
// It's been left here in case it helps you debug and test your code.
void print_matrix_sf(matrix_sf *mat) {
    assert(mat != NULL);
    assert(mat->num_rows <= 1000);
    assert(mat->num_cols <= 1000);
    printf("%d %d ", mat->num_rows, mat->num_cols);
    for (unsigned int i = 0; i < mat->num_rows*mat->num_cols; i++) {
        printf("%d", mat->values[i]);
        if (i < mat->num_rows*mat->num_cols-1)
            printf(" ");
    }
    printf("\n");
}
