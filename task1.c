#include "Task1.h" // <-- Includes the Stack definition

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// --- Private Helper Function (for portability) ---
/**
 * @brief Reverses a string in-place. (Replaces non-standard strrev)
 */
static char* my_strrev(char* str) {
    if (!str || !*str) return str;
    int i = 0;
    int j = strlen(str) - 1;
    char temp;
    while (i < j) {
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
    return str;
}

// --- Stack Function Implementations ---

Stack *createStack(int capacity)
{
    Stack *stack = (Stack *)malloc(sizeof(Stack));
    if (!stack)
        return NULL;
    stack->data = (char **)malloc(sizeof(char *) * capacity);
    if (!stack->data)
    {
        free(stack);
        return NULL;
    }
    stack->capacity = capacity;
    stack->top = -1;
    return stack;
}

int isEmpty(Stack *stack)
{
    return stack->top == -1;
}

void push(Stack *stack, const char *item)
{
    if (stack->top >= stack->capacity - 1)
    {
        fprintf(stderr, "Stack overflow error\n");
        return;
    }
    stack->data[++stack->top] = strdup(item);
    if (stack->data[stack->top] == NULL)
    {
        fprintf(stderr, "Memory allocation failed for push\n");
        stack->top--;
    }
}

char *pop(Stack *stack)
{
    if (isEmpty(stack))
    {
        return NULL;
    }
    return stack->data[stack->top--];
}

void freeStack(Stack *stack)
{
    if (stack)
    {
        while (!isEmpty(stack))
        {
            free(pop(stack));
        }
        free(stack->data);
        free(stack);
    }
}
// --- End of Stack Implementation ---


// --- Private Helper Functions ---

static int isOperator(char ch)
{
    return ch == '~' || ch == '*' || ch == '+' || ch == '>';
}

static int isOperandStart(char ch)
{
    // We assume variables are like 'x1', 'x24', or single letters 'a', 'b'
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}


// --- Public Function Implementations ---

char *read_line(FILE *stream)
{
    size_t capacity = 1024;
    char *buffer = (char *)malloc(capacity);
    if (!buffer)
        return NULL;

    size_t i = 0;
    int c;

    while ((c = fgetc(stream)) != EOF && c != '\n')
    {
        buffer[i++] = (char)c;
        if (i >= capacity - 1)
        {
            capacity *= 2;
            char *new_buffer = (char *)realloc(buffer, capacity);
            if (!new_buffer)
            {
                free(buffer);
                return NULL;
            }
            buffer = new_buffer;
        }
    }
    buffer[i] = '\0';
    return buffer;
}

Stack *task1_infixToPrefix(const char *infix)
{
    int n = strlen(infix);
    if (n == 0)
        return NULL;

    Stack *op_stack = createStack(n);
    Stack *final_stack = createStack(n);
    if (!op_stack || !final_stack)
    {
        freeStack(op_stack);
        freeStack(final_stack);
        return NULL;
    }

    char token_buffer[128]; 

    for (int i = n - 1; i >= 0; i--)
    {
        char c = infix[i];

        if (isspace(c))
        {
            continue;
        }
        else if (isdigit(c) || isOperandStart(c))
        {
            int k = 0;
            while (i >= 0 && (isOperandStart(infix[i]) || isdigit(infix[i])))
            {
                if (k < 127)
                {
                    token_buffer[k++] = infix[i--];
                }
                else
                {
                    i--;
                }
            }
            i++;
            token_buffer[k] = '\0';

            my_strrev(token_buffer); // Use portable reverse
            
            push(final_stack, token_buffer);
        }
        else if (c == ')')
        {
            push(op_stack, ")");
        }
        // --- 3. Handle '(' ---
        // *** THIS BLOCK IS NOW FIXED ***
        else if (c == '(')
        {
            char *op = NULL;
            // Pop operators until we find the matching ')'
            while ((op = pop(op_stack)) != NULL) 
            {
                if (strcmp(op, ")") == 0) 
                {
                    // Found the matching parenthesis
                    free(op); // Free the ")"
                    break;    // Stop popping
                }
                
                // It wasn't ")", so push the operator to the final stack
                push(final_stack, op);
                free(op); // We're done with this operator token
            }
            // 'op' is either NULL or was already freed.
            // No final 'free(op)' is needed.
        }
        // --- END OF FIX ---
        else if (isOperator(c))
        {
            token_buffer[0] = c;
            token_buffer[1] = '\0';
            push(op_stack, token_buffer);
        }
    }

    // --- 5. Pop remaining operators ---
    char *op;
    while ((op = pop(op_stack)) != NULL)
    {
        push(final_stack, op);
        free(op);
    }
    freeStack(op_stack);

// --- 6. Return the final stack ---
    // The `final_stack` has the prefix expression in reverse order,
    // which is exactly what prefixToTree expects to pop from.
    // We do NOT reverse it.
    
    // We free the op_stack, but we return the final_stack.
    freeStack(op_stack); 
    return final_stack;

}

// --- NO MAIN FUNCTION ---