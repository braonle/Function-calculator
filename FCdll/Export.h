#pragma once
#include <string>

typedef struct
{
	long double value;
	int result;
}TData, *PData;

#ifdef __DLL__

extern "C" TData __declspec(dllexport) __stdcall maintain(char* s);
extern "C" void __declspec(dllexport) __stdcall freeDllPointer(void *pointer);

#else

///Error codes given through exceptions
enum ErrCode
{
	invalid_name, invalid_expression, division_by_zero, invalid_operand, invalid_value, \
	memory_error, need_bracket, unknown_name, you_are_fool, wrong_number_of_argument,
	bad_power, invalid_bracket_position, invalid_alloc, bad_re
};

extern "C" TData __declspec(dllimport) __stdcall maintain(char* s);
extern "C" void __declspec(dllimport) __stdcall freeDllPointer(void *pointer);

#endif