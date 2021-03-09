/* How to use */

#include "tesut.h"
#include <stdio.h>

/* CHECK CODE */
int sum_all(int data[], size_t size)
{
	unsigned int i;
	int sum = 0;
	for (i = 0; i < size; i++) {
		sum += data[i];
	}

	return sum;
}

int sub_all(int data[], size_t size)
{
	unsigned int i;
	int sub = 1;
	for (i = 0; i < size; i++) {
		sub *= data[i];
	}

	return sub;
}

/* TEST CODE */
void setup(void)
{
	printf("setup\n");
}

void tear_down(void)
{
	printf("tear_down\n");
}

void sum_all_test00(void)
{
	int sum;
	int data[] = {51, 4654, 215, 465, 156, 894, 12};
	sum = sum_all(data, sizeof(data) / sizeof(data[0]));

	TESU_ASSERT_EQUAL(sum, 6447);
}

void sum_all_test01(void)
{
	int sum;
	int data[] = {27, 53, 1, 8, 63, 21};
	sum = sum_all(data, sizeof(data) / sizeof(data[0]));

	TESU_ASSERT(sum > 100);
}

void sum_all_test02(void)
{
	int sum;
	int data[] = {2, 9, 32, 27, 53, 1, 8, 63, 21};
	sum = sum_all(data, sizeof(data) / sizeof(data[0]));

	TESU_ASSERT_NOT_EQUAL(sum, 200);
}

void sub_all_test00(void)
{
	int sub;
	int data[] ={2, 5, 8, 7, 6, 5, 4, 3};
	sub  = sub_all(data, sizeof(data) / sizeof(data[0]));

	TESU_ASSERT_EQUAL(sub, 12);
}

void sub_all_test01(void)
{
	int sub;
	int data[] ={2, 6, 7, 8, 9};
	sub  = sub_all(data, sizeof(data) / sizeof(data[0]));

	TESU_ASSERT_EQUAL_FATAL(sub, 100);
	TESU_ASSERT_EQUAL(sub, 6048);
}

void sub_all_test02(void)
{
	int sub;
	int data[] ={2, 6, 7, 8, 9};
	sub  = sub_all(data, sizeof(data) / sizeof(data[0]));

	TESU_ASSERT_EQUAL(sub, 6048);
}

/* HANDLER */
void HANDLER_Suite(const TESU_HANDLER_ARGS *args)
{
	printf("%s:%d\n", args->file_name, args->line);
	printf("<%5d> Test Suite: %s\n", args->suite_num, args->message);

	printf("--Run Summary: Type      Total     Ran       Passed    Failed\n");
	printf("               tests     %5d   %5d        %5d    %5d\n",
	       Get_Summary_Test_Total(),  Get_Summary_Test_Ran(),
	       Get_Summary_Test_Passed(), Get_Summary_Test_Failed());
	printf("               asserts   %5d   %5d        %5d    %5d\n",
	       Get_Summary_Assert_Total(),  Get_Summary_Assert_Ran(),
	       Get_Summary_Assert_Passed(), Get_Summary_Assert_Failed());
	printf("\n");
	printf("===============================================================\n\n\n");

	/* ERROR Pattern(ignored) */
	TESU_ASSERT(args->suite_num == 4);
	return;
}

void HANDLER_Test(const TESU_HANDLER_ARGS *args)
{
	printf("%5d: Test Suite: %s --------------- ", args->test_num, args->message);
	if (TESU_PASSED == args->result) {
		printf("PASSED\n");
	} else if (TESU_SKIP == args->result) {
		printf("SKIP\n");
	} else {
		printf("FAILED\n");
	}

	printf("\n\n");
	return;
}

void HANDLER_Assert(const TESU_HANDLER_ARGS *args)
{
	printf("%s:%d Assert : %s ----------- ", args->file_name, args->line, args->message);
	if (TESU_PASSED == args->result) {
		printf("PASSED\n");
	} else {
		printf("FAILED\n");
	}
	return;
}

void HANDLER_ERROR(const TESU_ERROR_HANDLER_ARGS *args)
{
	printf("ERROR!: %s:%d : ", args->file_name, args->line);
	switch (args->code) {
		case TESU_CALLED_FROM_HANDLER:
			printf("Called from Handler");
			break;

		case TESU_NO_RUN_SUITE:
			printf("No Run Suite");
			break;

		case TESU_DOUBLE_RUN_SUITE:
			printf("Suite is already running.");
			break;

		case TESU_SUITE_TEST_IS_NULL:
			printf("Suite Test is NULL.");
			break;
		default:
			printf("Unknown.");
			break;
	}
	printf("\n");
	return;
}

int main(void)
{
	TESU_TEST Suite0[] = {
		{sum_all_test00, "SUM_ALL_TEST00", TESU_NO_FATAL},
		{sum_all_test01, "SUM_ALL_TEST01", TESU_NO_FATAL},
		{sum_all_test02, "SUM_ALL_TEST02", TESU_NO_FATAL},
		TESU_TEST_INFO_NULL
	};

	TESU_TEST Suite1[] = {
		{sub_all_test00, "SUB_ALL_TEST00", TESU_NO_FATAL},
		{sub_all_test01, "SUB_ALL_TEST01", TESU_NO_FATAL},
		{sub_all_test02, "SUB_ALL_TEST02", TESU_NO_FATAL},
		TESU_TEST_INFO_NULL
	};

	TESU_Initialize();
	TESU_Set_User_Suite_Handler(HANDLER_Suite);
	TESU_Set_User_Test_Handler(HANDLER_Test);
	TESU_Set_User_Assert_Handler(HANDLER_Assert);
	TESU_Set_User_Error_Handler(HANDLER_ERROR);

	TESU_Start_Suite(Suite0, setup, tear_down, "Suite0");
	TESU_Start_Suite(Suite1, NULL, NULL, "Suite1");

	return 0;
}

