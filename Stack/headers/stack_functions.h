#ifndef STACK_FUNCTIONS_H
#define STACK_FUNCTIONS_H
enum return_codes
{
    OK = 0,
    ERROR = -1
};

//==================================================================

/** \typedef data_t
    \brief   Type of elements stored in data structure
*/
typedef int data_t;

/** \typedef stack_t
    \brief   A sample of stack

    \param  stack_arr    Stack pointer
    \param  size         Current number of elements in stack
    \param  max_size     The limit for size(without new allocation)
*/
typedef struct STACK_t
{
    data_t* stack_arr;
    size_t  size;
    size_t  max_size;
} stack_t;

/** \struct Iterator

    \param  first_ptr  Points at the first element(first to pop)
    \param  end_ptr    Points at the place behind the last element
    \param  cur        Points at the current element
*/

typedef struct Iterator
{
    data_t* first_ptr;
    data_t* cur;
    data_t* end_ptr;

}   Iterator;
//==================================================================
/*! \fn     stack_t* stack_ctor ( size_t size );
    \brief  Initializes new stack structure.

    \return Stack pointer or NULL(in case of an error, and if size is equal to 0)
*/
stack_t* stack_ctor ( size_t size );
//==================================================================
/*! \fn     int resize ( stack_t* stack, size_t new_max_size );
    \brief  Expands and contracts the stack.

            If new_max_size is equal to 0, this function does nothing
            and returns 0(SUCCESS) in case of valid state of the stack.

    \param  stack           Stack pointer
    \param  new_max_size    New maximal size of stack

    \return 0(SUCCESS) or -1(ERROR)
*/
int resize ( stack_t* stack, size_t new_max_size );
//==================================================================
/*! \fn     int push ( stack_t* stack, data_t value );
    \brief  Pushes the value onto the stack.

    \param  stack       Stack pointer
    \param  value       Value to be pushed

    \return 0(SUCCESS) or -1(ERROR)
*/
int push ( stack_t* stack, data_t value );
//==================================================================
/*! \fn     data_t* pop ( stack_t* stack );
    \brief  Gets the last pushed object reducing
            the effictive size of the stack by 1 element

    \param  stack       Stack pointer

    \return Pointer to the data instance, or NULL
*/
data_t* pop ( stack_t* stack );
//==================================================================
/*! \fn     data_t* peek ( const stack_t* stack );
    \brief  Gets the next object, but does not remove it from the stack

    \param  stack    Stack pointer

    \return Pointer to the data instance, or NULL
*/
data_t* peek ( const stack_t* stack );
//==================================================================
/*! \fn     int init ( Iterator* itr, const stack* stack );
    \brief  Attaches existing Iterator to specified stack

    \param  itr      Iterator pointer
    \param  stack    Stack pointer

    \return 0(SUCCESS) or -1(ERROR)
*/
int init ( Iterator* itr, const stack_t* stack );
//==================================================================
/*! \fn     int isEnd ( Iterator* itr );
    \brief  Checks if it is the end of the stack attached to this Iterator

    \param  itr    Pointer to the iterator structure

    \return Return 1(true) if it is the end. Otherwise, it returns 0(false).
*/
int isEnd ( Iterator* itr );
//==================================================================
/*! \fn     data_t* getFirst ( Iterator* itr );
    \brief  Gets the pointer of the last pushed object in the stack

    \param  stack    Stack pointer

    \return Pointer to the data instance or NULL
*/
data_t* getFirst ( Iterator* itr );
//==================================================================
/*! \fn     int next ( Iterator* itr );
    \brief  Moves the iterator to the next element of the stack if it is not the last element

    \param  itr    Pointer to the iterator structure

    \return 0(SUCCESS) or -1(ERROR)
*/
int next ( Iterator* itr );
//==================================================================
/*! \fn     data_t* current ( Iterator* itr );
    \brief  Gets the pointer, but does not remove it from the stack

    \param  itr    Pointer to the iterator structure

    \return Pointer to the data instance or NULL
*/
data_t* current ( Iterator* itr );
//==================================================================
/*! \fn     void stack_dtor( stack_t* stack );
    \brief  Frees memomry occupied by the stack.

    \param  Stack    Stack pointer

    \return Does not have return value.
*/
void stack_dtor( stack_t* stack );
//==================================================================
#endif /* STACK_FUNCTIONS_H */
