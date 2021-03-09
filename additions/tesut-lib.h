/*
 * tesut-lib --  tesut simple library
 * Licenise : zlib
 * See tesut.h
 */

void TESU_Putchar(const char c);
void TESU_Print(const char *str);
void TESU_Print_Int(int num, unsigned int digit);

void Set_TESU_Putchar(void (*putchar_func)(const char));
void Set_TESU_Print(void (*print_func)(const char*));
void Set_TESU_Print_Int(void (*print_int_func)(int, unsigned int));
