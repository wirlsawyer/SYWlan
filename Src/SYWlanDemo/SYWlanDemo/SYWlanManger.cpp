#include "stdafx.h"
#include "SYWlanManger.h"


SYWlanManger* SYWlanManger::m_instance = NULL;

SYWlanManger* SYWlanManger::Instance()
{
	if (m_instance == NULL){
		m_instance = new SYWlanManger(); 
	}
	return m_instance;
}

SYWlanManger::SYWlanManger()
{
	m_hModule				= NULL;
	m_DLLFuncTestAdd		= NULL;

	//-------------------------------------------------------------------------------
	m_hModule = LoadLibrary(L".\\SYWlan.dll");
	//-------------------------------------------------------------------------------
	if ( m_hModule != NULL )
	{
		//-------------------------------------------------------------------------------
		m_DLLFuncTestAdd = (SYWlan_TestAdd)GetProcAddress( m_hModule, "TestAdd" );
		if (m_DLLFuncTestAdd == NULL)
		{
			OutputDebugString(L"GetProc fault:TestAdd\n");
			::MessageBox(NULL, L"GetProc fault:TestAdd", L"", MB_OK);
			FreeLibrary(m_hModule);
			return;
		}

		m_DLLFuncScan = (SYWlan_Scan)GetProcAddress( m_hModule, "Scan" );
		if (m_DLLFuncScan == NULL)
		{
			OutputDebugString(L"GetProc fault:Scan\n");
			::MessageBox(NULL, L"GetProc fault:Scan", L"", MB_OK);
			FreeLibrary(m_hModule);
			return;
		}

		m_DLLFuncConnetWithSSID = (SYWlan_ConnetWithSSID)GetProcAddress( m_hModule, "ConnetWithSSID" );
		if (m_DLLFuncConnetWithSSID == NULL)
		{
			OutputDebugString(L"GetProc fault:ConnetWithSSID\n");
			::MessageBox(NULL, L"GetProc fault:ConnetWithSSID", L"", MB_OK);
			FreeLibrary(m_hModule);
			return;
		}
		

	}else{
		OutputDebugString(L"SYWlanManger fault:Can't find SYWlan.dll\n");
	}
}

SYWlanManger::~SYWlanManger()
{
	FreeLibrary(m_hModule);
}


int SYWlanManger::TestAdd(int a, int b)
{
	return m_DLLFuncTestAdd(a, b);
}

std::vector<SYWlanInfo*> *SYWlanManger::Scan(void)
{
	return m_DLLFuncScan();
}

bool SYWlanManger::ConnetWithSSID(WCHAR *wszSSID)
{
	return m_DLLFuncConnetWithSSID(wszSSID);
}

SYWlanInfo *SYWlanManger::GetNowConnectedSSID(void)
{

	std::vector<SYWlanInfo*> *pVecWlanInfo = this->Scan();
	for (unsigned int i=0; i<pVecWlanInfo->size(); i++)
	{
		SYWlanInfo *info = pVecWlanInfo->at(i);
		if (info->bCurrentConnected)
		{
			return info;			
		}
	}
	return NULL;
}