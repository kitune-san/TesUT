/*
 *      /\^_.  o
 *  ^  ;    / ~~~
 * ( ( /   |      tesUt -  The Embedded System Unit Testing Framework
 *  \_)__)_b      V0.1
 *
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
#include "tesut.h"

#ifdef TESU_USE_LONGJMP
#include <setjmp.h>
#endif

typedef struct {
	int total;
	int ran;
	int failure;
} TESU_RUN_SUMMARY;

/*******************************************************************
 * Static Definitions
 *******************************************************************/
/* Tests result summary. total/ran/failure/(success = ran-failure) */
static TESU_RUN_SUMMARY test_summary   = {0, 0, 0};
static TESU_RUN_SUMMARY assert_summary = {0, 0, 0};
static int suite_total = 0;

static TESU_BOOL is_assert_fatal      = TESU_FALSE;
static TESU_BOOL is_test_fatal        = TESU_FALSE;

/* Process Control Flags */
static TESU_BOOL is_suite_running_now = TESU_FALSE;
static TESU_BOOL is_lock_suite_test   = TESU_FALSE;

/* Result of one test */
static TESU_RESULT test_run_result    = TESU_FAILED;

/* Interface functions. Default is NULL. */
static TESU_HANDLER *User_Assert_Handler = TESU_NULL;
static TESU_HANDLER *User_Test_Handler   = TESU_NULL;
static TESU_HANDLER *User_Suite_Handler  = TESU_NULL;
static TESU_ERROR_HANDLER *User_Error_Handler = TESU_NULL;

/*******************************************************************
 * Functions (Initialize)
 *******************************************************************/
static void TESU_Clear_Test_Summary(void)
{
	test_summary.total   = 0;
	test_summary.ran     = 0;
	test_summary.failure = 0;
	return;
}

static void TESU_Clear_Assert_Summary(void)
{
	assert_summary.total   = 0;
	assert_summary.ran     = 0;
	assert_summary.failure = 0;
	return;
}

void TESU_Initialize(void)
{
	/* Reset user interface function pointers. */
	User_Assert_Handler = TESU_NULL;
	User_Test_Handler   = TESU_NULL;
	User_Suite_Handler  = TESU_NULL;
	User_Error_Handler  = TESU_NULL;

	/* Clear result counters */
	TESU_Clear_Test_Summary();
	TESU_Clear_Assert_Summary();
	suite_total = 0;

	/* Clear fatal flags */
	is_test_fatal   = TESU_FALSE;
	is_assert_fatal = TESU_FALSE;

	is_suite_running_now = TESU_FALSE;
	is_lock_suite_test   = TESU_FALSE;

	test_run_result   = TESU_FAILED;

	return;
}

/*******************************************************************
 * Functions (Summary Getter)
 *******************************************************************/
int Get_Summary_Test_Total(void)
{
	return test_summary.total;
}

int Get_Summary_Test_Ran(void)
{
	return test_summary.ran;
}

int Get_Summary_Test_Passed(void)
{
	return (test_summary.ran - test_summary.failure);
}

int Get_Summary_Test_Failed(void)
{
	return test_summary.failure;
}

int Get_Summary_Assert_Total(void)
{
	return assert_summary.total;
}

int Get_Summary_Assert_Ran(void)
{
	return assert_summary.ran;
}

int Get_Summary_Assert_Passed(void)
{
	return (assert_summary.ran - assert_summary.failure);
}

int Get_Summary_Assert_Failed(void)
{
	return assert_summary.failure;
}

/*******************************************************************
 * Functions (Regester User Handler)
 *******************************************************************/
void TESU_Set_User_Suite_Handler(TESU_HANDLER *Suite_Handler)
{
	/* Register User Suite Handler */
	User_Suite_Handler = Suite_Handler;
	return;
}

void TESU_Set_User_Test_Handler(TESU_HANDLER *Test_Handler)
{
	/* Register User Test Handler */
	User_Test_Handler = Test_Handler;
	return;
}

void TESU_Set_User_Assert_Handler(TESU_HANDLER *Assert_Handler)
{
	/* Register User Assert Handler */
	User_Assert_Handler = Assert_Handler;
	return;
}

void TESU_Set_User_Error_Handler(TESU_ERROR_HANDLER *Error_Handler)
{
	/* Register User Error Handler */
	User_Error_Handler = Error_Handler;
	return;
}

/*******************************************************************
 * Functions (Call User Handler)
 * *=MUST=*
 * Before call func : flag set   (is_lock_suite_test = TESU_TRUE)
 * After  call func : flag clear (is_lock_suite_test = TESU_FALSE)
 *******************************************************************/
static void TESU_Run_User_Func(void (*func)(void))
{
	if (TESU_NULL != func) {
		is_lock_suite_test = TESU_TRUE;
		(*func)();
		is_lock_suite_test = TESU_FALSE;
	}
}

static void TESU_Call_User_Handler(TESU_HANDLER *handler,
                                   const TESU_HANDLER_ARGS *args)
{
	if ((TESU_NULL != handler)
	 && (TESU_NULL != args)) {
		is_lock_suite_test = TESU_TRUE;
		(*handler)(args);
		is_lock_suite_test = TESU_FALSE;
	}
	return;
}

static void TESU_Call_User_Error_Handler(const int line, const char *file_name,
                                         const TESU_ERROR code)
{
	TESU_ERROR_HANDLER_ARGS args;

	/* Sent Error result to user Handler */
	args.line      = line;
	args.file_name = file_name;
	args.code      = code;

	if ((TESU_NULL  != User_Error_Handler)
	 && (TESU_FALSE == is_lock_suite_test)) {
		is_lock_suite_test = TESU_TRUE;
		(*User_Error_Handler)(&args);
		is_lock_suite_test = TESU_FALSE;
	}
	return;
}

static void TESU_Call_User_Assert_Handler(const int line,
                                          const char *file_name,
                                          const char *message,
                                          const TESU_RESULT assert_run_result)
{
	TESU_HANDLER_ARGS args;
	/* Sent assert result to user Handler */
	args.suite_num  = suite_total;
	args.test_num   = test_summary.total;
	args.assert_num = assert_summary.total;
	args.result     = assert_run_result;
	args.line       = line;
	args.file_name  = file_name;
	args.message    = message;
	TESU_Call_User_Handler(User_Assert_Handler, &args);
	return;
}

static void TESU_Call_User_Test_Handler(const char *message)
{
	TESU_HANDLER_ARGS args;
	/* Sent test result to user Handler */
	args.suite_num  = suite_total;
	args.test_num   = test_summary.total;
	args.assert_num = assert_summary.total;
	args.result     = test_run_result;
	args.line       = 0;
	args.file_name  = "";
	args.message    = message;
	TESU_Call_User_Handler(User_Test_Handler, &args);
	return;
}

static void TESU_Call_User_Suite_Handler(const int line,
                                         const char *file_name,
                                         const char *message)
{
	TESU_HANDLER_ARGS args;

	/* Sent Suite result to user Handler */
	args.suite_num  = suite_total;
	args.test_num   = test_summary.total;
	args.assert_num = assert_summary.total;
	args.result     = TESU_PASSED;
	args.line       = line;
	args.file_name  = file_name;
	args.message    = message;
	TESU_Call_User_Handler(User_Suite_Handler, &args);
	return;
}

/*******************************************************************
 * Functions (Assert)
 *******************************************************************/
static TESU_ERROR TESU_Check_Error_Assert(void)
{
	TESU_ERROR result;

	if (TESU_FALSE != is_lock_suite_test) {
		result = TESU_CALLED_FROM_HANDLER;
	} else if (TESU_FALSE == is_suite_running_now) {
		/* Error no run test suite */
		result = TESU_NO_RUN_SUITE;
	} else {
		result = TESU_NO_ERROR;
	}
	return result;
}

#ifdef TESU_USE_LONGJMP
jmp_buf test_run_point;
#endif

static void TESU_Fatal_Stop_Test(void)
{
#ifdef TESU_USE_LONGJMP
	longjmp(test_run_point, 1);
#endif
	return;
}

TESU_BOOL TESU_Assert_Implementation(const TESU_BOOL value,
                                     const char *file_name,
                                     const int line,
                                     const char *message,
                                     const TESU_BOOL fatal)
{
	TESU_BOOL is_stop_test = TESU_FALSE;
	TESU_RESULT assert_run_result = TESU_FAILED;
	TESU_ERROR error;

	error = TESU_Check_Error_Assert();
	if (TESU_NO_ERROR != error) {
		TESU_Call_User_Error_Handler(line, file_name, error);
		is_stop_test = TESU_TRUE;
		return is_stop_test;
	}

	++assert_summary.total;

	if (TESU_FALSE != is_assert_fatal) {
		is_stop_test = TESU_TRUE;
		return is_stop_test;
	}

	++assert_summary.ran;

	if (TESU_FALSE != value) {
		/* Passed Assert */
		assert_run_result = TESU_PASSED;
	} else {
		/* Failure Assert */
		++assert_summary.failure;
		assert_run_result = TESU_FAILED;
		test_run_result   = TESU_FAILED;

		if (TESU_FALSE != fatal) {
			/* Fatal */
			is_assert_fatal = TESU_TRUE;
			is_stop_test    = TESU_TRUE;
		}
	}

	/* Sent assert result to user handler */
	TESU_Call_User_Assert_Handler(line, file_name,
	                              message, assert_run_result);

	if (TESU_FALSE != is_assert_fatal) {
		/* Fatal Stop Test */
		TESU_Fatal_Stop_Test();
	}
	return is_stop_test;
}

/*******************************************************************
 * Functions (Test)
 *******************************************************************/
static void TESU_Check_Test_Result(const TESU_BOOL fatal)
{
	/* Test Result */
	switch (test_run_result) {
		case TESU_PASSED:
			break;
		case  TESU_FAILED:
			++test_summary.failure;
			if (TESU_FALSE != fatal)
			{
				/* All tests skip, After fatal test. */
				is_test_fatal = TESU_TRUE;
			}
			break;
		case TESU_SKIP:
			break;
		default:
			break;
	}
}

static void TESU_Test_Run(void (*run_test)(void))
{
#ifdef TESU_USE_LONGJMP
	if (0 == setjmp(test_run_point)) {
		(*run_test)();
	}
#else
	(*run_test)();
#endif
	return;
}

static void TESU_Run_Test_Implementation(const TESU_TEST *test)
{
	/* Clear Test Result. These flags will Rewriten in Assert. */
	is_assert_fatal = TESU_FALSE;
	test_run_result = TESU_PASSED;

	++test_summary.total;

	if (TESU_FALSE == is_test_fatal) {
		++test_summary.ran;
		/* Start Test Code */
		TESU_Test_Run(test->test_func);
	} else {
		/* Test Skip */
		test_run_result = TESU_SKIP;
	}
	/* Sent test result to user handler */
	TESU_Call_User_Test_Handler(test->message);

	TESU_Check_Test_Result(test->fatal);

	return;
}

/*******************************************************************
 * Functions (Test Suite)
 *******************************************************************/
static void TESU_Initialize_New_Suite(void)
{
	TESU_Clear_Test_Summary();
	TESU_Clear_Assert_Summary();

	/* Clear fatal flags */
	is_test_fatal   = TESU_FALSE;
	is_assert_fatal = TESU_FALSE;

	return;
}

static TESU_ERROR TESU_Check_Error_Suite(const TESU_TEST *test_list)
{
	TESU_ERROR result;

	if (TESU_FALSE != is_lock_suite_test) {
		result = TESU_CALLED_FROM_HANDLER;
	} else if (TESU_FALSE != is_suite_running_now) {
		/* Error : Test Suite is already running.\n") */
		result = TESU_DOUBLE_RUN_SUITE;
	} else if (TESU_NULL == test_list) {
		/* Error : Test Suite Test is NULL.\n") */
		result = TESU_SUITE_TEST_IS_NULL;
	} else {
		/* OK */
		result = TESU_NO_ERROR;
	}
	return result;
}

void TESU_Start_Suite_Implementation(const TESU_TEST *test_list,
                                     const char *file_name,
                                     const int line,
                                     const char *message,
                                     void (*set_up)(void),
                                     void (*tear_down)(void))
{
	TESU_ERROR error;

	error = TESU_Check_Error_Suite(test_list);
	if (TESU_NO_ERROR != error) {
		/* Error */
		TESU_Call_User_Error_Handler(line, file_name, error);
		return;
	}

	TESU_Initialize_New_Suite();

	++suite_total;

	TESU_Run_User_Func(set_up);

	/* Running Test */
	is_suite_running_now = TESU_TRUE;
	while (TESU_NULL != test_list->test_func) {
		TESU_Run_Test_Implementation(test_list);
		++test_list;
	}
	is_suite_running_now = TESU_FALSE;

	TESU_Run_User_Func(tear_down);

	/* Sent suite result to user handler */
	TESU_Call_User_Suite_Handler(line, file_name, message);

	return;
}
