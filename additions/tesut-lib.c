/*
 * tesut-lib --  tesut simple library
 * Licenise : zlib
 * See tesut.h
 */
#include "tesut.h"
#include "tesut-lib.h"

/* User functions */
void (*User_Putchar_Func)(const char) = TESU_NULL;
void (*User_Print_Func)(const char*)  = TESU_NULL;
void (*User_Print_Int_Func)(int, unsigned int) = TESU_NULL;

/* Default functions */
static void Default_Putchar(const char c)
{
	return;
}

static void Default_Print(const char *str)
{
	while (*str) {
		TESU_Putchar(*str);
		++str;
	}
	return;
}

static void Default_Print_Int(int num, unsigned int digit)
{
	int  sign  = 0;
	unsigned int d = 0;
	char text[10];

	/* Check Sign */
	if (0 > num) {
		sign = -1;
		num *= -1;
	} else {
		sign = 1;
	}

	/* itoa */
	while ((0 < num)
	 &&    (sizeof(text) / sizeof(text[0])) > d) {
		text[d] = (char)(num % 10) + '0';
		num /= 10;
		++d;
	}

	if (0 == d) {
		text[d] = '0';
		++d;
	}

	/* ++ sign digit */
	++d;

	while (d < digit) {
		TESU_Putchar(' ');
		--digit;
	}

	/* print sign */
	if (0 > sign) {
		TESU_Putchar('-');
	} else if (d == digit) {
		TESU_Putchar(' ');
	}
	--d;

	/* print number */
	while (0 < d) {
		--d;
		TESU_Putchar(text[d]);
	}

	return;
}

/* Call User functions */
void TESU_Putchar(const char c)
{
	if (TESU_NULL != User_Putchar_Func) {
		(*User_Putchar_Func)(c);
	} else {
		Default_Putchar(c);
	}
	return;
}

void TESU_Print(const char *str)
{
	if (TESU_NULL != User_Print_Func) {
		(*User_Print_Func)(str);
	} else {
		Default_Print(str);
	}
	return;
}

void TESU_Print_Int(int num, unsigned int digit)
{
	if (TESU_NULL != User_Print_Int_Func) {
		(*User_Print_Int_Func)(num, digit);
	} else {
		Default_Print_Int(num, digit);
	}
	return;
}

/* Registry User Functions */
void Set_TESU_Putchar(void (*putchar_func)(const char))
{
	User_Putchar_Func = putchar_func;
	return;
}

void Set_TESU_Print(void (*print_func)(const char*))
{
	User_Print_Func = print_func;
	return;
}

void Set_TESU_Print_Int(void (*print_int_func)(int, unsigned int))
{
	User_Print_Int_Func = print_int_func;
	return;
}
