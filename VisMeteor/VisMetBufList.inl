// VisMetBufList.inl
//
// Copyright © 1997-2000 Microsoft Corporation, All Rights Reserved


inline bool CVisMeteorBuf::FIsInUse(void) const
{
	assert(FIsValid());
	return m_fInUse;
}

inline void CVisMeteorBuf::SetFIsInUse(bool f)
{
	assert(FIsValid());
	m_fInUse = f;
}

inline MIL_ID CVisMeteorBuf::Milid(void) const
{
	assert(FIsValid());
	return m_milidBuf;
}

inline EVisImEncoding CVisMeteorBuf::ImEncoding(void) const
{
	assert(FIsValid());
	return m_evisimencoding;
}

inline int CVisMeteorBuf::Dx(void) const
{
	assert(FIsValid());
	return m_size.cx;
}

inline int CVisMeteorBuf::Dy(void) const
{
	assert(FIsValid());
	return m_size.cy;
}

inline SIZE CVisMeteorBuf::Size(void) const
{
	assert(FIsValid());
	return m_size;
}

inline int CVisMeteorBuf::CbData(void) const
{
	assert(FIsValid());
	return m_cbData;
}

inline void *CVisMeteorBuf::PvData(void) const
{
	assert(FIsValid());
	return m_pvData;
}

inline class CVisMeteor *CVisMeteorBuf::PMeteor(void) const
{
	assert(FIsValid());
	return m_pMeteor;
}



inline CVisMeteorBufList::CVisMeteorBufList(void)
  : m_criticalsection(),
	m_vectorpMeteorBuf()
{
	InitializeCriticalSection(&m_criticalsection);
}

inline CVisMeteorBufList::~CVisMeteorBufList(void)
{
	DeleteCriticalSection(&m_criticalsection);

#ifdef _DEBUG
	if (!m_vectorpMeteorBuf.empty())
		OutputDebugString("\nWARNING:  Some Meteor buffers were not freed!\n");

	char szBuf[256];
	sprintf(szBuf,
			"CVisMeteorBufList::~CVisMeteorBufList called,"
			" cBufInUseCur = %d, cBufInUseHigh = %d.\n",
			CVisMeteorBuf::s_cBufInUseCur,
			CVisMeteorBuf::s_cBufInUseHigh);
	OutputDebugString(szBuf);
#endif // _DEBUG
}
