//==========================================================================;
//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) 1992 - 2000  Microsoft Corporation.  All Rights Reserved.
//
//--------------------------------------------------------------------------;

// The CLSID used by the renderer filter

#ifdef _DEBUG
// {8BA4517D-D48B-42f3-A4FD-C7E709709DE7}
DEFINE_GUID(CLSID_RenderDB, 
0x8ba4517d, 0xd48b, 0x42f3, 0xa4, 0xfd, 0xc7, 0xe7, 0x9, 0x70, 0x9d, 0xe7);
#else // _DEBUG
// { 36a5f770-fe4c-11ce-a8ed-00aa002feab5 }
DEFINE_GUID(CLSID_Render,
0x36a5f771, 0xfe4c, 0x11ce, 0xa8, 0xed, 0x00, 0xaa, 0x00, 0x2f, 0xea, 0xb5);
#endif // _DEBUG

