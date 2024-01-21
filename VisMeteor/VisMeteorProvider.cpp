
// CVisMeteorProvider.cpp
//
// Copyright © 1997-2000 Microsoft Corporation, All Rights Reserved


#include "VisMeteorPch.h"
#include "VisMeteor.h"


#include <string>


const char *CVisMeteorProvider::s_szID = "VisMeteor";

#ifdef _DEBUG
const char *CVisMeteorProvider::s_szName = "Matrox Meteor Card";
#else // _DEBUG
const char *CVisMeteorProvider::s_szName = "Matrox Meteor Card (Debug)";
#endif // _DEBUG


CVisMeteorProvider *CVisMeteorProvider::s_pMeteorProvider = 0;


CVisMeteorProvider *CVisMeteorProvider::PMeteorProviderGet(void)
{
	if (s_pMeteorProvider == 0)
		s_pMeteorProvider = new CVisMeteorProvider;
	else
		s_pMeteorProvider->AddRef();

	return s_pMeteorProvider;
}


long CVisMeteorProvider::s_rglSysNum[16] =
{
	M_DEV0,  M_DEV1,  M_DEV2,  M_DEV3,
	M_DEV4,  M_DEV5,  M_DEV6,  M_DEV7,
	M_DEV8,  M_DEV9,  M_DEV10, M_DEV11,
	M_DEV12, M_DEV13, M_DEV14, M_DEV15
};

char *CVisMeteorProvider::s_rgszDigFormat[15] =
{
	"M_RS170",
	"M_RS170_LOW",
	"M_CCIR",
	"M_NTSC",
	"M_NTSC_RGB",
	"M_NTSC_YC",
	"M_NTSC_LOW",
	"M_NTSC_YC_LOW",
	"M_PAL",
	"M_PAL_RGB",
	"M_PAL_YC",
	"M_SECAM",
	"M_SECAM_RGB",
	"M_RS170_VIA_RGB",
	"M_CCIR_VIA_RGB"
};

char *CVisMeteorProvider::s_rgszChannel[5] =
{
	"Default",
	"Ch0",
	"Ch1",
	"Ch2",
	"Ch3"
};


CVisMeteorProvider::CVisMeteorProvider(void)
  : m_cRef(1),
	m_plistszConnectedDevices(0),
	m_milidApp(M_NULL),
	m_mpisdcConnected(),
	m_listszDig()
{
	assert(s_pMeteorProvider == 0);

	m_milidApp = MappAlloc(M_QUIET, M_NULL);

	// Don't display error message to the user.
	if (m_milidApp != M_NULL)
	{
		MappControl(M_ERROR, M_PRINT_DISABLE);
		MappControl(M_TRACE, M_PRINT_DISABLE);
		MappControl(M_MEMORY, M_COMPENSATION_ENABLE);
	}
}

CVisMeteorProvider::~CVisMeteorProvider(void)
{
	s_pMeteorProvider = 0;
	
	if (m_milidApp != M_NULL)
		MappFree(m_milidApp);

	if (m_plistszConnectedDevices != 0)
	{
		m_plistszConnectedDevices->Release();
		m_plistszConnectedDevices = 0;
	}
}



STDMETHODIMP CVisMeteorProvider::QueryInterface(THIS_ REFIID riid,
		void **ppvObject)
{
	HRESULT hrRet;

	if (ppvObject == 0)
	{
		hrRet = ResultFromScode(E_POINTER);
	}
	else if ((riid == IID_IUnknown)
				|| (riid == IID_IVisImSrcProvider))
	{
		hrRet = ResultFromScode(S_OK);
		*ppvObject = (IVisImSrcProvider *) this;
		AddRef();
	}
	else
	{
		*ppvObject = 0;
		hrRet = ResultFromScode(E_NOINTERFACE);
	}

	return hrRet;
}

STDMETHODIMP_(ULONG) CVisMeteorProvider::AddRef(THIS)
{
	assert(m_cRef > 0);

	return ++m_cRef;
}

STDMETHODIMP_(ULONG) CVisMeteorProvider::Release(THIS)
{
	unsigned long cRef = --m_cRef;

	if (cRef == 0)
		delete this;

	return cRef;
}



STDMETHODIMP CVisMeteorProvider::HasConnectedDevices(THIS)
{
	if (m_plistszConnectedDevices == 0)
		FindConnectedDevices();
	
	HRESULT hrRet;

	if (m_plistszConnectedDevices == 0)
		hrRet = ResultFromScode(E_FAIL);
	else if (m_plistszConnectedDevices->CszGet() == 0)
		hrRet = ResultFromScode(S_FALSE);
	else
		hrRet = ResultFromScode(S_OK);

	return hrRet;
}

STDMETHODIMP CVisMeteorProvider::HasSelectionDialog(THIS)
{
	// Meteor devices do not have their own selection dialog.
	return ResultFromScode(S_FALSE);
}

STDMETHODIMP_(LONG) CVisMeteorProvider::DoSelectionDialog(THIS_
		HWND hwndParent)
{
	// Meteor devices do not have their own selection dialog.
	return - 1;
}

STDMETHODIMP CVisMeteorProvider::GetConnectedDeviceList(THIS_
		IVisListSz **ppIVisListSz)
{
	if (m_plistszConnectedDevices == 0)
		FindConnectedDevices();
	
	HRESULT hrRet;

	if (ppIVisListSz == 0)
	{
		hrRet = ResultFromScode(E_POINTER);
	}
	else
	{
		if (m_plistszConnectedDevices == 0)
			FindConnectedDevices();
		
		if (m_plistszConnectedDevices == 0)
			hrRet = ResultFromScode(E_FAIL);
		else
			hrRet = ResultFromScode(S_OK);

		*ppIVisListSz = m_plistszConnectedDevices;

		if (m_plistszConnectedDevices != 0)
			m_plistszConnectedDevices->AddRef();
	}

	return hrRet;
}

STDMETHODIMP CVisMeteorProvider::GetDeviceSz(THIS_
		IVisImSrcDevice **ppIVisImSrcDevice, LPCSTR sz)
{
	if (m_plistszConnectedDevices == 0)
		FindConnectedDevices();
	
	HRESULT hrRet;

	if (ppIVisImSrcDevice == 0)
	{
		hrRet = ResultFromScode(E_POINTER);
	}
	else if (sz == 0)
	{
		hrRet = ResultFromScode(E_INVALIDARG);
		*ppIVisImSrcDevice = 0;
	}
	else if (m_plistszConnectedDevices == 0)
	{
		hrRet = ResultFromScode(E_FAIL);
		*ppIVisImSrcDevice = 0;
	}
	else if ((sz[0] >= '0') && (sz[0] <= '9') && (sz[1] == 0))
	{
		hrRet = GetDeviceI(ppIVisImSrcDevice, sz[0] - '0');
	}
	else
	{
		hrRet = GetDeviceI(ppIVisImSrcDevice,
				m_plistszConnectedDevices->IGetSz(sz));

		if (FAILED(hrRet))
		{
			hrRet = FindDeviceSz(ppIVisImSrcDevice, sz);
		}
	}

	return hrRet;
}

STDMETHODIMP CVisMeteorProvider::GetDeviceI(THIS_
		IVisImSrcDevice **ppIVisImSrcDevice, LONG i)
{
	if (m_plistszConnectedDevices == 0)
		FindConnectedDevices();
	
	HRESULT hrRet;

	if (ppIVisImSrcDevice == 0)
	{
		hrRet = ResultFromScode(E_POINTER);
	}
	else if (m_plistszConnectedDevices == 0)
	{
		hrRet = ResultFromScode(E_FAIL);
		*ppIVisImSrcDevice = 0;
	}
	else if ((i < 0) || (i >= m_plistszConnectedDevices->CszGet()))
	{
		hrRet = ResultFromScode(E_INVALIDARG);
		*ppIVisImSrcDevice = 0;
	}
	else
	{
		SVisSDC sdc = m_mpisdcConnected[i];
		assert(m_listszDig.SzGetI(sdc.iDig) != 0);

		CVisMeteor *pMeteor = new CVisMeteor(this, i, sdc.iSys,
				m_listszDig.SzGetI(sdc.iDig), sdc.iChannel);
		
		if (pMeteor == 0)
			hrRet = ResultFromScode(E_OUTOFMEMORY);
		else
			hrRet = pMeteor->Init();

		if (FAILED(hrRet))
		{
			delete pMeteor;
			pMeteor = 0;
		}

		*ppIVisImSrcDevice = pMeteor;
	}

	return hrRet;
}

STDMETHODIMP CVisMeteorProvider::GetSzID(THIS_ LPSTR sz, LONG *pcb)
{
	return VisGetSzPcbFromSz(sz, pcb, s_szID);
}

STDMETHODIMP CVisMeteorProvider::GetSzName(THIS_ LPSTR sz, LONG *pcb)
{
	return VisGetSzPcbFromSz(sz, pcb, s_szName);
}


void  CVisMeteorProvider::FindConnectedDevices(void)
{
	if ((m_milidApp != M_NULL) && (m_plistszConnectedDevices == 0))
	{
		CVisListSz *plistsz = new CVisListSz;
		assert(m_listszDig.CszGet() == 0);

#if M_MIL_USE_METEOR_II
		void *pvSystemTypePtr = M_SYSTEM_METEOR_II;
#else // M_MIL_USE_METEOR_II
		void *pvSystemTypePtr = M_SYSTEM_METEOR;
#endif // M_MIL_USE_METEOR_II
		for (int iSysNum = 0; iSysNum < e_iMeteorDevLim; ++iSysNum)
		{
			const char *szKeySysBase = VIS_SZ_REG_IMSRC_DEVS "\\VisMeteor\\Dev";
			char szSysNum[20];
			assert(iSysNum < 10);
			szSysNum[0] = '0' + iSysNum;
			szSysNum[1] = 0;
			std::string strKeySys = szKeySysBase;
			strKeySys += szSysNum;

			if (!VisInlineFIsSzKeyAvailable(strKeySys.c_str()))
				continue;

			MIL_ID milidSys = MsysAlloc(pvSystemTypePtr,
					s_rglSysNum[iSysNum], M_PARTIAL, M_NULL);

			if (milidSys == M_NULL)
				continue;

			// Look for available subkeys, and try to allocate digitizers
			// using available subkey names.
			IVisListSz *plistszDig = VisInlineGetSubkeyList(HKEY_LOCAL_MACHINE,
					strKeySys.c_str(), eviskeylistAvailID);
			if (plistszDig != 0)
			{
				for (int iDig = 0; iDig < plistszDig->CszGet(); ++iDig)
				{
					MIL_ID milidDig = MdigAlloc(milidSys, M_DEFAULT,
							const_cast<char *>(plistszDig->SzGetI(iDig)),
							M_DEFAULT, M_NULL);

					if (milidDig != M_NULL)
					{
						// Remember the string the specifies this digitizer type.
						int iDigInListSzDig = m_listszDig.IGetSz(plistszDig->SzGetI(iDig));
						if (iDigInListSzDig == - 1)
						{
							m_listszDig.AddSz(plistszDig->SzGetI(iDig));
							iDigInListSzDig = m_listszDig.IGetSz(plistszDig->SzGetI(iDig));
							assert(iDigInListSzDig >= 0);
						}

						std::string strKeyDig = strKeySys;
						strKeyDig += "\\";
						strKeyDig += plistszDig->SzGetI(iDig);

						// Look for available channels.
						for (int iChannel = 0; iChannel < 5; ++iChannel)
						{
							std::string strKeyChannel = strKeyDig;
							strKeyChannel += "\\";
							strKeyChannel += s_rgszChannel[iChannel];

							HKEY hkeyChannel;
							if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
									strKeyChannel.c_str(), 0, KEY_QUERY_VALUE,
									&hkeyChannel) == ERROR_SUCCESS)
							{
								DWORD dwType;
								char szValue[256];
								DWORD cbValue = sizeof(szValue);
								if (RegQueryValueEx(hkeyChannel, "Available",
												0, &dwType,
												(BYTE *) szValue, &cbValue)
												== ERROR_SUCCESS)
								{
									if (((dwType == REG_SZ)
												&& (szValue[0] == '1')
												&& (szValue[1] == 0))
											|| ((dwType == REG_DWORD)
												&& (*((DWORD *) szValue) == 1)))
									{
										// This channel is available, so we
										// can add it to the list of available
										// channels.
										cbValue = sizeof(szValue);
										if ((RegQueryValueEx(hkeyChannel, 0,
														0, &dwType,
														(BYTE *) szValue, &cbValue)
														== ERROR_SUCCESS)
												&& (dwType == REG_SZ))
										{
											// Add the string describing this device.
											plistsz->AddSz(szValue);

											// Remember the information needed to create this device.
											SVisSDC sdc;
											sdc.iSys = iSysNum;
											sdc.iDig = iDigInListSzDig;
											sdc.iChannel = iChannel;
											m_mpisdcConnected.push_back(sdc);
										}
									}
								}

								RegCloseKey(hkeyChannel);
							}
						}

						MdigFree(milidDig);
					}
				}
			}

			MsysFree(milidSys);
		}

		m_plistszConnectedDevices = plistsz;

		assert(m_plistszConnectedDevices->CszGet() == m_mpisdcConnected.size());
	}
}

HRESULT CVisMeteorProvider::FindDeviceSz(IVisImSrcDevice **ppIVisImSrcDevice,
		LPCSTR sz)
{
	// This function attempts to parse a string (and use default registry
	// entries) to find a device.
	HRESULT hrRet;

	assert(sz != 0);
	assert(ppIVisImSrcDevice != 0);
	assert(m_plistszConnectedDevices != 0);

	std::string strDeviceID = sz;
	std::string::size_type ichNil = std::string::npos;
	
	// Find the default card by looking in the registry.
	int iSys = 0;
	HKEY hkeyT;
	std::string strReg = VIS_SZ_REG_IMSRC_DEVS "\\VisMeteor";
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, strReg.c_str(),
			0, KEY_QUERY_VALUE, &hkeyT) == ERROR_SUCCESS)
	{
		DWORD dwType;
		char szT[256];
		DWORD cbT = sizeof(szT);
		if (RegQueryValueEx(hkeyT, "Default", 0, &dwType,
				(BYTE *) &szT, &cbT) == ERROR_SUCCESS)
		{
			if (dwType == REG_DWORD)
			{
				DWORD dwT = *((DWORD *) szT);
				if ((dwT >= 0) && (dwT <= 9))
					iSys = dwT;
			}
			else if (dwType == REG_SZ)
			{
				const char *pch = szT;
				while ((*pch != 0)
						&& ((*pch < '0') || (*pch > '9')))
				{
					++ pch;
				}
				
				
				if ((*pch >= '0') && (*pch <= '9'))
					iSys = *pch - '0';
			}
		}

		RegCloseKey(hkeyT);
	}

	// If a card is specified in the string, use it instead.
	if ((strDeviceID.find("Card ") != ichNil)
			|| (strDeviceID.find("card ") != ichNil))
	{
		int ich = strDeviceID.find("Card ") + strlen("Card ");
		while (ich == ' ')
			++ ich;
		if ((sz[ich] >= '0') && (sz[ich] <= '9'))
			iSys = sz[ich] - '0';
	}
	
	// Find the default channel and digitizer by looking in the registry.
	int iChannel = 0;
	std::string strDig = "M_NTSC_YC";

	strReg += "\\Dev";
	strReg += ((char) ('0' + iSys));
	strReg += "\\Default";

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, strReg.c_str(),
			0, KEY_QUERY_VALUE, &hkeyT) == ERROR_SUCCESS)
	{
		DWORD dwType;
		char szT[256];
		DWORD cbT = sizeof(szT);
		if (RegQueryValueEx(hkeyT, "Channel", 0, &dwType,
				(BYTE *) &szT, &cbT) == ERROR_SUCCESS)
		{
			if (dwType == REG_DWORD)
			{
				DWORD dwT = *((DWORD *) szT);
				if ((dwT >= 0) && (dwT <= 3))
					iChannel = dwT + 1;
			}
			else if (dwType == REG_SZ)
			{
				const char *pch = szT;
				while ((*pch != 0)
						&& ((*pch < '0') || (*pch > '9')))
				{
					++ pch;
				}
				
				
				if ((*pch >= '0') && (*pch <= '9'))
					iChannel = *pch - '0' + 1;
				else if (strcmpi(szT, "Default") == 0)
					iChannel = 0;
			}
		}

		if (RegQueryValueEx(hkeyT, "Dig", 0, &dwType,
				(BYTE *) &szT, &cbT) == ERROR_SUCCESS)
		{
			if (dwType == REG_DWORD)
			{
				DWORD dwT = *((DWORD *) szT);
				if ((dwT >= 0) && (dwT <= 15))
					strDig = s_rgszDigFormat[dwT];
			}
			else if (dwType == REG_SZ)
			{
				strDig = szT;
			}
		}

		RegCloseKey(hkeyT);
	}

	// If a channel is specified in the string, use it instead.
	if ((strDeviceID.find("Channel ") != ichNil)
			|| (strDeviceID.find("channel ") != ichNil))
	{
		int ich = strDeviceID.find("Channel ") + strlen("Channel ");
		while (ich == ' ')
			++ ich;
		if ((sz[ich] >= '0') && (sz[ich] <= '3'))
			iChannel = sz[ich] - '0';
	}
	
	// Find the digitizer.
	// (To do this, we need to parse the string.)
	// UNDONE:  Get the default from the registry.  Don't use
	// a hard-coded value here.
	if ((strDeviceID.find(" NTSC ") != ichNil)
			|| (strDeviceID.find("_NTSC") != ichNil))
	{
		if ((strDeviceID.find(" Low ") != ichNil)
				|| (strDeviceID.find(" low ") != ichNil)
				|| (strDeviceID.find("_LOW") != ichNil))
		{
			if ((strDeviceID.find(" YC ") != ichNil)
					|| (strDeviceID.find(" Y/C ") != ichNil)
					|| (strDeviceID.find(" Y / C ") != ichNil)
					|| (strDeviceID.find("_YC") != ichNil))
			{
				strDig = "M_NTSC_YC_LOW";
			}
			else
			{
				strDig = "M_NTSC_LOW";
			}
		}
		else
		{
			if ((strDeviceID.find(" YC ") != ichNil)
					|| (strDeviceID.find(" Y/C ") != ichNil)
					|| (strDeviceID.find(" Y / C ") != ichNil)
					|| (strDeviceID.find("_YC") != ichNil))
			{
				strDig = "M_NTSC_YC";
			}
			else if ((strDeviceID.find(" RGB ") != ichNil)
					|| (strDeviceID.find("_RGB") != ichNil))
			{
				strDig = "M_NTSC_RGB";
			}
			else
			{
				strDig = "M_NTSC";
			}
		}
	}
	else if ((strDeviceID.find(" RS170 ") != ichNil)
			|| (strDeviceID.find("_RS170") != ichNil))
	{
		if ((strDeviceID.find(" RGB ") != ichNil)
				|| (strDeviceID.find("_RGB") != ichNil))
		{
			strDig = "M_RS170_VIA_RGB";
		}
		else if ((strDeviceID.find(" Low ") != ichNil)
				|| (strDeviceID.find(" low ") != ichNil)
				|| (strDeviceID.find("_LOW") != ichNil))
		{
			strDig = "M_RS170_LOW";
		}
		else 
		{
			strDig = "M_RS170";
		}
	}
	else if (strDeviceID.find(" CCIR ") != ichNil)
	{
		if ((strDeviceID.find(" RGB ") != ichNil)
				|| (strDeviceID.find("_RGB") != ichNil))
		{
			strDig = "M_CCIR_VIA_RGB";
		}
		else 
		{
			strDig = "M_CCIR";
		}
	}
	else if ((strDeviceID.find(" PAL ") != ichNil)
			|| (strDeviceID.find("_PAL") != ichNil))
	{
		if ((strDeviceID.find(" YC ") != ichNil)
				|| (strDeviceID.find(" Y/C ") != ichNil)
					|| (strDeviceID.find(" Y / C ") != ichNil)
					|| (strDeviceID.find("_YC") != ichNil))
		{
			strDig = "M_PAL_YC";
		}
		else if ((strDeviceID.find(" RGB ") != ichNil)
				|| (strDeviceID.find("_RGB") != ichNil))
		{
			strDig = "M_PAL_RGB";
		}
		else
		{
			strDig = "M_PAL";
		}
	}
	else if ((strDeviceID.find(" SECAM ") != ichNil)
			|| (strDeviceID.find("_SECAM") != ichNil))
	{
		if ((strDeviceID.find(" RGB ") != ichNil)
				|| (strDeviceID.find("_RGB") != ichNil))
		{
			strDig = "M_SECAM_RGB";
		}
		else
		{
			strDig = "M_SECAM";
		}
	}
	
	// Find the index of the digitizer string in out list of known
	// digitizers.
	int iDigInListSzDig = m_listszDig.IGetSz(strDig.c_str());
	if (iDigInListSzDig == - 1)
	{
		m_listszDig.AddSz(strDig.c_str());
		iDigInListSzDig = m_listszDig.IGetSz(strDig.c_str());
		assert(iDigInListSzDig >= 0);
	}

	// Create an SDC structure describing this device.
	SVisSDC sdc;
	sdc.iSys = iSys;
	sdc.iDig = iDigInListSzDig;
	sdc.iChannel = iChannel;
	
	// Check for an SDC match in m_mpisdcConnected.  If no
	// match is found, add this information to m_mpisdcConnected.
	for (int isdc = 0; isdc < m_mpisdcConnected.size(); ++isdc)
	{
		if (m_mpisdcConnected[isdc] == sdc)
			break;
	}
	if (isdc == m_mpisdcConnected.size())
		m_mpisdcConnected.push_back(sdc);

	CVisMeteor *pMeteor = new CVisMeteor(this, isdc, sdc.iSys,
			m_listszDig.SzGetI(sdc.iDig), sdc.iChannel);
	
	if (pMeteor == 0)
		hrRet = ResultFromScode(E_OUTOFMEMORY);
	else
		hrRet = pMeteor->Init();

	if (FAILED(hrRet))
	{
		delete pMeteor;
		pMeteor = 0;
	}

	*ppIVisImSrcDevice = pMeteor;

	return hrRet;
}
