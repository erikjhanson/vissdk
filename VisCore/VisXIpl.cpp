// VisXIpl.cpp
//
// Functions to convert between Vision SDK images and the IplImages
// used in Intel's Image Processing Library.
//
// Warning:  These functions have not been thoroughly tested.
//
// Copyright � 2000 Microsoft Corporation.  All Rights Reserved


#include "VisCorePch.h"
#include "VisCoreProj.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

typedef _IplImage * (__stdcall *TVisPfnIplCreateImageHeader)
               (int   nChannels,  int     alphaChannel, int     depth,
                char* colorModel, char*   channelSeq,   int     dataOrder,
                int   origin,     int     align,
                int   width,      int   height, _IplROI* roi, _IplImage* maskROI,
                void* imageId,    _IplTileInfo* tileInfo);


typedef void (__stdcall *TVisPfnIplDeallocateHeader) (_IplImage* image, int flag);


_IplImage *VisIplCreateImage(CVisImageBase& refimage, bool fCopyMemory)
{
	_IplImage *piplimage = 0;

#ifdef VIS_IPL_IPL_H
	// Clone the image.
	CVisImageBase *pimageClone = refimage.Clone();
	if (fCopyMemory)
		pimageClone->Copy(refimage);

	// Assume that image memory is aligned.
	assert((((int) (void *) pimageClone->PbPixel(pimageClone->StartPoint(), 0))
			& 3) == 0);

#ifdef OLD      // we now handle multi-band (Rick, 3/23/2000)
	// Assume that this is a single-band image.
	assert(refimage.NBands() == 1);
#endif

	// Options based on pixel type.
	// UNDONE:  Support other pixel types.
	int nChannels = 0;
	int nAlpha = 0;
    int depth = 0;
	char *szColorModel = 0;
	char *szColorOrder = 0;
	if (pimageClone->PixFmt() == evispixfmtRGBAByte)
	{
		nChannels = 4;
		nAlpha = 4;
        depth = IPL_DEPTH_8U;
		szColorModel = "RGBA";
		szColorOrder = "BGRA";
	}
	else if (pimageClone->PixFmt() == evispixfmtGrayByte)
	{
		nChannels = 1;
		nAlpha = 0;
        depth = IPL_DEPTH_8U;
		szColorModel = "Gray";
		szColorOrder = "GRAY";
	}
	else if (pimageClone->PixFmt() == evispixfmtRGBAInt)
	{
		nChannels = 4;
		nAlpha = 4;
        depth = IPL_DEPTH_32S;
		szColorModel = "RGBA";
		szColorOrder = "BGRA";
    }
	else if (pimageClone->PixFmt() == evispixfmtGrayInt)
	{
		nChannels = 1;
		nAlpha = 0;
        depth = IPL_DEPTH_32S;
		szColorModel = "Gray";
		szColorOrder = "GRAY";
	}
	else if (pimageClone->PixFmt() == evispixfmtRGBAFloat)
	{
		nChannels = 4;
		nAlpha = 4;
        depth = IPL_DEPTH_32F;
		szColorModel = "RGBA";
		szColorOrder = "BGRA";
    }
	else if (pimageClone->PixFmt() == evispixfmtGrayFloat)
	{
		nChannels = 1;
		nAlpha = 0;
        depth = IPL_DEPTH_32F;
		szColorModel = "Gray";
		szColorOrder = "GRAY";
	}
    nChannels *= pimageClone->NBands();     // multi-band images

	int nAlign;
	BYTE *pbData = pimageClone->PbPixel(pimageClone->StartPoint(), 0);
	assert(((((int) pbData) & 3) == 0) && ((pimageClone->CbRow() & 3) == 0));
	if (((((int) pbData) & 7) == 0) && ((pimageClone->CbRow() & 7) == 0))
		nAlign = IPL_ALIGN_QWORD;
	else
		nAlign = IPL_ALIGN_DWORD;

	// Create the IPL header.
	TVisPfnIplCreateImageHeader pfnIplCreateImageHeader
			= (TVisPfnIplCreateImageHeader) VisPGlobals()->PfnIplCreateImageHeader();
	if (pfnIplCreateImageHeader != 0)
	{
		piplimage = pfnIplCreateImageHeader(
			nChannels,				// number of channels
			nAlpha,					// no alpha channel
			depth,					// unsigned byte
			szColorModel,			// color model
			szColorOrder,			// color order
			IPL_DATA_ORDER_PIXEL,	// pixel order
			IPL_ORIGIN_TL,			// origin top left
			nAlign,					// alignment
			pimageClone->Width(),	// width
			pimageClone->Height(),	// height
			NULL,					// no ROI
			NULL,					// no mask ROI
			pimageClone,			// Remember the cloned image.
			NULL);					// not tiled
	}

	if (piplimage != 0)
	{
		// Fill in information about image memory.
		piplimage->imageSize = pimageClone->MemBlock().CbData();
		piplimage->imageData = (char *) pbData;
		piplimage->widthStep = pimageClone->CbRow();
		piplimage->imageDataOrigin = (char *) pimageClone->MemBlock().PbData();
	}
#else // VIS_IPL_IPL_H
	// UNDONE:  Throw an exception?
#endif // VIS_IPL_IPL_H

	return piplimage;
}

void VisIplDeleteImage(_IplImage *piplimage)
{
#ifdef VIS_IPL_IPL_H
	assert(piplimage != 0);
	assert(piplimage->imageId != 0);

	// Delete the cloned image.
	delete ((CVisImageBase *) piplimage->imageId);

	// Clear pointers to data before deleting IPL header (to be safe).
	piplimage->imageSize = 0;
	piplimage->imageData = 0;
	piplimage->widthStep = 0;
	piplimage->imageDataOrigin = 0;

	// Delete the header.
	assert(VisPGlobals()->PfnIplDeallocateHeader() != 0);
	((TVisPfnIplDeallocateHeader)
			VisPGlobals()->PfnIplDeallocateHeader())(piplimage, IPL_IMAGE_HEADER);
#else // VIS_IPL_IPL_H
	// UNDONE:  Throw an exception?
#endif // VIS_IPL_IPL_H
}

int VisIplBorderModeFromPad(EVisPad evispad)
{
	int nMode = 0;
#ifdef VIS_IPL_IPL_H
	switch (evispad)
	{
	case evispadNone:
	case evispadZero:
		nMode = IPL_BORDER_CONSTANT;
		break;

	case evispadReplicate:
		nMode = IPL_BORDER_REPLICATE;
		break;

	case evispadReflect:
		nMode = IPL_BORDER_REFLECT;
		break;

	case evispadCyclic:
		nMode = IPL_BORDER_WRAP;
		break;

#ifdef _DEBUG
	default:
		assert(0);
#endif // _DEBUG
	}
#endif // VIS_IPL_IPL_H

	return nMode;
}
