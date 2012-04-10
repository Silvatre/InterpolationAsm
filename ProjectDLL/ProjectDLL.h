// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the PROJECTDLL_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// PROJECTDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef PROJECTDLL_EXPORTS
#define PROJECTDLL_API __declspec(dllexport)
#else
#define PROJECTDLL_API __declspec(dllimport)
#endif

// This class is exported from the ProjectDLL.dll
class PROJECTDLL_API CProjectDLL {
public:
	CProjectDLL(void);
	// TODO: add your methods here.
};

extern PROJECTDLL_API int nProjectDLL;

PROJECTDLL_API int fnProjectDLL(void);
