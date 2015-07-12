#define __DLL__

#include "Export.h"
#include "Common.h"

extern "C" TData __declspec(dllexport) __stdcall maintain(char* s)
{
	static Drobot calc;
	
	std::string *tmp = new std::string;
	
	int i = 0;
	while (s[i]) *tmp += s[i++];

	TData ret;
	ret.result = -1;
	
	try
	{
		TInternalData *p = calc.drive(tmp);
		if (p)	ret.value = *p;
		else	ret.result = -2;

	}
	catch (ErrCode e)
	{
		ret.result = e;
	}
	catch (ErrStruct* p)
	{
		delete p->pointer;
		ret.result = p->_code;
		delete p;
	}

	return ret;
}

extern "C" void __declspec(dllexport) __stdcall freeDllPointer(void *pointer)
{
	if (pointer) delete pointer;
}