// VisMetBufList.cpp
//
// Copyright © 1997-2000 Microsoft Corporation, All Rights Reserved


#include "VisMeteorPch.h"
#include "VisMeteor.h"


CVisMeteorBuf::CVisMeteorBuf(void)
  : m_fInUse(false),
	m_milidBuf(M_NULL),
	m_evisimencoding(evisimencodingUnknown),
	m_cbData(0),
	m_pvData(0),
	m_pMeteor(0)
{
	m_size.cx = m_size.cy = 0;
}


CVisMeteorBuf::~CVisMeteorBuf(void)
{
	Deallocate();
}


bool CVisMeteorBuf::FIsValid(void) const
{
	if ((m_milidBuf != M_NULL)
			&& (m_evisimencoding != evisimencodingUnknown)
			&& (m_size.cx != 0)
			&& (m_size.cy != 0)
			&& (m_cbData != 0)
			&& (m_pvData != 0)
			&& (m_pMeteor != 0))
	{
		return true;
	}

	return false;
}


void CVisMeteorBuf::Allocate(CVisMeteor *pMeteor)
{
	assert(pMeteor != 0);
	assert(pMeteor->ImEncoding() != evisimencodingUnknown);

	Deallocate();

	// Note:  evisimencodingRGB555 is a different format than the Matrox
	// M_RGB15_PACKED format.
	// UNDONE:  We seem to get access violations when we try to use
	// planar YUV formats.  Maybe we're doing something wrong in this
	// class?
	long cBand = 3;
	long cbitPixel;
	long lBufAttribute = M_IMAGE + M_GRAB + M_PROC + M_DISP;
	switch (pMeteor->ImEncoding())
	{
	case evisimencodingUYVY:
#if defined(M_PACKED) && defined(M_BGR32) // Defined in newer versions of the MIL headers
		lBufAttribute += (M_YUV16 | M_PACKED);
#else // M_PACKED
		lBufAttribute += M_YUV16_PACKED;
#endif // M_PACKED
		cbitPixel = 16;
		break;

#ifdef LATER
	case :
		// UNDONE:  What 16-bit planar format is being used?
	#if defined(M_PLANAR) && defined(M_YUV16) // Defined in newer versions of the MIL headers
		lBufAttribute += (M_YUV16 | M_PLANAR);
	#else // M_PLANAR && M_YUV16
		lBufAttribute += M_YUV16_PLANAR;
	#endif // M_PLANAR && M_YUV16
		cbitPixel = 16;
		break;
#endif // LATER

	case evisimencodingIYUV:
#if defined(M_PLANAR) && defined(M_YUV12) // Defined in newer versions of the MIL headers
		lBufAttribute += (M_YUV12 | M_PLANAR);
#else // M_PLANAR && M_YUV12
		lBufAttribute += M_YUV12_PLANAR;
#endif // M_PLANAR && M_YUV12
		cbitPixel = 12;
		break;

	case evisimencodingYVU9:
#if defined(M_PLANAR) && defined(M_YUV9) // Defined in newer versions of the MIL headers
		lBufAttribute += (M_YUV9 | M_PLANAR);
#else // M_PLANAR && M_YUV9
		lBufAttribute += M_YUV9_PLANAR;
#endif // M_PLANAR && M_YUV9
		cbitPixel = 9;
		break;

	case evisimencodingGray8bit:
		cBand = 1;
		cbitPixel = 8;
		break;

	default:
		// evisimencodingRGBA32
		assert(pMeteor->ImEncoding() == evisimencodingRGBA32);
#if M_MIL_USE_METEOR_II
		lBufAttribute += (M_BGR32 | M_PACKED);
#endif // M_MIL_USE_METEOR_II
		cbitPixel = 32;
		break;
	}

	// Note:  We alloc on m_milidSys for the meteor because this seems to fail when
	// we try to alloc on the host.  We assume that this function gives us a buffer
	// that can be mapped to the host address space.
	m_milidBuf = MbufAllocColor(pMeteor->MilidSys(), cBand, pMeteor->SizeCur().cx,
			pMeteor->SizeCur().cy, M_UNSIGNED + 8, lBufAttribute, M_NULL);

	// Try to alloc the buffer again if MbufAllocColor fails.
	for (int iRetry = 0; ((m_milidBuf == M_NULL) && (iRetry < 400)); ++iRetry)
	{
		Sleep(5);
		m_milidBuf = MbufAllocColor(pMeteor->MilidSys(), cBand, pMeteor->SizeCur().cx,
				pMeteor->SizeCur().cy, M_UNSIGNED + 8, lBufAttribute, M_NULL);
	}

	if (m_milidBuf != M_NULL)
	{
		assert(!m_fInUse);

		m_pMeteor = pMeteor;
		m_evisimencoding = pMeteor->ImEncoding();
		m_size = pMeteor->SizeCur();
		m_cbData = cbitPixel * m_size.cx * m_size.cy;
		m_pvData = (void *) MbufInquire(m_milidBuf, M_HOST_ADDRESS, M_NULL);

#ifdef _DEBUG
		IncCBufInUse();
	}
	else
	{
		static bool fAsserted = false;
		assert(fAsserted);
		fAsserted = true;
#endif // _DEBUG
	}
}

void CVisMeteorBuf::Deallocate(void)
{
	assert(!m_fInUse);

	assert(M_NULL == 0);
	if (m_milidBuf != M_NULL)
	{
		MbufFree(m_milidBuf);

		m_milidBuf = M_NULL;
		m_pMeteor = 0;
		m_evisimencoding = evisimencodingUnknown;
		m_size.cx = m_size.cy = 0;
		m_cbData = 0;
		m_pvData = 0;

#ifdef _DEBUG
		DecCBufInUse();
	}
	else
	{
		assert(m_milidBuf == M_NULL);
		assert(m_evisimencoding == evisimencodingUnknown);
		assert(m_size.cx == 0);
		assert(m_size.cy == 0);
		assert(m_cbData == 0);
		assert(m_pvData == 0);
		assert(m_pMeteor == 0);
#endif // _DEBUG
	}
}


// UNDONE:  Use crit sect in buf list


int CVisMeteorBufList::CBufCur(const CVisMeteor *pMeteor)
{
	int cbuf = 0;

	EnterCriticalSection(&m_criticalsection);

	if (!m_vectorpMeteorBuf.empty())
	{
		std::vector<CVisMeteorBuf *>::const_iterator iteratorCur = m_vectorpMeteorBuf.begin();
		std::vector<CVisMeteorBuf *>::const_iterator iteratorLim = m_vectorpMeteorBuf.end();
		for ( ; iteratorCur != iteratorLim; ++iteratorCur)
		{
			if ((pMeteor == 0)
					|| (pMeteor == (*iteratorCur)->PMeteor()))
			{
				++ cbuf;
			}
		}
	}

	LeaveCriticalSection(&m_criticalsection);

	return cbuf;
}

int CVisMeteorBufList::CBufInUse(const CVisMeteor *pMeteor)
{
	int cbuf = 0;

	EnterCriticalSection(&m_criticalsection);

	if (!m_vectorpMeteorBuf.empty())
	{
		std::vector<CVisMeteorBuf *>::const_iterator iteratorCur = m_vectorpMeteorBuf.begin();
		std::vector<CVisMeteorBuf *>::const_iterator iteratorLim = m_vectorpMeteorBuf.end();
		for ( ; iteratorCur != iteratorLim; ++iteratorCur)
		{
			if (((pMeteor == 0)
						|| (pMeteor == (*iteratorCur)->PMeteor()))
					&& ((*iteratorCur)->FIsInUse()))
			{
				++ cbuf;
			}
		}
	}

	LeaveCriticalSection(&m_criticalsection);

	return cbuf;
}

int CVisMeteorBufList::CBufUnused(const CVisMeteor *pMeteor)
{
	int cbuf = 0;

	EnterCriticalSection(&m_criticalsection);

	if (!m_vectorpMeteorBuf.empty())
	{
		std::vector<CVisMeteorBuf *>::const_iterator iteratorCur = m_vectorpMeteorBuf.begin();
		std::vector<CVisMeteorBuf *>::const_iterator iteratorLim = m_vectorpMeteorBuf.end();
		for ( ; iteratorCur != iteratorLim; ++iteratorCur)
		{
			if (((pMeteor == 0)
						|| (pMeteor == (*iteratorCur)->PMeteor()))
					&& (!(*iteratorCur)->FIsInUse()))
			{
				++ cbuf;
			}
		}
	}

	LeaveCriticalSection(&m_criticalsection);

	return cbuf;
}


CVisMeteorBuf *CVisMeteorBufList::PBufGetPMeteor(CVisMeteor *pMeteor)
{
	assert(pMeteor != 0);
	
	CVisMeteorBuf *pBuf = 0;

	EnterCriticalSection(&m_criticalsection);

	if (!m_vectorpMeteorBuf.empty())
	{
		std::vector<CVisMeteorBuf *>::const_iterator iteratorCur = m_vectorpMeteorBuf.begin();
		std::vector<CVisMeteorBuf *>::const_iterator iteratorLim = m_vectorpMeteorBuf.end();
		for ( ; iteratorCur != iteratorLim; ++iteratorCur)
		{
			if ((pMeteor == (*iteratorCur)->PMeteor())
					&& (!(*iteratorCur)->FIsInUse()))
			{
				pBuf = (*iteratorCur);
				break;
			}
		}
	}

	if (pBuf != 0)
	{
		pBuf->SetFIsInUse(true);
		pBuf->PMeteor()->AddRef();
	}

	LeaveCriticalSection(&m_criticalsection);

	if (pBuf == 0)
	{
		pBuf = new CVisMeteorBuf;
		if (pBuf != 0)  // LATER:  Throw if OOM?
		{
			pBuf->Allocate(pMeteor);
			if (pBuf->FIsValid())
			{
				EnterCriticalSection(&m_criticalsection);

				m_vectorpMeteorBuf.push_back(pBuf);
				pBuf->SetFIsInUse(true);
				pBuf->PMeteor()->AddRef();

				LeaveCriticalSection(&m_criticalsection);
			}
			else
			{
				// Fail.
				delete pBuf;
				pBuf = 0;
			}
		}
	}

	return pBuf;
}


void CVisMeteorBufList::ReleaseBuf(CVisMeteorBuf *pBuf)
{
	assert(pBuf != 0);

	EnterCriticalSection(&m_criticalsection);

	assert(pBuf->FIsInUse());

	pBuf->SetFIsInUse(false);

	CVisMeteor *pMeteor = pBuf->PMeteor();
	if ((pBuf->ImEncoding() != pMeteor->ImEncoding())
			|| (pBuf->Size().cx != pMeteor->SizeCur().cx)
			|| (pBuf->Size().cy != pMeteor->SizeCur().cy))
	{
		RemoveBuf(pBuf);
	}

	pMeteor->Release();

	LeaveCriticalSection(&m_criticalsection);
}


void VISAPI CVisMeteorBufList::StaticReleaseBuf(void *pvData, void *pvBuf)
{
	assert(pvData != 0);
	assert(pvBuf != 0);

	CVisMeteorProvider *pMeteorProvider
			= CVisMeteorProvider::PMeteorProviderGet();

	assert(pMeteorProvider != 0);

	pMeteorProvider->MeteorBufList().ReleaseBuf((CVisMeteorBuf *) pvBuf);

	pMeteorProvider->Release();
}


void CVisMeteorBufList::ReallocBufs(CVisMeteor *pMeteor, int cBuf)
{
	assert(pMeteor != 0);

	EnterCriticalSection(&m_criticalsection);

	int cBufValid = 0;
	if (!m_vectorpMeteorBuf.empty())
	{
		// Note:  We'd like to use the reverse iterator to avoid extra copies, but the
		// erase() method takes an interator, not a reverse_iterator.
		std::vector<CVisMeteorBuf *>::iterator iteratorCur = m_vectorpMeteorBuf.begin();
		std::vector<CVisMeteorBuf *>::iterator iteratorLim = m_vectorpMeteorBuf.end();
		
		for ( ; iteratorCur != iteratorLim; ++iteratorCur)
		{
			CVisMeteorBuf *pBuf = (*iteratorCur);
			if ((pMeteor == pBuf->PMeteor()) && (!pBuf->FIsInUse()))
			{
				if ((pBuf->ImEncoding() != pMeteor->ImEncoding())
						|| (pBuf->Size().cx != pMeteor->SizeCur().cx)
						|| (pBuf->Size().cy != pMeteor->SizeCur().cy))
				{
					// Destroy the buffer.
					delete pBuf;
					
					// Remove the pointer to the buffer from the list.
					m_vectorpMeteorBuf.erase(iteratorCur);

					// Adjust the pointers used in the loop.
					-- iteratorCur;
					-- iteratorLim;
				}
				else if (++cBufValid > cBuf)
				{
					-- cBufValid;

					// Destroy the buffer.
					delete pBuf;
					
					// Remove the pointer to the buffer from the list.
					m_vectorpMeteorBuf.erase(iteratorCur);

					// Adjust the pointers used in the loop.
					-- iteratorCur;
					-- iteratorLim;
				}
			}
		}
	}

	LeaveCriticalSection(&m_criticalsection);

	// Try to allocate more buffers, if needed.
	while (cBufValid < cBuf)
	{
		CVisMeteorBuf *pBuf = new CVisMeteorBuf;
		if (pBuf == 0)
			break;  // LATER:  Throw if OOM?

		pBuf->Allocate(pMeteor);
		if (pBuf->FIsValid())
		{
			EnterCriticalSection(&m_criticalsection);

			m_vectorpMeteorBuf.push_back(pBuf);
			++ cBufValid;

			LeaveCriticalSection(&m_criticalsection);
		}
		else
		{
			// Fail.
			delete pBuf;
			break;
		}
	}
}

void CVisMeteorBufList::RemoveBufs(const CVisMeteor *pMeteor)
{
	EnterCriticalSection(&m_criticalsection);

	if (!m_vectorpMeteorBuf.empty())
	{
		// Note:  We'd like to use the reverse iterator to avoid extra copies, but the
		// erase() method takes an interator, not a reverse_iterator.
		std::vector<CVisMeteorBuf *>::iterator iteratorCur = m_vectorpMeteorBuf.begin();
		std::vector<CVisMeteorBuf *>::iterator iteratorLim = m_vectorpMeteorBuf.end();
		
		for ( ; iteratorCur != iteratorLim; ++iteratorCur)
		{
			if (((pMeteor == 0)
						|| (pMeteor == (*iteratorCur)->PMeteor()))
					&& (!(*iteratorCur)->FIsInUse()))
			{
				// Destroy the buffer.
				delete (*iteratorCur);
				
				// Remove the pointer to the buffer from the list.
				m_vectorpMeteorBuf.erase(iteratorCur);

				// Adjust the pointers used in the loop.
				-- iteratorCur;
				-- iteratorLim;
			}
		}
	}

	LeaveCriticalSection(&m_criticalsection);
}

void CVisMeteorBufList::RemoveBuf(CVisMeteorBuf *pMeteorBuf)
{
	assert(pMeteorBuf != 0);

	EnterCriticalSection(&m_criticalsection);

	assert(!(pMeteorBuf->FIsInUse()));

	if (!m_vectorpMeteorBuf.empty())
	{
		std::vector<CVisMeteorBuf *>::iterator iteratorCur = m_vectorpMeteorBuf.begin();
		std::vector<CVisMeteorBuf *>::iterator iteratorLim = m_vectorpMeteorBuf.end();
		for ( ; iteratorCur != iteratorLim; ++iteratorCur)
		{
			if (pMeteorBuf == (*iteratorCur))
			{
				// Remove the pointer to the buffer from the list.
				m_vectorpMeteorBuf.erase(iteratorCur);
				// We don't adjust the loop pointers here because we're
				// going to break out of the loop.
				break;
			}
		}
	}

	delete pMeteorBuf;

	LeaveCriticalSection(&m_criticalsection);
}



#ifdef _DEBUG

int CVisMeteorBuf::s_cBufInUseCur = 0;
int CVisMeteorBuf::s_cBufInUseHigh = 0;


// UNDONE:  Add an assert that s_cBufInUseCur is 0 on exit.


void CVisMeteorBuf::IncCBufInUse(void)
{
	if (++s_cBufInUseCur > s_cBufInUseHigh)
	{
		assert(s_cBufInUseCur == s_cBufInUseHigh + 1);
		++s_cBufInUseHigh;
	}
}


void CVisMeteorBuf::DecCBufInUse(void)
{
	--s_cBufInUseCur;
	assert(s_cBufInUseCur >= 0);
}
#endif // _DEBUG
