#define __DLL__

#include "Export.h"
#include "Common.h"

TData __declspec(dllexport) __stdcall maintain(char* s)
{	
	std::string *tmp = new std::string;
	
	int i = 0;
	while (s[i]) *tmp += s[i++];

	TData ret;
	ret.result = -1;
	
	try
	{
		TInternalData *p = Drobot::drive(tmp);
		if (p)	ret.value = p;
		else	ret.result = -2;

	}
	catch (ErrCode e)
	{
		ret.result = e;
	}
	catch (ErrStruct* p)
	{
		for (int i = 0; i < p->vec.size(); ++i)
			delete p->vec[i];
		ret.result = p->_code;
		delete p;
	}

	return ret;
}

void __declspec(dllexport) __stdcall freeDllPointer(void *pointer)
{
	if (pointer) delete pointer;
}