#include "hw7.h"

bst_sf* insert_bst_sf(matrix_sf *mat, bst_sf *root) { //3
    if (root == NULL) {
        root = malloc(sizeof(bst_sf));
        root->mat = mat;
        root->right_child = NULL;
        root->left_child = NULL;
        return root;
    }
    char name = mat->name;
    bst_sf *parent = NULL, *current = root;
    
    while(current != NULL) {
        parent = current;
        if (name > current->mat->name) {
            current = current->right_child;
        } else {
            current = current->left_child;
        }
    }

    bst_sf *newNode = malloc(sizeof(bst_sf));
    newNode->mat = mat;
    newNode->left_child = NULL;
    newNode->right_child = NULL;
    if (name > parent->mat->name) {
        parent->right_child = newNode;
    } else {
        parent->left_child = newNode;
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
    if(root == NULL) return;
    free_bst_sf(root -> right_child);
    free_bst_sf(root -> left_child);
    free(root->mat);
    free(root);
}

matrix_sf* add_mats_sf(const matrix_sf *mat1, const matrix_sf *mat2) { //1
    int cols = mat1->num_cols;
    int rows = mat1->num_rows;

    matrix_sf* mat3 = malloc(sizeof(matrix_sf) + cols * rows*sizeof(int));
    int *m1Values = mat1->values;
    int *m2Values = mat2->values;
    int *m3Values = mat3->values;

    for (int i = 0; i < rows * cols; i++) {
        m3Values[i] = m1Values[i] + m2Values[i];
    }

    mat3->name = '@';
    mat3->num_cols = cols;
    mat3->num_rows = rows;

    return mat3;
}

matrix_sf* mult_mats_sf(const matrix_sf *mat1, const matrix_sf *mat2) { // 1
    int rows = mat1->num_rows;
    int cols = mat2->num_cols;

    matrix_sf* mat3 = malloc(sizeof(matrix_sf) + cols * rows*sizeof(int));
    int *m1Values = mat1->values;
    int *m2Values = mat2->values;
    int *m3Values = mat3->values;

    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            int sum = 0;
            for (int n = 0; n < mat1->num_cols; n++) {
                sum += m1Values[row * mat1->num_cols + n] * m2Values[n * mat2->num_cols + col];
            }
            m3Values[row * cols + col] = sum;
        }
    }

    mat3->name = '@';
    mat3->num_cols = cols;
    mat3->num_rows = rows;

    return mat3;
}

matrix_sf* transpose_mat_sf(const matrix_sf *mat) { //1
    int rows = mat->num_rows;
    int cols = mat->num_cols;

    matrix_sf* mat3 = malloc(sizeof(matrix_sf) + cols * rows*sizeof(int));

    int* newValues = mat3->values;
    for (int row = 0; row < cols; row++) {
        for (int col = 0; col < rows; col++) {
            newValues[row * rows + col] = mat->values[col * cols + row];
        }
    }
    mat3->name = '@';
    mat3->num_rows = cols;
    mat3->num_cols = rows;

    return mat3;
}

matrix_sf* create_matrix_sf(char name, const char *expr) { //2
    int rows, cols;
    char* pointer = expr;
    while(*pointer == ' ') pointer++;
    rows = strtol(pointer, &pointer, 10);
    while(*pointer == ' ') pointer++;
    cols = strtol(pointer, &pointer, 10);
    while(*pointer == ' ' || *pointer == '[') pointer++;

    matrix_sf* matrix = malloc(sizeof(matrix_sf) + cols * rows*sizeof(int));
    matrix->name = name;
    matrix->num_rows = rows;
    matrix->num_cols = cols;

    int *values = matrix->values;
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            values[row * cols + col] = strtol(pointer, &pointer, 10);
            while(*pointer == ' ') pointer++;
        }
        while(*pointer == ';' || *pointer == ' ') pointer++;
    }

    return matrix;
}

int calculatePrecedence(char operator) {
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
    int top = -1;
    while (*infix != '\0') {
        if (*infix == ' ' || *infix == '\n') {
            infix++;
            continue;
        }
        if (*infix == '\'') {
            *output++ = '\'';
            infix++;
            continue;
        }
        if (*infix == '(') {
            top++;
            *(stack+top) = *infix;
        } else if (*infix == ')') {
            while (top >= 0 && *(stack+top) != '(') {
                *output = *(stack+top);
                output++;
                top--;
            }
            if (top >= 0 && *(stack+top) == '(') top--;
        } 
        else if (isalpha(*infix)) {
            *output = *infix;
            output++;
        } else {
            if (top == -1 || *(stack+top) == '(' || calculatePrecedence(*infix) > calculatePrecedence(*(stack+top))) {
                top++;
                *(stack+top) = *infix;
            } else {
                while (top >= 0 && *(stack+top) != '(' && calculatePrecedence(*infix) <= calculatePrecedence(*(stack+top))) {
                    *output = *(stack+top);
                    output++;
                    top--;
                }
                top++;
                *(stack+top) = *infix;
            }
        }
        infix++;
    }
    while(top >= 0) {
        *output = *(stack + top);
        output++;
        top--;
    }
    *output = '\0';
    free(stack);
    return result;
}

matrix_sf* evaluate_expr_sf(char name, char *expr, bst_sf *root) { //5
    char* math = infix2postfix_sf(expr);
    matrix_sf** stack = malloc((strlen(math) + 1) * sizeof(matrix_sf*));
    int top = -1;
    
    char* original = math;

    while(*math != '\0') {
        if (isalpha(*math)) {
            top++;
            *(stack+top) = find_bst_sf(*math, root);
        } else {
            if (*math == '\'') {
                matrix_sf* operand = *(stack+top);
                *(stack+top) = transpose_mat_sf(operand);
                if (operand->name == '@') free(operand);
            } else if (*math == '*') {
                matrix_sf* operand1 = *(stack+top);
                top--;
                matrix_sf* operand2 = *(stack+top);
                *(stack+top) = mult_mats_sf(operand2, operand1);
                if (operand2->name == '@') free(operand2);
                if (operand1->name == '@') free(operand1);
            } else {
                matrix_sf* operand1 = *(stack+top);
                top--;
                matrix_sf* operand2 = *(stack+top);
                *(stack+top) = add_mats_sf(operand2, operand1);
                if (operand2->name == '@') free(operand2);
                if (operand1->name == '@') free(operand1);
            }
        }
        math++;
    }

    matrix_sf* result = *(stack + top);
    result->name = name;
    free(original);
    free(stack);
    return result;
}

bst_sf *find_node(bst_sf* root, matrix_sf* matrix) {
    if (root == NULL) return NULL;
    if (root->mat == matrix) return root;
    bst_sf *left_child = find_node(root->left_child, matrix);
    bst_sf *right_child = find_node(root->right_child, matrix);
    if (left_child != NULL) {
        return left_child;
    }
    if (right_child != NULL) {
        return right_child;
    }
    return NULL;
}

matrix_sf *execute_script_sf(char *filename) { //6
    FILE *fp = fopen(filename, "r");
    char *line = malloc(MAX_LINE_LEN);
    size_t max_line_size = MAX_LINE_LEN;
    bst_sf* root = NULL;
    matrix_sf* result;
    while(getline(&line, &max_line_size, fp) != -1) {
        char* pointer = line;
        char name = *pointer;
        pointer++;
        while(*pointer == ' ' || *pointer == '=') pointer++;
        if (isdigit(*pointer)) {
            result = create_matrix_sf(name, pointer);
        } else {
            result = evaluate_expr_sf(name, pointer, root);
        }
        root = insert_bst_sf(result, root);
    }
    free(line);
    if (root != NULL && result != NULL) {
        bst_sf *n = find_node(root, result);
        if (n != NULL) n->mat = NULL;
    }
    fclose(fp);
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
