// VisIjlHandlerMagick.cpp
//
// Copyright © 2000 Microsoft Corporation, All Rights Reserved


#include "VisCorePch.h"
#include "VisCoreProj.h"

#include "..\VisXIjl\VisXIjl.h"
#include "..\VisImSrc\VisConvertCbitToCbit.inl"

#ifdef _DEBUG
#undef new
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////
//  
// @class
//
// File handler to read and write image files using the Ijl
// library.
//
// @base public | CVisFileHandler
//
// @xref <c CVisFileHandler>
//
////////////////////////////////////////////////////////////////////////////
class VisCoreExport CVisIjlFileHandler : public CVisFileHandler
{
// @access Public members
public:
	//------------------------------------------------------------------
	// @group Constructor and destructor


	// @cmember
   	// 
	CVisIjlFileHandler(void);

	// @cmember
   	// 
	~CVisIjlFileHandler(void);


	//------------------------------------------------------------------
	// @group Virtual functions

	// @cmember
	// Does the handler support images of the specified pixel type with
	// the specifed number of bands?
    virtual BOOL SupportsPixelType(int evispixfmt, int nbands);

	// @cmember
	// Does the specified extension match the extension of a file type
	// supported by this file handler?
    virtual BOOL MatchExtension(const char *szFiletype);

	// @cmember
	// Should the stream be opened outside of this file handler.
	virtual BOOL ExternallyOpenStream(void);

	// @cmember
 	// Attempt to read the file header.  Return TRUE is successful.
 	// (This may return FALSE or throw an exception if not successful.)
    virtual BOOL ReadHeader(SVisFileDescriptor &fd, class CVisImageBase &img);

	// @cmember
  	// Attempt to read the file body.  Return TRUE is successful.
 	// (This may return FALSE or throw an exception if not successful.)
    virtual BOOL ReadBody(SVisFileDescriptor &fd, class CVisImageBase &img);

	// @cmember
  	// Attempt to write the file header.  Return TRUE is successful.
 	// (This may return FALSE or throw an exception if not successful.)
    virtual BOOL WriteHeader(SVisFileDescriptor &fd, class CVisImageBase &img);

	// @cmember
   	// Attempt to write the file body.  Return TRUE is successful.
 	// (This may return FALSE or throw an exception if not successful.)
    virtual BOOL WriteBody(SVisFileDescriptor &fd, class CVisImageBase &img);


	// Helper function
	bool IsAvailable(void);


// @access:(IMPL) Private members
private:
	HINSTANCE m_hinstIjlDll;

	// UNDONE:  Store the IJL data in the file descriptor to support
	// multi-threaded access.
	SVisIjlData m_ijldata;

	CVisXIjl* m_pIjl;
	bool m_fLookedForIjlDLL;

}; 



CVisIjlFileHandler::CVisIjlFileHandler(void)
  : m_hinstIjlDll(0),
	m_pIjl(0),
	m_fLookedForIjlDLL(false)
{
}




bool CVisIjlFileHandler::IsAvailable(void)
{
	if (!m_fLookedForIjlDLL)
	{
		m_fLookedForIjlDLL = true;

#ifdef _DEBUG
		const char *szIjlDll = VIS_XIJL_DEBUG_BASE_NAME ".dll";
#else // _DEBUG
		const char *szIjlDll = VIS_XIJL_BASE_NAME ".dll";
#endif // _DEBUG

		m_hinstIjlDll = LoadLibrary(szIjlDll);

		if (m_hinstIjlDll != 0)
		{
			const char *szVisXIjlExport = "?VisXIjl@@YGPAVCVisXIjl@@XZ";

			T_PfnVisIjl pfnVisIjl = (T_PfnVisIjl)
					GetProcAddress(m_hinstIjlDll, szVisXIjlExport);

			DWORD dwError = GetLastError();
			if (pfnVisIjl == 0)
			{
				FreeLibrary(m_hinstIjlDll);
				m_hinstIjlDll = 0;
			}
			else
			{
				m_pIjl = pfnVisIjl();
			}
		}
	}
	return (m_hinstIjlDll != 0);
}

// @cmember
// 
CVisIjlFileHandler::~CVisIjlFileHandler(void)
{
	if (m_hinstIjlDll != 0)
		FreeLibrary(m_hinstIjlDll);

	m_hinstIjlDll = 0;
}



CVisFileHandler *Ijl_FileHandler_Init()
{
    CVisIjlFileHandler* ijl_handler = new CVisIjlFileHandler;

    return ijl_handler;
}


BOOL CVisIjlFileHandler::MatchExtension(const char *szFiletype)
{
	if (IsAvailable())
	{
		if (MatchIgnoringCase(szFiletype, "JPEG") ||
			MatchIgnoringCase(szFiletype, "JPG"))
			return 1;
	}
    return 0;
}


BOOL CVisIjlFileHandler::ExternallyOpenStream(void)
{
    return FALSE;
}

BOOL CVisIjlFileHandler::SupportsPixelType(int evispixfmt, int nbands)
{
    return (IsAvailable())
			&& (evispixfmt == evispixfmtGrayByte || evispixfmt == evispixfmtRGBAByte)
			&& (nbands <= 1);
}


BOOL CVisIjlFileHandler::ReadHeader(SVisFileDescriptor &fd, CVisImageBase &img) 
{
	assert(m_hinstIjlDll != 0);

	// Img must be a CVisByteImage or a CVisRGBAByteImage
    assert(img.NBands() <= 1 &&
           (img.PixFmt() == evispixfmtGrayByte || img.PixFmt() == evispixfmtRGBAByte));

	ZeroMemory(&m_ijldata, sizeof(SVisIjlData));

	m_ijldata.m_fHasAlpha = (fd.has_alpha_channel > 0) ? true : false;
	if (img.PixFmt() == evispixfmtGrayByte)
		m_ijldata.m_cbitPixel = 8;
	else
		m_ijldata.m_cbitPixel = ((m_ijldata.m_fHasAlpha) ? 32 : 24);
	m_ijldata.m_filename = fd.filename;
	m_ijldata.m_filetype = fd.filetype;

	//////////////////////////////////////////////////////////////////

	assert(m_pIjl != 0);
	BOOL fRet = m_pIjl->FReadIjlHeader(m_ijldata);

	//////////////////////////////////////////////////////////////////

	// Fill in the fd structure
    fd.rows = m_ijldata.m_dyImage;
    fd.cols = m_ijldata.m_dxImage;
    fd.bands = 1; // LATER:  Support R/W of 3-band BYTE images?
    fd.evispixfmt = img.PixFmt();
 	fd.bits_per_pixel = m_ijldata.m_cbitPixel;
    fd.has_alpha_channel = ((m_ijldata.m_fHasAlpha) ? 1 : 0);
	fd.jpeg_quality = m_ijldata.m_wQuality;

    return fRet;
}

BOOL CVisIjlFileHandler::ReadBody(SVisFileDescriptor &fd, CVisImageBase &img)
{
	assert(m_hinstIjlDll != 0);

 	m_ijldata.m_dxImage = img.Width();
    m_ijldata.m_dyImage = img.Height();
    m_ijldata.m_cbRow = img.CbRow();
    m_ijldata.m_pbImageFirst = img.PbPixel(img.StartPoint());

    CVisMemBlock visMemblock;
	if(m_ijldata.m_cbitPixel == 24)
	{
		visMemblock = CVisMemBlock(img.Width() * img.Height() * m_ijldata.m_cbitPixel / 8);
		m_ijldata.m_pbImageFirst = visMemblock.PbData();
	}

	//////////////////////////////////////////////////////////////////

	assert(m_pIjl != 0);
	BOOL fRet = m_pIjl->FReadIjlBody(m_ijldata);

    if ((fRet == 0) && (m_ijldata.m_cbitPixel == 24))
	{
		VisInlineConvertCbitToCbit(m_ijldata.m_pbImageFirst, 24, img.PbPixel(img.StartPoint()), 
									32, m_ijldata.m_dxImage, m_ijldata.m_dyImage);
	}
    return fRet;
}

BOOL CVisIjlFileHandler::WriteHeader(SVisFileDescriptor &fd, CVisImageBase &img)
{
	assert(m_hinstIjlDll != 0);

	// Img must be a CVisByteImage or a CVisRGBAByteImage
    assert(img.NBands() <= 1 &&
           (img.PixFmt() == evispixfmtGrayByte || img.PixFmt() == evispixfmtRGBAByte));

	ZeroMemory(&m_ijldata, sizeof(SVisIjlData));

	m_ijldata.m_filename = fd.filename;
	m_ijldata.m_filetype = fd.filetype;

	m_ijldata.m_wQuality = fd.jpeg_quality;

	if (img.PixFmt() == evispixfmtGrayByte)
	{
		m_ijldata.m_cbitPixel = 8;
	}
	else if (fd.has_alpha_channel)
	{
		m_ijldata.m_cbitPixel = 32;
		m_ijldata.m_fHasAlpha = true;
	}
	else
	{
		m_ijldata.m_cbitPixel = 24;
		m_ijldata.m_fHasAlpha = false;
	}
    m_ijldata.m_cbRow = img.CbRow();
    m_ijldata.m_pbImageFirst = img.PbPixel(img.StartPoint());
    m_ijldata.m_dxImage = img.Width();
    m_ijldata.m_dyImage = img.Height();

	//////////////////////////////////////////////////////////////////

	assert(m_pIjl != 0);
	BOOL fRet = m_pIjl->FWriteIjlHeader(m_ijldata);

	//////////////////////////////////////////////////////////////////

	// Fill in the fd structure
    fd.rows = m_ijldata.m_dyImage;
    fd.cols = m_ijldata.m_dxImage;
    fd.bands = 1; // LATER:  Support R/W of 3-band BYTE images?
    fd.evispixfmt = img.PixFmt();
 	fd.bits_per_pixel = m_ijldata.m_cbitPixel;
    fd.has_alpha_channel = ((m_ijldata.m_fHasAlpha) ? 1 : 0);

    return fRet;
}

BOOL CVisIjlFileHandler::WriteBody(SVisFileDescriptor &fd, CVisImageBase &img)
{
	assert(m_hinstIjlDll != 0);

	CVisMemBlock memBlock;
    if(m_ijldata.m_cbitPixel == 32)
	{
		memBlock = CVisMemBlock(img.Width() * img.Height() * img.CbPixel() * 3 / 4);
		VisInlineConvertCbitToCbit(m_ijldata.m_pbImageFirst, 32, memBlock.PbData(), 
									24, m_ijldata.m_dxImage, m_ijldata.m_dyImage);
		m_ijldata.m_cbitPixel = 24;
		m_ijldata.m_pbImageFirst = memBlock.PbData();
	}

	//////////////////////////////////////////////////////////////////

	assert(m_pIjl != 0);
	BOOL fRet = m_pIjl->FWriteIjlBody(m_ijldata);

	//Run-time exception generation
	// UNDONE:  Should this be here?  If so, should we have similar
	// exception code in the other methods?
	if ( fRet )
	{
		throw CVisFileIOError(fd.filename, 
		        "Error while attempting to write image into file", 
				eviserrorWrite, GetClientName(), __FILE__, __LINE__);
	}

    return fRet;
}
