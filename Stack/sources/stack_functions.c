#include <stdlib.h>
#include <string.h>

#include "stack_functions.h"

//===========================================================
#define check_invalid( stack_ptr )                          \
       ( ( stack_ptr == NULL ) ||                          \
         ( stack_ptr->stack_arr == NULL ) ||                \
         ( stack_ptr->size < 0 ) ||                         \
         ( stack_ptr->size > stack_ptr->max_size ) )        \
//===========================================================
stack_t* stack_ctor ( size_t size )
{
    stack_t* new_stack = ( stack_t* ) malloc ( sizeof( stack_t ) );

    if ( new_stack == NULL )
        return NULL;

    new_stack->stack_arr = NULL;
	new_stack->stack_arr = ( data_t* ) malloc ( size * sizeof( data_t ) );

	if ( new_stack->stack_arr == NULL )
	{
        free( new_stack );
        return NULL;
    }

    new_stack->size = 0;
    new_stack->max_size = size;
    return new_stack;
}
//===========================================================
int resize ( stack_t* stack, size_t new_max_size )
{
    if ( check_invalid( stack ) ) /// Stack is spoiled (impossible to operate)
        return ERROR;

    if ( new_max_size == 0 ) /// Do nothing in this case
        return OK;

    data_t* oldStack = stack->stack_arr;
    stack->stack_arr = ( data_t* ) realloc ( stack->stack_arr, sizeof( data_t ) * new_max_size );

    if ( stack->stack_arr == NULL )
    {
        stack->stack_arr = oldStack;
        return ERROR;
    }

    if ( stack->size > new_max_size ) /// Cuts down all the rest of the stack
        stack->size = new_max_size;

    stack->max_size = new_max_size;
    return OK;
}
//========================================================
int push ( stack_t* stack, data_t value )
{
    if ( check_invalid( stack ) ) /// Stack is spoiled (impossible to operate)
        return ERROR;

    if ( stack->size == stack->max_size )
    {
       if ( resize( stack, stack->max_size * 2 ) == ERROR )
       {
            return ERROR;
       }
    }

    memcpy( stack->stack_arr + stack->size, &value, sizeof( data_t ) );
    stack->size++;
    return OK;
}
//======================================================
data_t* peek ( const stack_t* stack )
{
    if ( check_invalid( stack ) ) /// Stack is spoiled (impossible to operate)
        return NULL;

    if ( stack->size == 0 )  /// Empty
    {
        return NULL;
    }

    return &stack->stack_arr[stack->size - 1];
}
//======================================================
data_t* pop ( stack_t* stack )
{
    if ( check_invalid( stack ) ) /// Invalid
        return NULL;

    if ( stack->size == 0 )  /// Empty
    {
        return NULL;
    }

    stack->size--;
    return &stack->stack_arr[stack->size];
}

//======================================================
void stack_dtor( stack_t* stack )
{
    if ( stack != NULL )
    {
        free( stack->stack_arr );
        free( stack );
    }
}
//======================================================
int init ( Iterator* itr, const stack_t* stack )
{
    if ( itr == NULL || stack == NULL )
        return ERROR;

    itr->cur = stack->stack_arr + stack->size - 1;
    itr->end_ptr = stack->stack_arr - 1;
    itr->first_ptr = stack->stack_arr + stack->size - 1;
    return OK;
}
//======================================================
int isEnd ( Iterator* itr )
{
    if ( itr == NULL )
        return ERROR;

    if ( itr->end_ptr == itr->cur )
        return 1; /// true
    else
        return 0; /// false
}
//======================================================
data_t* getFirst ( Iterator* itr )
{
    if ( itr == NULL )
        return NULL;

    if ( itr->end_ptr == itr->first_ptr )
        return NULL;

    return itr->first_ptr;
}
//======================================================
int next ( Iterator* itr )
{
    if ( itr == NULL )
    return ERROR;

    if ( isEnd( itr ) == 1 )
        return OK;

    itr->cur--;
    return OK;
}
//======================================================
data_t* current ( Iterator* itr )
{
    if ( itr == NULL )
    return NULL;

    return itr->cur;
}
//=======================================================
