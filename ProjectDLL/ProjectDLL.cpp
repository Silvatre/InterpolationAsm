// ProjectDLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "ProjectDLL.h"


// This is an example of an exported variable
PROJECTDLL_API int nProjectDLL=0;

// This is an example of an exported function.
PROJECTDLL_API int fnProjectDLL(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see ProjectDLL.h for the class definition
CProjectDLL::CProjectDLL()
{
	return;
}
