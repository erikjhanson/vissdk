// VisXIjl.h
//
// Copyright © 2000 Microsoft Corporation, All Rights Reserved

#ifdef VIS_XIJL_DLL
	#define VIS_XIJL_BUILD
#endif // VIS_XIJL_DLL

// Version information
#include "VisXIjlVersion.h"

#ifdef VIS_XIJL_LIB
#define VisXIjlExport			
#elif defined(VIS_XIJL_BUILD)
#define VisXIjlExport			_declspec(dllexport)
#else // VIS_XIJL_LIB
#define VisXIjlExport			_declspec(dllimport)
#endif // VIS_XIJL_LIB


typedef class CVisXIjl* (__stdcall *T_PfnVisIjl)(void);

//VisXIjlExport class CVisXIjl* STDAPICALLTYPE VisXIjl(void);

struct SVisIjlData
{
    const char *m_filename;   // file name
    const char *m_filetype;   // file type (same as allowed extensions)
	int m_cbitPixel;	// We only support 8 and 24
	int m_dxImage;		// Width of image
	int m_dyImage;		// Height of Image

	// Other entries
	int m_cbRow;		// Offset between pixels in same col and adjacent rows
	BYTE *m_pbImageFirst;	// Pointer to first byte in top-left pixel

	bool m_fHasAlpha;	// Has a valid alpha channel

	int m_wQuality;		// Quality of compresstion (0,,100)

	int wReserved;		// Size of JPEG_CORE_PROPERTIES (used internally).
	void *pvReserved;	// Pointer to JPEG_CORE_PROPERTIES (used internally).
};


class CVisXIjl : public IUnknown
{
public:
	CVisXIjl(void);

	~CVisXIjl(void);


	//------------------------------------------------------------------
	// @group IUnknown methods

	STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppvObject);

    STDMETHOD_(ULONG, AddRef) (THIS);

    STDMETHOD_(ULONG, Release) (THIS);

	//------------------------------------------------------------------
	
	virtual int FReadIjlHeader(SVisIjlData& ijldata);

	virtual int FReadIjlBody(SVisIjlData& ijldata);

	virtual int FWriteIjlHeader(SVisIjlData& ijldata);

	virtual int FWriteIjlBody(SVisIjlData& ijldata);

	virtual bool FHadError(void);

	virtual void ClearError(void);

	virtual const char* SzLastErrorMsg(void);

private:
	long m_cRef;

	int m_nIjlError;
};

