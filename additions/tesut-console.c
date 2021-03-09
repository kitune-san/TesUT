/*
 * tesut-console -- output console plugin
 * Licenise : zlib
 * See tesut.h
 */
#include "tesut.h"
#include "tesut-console.h"

#ifdef TESU_ENABLE_COLOR_CONSOLE
#define TESU_CONSOLE_DEFAULT_CHAR() TESU_Print("\033[39m")
#define TESU_CONSOLE_RED_CHAR()   TESU_Print("\033[31m")
#define TESU_CONSOLE_GREEN_CHAR() TESU_Print("\033[32m")
#else
#define TESU_CONSOLE_DEFAULT_CHAR()
#define TESU_CONSOLE_RED_CHAR()
#define TESU_CONSOLE_GREEN_CHAR()
#endif

/* HANDLER */
void HANDLER_Suite(const TESU_HANDLER_ARGS *args)
{
	TESU_Print(args->file_name);
	TESU_Print(":");
	TESU_Print_Int(args->line, 0);
	TESU_Print("\n");
	TESU_Print_Int(args->suite_num, 5);
	TESU_Print(" Test Suite: ");
	TESU_Print(args->message);
	TESU_Print("\n");

	TESU_Print("--Run Summary: Type      Total     Ran    Passed    Failed\n");
	TESU_Print("               tests     ");
	TESU_Print_Int(Get_Summary_Test_Total(), 5);
	TESU_Print("   ");
	TESU_Print_Int(Get_Summary_Test_Ran(), 5);
	TESU_Print("     ");
	TESU_Print_Int(Get_Summary_Test_Passed(), 5);
	TESU_Print("     ");
	TESU_Print_Int(Get_Summary_Test_Failed(), 5);
	TESU_Print("\n");

	TESU_Print("               asserts   ");
	TESU_Print_Int(Get_Summary_Assert_Total(), 5);
	TESU_Print("   ");
	TESU_Print_Int(Get_Summary_Assert_Ran(), 5);
	TESU_Print("     ");
	TESU_Print_Int(Get_Summary_Assert_Passed(), 5);
	TESU_Print("     ");
	TESU_Print_Int(Get_Summary_Assert_Failed(), 5);
	TESU_Print("\n");

	TESU_Print("===============================================================\n\n\n");

	return;
}

void HANDLER_Test(const TESU_HANDLER_ARGS *args)
{
	TESU_Print(" TEST   ");
	if (TESU_PASSED == args->result) {
		TESU_CONSOLE_GREEN_CHAR();
		TESU_Print("[ PASSED ]");
	} else if (TESU_SKIP == args->result) {
		TESU_CONSOLE_RED_CHAR();
		TESU_Print("[ SKIPED ]");
	} else {
		TESU_CONSOLE_RED_CHAR();
		TESU_Print("[ FAILED ]");
	}
	TESU_CONSOLE_DEFAULT_CHAR();
	TESU_Print(" TEST No.");
	TESU_Print_Int(args->test_num, 5);
	TESU_Print(" : ");
	TESU_Print(args->message);

	TESU_Print("\n\n");

	return;
}

void HANDLER_Assert(const TESU_HANDLER_ARGS *args)
{
	TESU_Print(" ASSERT ");
	if (TESU_PASSED == args->result) {
		TESU_CONSOLE_GREEN_CHAR();
		TESU_Print("[ PASSED ]");
	} else {
		TESU_CONSOLE_RED_CHAR();
		TESU_Print("[ FAILED ]");
	}
	TESU_CONSOLE_DEFAULT_CHAR();
	TESU_Print(" ");
	TESU_Print(args->file_name);
	TESU_Print(":");
	TESU_Print_Int(args->line, 0);
	TESU_Print("  ");
	TESU_Print(args->message);
	TESU_Print("\n");
	return;
}

void TESU_Set_Console_Handler(void)
{
	TESU_Set_User_Suite_Handler(HANDLER_Suite);
	TESU_Set_User_Test_Handler(HANDLER_Test);
	TESU_Set_User_Assert_Handler(HANDLER_Assert);

	return;
}

