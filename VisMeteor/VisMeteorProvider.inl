// VisMeteorProvider.inl
//
// Copyright © 1997-2000 Microsoft Corporation, All Rights Reserved



inline const char *CVisMeteorProvider::SzNameGetIDevice(int iDevice) const
{
	assert(m_plistszConnectedDevices != 0);
	assert((iDevice >= 0)
			&& (iDevice < m_plistszConnectedDevices->CszGet()));

	return m_plistszConnectedDevices->SzGetI(iDevice);
}


inline SVisSDC CVisMeteorProvider::SdcGetIDevice(int iDevice) const
{
	assert((iDevice >= 0)
			&& (iDevice < m_mpisdcConnected.size()));

	return m_mpisdcConnected[iDevice];
}


inline long CVisMeteorProvider::LSysGetISys(int iSys) const
{
	assert((iSys >= 0) && (iSys < 10));

	return s_rglSysNum[iSys];
}


inline char *CVisMeteorProvider::SzDigGetIDig(int iDig) const
{
	assert((iDig >= 0) && (iDig < m_listszDig.CszGet()));

	return const_cast<char *> (m_listszDig.SzGetI(iDig));
}


inline char *CVisMeteorProvider::SzChannelGetIChannel(int iChannel) const
{
	assert((iChannel >= 0) && (iChannel < 5));

	return s_rgszChannel[iChannel];
}

inline const CVisMeteorBufList& CVisMeteorProvider::MeteorBufList(void) const
{
	return m_meteorbuflist;
}

inline CVisMeteorBufList& CVisMeteorProvider::MeteorBufList(void)
{
	return m_meteorbuflist;
}
