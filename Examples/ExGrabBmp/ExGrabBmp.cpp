// ExGrabBmp.cpp
//
// Copyright © 1997-2000 Microsoft Corporation, All Rights Reserved


#include "VisWin.h"

#include "VisImSrc.h"
#include "VisMemoryChecks.h"



#ifdef VIS_SDK_LIBS
	// If VIS_SDK_LIBS is defined, we can include "VisVFWCameraLib.h" to
	// link with the VisVFWCamera code (so that the DLL is not needed).
	#include "VisVFWCameraLib.h"
#endif // VIS_SDK_LIBS


main(int cArg, char *rgszArg[])
{
	std::string strFile;
	if (cArg > 1)
	{
		// Check to see if we should print usage information.
		if ((rgszArg[1][0] == '-') || (rgszArg[1][0] == '/'))
		{
			printf("\nUsage:  %s  [ filename ]\n", rgszArg[0]);
			return 0;
		}

		// Otherwise, use the first argument as the filename.
		strFile = rgszArg[1];

		// If no extension is given, use ".bmp".
		if (strchr(strFile.c_str(), '.') == 0)
			strFile += ".bmp";
	}
	else
	{
		strFile = rgszArg[0];
		int cchFile = strFile.size();
		assert(cchFile > 4);
		if (stricmp(strFile.c_str() + cchFile - 4, ".exe") == 0)
		{
			strFile[cchFile - 3] = 'b';
			strFile[cchFile - 2] = 'm';
			strFile[cchFile - 1] = 'p';
		}
		else
		{
			strFile += ".bmp";
		}
	}

	VisAddProviderRegEntryForVFW();

	// Use an empty string when finding the image source so that we
	// don't attempt to display an MFC dialog to the user.
	CVisImageSource imagesource = VisFindImageSource("");

	if (imagesource.IsValid())
	{
		imagesource.SetUseContinuousGrab(false);
		CVisSequence<CVisRGBABytePixel> sequence;
		sequence.ConnectToSource(imagesource, true, false);

		CVisRGBAByteImage imageT;
		if (sequence.Pop(imageT, 2000))
		{
			imageT.FWriteFile(strFile.c_str());
		}

		sequence.DisconnectFromSource();
	}

	return 0;
}