// SYWlan.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "SYWlanDefine.h"

bool flag_Debug = !true;

//[WLAN API]
#define  WIN32_LEAN_AND_MEAN
#include <stdio.h>
#include <windows.h>
#include <winnt.h>
#include <wlanapi.h>
#pragma comment (lib, "wlanapi.lib")
//[WLAN API]

#include <Vector>
std::vector<SYWlanInfo*> *g_pVecWlan;


#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	
	switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:			
			g_pVecWlan = new std::vector<SYWlanInfo*>;			
			break;

		case DLL_THREAD_ATTACH:
			break;
		case DLL_THREAD_DETACH:
			break;
		case DLL_PROCESS_DETACH:
			if (g_pVecWlan)
			{
				//clear
				while(!g_pVecWlan->empty())
				{
					delete g_pVecWlan->back();
					g_pVecWlan->pop_back();
				}
				delete g_pVecWlan;
			}
			break;
	}

    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

//======================================================================================================================
int WINAPI TestAdd(int a, int b)
{
	return a+b;
}


//======================================================================================================================
std::vector<SYWlanInfo*> *WINAPI Scan(void)
{
	
	HANDLE hWLAN = NULL; 
	PWLAN_INTERFACE_INFO_LIST pInterfaceList = NULL;

	DWORD dwNicCount;
	DWORD dwVersion;
	WCHAR wszBuf[256];


	//clear
	while(!g_pVecWlan->empty())
	{
		delete g_pVecWlan->back();
		g_pVecWlan->pop_back();
	}
	

	// WLAN取得
	if(WlanOpenHandle(WLAN_API_MAKE_VERSION(1, 0), NULL, &dwVersion, &hWLAN) != ERROR_SUCCESS){
		return g_pVecWlan;
	}

	// NIC清單取得
	if(WlanEnumInterfaces(hWLAN, NULL, &pInterfaceList) != ERROR_SUCCESS)
	{
		WlanCloseHandle(hWLAN, NULL);
		return g_pVecWlan;
	}

	if (flag_Debug)
	{
		memset(wszBuf, 0, sizeof(wszBuf));
		wsprintf(wszBuf, L"NIC Count=%d\n", pInterfaceList->dwNumberOfItems);
		OutputDebugString(wszBuf);
		OutputDebugString(L"---------------------------------\n");
	}

	// NIC列舉
	for(dwNicCount=0; dwNicCount < pInterfaceList->dwNumberOfItems; dwNicCount++)
	{
		GUID*       pGuid;
		pGuid = &pInterfaceList->InterfaceInfo[dwNicCount].InterfaceGuid;



		//http://msdn.microsoft.com/en-us/library/windows/desktop/ms706749(v=vs.85).aspx
		PWLAN_AVAILABLE_NETWORK_LIST pAvailableNetworkList;		
		//讀取可用的網路清單
		if(WlanGetAvailableNetworkList(hWLAN, pGuid, 0, NULL, &pAvailableNetworkList) == ERROR_SUCCESS)
		{			
			if (flag_Debug)
			{
				memset(wszBuf, 0, sizeof(wszBuf));
				wsprintf(wszBuf, L"Items Count=%d\n", pAvailableNetworkList->dwNumberOfItems);
				OutputDebugString(wszBuf);
				OutputDebugString(L"---------------------------------\n");
			}

			DWORD dwAvailableCount;

			//試圖可以從網路找到的AP
			for(dwAvailableCount=0; dwAvailableCount < pAvailableNetworkList->dwNumberOfItems; dwAvailableCount++)
			{
				PWLAN_AVAILABLE_NETWORK  pAvailableNetwork = &pAvailableNetworkList->Network[dwAvailableCount];



				SYWlanInfo *info = new SYWlanInfo();
				info->uSignalQuality = pAvailableNetwork->wlanSignalQuality;
				wsprintf(info->wszSSID, L"%S", pAvailableNetwork->dot11Ssid.ucSSID);

				if (pAvailableNetwork->dwFlags) {
					if (pAvailableNetwork->dwFlags & WLAN_AVAILABLE_NETWORK_CONNECTED) info->bCurrentConnected = true;				
				}   

				//[DOT11_AUTH_ALGORITHM dot11DefaultAuthAlgorithm]
				switch (pAvailableNetwork->dot11DefaultAuthAlgorithm)
				{
				case DOT11_AUTH_ALGO_80211_OPEN:					
					wsprintf(info->wszAuthAlgorithm, L"%s", L"80211_OPEN");
					break;
				case DOT11_AUTH_ALGO_80211_SHARED_KEY:				
					wsprintf(info->wszAuthAlgorithm, L"%s", L"80211_SHARED_KEY");
					break;
				case DOT11_AUTH_ALGO_WPA:					
					wsprintf(info->wszAuthAlgorithm, L"%s", L"WPA");
					break;
				case DOT11_AUTH_ALGO_WPA_PSK:				
					wsprintf(info->wszAuthAlgorithm, L"%s", L"WPA_PSK");
					break;
				case DOT11_AUTH_ALGO_WPA_NONE:			
					wsprintf(info->wszAuthAlgorithm, L"%s", L"WPA_NONE");
					break;
				case DOT11_AUTH_ALGO_RSNA:				
					wsprintf(info->wszAuthAlgorithm, L"%s", L"RSNA");
					break;
				case DOT11_AUTH_ALGO_RSNA_PSK:					
					wsprintf(info->wszAuthAlgorithm, L"%s", L"RSNA_PSK");
					break;
				case DOT11_AUTH_ALGO_IHV_START:					
					wsprintf(info->wszAuthAlgorithm, L"%s", L"IHV_START");
					break;
				case DOT11_AUTH_ALGO_IHV_END:					
					wsprintf(info->wszAuthAlgorithm, L"%s", L"IHV_END");
					break;
				default:					
					wsprintf(info->wszAuthAlgorithm, L"%s", L"UNKNOW");
					break;
				}

				g_pVecWlan->push_back(info);


				//WLAN_CONNECTION_PARAMETERS wlanConnectionParams;

				//連線參數
				//ZeroMemory(&wlanConnectionParams, sizeof(WLAN_CONNECTION_PARAMETERS));
				//wlanConnectionParams.wlanConnectionMode = wlan_connection_mode_profile;
				//wlanConnectionParams.strProfile         = pAvailableNetwork->strProfileName;
				//wlanConnectionParams.pDot11Ssid         = &pAvailableNetwork->dot11Ssid;
				//wlanConnectionParams.pDesiredBssidList  = NULL;
				//wlanConnectionParams.dot11BssType       = pAvailableNetwork->dot11BssType;
				//wlanConnectionParams.dwFlags            = 0;

				//連線
				//if(WlanConnect(hWLAN, pGuid, &wlanConnectionParams, NULL) == ERROR_SUCCESS)
				//{
				//	retCode = true;
				//	break;
				//}
				if (flag_Debug)
				{
					//[ WCHAR strProfileName[WLAN_MAX_NAME_LENGTH] ]
					memset(wszBuf, 0, sizeof(wszBuf));
					wsprintf(wszBuf, L"ProfileName:%s\n", pAvailableNetwork->strProfileName);					
					OutputDebugString(wszBuf);
					
					//[DOT11_SSID dot11Ssid]
					memset(wszBuf, 0, sizeof(wszBuf));
					wsprintf(wszBuf, L"SSID:%S\n", pAvailableNetwork->dot11Ssid.ucSSID);					
					OutputDebugString(wszBuf);

					//[DOT11_BSS_TYPE dot11BssType]
					switch(pAvailableNetwork->dot11BssType)
					{
					case dot11_BSS_type_infrastructure:
						OutputDebugString(L"BssType:infrastructure\n");
						break;
					case dot11_BSS_type_independent:
						OutputDebugString(L"BssType:independent\n");
						break;
					case dot11_BSS_type_any:
						OutputDebugString(L"BssType:any\n");
						break;
					default:
						OutputDebugString(L"BssType:error!\n");
						break;
					}

					//[ULONG uNumberOfBssids]
					memset(wszBuf, 0, sizeof(wszBuf));
					wsprintf(wszBuf, L"NumberOfBssids:%d\n", pAvailableNetwork->uNumberOfBssids);					
					OutputDebugString(wszBuf);
					
					//[BOOL bNetworkConnectable]
					if(pAvailableNetwork->bNetworkConnectable){
						OutputDebugString(L"NetworkConnectable:True\n");
					}else{
						OutputDebugString(L"NetworkConnectable:False\n");
					}

					//[WLAN_REASON_CODE wlanNotConnectableReason]
					//None

					//[ULONG uNumberOfPhyTypes]
					memset(wszBuf, 0, sizeof(wszBuf));
					wsprintf(wszBuf, L"NumberOfPhyTypes:%d\n", pAvailableNetwork->uNumberOfPhyTypes);					
					OutputDebugString(wszBuf);


					switch (pAvailableNetwork->dot11PhyTypes[0])
					{
					//case dot11_phy_type_unknown:
					//	OutputDebugString(L"PhyTypes:unknown\n");
					//	break;

					case dot11_phy_type_any:
						OutputDebugString(L"PhyTypes:any or unknown\n");
						break;

					case dot11_phy_type_fhss:
						OutputDebugString(L"PhyTypes:fhss\n");
						break;

					case dot11_phy_type_dsss:
						OutputDebugString(L"PhyTypes:dsss\n");
						break;

					case dot11_phy_type_irbaseband:
						OutputDebugString(L"PhyTypes:irbaseband\n");
						break;

					case dot11_phy_type_ofdm:
						OutputDebugString(L"PhyTypes:ofdm\n");
						break;

					case dot11_phy_type_hrdsss:
						OutputDebugString(L"PhyTypes:hrdsss\n");
						break;

					case dot11_phy_type_erp:
						OutputDebugString(L"PhyTypes:erp\n");
						break;

					case dot11_phy_type_IHV_start:
						OutputDebugString(L"PhyTypes:IHV_start\n");
						break;

					case dot11_phy_type_IHV_end:
						OutputDebugString(L"PhyTypes:IHV_end\n");
						break;

					default:
						OutputDebugString(L"PhyTypes:error!\n");
						break;
					}



					//[BOOL bMorePhyTypes]
					if(pAvailableNetwork->bMorePhyTypes){
						OutputDebugString(L"MorePhyTypes:True\n");
					}else{
						OutputDebugString(L"MorePhyTypes:False\n");
					}


					//[WLAN_SIGNAL_QUALITY wlanSignalQuality] 信號強度
					memset(wszBuf, 0, sizeof(wszBuf));
					wsprintf(wszBuf, L"SignalQuality:%d%%\n", pAvailableNetwork->wlanSignalQuality);					
					OutputDebugString(wszBuf);

					//[BOOL bSecurityEnabled] 透過這個網路傳輸資訊可能會被其他人看到
					if(pAvailableNetwork->bSecurityEnabled){
						OutputDebugString(L"SecurityEnabled:True\n");
					}else{
						OutputDebugString(L"SecurityEnabled:False\n");
					}


					//[DOT11_AUTH_ALGORITHM dot11DefaultAuthAlgorithm]
					switch (pAvailableNetwork->dot11DefaultAuthAlgorithm)
					{
					case DOT11_AUTH_ALGO_80211_OPEN:
						OutputDebugString(L"AuthAlgorithm:80211_OPEN\n");
						break;
					case DOT11_AUTH_ALGO_80211_SHARED_KEY:
						OutputDebugString(L"AuthAlgorithm:80211_SHARED_KEY\n");
						break;
					case DOT11_AUTH_ALGO_WPA:
						OutputDebugString(L"AuthAlgorithm:WPA\n");
						break;
					case DOT11_AUTH_ALGO_WPA_PSK:
						OutputDebugString(L"AuthAlgorithm:WPA_PSK\n");
						break;
					case DOT11_AUTH_ALGO_WPA_NONE:
						OutputDebugString(L"AuthAlgorithm:WPA_NONE\n");
						break;
					case DOT11_AUTH_ALGO_RSNA:
						OutputDebugString(L"AuthAlgorithm:RSNA\n");
						break;
					case DOT11_AUTH_ALGO_RSNA_PSK:
						OutputDebugString(L"AuthAlgorithm:RSNA_PSK\n");
						break;
					case DOT11_AUTH_ALGO_IHV_START:
						OutputDebugString(L"AuthAlgorithm:IHV_START\n");
						break;
					case DOT11_AUTH_ALGO_IHV_END:
						OutputDebugString(L"AuthAlgorithm:IHV_END\n");
						break;
					default:
						OutputDebugString(L"AuthAlgorithm:error!\n");
						break;
					}
					

					//[DOT11_CIPHER_ALGORITHM dot11DefaultCipherAlgorithm]
					switch (pAvailableNetwork->dot11DefaultCipherAlgorithm)
					{
					case DOT11_CIPHER_ALGO_NONE:
						OutputDebugString(L"CipherAlgorithm:NONE\n");
						break;
					case DOT11_CIPHER_ALGO_WEP40:
						OutputDebugString(L"CipherAlgorithm:WEP40\n");
						break;
					case DOT11_CIPHER_ALGO_TKIP:
						OutputDebugString(L"CipherAlgorithm:TKIP\n");
						break;
					case DOT11_CIPHER_ALGO_CCMP:
						OutputDebugString(L"CipherAlgorithm:CCMP\n");
						break;
					case DOT11_CIPHER_ALGO_WEP104:
						OutputDebugString(L"CipherAlgorithm:WEP104\n");
						break;
					case DOT11_CIPHER_ALGO_WPA_USE_GROUP:
						OutputDebugString(L"CipherAlgorithm:WPA_USE_GROUP or RSN_USE_GROUP\n");					
						break;
					case DOT11_CIPHER_ALGO_WEP:
						OutputDebugString(L"CipherAlgorithm:WEP\n");
						break;
					case DOT11_CIPHER_ALGO_IHV_START:
						OutputDebugString(L"CipherAlgorithm:IHV_START\n");
						break;
					case DOT11_CIPHER_ALGO_IHV_END:
						OutputDebugString(L"CipherAlgorithm:IHV_END\n");
						break;
					default:
						OutputDebugString(L"CipherAlgorithm:error!\n");
						break;
					}
	
		

					//[DWORD dwFlags]
					//[DWORD dwReserved]
			
					OutputDebugString(L"---------------------------------\n");

		
				}//end if(flag_Debug)
				

			}//end for (dwAvailableCount)

			WlanFreeMemory(pAvailableNetworkList);  
		}//end if (WlanGetAvailableNetworkList)
		

		//***************************************************************************************
		//[Load PWLAN_BSS_LIST] 
		PWLAN_BSS_LIST pWlanBssList;		
		if (WlanGetNetworkBssList(hWLAN, pGuid, NULL, dot11_BSS_type_any, NULL, NULL, &pWlanBssList) != ERROR_SUCCESS)
		{
			OutputDebugString(L"something is wrong with WlanGetNetworkBssList()\n");
		}else{

			DWORD dwBssCount;

			for(dwBssCount=0; dwBssCount < pWlanBssList->dwNumberOfItems; dwBssCount++)			
			{
				WLAN_BSS_ENTRY *pBssEntry = &pWlanBssList->wlanBssEntries[dwBssCount];

				bool bFind = false;

				memset(wszBuf, 0, sizeof(wszBuf));
				wsprintf(wszBuf, L"%S", pBssEntry->dot11Ssid.ucSSID);
				SYWlanInfo *info = NULL;
				for (unsigned int i=0; i<g_pVecWlan->size(); i++)
				{
					info = g_pVecWlan->at(i);
					if (info->IsEqualSSID(wszBuf))
					{
						info->uLinkQuality = pBssEntry->uLinkQuality;
						info->MAC_ADDRESS[0] = pBssEntry->dot11Bssid[0];
						info->MAC_ADDRESS[1] = pBssEntry->dot11Bssid[1];
						info->MAC_ADDRESS[2] = pBssEntry->dot11Bssid[2];
						info->MAC_ADDRESS[3] = pBssEntry->dot11Bssid[3];
						info->MAC_ADDRESS[4] = pBssEntry->dot11Bssid[4];
						info->MAC_ADDRESS[5] = pBssEntry->dot11Bssid[5];
						bFind = true;
						break;
					}
				}


				if (!bFind)
				{
					info = NULL;
					info = new SYWlanInfo();
					info->uLinkQuality = pBssEntry->uLinkQuality;

					memset(info->wszSSID, 0, sizeof(info->wszSSID));
					wsprintf(info->wszSSID, L"%S", pBssEntry->dot11Ssid.ucSSID);
					info->MAC_ADDRESS[0] = pBssEntry->dot11Bssid[0];
					info->MAC_ADDRESS[1] = pBssEntry->dot11Bssid[1];
					info->MAC_ADDRESS[2] = pBssEntry->dot11Bssid[2];
					info->MAC_ADDRESS[3] = pBssEntry->dot11Bssid[3];
					info->MAC_ADDRESS[4] = pBssEntry->dot11Bssid[4];
					info->MAC_ADDRESS[5] = pBssEntry->dot11Bssid[5];
					g_pVecWlan->push_back(info);
				}
				
				
				if (flag_Debug)
				{
					OutputDebugString(L"---------------------------------\n");
					info->Print();
					OutputDebugString(L"---------------------------------\n");

				}

			}//end for
			WlanFreeMemory(pWlanBssList); 
		}//end if (WlanGetNetworkBssList)


	}//end for (dwNicCount)

	WlanFreeMemory(pInterfaceList);
	WlanCloseHandle(hWLAN, NULL);

	return g_pVecWlan;
}




//======================================================================================================================
bool WINAPI ConnetWithSSID(WCHAR *wszSSID)
{

	HANDLE hWLAN = NULL; 
	PWLAN_INTERFACE_INFO_LIST pInterfaceList = NULL;

	DWORD dwNicCount;
	DWORD dwVersion;
	WCHAR wszBuf[256];
	bool  retCode = false;

	// WLAN取得
	if(WlanOpenHandle(WLAN_API_MAKE_VERSION(1, 0), NULL, &dwVersion, &hWLAN) != ERROR_SUCCESS){
		return false;
	}

	// NIC清單取得
	if(WlanEnumInterfaces(hWLAN, NULL, &pInterfaceList) != ERROR_SUCCESS)
	{
		WlanCloseHandle(hWLAN, NULL);
		return false;
	}

	
	// NIC列舉
	for(dwNicCount=0; dwNicCount < pInterfaceList->dwNumberOfItems; dwNicCount++)
	{
		GUID*       pGuid;
		pGuid = &pInterfaceList->InterfaceInfo[dwNicCount].InterfaceGuid;

		PWLAN_AVAILABLE_NETWORK_LIST pAvailableNetworkList;		
		//讀取可用的網路清單
		if(WlanGetAvailableNetworkList(hWLAN, pGuid, 0, NULL, &pAvailableNetworkList) == ERROR_SUCCESS)
		{			
			
			DWORD dwAvailableCount;

			//試圖可以從網路找到的AP
			for(dwAvailableCount=0; dwAvailableCount < pAvailableNetworkList->dwNumberOfItems; dwAvailableCount++)
			{
				PWLAN_AVAILABLE_NETWORK  pAvailableNetwork = &pAvailableNetworkList->Network[dwAvailableCount];


				memset(wszBuf, 0, sizeof(wszBuf));
				wsprintf(wszBuf, L"%S", pAvailableNetwork->dot11Ssid.ucSSID);


				if (_wcsicmp(wszBuf, wszSSID) == 0)
				{
					if (pAvailableNetwork->dwFlags) {
						if (pAvailableNetwork->dwFlags & WLAN_AVAILABLE_NETWORK_CONNECTED)
						{
							//has connected
							WlanFreeMemory(pAvailableNetworkList);  
							WlanFreeMemory(pInterfaceList);
							WlanCloseHandle(hWLAN, NULL);
							return true;
						}
					}   


					WLAN_CONNECTION_PARAMETERS wlanConnectionParams;
					//連線參數
					ZeroMemory(&wlanConnectionParams, sizeof(WLAN_CONNECTION_PARAMETERS));
					wlanConnectionParams.wlanConnectionMode = wlan_connection_mode_profile;
					wlanConnectionParams.strProfile         = pAvailableNetwork->strProfileName;
					wlanConnectionParams.pDot11Ssid         = &pAvailableNetwork->dot11Ssid;
					wlanConnectionParams.pDesiredBssidList  = NULL;
					wlanConnectionParams.dot11BssType       = pAvailableNetwork->dot11BssType;
					wlanConnectionParams.dwFlags            = 0;

					//連線
					if(WlanConnect(hWLAN, pGuid, &wlanConnectionParams, NULL) == ERROR_SUCCESS)
					{
						retCode = true;
						break;
					}
				}


			}//end for (dwAvailableCount)

			WlanFreeMemory(pAvailableNetworkList);  
		}//end if (WlanGetAvailableNetworkList)



	}//end for (dwNicCount)

	WlanFreeMemory(pInterfaceList);
	WlanCloseHandle(hWLAN, NULL);

	return retCode;

}