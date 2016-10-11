#ifndef _SYWLANDEFINE_H_
#define _SYWLANDEFINE_H_

typedef wchar_t 		WCHAR;

class SYWlanInfo
{
public:

	SYWlanInfo()
	{
		memset(wszSSID, 0, sizeof(wszSSID));
		memset(wszAuthAlgorithm, 0, sizeof(wszAuthAlgorithm));
		MAC_ADDRESS[0] = -1;
		MAC_ADDRESS[1] = -1;
		MAC_ADDRESS[2] = -1;
		MAC_ADDRESS[3] = -1;
		MAC_ADDRESS[4] = -1;
		MAC_ADDRESS[5] = -1;

		uLinkQuality   = -1;
		uSignalQuality = -1;

		bCurrentConnected = false;
	}

	WCHAR wszSSID[32];
	int	  MAC_ADDRESS[6];
	ULONG uLinkQuality;
	ULONG uSignalQuality;
	bool  bCurrentConnected;
	WCHAR wszAuthAlgorithm[32];

	//Method
	bool IsEqualSSID(WCHAR *SSID)
	{
		return (_wcsicmp(SSID, wszSSID) == 0?true:false);
	}
	void Print(void)	
	{
		WCHAR wszBuf[256];
		memset(wszBuf, 0, sizeof(wszBuf));
		wsprintf(wszBuf, L"SSID:%s\n", wszSSID);
		OutputDebugString(wszBuf);

		memset(wszBuf, 0, sizeof(wszBuf));
		wsprintf(wszBuf, L"MAC:%02x:%02x:%02x:%02x:%02x:%02x\n", MAC_ADDRESS[0], MAC_ADDRESS[1], MAC_ADDRESS[2], MAC_ADDRESS[3], MAC_ADDRESS[4], MAC_ADDRESS[5]);
		OutputDebugString(wszBuf);

		memset(wszBuf, 0, sizeof(wszBuf));
		wsprintf(wszBuf, L"AuthAlgorithm:%s\n", wszAuthAlgorithm);
		OutputDebugString(wszBuf);


		memset(wszBuf, 0, sizeof(wszBuf));
		wsprintf(wszBuf, L"LinkQuality=%d%%\n", uLinkQuality);
		OutputDebugString(wszBuf);

		memset(wszBuf, 0, sizeof(wszBuf));
		wsprintf(wszBuf, L"SignalQuality=%d%%\n", uSignalQuality);
		OutputDebugString(wszBuf);

		if (bCurrentConnected) {
			OutputDebugString(L"CurrentConnected=true\n");
		}else{
			OutputDebugString(L"CurrentConnected=false\n");
		}
	}
};

#endif