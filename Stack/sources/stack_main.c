#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <limits.h>

#include "stack_functions.h"

enum result
{
    PASSED,
    FAILED
};
//===========================Dump===========================
void dump ( stack_t* stack )
{
    if ( stack == NULL )
    {
        fprintf( stdout, "stack: NULL\n");
        return;
    }
    Iterator itr;
    init( &itr, stack );
    fprintf( stdout, "stack:\n");

    while ( isEnd( &itr ) != 1 )
    {
        fprintf( stdout, "| %d", *current( &itr ) );
        next( &itr );
    }
    fprintf( stdout, " |\n" );
}
//============================Test 1========================
int ok_ctor_test()
{
    stack_t* new_stack = stack_ctor( 100 );

    if ( ( new_stack == NULL ) ||
         ( new_stack->stack_arr == NULL ) )
    {
        fprintf( stderr, "    \033[31mFAILED\033[0m -- ok_ctor_test, line %d\n", __LINE__ );
        return FAILED;
    }

    fprintf( stderr, "    \033[32mPASSED\033[0m -- ok_ctor_test\n" );
    stack_dtor( new_stack );
    return PASSED;
}
//============================Test 2========================
int crash_ctor_and_push_test()
{
    struct rlimit new_rlimit, old_rlimit;

    getrlimit( RLIMIT_AS, &old_rlimit);
    new_rlimit.rlim_cur = 23;
    new_rlimit.rlim_max = old_rlimit.rlim_max;

    if ( setrlimit( RLIMIT_AS, &new_rlimit ) == -1 )
    {
        perror("    \033[33mERROR\033[0m: -- setrlimit in crash_ctor_test");
        return ERROR;
    }

    stack_t* new_stack = NULL;
    new_stack = stack_ctor(10);

    if ( new_stack != NULL )
    {
        fprintf( stderr, "    \033[31mFAILED\033[0m -- crash_ctor_test, line %d\n", __LINE__ );
        stack_dtor ( new_stack );
        return FAILED;
    }

    if ( setrlimit( RLIMIT_AS, &old_rlimit ) == -1 )
    {
        perror("    \033[33mERROR\033[0m: -- setrlimit in crash_ctor_test");
        return ERROR;
    }

    new_stack = stack_ctor( 32768 );
    stack_t* old_stack = NULL;

    while ( push( new_stack, 666 ) != ERROR )
    {
        new_stack->size = new_stack->max_size;
        old_stack = new_stack;
    }

    if ( push( new_stack, 666 ) != ERROR || old_stack != new_stack )
    {
        fprintf( stderr, "    \033[31mFAILED\033[0m -- crash_ctor_test, line %d\n", __LINE__ );
        stack_dtor ( new_stack );
        return FAILED;
    }

    stack_dtor( new_stack );
    new_stack = NULL;
    old_stack = NULL;

    new_stack = stack_ctor( INT_MAX );

    if ( new_stack != NULL && new_stack->stack_arr != NULL )
    {
        fprintf( stderr, "    \033[31mFAILED\033[0m -- crash_ctor_test, line %d\n", __LINE__ );
        stack_dtor ( new_stack );
        return FAILED;
    }

    fprintf( stderr, "    \033[32mPASSED\033[0m -- crash_ctor_test\n" );
    return PASSED;
}
//============================Test 3========================
int resize_test()
{
    stack_t* stack = stack_ctor(1000);
    size_t old_max_size = stack->max_size;
    resize( stack, 0 );

    if ( ( stack == NULL ) ||
         ( stack->max_size != old_max_size ) )
    {
        stack_dtor( stack );
        fprintf( stderr, "    \033[31mFAILED\033[0m -- resize_test, line %d\n", __LINE__ );
        return FAILED;
    }

    int res = resize( stack, 300 );

    if ( ( stack == NULL ) ||
         ( res != ERROR && stack->max_size != 300 ) )
    {
        stack_dtor( stack );
        fprintf( stderr, "    \033[31mFAILED\033[0m -- resize_test, line %d\n", __LINE__ );
        return FAILED;
    }
    int i = 0;
    resize( stack, 200 );
    for ( i = 0; i < 200; i++ )
        push( stack, ( data_t ) i );
//    dump( stack );

    resize( stack, 10 );
//    dump( stack );

    resize( stack, 200 );
//    dump( stack );

    res = resize ( stack, INT_MAX );

    if ( ( stack == NULL ) ||
         ( res != ERROR ) )
    {
        stack_dtor( stack );
        fprintf( stderr, "    \033[31mFAILED\033[0m -- resize_test, line %d\n", __LINE__ );
        return FAILED;
    }

    stack_dtor(stack );
    stack = NULL;

    if ( resize ( stack, 100 ) != ERROR )
    {
        fprintf( stderr, "    \033[31mFAILED\033[0m -- resize_test, line %d\n", __LINE__ );
        return FAILED;
    }

    fprintf( stderr, "    \033[32mPASSED\033[0m -- resize_test\n" );
    return PASSED;
}
//============================Test 4========================
int push_test()
{
    stack_t*  stack = stack_ctor(1000);
    size_t old_max_size = stack->max_size;
    int i = 0;

    for ( i = 0; i < 100; i++ )
        push( stack, ( data_t ) i );

    for ( i = 0; i < 100; i++ )
    {
        if ( stack->stack_arr[i] != i )
        {
            stack_dtor( stack );
            fprintf( stderr, "    \033[31mFAILED\033[0m -- push_test, line %d\n", __LINE__ );
            return FAILED;
        }
    }

    if ( stack->size != 100 || stack->max_size != old_max_size || stack->size < 0 || stack->size > stack->max_size )
    {
        stack_dtor( stack );
        fprintf( stderr, "    \033[31mFAILED\033[0m -- push_test, line %d\n", __LINE__ );
        return FAILED;
    }

    int returned = 0;

    for ( i = 0; i < old_max_size; i++ )
        returned = push( stack, ( data_t ) i );

    if ( ( stack->max_size != 2 * old_max_size ) && ( returned != ERROR ) )
    {
        stack_dtor( stack );
        fprintf( stderr, "    \033[31mFAILED\033[0m -- push_test, line %d\n", __LINE__ );
        return FAILED;
    }

    stack_dtor( stack );
    stack = NULL;

    if ( push( stack, 12 ) != ERROR )
    {
        fprintf( stderr, "    \033[31mFAILED\033[0m -- push_test, line %d\n", __LINE__ );
        return FAILED;
    }

    fprintf( stderr, "    \033[32mPASSED\033[0m -- push_test\n" );
    return PASSED;
}
//============================Test 5========================
int pop_test()
{
    stack_t*  stack = stack_ctor(1000);

    if ( pop( stack ) != NULL )
    {
        stack_dtor( stack );
        fprintf( stderr, "    \033[31mFAILED\033[0m -- pop_test, line %d\n", __LINE__ );
        return FAILED;
    }

    int i = 0;

    for ( i = 0; i < 100; i++ )
        stack->stack_arr[i] = (data_t) i;

     stack->size = 100;
     data_t* got = NULL;

     for ( i = 99;  i >= 0; i-- )
     {
        got = pop( stack );

        if ( ( int ) *got != i || stack->size != i )
        {
            stack_dtor( stack );
            fprintf( stderr, "    \033[31mFAILED\033[0m -- pop_test, line %d\n", __LINE__ );
            return FAILED;
        }
     }

    stack_dtor( stack );
    stack = NULL;

    if ( pop( stack ) != NULL )
    {
        fprintf( stderr, "    \033[31mFAILED\033[0m -- pop_test, line %d\n", __LINE__ );
        return FAILED;
    }

    fprintf( stderr, "    \033[32mPASSED\033[0m -- pop_test\n" );
    return PASSED;
}
//============================Test 6========================
int peek_test()
{
    stack_t*  stack = stack_ctor(1000);
    int i = 0;

    for ( i = 0; i < 100; i++ )
        push( stack, ( data_t ) i );

     data_t* touched = NULL;

     for ( i = 99;  i >= 0; i-- )
     {
        touched = peek( stack );

        if ( (int) *touched != i || ( stack->size -1 ) != i )
        {
            fprintf( stderr, "    \033[31mFAILED\033[0m -- peek_test, line %d\n", __LINE__ );
            stack_dtor( stack );
            return FAILED;
        }
        stack->size--;
     }

    stack_dtor ( stack );
    stack = NULL;

    if ( peek( stack ) != NULL )
    {
        fprintf( stderr, "    \033[31mFAILED\033[0m -- peek_test, line %d\n", __LINE__ );
        return FAILED;
    }

    fprintf( stderr, "    \033[32mPASSED\033[0m -- peek_test\n" );
    return PASSED;
}
//=============================Test 7======================
int iterator_test() {

    stack_t*  stack = stack_ctor(100);
    data_t* tmp = NULL;
    Iterator itr;
    init( &itr, stack );
    tmp = getFirst( &itr );

    if ( getFirst( &itr ) != NULL )
    {
        fprintf( stderr, "    \033[31mFAILED\033[0m -- iterator_test, line %d\n", __LINE__ );
        stack_dtor( stack );
        return FAILED;
    }

    int i = 0;

    init( &itr, NULL );
    init( NULL, stack );

    isEnd( NULL );
    current( NULL );
    getFirst( NULL );
    next( NULL );

    for ( i = 0; i < 15; i++ )
        push( stack, i );

    i--;
    init( &itr, stack );

    while ( !isEnd( &itr ) )
    {
        tmp = current( &itr );

        if ( i != (int) *tmp )
        {
            fprintf( stderr, "    \033[31mFAILED\033[0m -- iterator_test, line %d\n", __LINE__ );
            stack_dtor( stack );
            return FAILED;
        }

        i--;
        next( &itr );
    }

    next( &itr );

    if ( !isEnd( &itr ) )
    {
        fprintf( stderr, "    \033[31mFAILED\033[0m -- iterator_test, line %d\n", __LINE__ );
        stack_dtor( stack );
        return FAILED;
    }

    tmp = getFirst( &itr );

    if ( (int) *tmp != 14 )
    {
        fprintf( stderr, "    \033[31mFAILED\033[0m -- iterator_test, line %d\n", __LINE__ );
        stack_dtor( stack );
        return FAILED;
    }

    fprintf( stderr, "    \033[32mPASSED\033[0m -- iterator_test\n" );
    stack_dtor( stack );
    return PASSED;
}
//=========================================================
#define test_report( param )  \
do                            \
{                             \
    switch ( param )          \
    {                         \
        case PASSED:          \
            ++passed;         \
            break;            \
        case FAILED:          \
            ++failed;         \
            break;            \
        default:              \
            ++errors;         \
    }                         \
} while ( 0 );
//=========================================================
int main()
{
    size_t passed = 0;
    size_t failed = 0;
    size_t errors = 0;
    int test = 0;

    fprintf( stderr, "============UNIT TEST RESULTS===============\n" );

    test = crash_ctor_and_push_test();
    test_report( test );

    test = resize_test();
    test_report( test );

    test = push_test();
    test_report( test );

    test = pop_test();
    test_report( test );

    test = peek_test();
    test_report( test );

    test = ok_ctor_test();
    test_report( test );

    test = iterator_test();
    test_report( test );

    fprintf( stderr, "============UNIT TEST SUMMARY=============\n"
                                    "    \033[32mPASSED\033[0m: %ld \n"
                                    "    \033[31mFAILED\033[0m: %ld  \n"
                                    "    \033[33mERROR \033[0m: %ld  \n"
                                    "==========================================\n",
                                     passed, failed, errors );

    return 0;
}
