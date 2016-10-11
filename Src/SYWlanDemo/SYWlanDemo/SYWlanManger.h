#ifndef _SYELANMANGER_H_
#define _SYELANMANGER_H_

#include "SYWlanDefine.h"
#include <vector>

typedef int (WINAPI* SYWlan_TestAdd)(int a, int b);
typedef std::vector<SYWlanInfo*> *(WINAPI* SYWlan_Scan)(void);
typedef bool (WINAPI* SYWlan_ConnetWithSSID)(WCHAR *wszSSID);

class SYWlanManger
{

public:	
	//creat
	static SYWlanManger* Instance(void);
	~SYWlanManger(void);
	//method
	int TestAdd(int a, int b);
	std::vector<SYWlanInfo*> *Scan(void);
	bool ConnetWithSSID(WCHAR *wszSSID);

	SYWlanInfo *GetNowConnectedSSID(void);
private:
	//method
	SYWlanManger(void);	
	

private:
	//var
	//creat
	static SYWlanManger*	m_instance;		
	//dll
	HMODULE  				m_hModule;

	SYWlan_TestAdd			m_DLLFuncTestAdd;
	SYWlan_Scan				m_DLLFuncScan;
	SYWlan_ConnetWithSSID	m_DLLFuncConnetWithSSID;
};

#endif