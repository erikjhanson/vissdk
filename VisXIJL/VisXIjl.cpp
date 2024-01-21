// VisXIjl.cpp
//
// Copyright © 2000 Microsoft Corporation, All Rights Reserved

#include "VisWin.h"

#include "ole2.h"

// Note:  If you get an error C2006 when compiliing the #include line,
// do a clean build to work around the error.
#ifdef VIS_IPL_IJL_H
	#include VIS_IPL_IJL_H
#else // VIS_IPL_IJL_H
	#error VIS_IPL_IJL_H needs to be defined in VisLocalInfo.h
#endif // VIS_IPL_IJL_H

#include "VisXIjl.h"


VisXIjlExport CVisXIjl* STDAPICALLTYPE VisXIjl(void)
{
	static CVisXIjl visIjl;
	return &visIjl;
}

CVisXIjl::CVisXIjl(void)
	:m_cRef(0),
	 m_nIjlError(IJL_OK)
{
}

CVisXIjl::~CVisXIjl(void)
{
}

STDMETHODIMP CVisXIjl::QueryInterface(THIS_ REFIID riid, void **ppvObject)
{
	HRESULT hrRet;

	if (ppvObject == 0)
	{
		hrRet = ResultFromScode(E_POINTER);
	}
	else if (riid == IID_IUnknown)
	{
		hrRet = ResultFromScode(S_OK);
		*ppvObject = this;
		AddRef();
	}
	else
	{
		*ppvObject = 0;
		hrRet = ResultFromScode(E_NOINTERFACE);
	}

	return hrRet;
}

STDMETHODIMP_(ULONG) CVisXIjl::AddRef(THIS)
{
	return InterlockedIncrement( &m_cRef );
}

STDMETHODIMP_(ULONG) CVisXIjl::Release(THIS)
{
	ULONG cRef = InterlockedDecrement(&m_cRef);

	if (cRef == 0)
		delete this;

	return cRef;
}

//------------------------------------------------------------------

int CVisXIjl::FReadIjlHeader(SVisIjlData& ijldata)
{
	int iRet = 1;

    ClearError();

	JPEG_CORE_PROPERTIES *pijlprops = new JPEG_CORE_PROPERTIES;
    ZeroMemory( pijlprops, sizeof( JPEG_CORE_PROPERTIES ) );
	ijldata.wReserved = sizeof( JPEG_CORE_PROPERTIES );
	ijldata.pvReserved = pijlprops;

	if((m_nIjlError = ijlInit(pijlprops)) == IJL_OK)
	{
		pijlprops->JPGFile = const_cast<char*>(ijldata.m_filename);
		m_nIjlError = ijlRead(pijlprops, IJL_JFILE_READPARAMS);
		if( m_nIjlError == IJL_OK )
		{

			ijldata.m_dxImage = pijlprops->JPGWidth;
			ijldata.m_dyImage = pijlprops->JPGHeight;

			ijldata.m_cbitPixel = pijlprops->DIBChannels * 8;

			// UNDONE:  Does read set DIBPadBytes correctly?
			pijlprops->DIBPadBytes = IJL_DIB_PAD_BYTES(pijlprops->DIBWidth,
					pijlprops->DIBChannels);
			ijldata.m_cbRow = pijlprops->DIBPadBytes;
			ijldata.m_fHasAlpha = false;
			iRet = 0;
		}
	}

	if (iRet)
	{
		ijlFree( ( JPEG_CORE_PROPERTIES * ) ijldata.pvReserved );
		delete ( JPEG_CORE_PROPERTIES * ) ijldata.pvReserved;
		ijldata.pvReserved = 0;
	}

	return iRet;
}

int CVisXIjl::FReadIjlBody(SVisIjlData& ijldata)
{
	int iRet = 1;

    ClearError();

	JPEG_CORE_PROPERTIES *pijlprops
			= ( JPEG_CORE_PROPERTIES * ) ijldata.pvReserved;

	if (pijlprops != 0)
	{
		pijlprops->DIBWidth = ijldata.m_dxImage;
		pijlprops->DIBHeight = ijldata.m_dyImage;
		pijlprops->DIBBytes = ijldata.m_pbImageFirst;
		m_nIjlError = ijlRead( pijlprops, IJL_JFILE_READWHOLEIMAGE );
		if( m_nIjlError == IJL_OK )
			iRet = 0;

		ijlFree( ( JPEG_CORE_PROPERTIES * ) ijldata.pvReserved );
		delete ( JPEG_CORE_PROPERTIES * ) ijldata.pvReserved;
		ijldata.pvReserved = 0;
	}

	return iRet;
}

int CVisXIjl::FWriteIjlHeader(SVisIjlData& ijldata)
{
  	int iRet = 1;


    ClearError();

	JPEG_CORE_PROPERTIES *pijlprops = new JPEG_CORE_PROPERTIES;
    ZeroMemory( pijlprops, sizeof( JPEG_CORE_PROPERTIES ) );
	ijldata.wReserved = sizeof( JPEG_CORE_PROPERTIES );
	ijldata.pvReserved = pijlprops;

	if((m_nIjlError = ijlInit(pijlprops)) == IJL_OK)
	{
        // Setup DIB
        pijlprops->DIBWidth    = ijldata.m_dxImage;
        pijlprops->DIBHeight   = ijldata.m_dyImage;
        pijlprops->DIBPadBytes = IJL_DIB_PAD_BYTES(pijlprops->DIBWidth, 3);

        // Setup JPEG
		pijlprops->JPGFile = const_cast<char*>(ijldata.m_filename);
        pijlprops->JPGWidth  = ijldata.m_dxImage;
        pijlprops->JPGHeight = ijldata.m_dyImage;
        pijlprops->jquality = ijldata.m_wQuality;

		m_nIjlError = ijlWrite( pijlprops, IJL_JFILE_WRITEHEADER );
		if ( m_nIjlError == IJL_OK )
			iRet = 0;
	}

	if (iRet)
	{
		ijlFree( ( JPEG_CORE_PROPERTIES * ) ijldata.pvReserved );
		delete ( JPEG_CORE_PROPERTIES * ) ijldata.pvReserved;
		ijldata.pvReserved = 0;
	}

	return iRet;
}

int CVisXIjl::FWriteIjlBody(SVisIjlData& ijldata)
{
	int iRet = 1;

    ClearError();

	JPEG_CORE_PROPERTIES *pijlprops
			= ( JPEG_CORE_PROPERTIES * ) ijldata.pvReserved;

	if (pijlprops != 0)
	{
		pijlprops->DIBBytes = ijldata.m_pbImageFirst;
		m_nIjlError = ijlWrite( pijlprops, IJL_JFILE_WRITEWHOLEIMAGE );
		if( m_nIjlError == IJL_OK )
			iRet = 0;

		ijlFree( ( JPEG_CORE_PROPERTIES * ) ijldata.pvReserved );
		delete ( JPEG_CORE_PROPERTIES * ) ijldata.pvReserved;
		ijldata.pvReserved = 0;
	}

	return iRet;
}

bool CVisXIjl::FHadError(void)
{
	return (m_nIjlError != (int)IJL_OK);
}

void CVisXIjl::ClearError(void)
{
	m_nIjlError = (int)IJL_OK;
}

const char* CVisXIjl::SzLastErrorMsg(void)
{
	return (ijlErrorStr((IJLERR)m_nIjlError));
}

