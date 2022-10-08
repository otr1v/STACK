#include <stdio.h>
#include <stdlib.h>

typedef int elem_t;
typedef unsigned long long CANARY_TYPE;
#define TRY
#define HASH
typedef struct stack_type 
{
    CANARY_TYPE LEFT_MASTER;
    elem_t* data;
    size_t size;
    size_t capacity;
    int error;
    #ifdef DEBUG_INFO
        const char* func; 
        const char* file; 
        const char* name;
        int         line;
    #endif
    const char* name;
    CANARY_TYPE RIGHT_MASTER;
    #ifdef HASH
        unsigned int hash;
    #endif
} stack_type;
#define STACK_NAME(var)   \
    (#var);
#ifdef DEBUG_INFO
    #define stackCreator(stack, number_of_elems)        \
        stackCreator_(&stack, number_of_elems, #stack, __FILE__, __LINE__, __PRETTY_FUNCTION__);
    
#endif


stack_type* stackCreator_(stack_type* stack, int number_of_elems);
void stackPush(stack_type* stack, elem_t value);
const int* stackResize(stack_type* stack);
elem_t stackPop(stack_type* stack, int* err);
void stackDestroyer(stack_type* stack);
int stackError(stack_type* stack);
void stackDump(stack_type* stack, int* err,  const char* name, const int line, const char* function, const char* file);
int Hash(const char* str, int len, stack_type* stack);

const CANARY_TYPE LEFT_CANARY       = 0xBEADFACE;
const CANARY_TYPE RIGHT_CANARY      = 0xBAADF00D;
const CANARY_TYPE LEFT_DATA_CANARY  = 0xCAFEBABE;
const CANARY_TYPE RIGHT_DATA_CANARY = 0xDEADBABE;
const elem_t POISON = -2281337;
const int ZERO_ELEM_POP = -1;
const int* NO_MEMORY = (int*)0x666;

enum errCodes {
    STACK_NULL                = 1,
    STACK_DATA_NULL           = 1 << 1,
    BAD_POINTER               = 1 << 2,
    STACK_DELETED             = 1 << 3,
    DAMAGED_RIGHT_CANARY      = 1 << 4,
    DAMAGED_LEFT_CANARY       = 1 << 5,
    DAMAGED_LEFT_DATA_CANARY  = 1 << 6,
    DAMAGED_RIGHT_DATA_CANARY = 1 << 7
};


int main()
{
    stack_type stk = {};
    int* err = NULL;
    elem_t k = 5;
    elem_t d = 7;
    elem_t l = 8;
    stk.name = STACK_NAME(stk);
    stackCreator_(&stk, 5);
    stackPush(&stk, k);
    stackPush(&stk, l);
    stackPush(&stk, d);
    stackPush(&stk, d);
    stackPush(&stk, d);
    int sum_of_errors = stackError(&stk);
    stackDump(&stk, NULL, (stk.name), __LINE__, __PRETTY_FUNCTION__, __FILE__);
    return 0;
}

//=========================================================================================

stack_type* stackCreator_(stack_type* stack, int number_of_elems)
{
    if (stack -> data == NULL)
    {
        stack -> data = (elem_t *) calloc(sizeof(CANARY_TYPE) * 2 + number_of_elems * sizeof(elem_t) + 0, sizeof(char));

        if (stack -> data == NULL)
        {
            printf("can't give memory to create stack_data_array\n");
        }
      
        *(long long *)(stack -> data)                         = LEFT_DATA_CANARY;
        stack -> data = stack -> data + sizeof(long long) / sizeof(elem_t);
        *(long long *)(stack -> data + number_of_elems)       = RIGHT_DATA_CANARY;
        for (int i = 0; i < (stack -> capacity); i++)
        { 
            stack -> data[i] = POISON;
        }
        stack -> RIGHT_MASTER = RIGHT_CANARY;
        stack -> LEFT_MASTER  = LEFT_CANARY;

        stack -> error = 0;
        stack -> size = 0;
        stack -> capacity = number_of_elems;
        #ifdef DEBUG_INFO
            stack -> line = __LINE__;
            stack -> func = __PRETTY_FUNCTION__;
            stack -> file = __FILE__;
            stack -> name = name;
        #endif
    }
    stack -> error = stackError(stack);
    if (stack -> error != 0)
    {
        stackDump(stack, NULL, (stack -> name), __LINE__, __PRETTY_FUNCTION__, __FILE__);
    }
    return stack;
}

//============================================================================================

void stackPush(stack_type* stack, elem_t value)
{
    if (stack -> size >= stack -> capacity)
    {
        stackResize(stack);
    }
    stack -> data[stack -> size] = value;
    stack -> size++;
    stack -> error = stackError(stack);
    if (stack -> error != 0)
    {
        stackDump(stack, NULL, (stack -> name), __LINE__, __PRETTY_FUNCTION__, __FILE__);
    }
}

//=================================================================

const int* stackResize(stack_type* stack)
{
    stack -> capacity *= 2;
    stack -> data = stack -> data - sizeof(long long) / sizeof(elem_t);
    stack -> data = (elem_t*)realloc((stack -> data) , (stack -> capacity) * sizeof(elem_t) + 2 * sizeof(CANARY_TYPE) + 0);

    if (stack -> data == NULL)
    {
        printf("can't give memory to resize stack data array\n");
        return NO_MEMORY;
    }

    stack -> data = stack -> data + sizeof(long long) / sizeof(elem_t);
    *(long long *)(stack -> data + stack -> capacity) = RIGHT_DATA_CANARY;

    for (int i = (stack -> size); i < (stack -> capacity); i++)
    {
        stack -> data[i] = POISON;
    }
    stack -> error = stackError(stack);
    if (stack -> error != 0)
    {
        stackDump(stack, NULL, (stack -> name), __LINE__, __PRETTY_FUNCTION__, __FILE__);
    }
}

//======================================================================

elem_t stackPop(stack_type* stack, int *err)
{
    if ((stack -> size) < 1)
    {
        printf("pop when size is 0, try another time\n");
        *err = ZERO_ELEM_POP;
    }
    elem_t tmp = stack -> data[stack -> size];
    stack -> data[stack -> size] = POISON;
    stack -> size--;
    stack -> error = stackError(stack);
    if (stack -> error != 0)
    {
        stackDump(stack, err, (stack -> name), __LINE__, __PRETTY_FUNCTION__, __FILE__);
    }
    return tmp;
}

//=========================================================================

void stackDestroyer(stack_type* stack)
{
    stack -> size = -1;
    stack -> capacity = -228;
    free(stack -> data);
}

//=================================================================================

// int Hash(const char* str, int len, stack_type* stack)
// {
//     int len = *((&(stack -> RIGHT_MASTER)- &(stack -> LEFT_MASTER)))
// }

//====================================================================================

int stackError(stack_type* stack)
{
    int sum_of_errors = 0;
    if (stack == NULL)
    {
        sum_of_errors |= STACK_NULL;
    }
    if ((stack -> data) == NULL)
    {
        sum_of_errors |= STACK_DATA_NULL;
    }
    if ((stack -> size) > (stack -> capacity))
    {
        sum_of_errors |= BAD_POINTER;
    }
    if ((stack -> size) == -1)
    {
        sum_of_errors |= STACK_DELETED;
    }
    if ((stack -> RIGHT_MASTER) != RIGHT_CANARY)
    {
        sum_of_errors |= DAMAGED_RIGHT_CANARY;
    }
    if ((stack -> LEFT_MASTER) != LEFT_CANARY)
    {
        sum_of_errors |= DAMAGED_LEFT_CANARY;
    }
    if ((*(long long*)(stack -> data - sizeof(long long) / sizeof(elem_t))) != LEFT_DATA_CANARY)
    {
        sum_of_errors |= DAMAGED_LEFT_DATA_CANARY;
    }
    if ((*(long long*)(stack -> data + stack -> capacity)) != RIGHT_DATA_CANARY)
    {
        sum_of_errors |= DAMAGED_RIGHT_DATA_CANARY;
    }
    return sum_of_errors;
}

//===========================================================================

void stackDump(stack_type* stack, int* err, const char* name, const int line, const char* function, const char* file)
{
    FILE* fp = fopen("log.txt", "w");
    fprintf(fp, "capacity = %ld\n", stack -> capacity);
    fprintf(fp, "size = %ld\n", stack -> size);
    #ifdef TRY
        fprintf(fp, "stack has been called from: ");
        fprintf(fp, "line : %d, ", line);
        fprintf(fp, "function : %s, ", function);
        fprintf(fp, "file : %s, ", file);
        fprintf(fp, "name : %s, \n", name);
    #endif
    if (err != NULL)
    {
        fprintf(fp, "size when pop 0: %d \n", ZERO_ELEM_POP);
    }
    if (stack -> error == 0)
    {
        fprintf(fp, "all is ok\n");
    }
    else
    {
        for (int i = 0; i < (stack -> capacity); i++)
        {
            fprintf(fp, "data[%d] = %d\n", i, stack -> data[i]);
        }
        fprintf(fp, "%d\n", stack -> error);
        if (((stack -> error) & STACK_NULL) == STACK_NULL)
        {
            fprintf(fp, "your stack hasn't been created yet\n");
        } 
        if (((stack -> error) & STACK_DATA_NULL) == STACK_DATA_NULL)
        {
            fprintf(fp, "your stack data has NULL pointer\n");
        } 
        if (((stack -> error) & BAD_POINTER) == BAD_POINTER)
        {
            fprintf(fp, "size > capacity\n");
        } 
        if (((stack -> error) & STACK_DELETED) == STACK_DELETED)
        {
            fprintf(fp, "your stack hasn been already deleted\n");
        } 
        if (((stack -> error) & DAMAGED_RIGHT_CANARY) == DAMAGED_RIGHT_CANARY)
        {
            fprintf(fp, "your stack has been hacked(right canary has been damaged)\n");
        } 
        if (((stack -> error) & DAMAGED_LEFT_CANARY) == DAMAGED_LEFT_CANARY)
        {
            fprintf(fp, "your stack has been hacked(left canary has been damaged)\n");
            
        } 
        if (((stack -> error) & DAMAGED_LEFT_DATA_CANARY) == DAMAGED_LEFT_DATA_CANARY)
        {
            fprintf(fp, "your stack has been hacked(left data canary has been damaged)\n");
        } 
        if (((stack -> error) & DAMAGED_RIGHT_DATA_CANARY) == DAMAGED_RIGHT_DATA_CANARY)
        {
            fprintf(fp, "your stack has been hacked(right data canary has been damaged)\n");
        } 
    }
    fclose(fp);
}

