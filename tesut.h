/*
 *      /\^_.  o
 *  ^  ;    / ~~~
 * ( ( /   |      tesUt -  The Embedded System Unit Testing Framework
 *  \_)__)_b
 */
/*
  Copyright (c) 2016 kitune-san

  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

     1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.

     2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.

     3. This notice may not be removed or altered from any source
     distribution.

 */
/*******************************************************************
 * Options
 *******************************************************************/
#include <stddef.h>
/* tesUt do not use setjmp/longjmp, if comment out this statement. */
#define TESU_USE_LONGJMP

/*******************************************************************
 * Defines
 *******************************************************************/
#ifndef NULL
	#define TESU_NULL ((void *)0)
#else
	#define TESU_NULL NULL
#endif

/*******************************************************************
 * Typedefines
 *******************************************************************/
/* Bool */
#define TESU_FALSE 0
#define TESU_TRUE  1
typedef int TESU_BOOL;

/* Test/Assert Result */
typedef enum {
	TESU_PASSED,
	TESU_FAILED,
	TESU_SKIP
} TESU_RESULT;

/* Error Code */
typedef enum {
	TESU_NO_ERROR,
	TESU_CALLED_FROM_HANDLER,
	TESU_NO_RUN_SUITE,
	TESU_DOUBLE_RUN_SUITE,
	TESU_SUITE_TEST_IS_NULL,
} TESU_ERROR;

/* Test Profile */
typedef struct {
	void (*test_func)(void);
	const char *message;
	TESU_BOOL fatal;
} TESU_TEST;
#define TESU_TEST_INFO_NULL {NULL, "", TESU_FALSE}

/* Fatal Flag */
#define TESU_FATAL    TESU_TRUE
#define TESU_NO_FATAL TESU_FALSE

/* Error Handler Argument */
typedef struct {
	int line;
	const char *file_name;
	TESU_ERROR code;
} TESU_ERROR_HANDLER_ARGS;

/* Handler Argument */
typedef struct {
	int suite_num;
	int test_num;
	int assert_num;
	TESU_RESULT result;
	int line;
	const char *file_name;
	const char *message;
} TESU_HANDLER_ARGS;

/* Interface functions */
typedef void TESU_HANDLER(const TESU_HANDLER_ARGS *args);
typedef void TESU_ERROR_HANDLER(const TESU_ERROR_HANDLER_ARGS *args);

/*******************************************************************
 * Functions (Global)
 *******************************************************************/
void TESU_Initialize(void);

void TESU_Set_User_Suite_Handler(TESU_HANDLER *Suite_Handler);

void TESU_Set_User_Test_Handler(TESU_HANDLER *Test_Handler);

void TESU_Set_User_Assert_Handler(TESU_HANDLER *Assert_Handler);

void TESU_Set_User_Error_Handler(TESU_ERROR_HANDLER *Error_Handler);

void TESU_Start_Suite_Implementation(const TESU_TEST *test_list,
                                     const char *file_name,
                                     const int line,
                                     const char *message,
                                     void (*set_up)(void),
                                     void (*tear_down)(void));

TESU_BOOL TESU_Assert_Implementation(const TESU_BOOL value,
                                     const char *file_name,
                                     const int line,
                                     const char *message,
                                     const TESU_BOOL fatal);


int Get_Summary_Test_Total(void);
int Get_Summary_Test_Ran(void);
int Get_Summary_Test_Passed(void);
int Get_Summary_Test_Failed(void);

int Get_Summary_Assert_Total(void);
int Get_Summary_Assert_Ran(void);
int Get_Summary_Assert_Passed(void);
int Get_Summary_Assert_Failed(void);

/*******************************************************************
 * Test Function macros
 *******************************************************************/
#define TESU_Start_Suite(TEST_LIST, SETUP_FUNC, TEAR_DOWN_FUNC, MESSAGE) \
	do { \
		TESU_Start_Suite_Implementation(TEST_LIST, __FILE__, __LINE__, MESSAGE, SETUP_FUNC, TEAR_DOWN_FUNC); \
	} while(0)


#define TESU_CALL_ASSERT(VALUE, MESSAGE, FATAL_FLUG) \
	do { \
		if (TESU_FALSE != TESU_Assert_Implementation((VALUE), __FILE__, __LINE__, MESSAGE, FATAL_FLUG)) { \
			return; /* Fatal return. (do not use setjmp/longjmp.) */ \
		} \
	} while(0)

/* Assert Macros */
#define TESU_ASSERT(VALUE) \
        TESU_CALL_ASSERT((VALUE), #VALUE,  TESU_NO_FATAL)

#define TESU_ASSERT_MESSAGE(VALUE, MESSAGE) \
        TESU_CALL_ASSERT((VALUE), MESSAGE, TESU_NO_FATAL)

#define TESU_ASSERT_FATAL(VALUE) \
        TESU_CALL_ASSERT((VALUE), #VALUE,  TESU_FATAL)

#define TESU_ASSERT_FATAL_MESSAGE(VALUE, MESSAGE) \
        TESU_CALL_ASSERT((VALUE), MESSAGE, TESU_FATAL)

#define TESU_TEST(VALUE) \
        TESU_CALL_ASSERT((VALUE), #VALUE,  TESU_NO_FATAL)

#define TESU_TEST_FATAL(VALUE, MESSAGE) \
        TESU_CALL_ASSERT((VALUE), MESSAGE, TESU_FATAL)

#define TESU_FAIL(MESSAGE) \
        TESU_ASSERT_MESSAGE(TESU_FALSE, "TESU_FAIL(" #MESSAGE ")")

#define TESU_FAIL_FATAL(MESSAGE) \
        TESU_ASSERT_FATAL_MESSAGE(TESU_FALSE, "TESU_FAIL_FATAL(" #MESSAGE ")")

#define TESU_ASSERT_TRUE(VALUE) \
        TESU_ASSERT_MESSAGE((VALUE), "TESU_ASSERT_TRUE(" #VALUE ")")

#define TESU_ASSERT_TRUE_FATAL(VALUE) \
        TESU_ASSERT_FATAL_MESSAGE((VALUE), "TESU_ASSERT_TRUE_FATAL(" #VALUE ")")

#define TESU_ASSERT_FALSE(VALUE) \
        TESU_ASSERT_MESSAGE(!(VALUE), "TESU_ASSERT_FALSE(" #VALUE ")")

#define TESU_ASSERT_FALSE_FATAL(VALUE) \
        TESU_ASSERT_FATAL_MESSAGE(!(VALUE), "TESU_ASSERT_FALSE_FATAL(" #VALUE ")")

#define TESU_ASSERT_EQUAL(ACTUAL, EXPECTED) \
        TESU_ASSERT_MESSAGE(((ACTUAL) == (EXPECTED)), "TESU_ASSERT_EQUAL(" #ACTUAL "," #EXPECTED ")")

#define TESU_ASSERT_EQUAL_FATAL(ACTUAL, EXPECTED) \
        TESU_ASSERT_FATAL_MESSAGE(((ACTUAL) == (EXPECTED)), "TESU_ASSERT_EQUAL_FATAL(" #ACTUAL "," #EXPECTED ")")

#define TESU_ASSERT_NOT_EQUAL(ACTUAL, EXPECTED) \
        TESU_ASSERT_MESSAGE(((ACTUAL) != (EXPECTED)), "TESU_ASSERT_NOT_EQUAL(" #ACTUAL "," #EXPECTED ")")

#define TESU_ASSERT_NOT_EQUAL_FATAL(ACTUAL, EXPECTED) \
        TESU_ASSERT_FATAL_MESSAGE(((ACTUAL) != (EXPECTED)), "TESU_ASSERT_NOT_EQUAL_FATAL(" #ACTUAL "," #EXPECTED ")")

#define TESU_ASSERT_PTR_EQUAL(ACTUAL, EXPECTED) \
        TESU_ASSERT_MESSAGE(((const void*)(ACTUAL) == (const void*)(EXPECTED)), "TESU_ASSERT_PTR_EQUAL(" #ACTUAL "," #EXPECTED ")")

#define TESU_ASSERT_PTR_EQUAL_FATAL(ACTUAL, EXPECTED) \
        TESU_ASSERT_FATAL_MESSAGE(((const void*)(ACTUAL) == (const void*)(EXPECTED)), "TESU_ASSERT_PTR_EQUAL_FATAL(" #ACTUAL "," #EXPECTED ")")

#define TESU_ASSERT_PTR_NOT_EQUAL(ACTUAL, EXPECTED) \
        TESU_ASSERT_MESSAGE(((const void*)(ACTUAL) != (const void*)(EXPECTED)), "TESU_ASSERT_PTR_NOT_EQUAL(" #ACTUAL "," #EXPECTED ")")

#define TESU_ASSERT_PTR_NOT_EQUAL_FATAL(ACTUAL, EXPECTED) \
        TESU_ASSERT_FATAL_MESSAGE(((const void*)(ACTUAL) != (const void*)(EXPECTED)), "TESU_ASSERT_PTR_NOT_EQUAL_FATAL(" #ACTUAL "," #EXPECTED ")")

#define TESU_ASSERT_PTR_NULL(VALUE) \
        TESU_ASSERT_MESSAGE((TESU_NULL == (const void*)(VALUE)), "TESU_ASSERT_PTR_NULL(" #VALUE ")")

#define TESU_ASSERT_PTR_NULL_FATAL(VALUE) \
        TESU_ASSERT_FATAL_MESSAGE((TESU_NULL == (const void*)(VALUE)), "TESU_ASSERT_PTR_NULL_FATAL(" #VALUE ")")

#define TESU_ASSERT_PTR_NOT_NULL(VALUE) \
        TESU_ASSERT_MESSAGE((TESU_NULL != (const void*)(VALUE)), "TESU_ASSERT_PTR_NOT_NULL(" #VALUE ")")

#define TESU_ASSERT_PTR_NOT_NULL_FATAL(VALUE) \
        TESU_ASSERT_FATAL_MESSAGE((TESU_NULL != (const void*)(VALUE)), "TESU_ASSERT_PTR_NOT_NULL_FATAL(" #VALUE ")")


