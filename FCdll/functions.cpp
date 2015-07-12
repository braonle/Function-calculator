#include "function.h"

using namespace std;

///Symbols to skip (unrecognisable)
const char empty [] = { ' ', '\n', '\t', 8, 10, 13, '\0' };


bool isEmptySimbol(char c)
{
	for (int i = 0; i < strlen(empty); ++i)
		if (c == empty[i])
			return true;
	return false;
}


op analisis(string &s)
{
	///if no '=' - calculation
	int pos_of_equ = s.find('=');
	if (pos_of_equ == -1)
		return eval;
	
	///if there are no recognisable symbols after '=' - calculation
	bool f = true;
	for (int i = pos_of_equ + 1; i < s.size() && f; ++i)
		if (!isEmptySimbol(s[i]))
			f = false;
	if (f)
		return eval;

	///if no bracket - adding constant as it is not a calculation
	int pos_of_opened_bracket = s.find('(');
	if (pos_of_opened_bracket == -1)
		return cons;

	///if there are no brackets before '=' - this is a constant
	if (pos_of_opened_bracket > pos_of_equ)
		return cons;

	///Finds if bracket expression if invalid
	int pos_of_closed_bracket = s.find(')');
	if (pos_of_closed_bracket < pos_of_opened_bracket)
		return err;

	///if brackets contain no arguments, it is a constant, otherwise - function
	f = true;
	for (int i = pos_of_opened_bracket + 1; i < pos_of_closed_bracket && f; ++i)
		if (!isEmptySimbol(s[i]))
			f = false;

	if (f)
		return cons;

	return fun;
}