// VisLuciformSDIO.cpp
//
// Copyright © 2000 Microsoft Corporation, All Rights Reserved

#include "VisMatrixPch.h"
#include "VisMatrixProj.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#ifndef VIS_NO_LUC_FILES

#ifndef VIS_ONLY_DECLARE_PROPLIST

// Allow vector and covector types with float coordinates to be used in
// arrays and vectors and allow point types to be used in arrays, vectors,
// and tables.
// LATER:  We could add more types or support for double coordinates, but
// that could use up a lot of the compiler's heap space.
VIS_DEFINE_PROP_LIST_CLASS_AVT(VisMatrixExport, CVisPoint2TC<float>)
VIS_DEFINE_PROP_LIST_CLASS_AVT(VisMatrixExport, CVisPoint3TC<float>)
VIS_DEFINE_PROP_LIST_CLASS_AVT(VisMatrixExport, CVisPoint4TC<float>)

VIS_DEFINE_PROP_LIST_CLASS_AVT(VisMatrixExport, CVisHPoint2TC<float>)
VIS_DEFINE_PROP_LIST_CLASS_AVT(VisMatrixExport, CVisHPoint3TC<float>)

//VIS_DEFINE_PROP_LIST_CLASS_AV(VisMatrixExport, CVisHPlane3TC<float>)

VIS_DEFINE_PROP_LIST_CLASS_AV(VisMatrixExport, CVisVector2TC<float>)
VIS_DEFINE_PROP_LIST_CLASS_AV(VisMatrixExport, CVisVector3TC<float>)
VIS_DEFINE_PROP_LIST_CLASS_AV(VisMatrixExport, CVisVector4TC<float>)

VIS_DEFINE_PROP_LIST_CLASS_AV(VisMatrixExport, CVisCoVector2TC<float>)
VIS_DEFINE_PROP_LIST_CLASS_AV(VisMatrixExport, CVisCoVector3TC<float>)
VIS_DEFINE_PROP_LIST_CLASS_AV(VisMatrixExport, CVisCoVector4TC<float>)

//VIS_DEFINE_PROP_LIST_CLASS_AV(VisMatrixExport, CVisMatrix2TC<float>)
//VIS_DEFINE_PROP_LIST_CLASS_AV(VisMatrixExport, CVisMatrix3TC<float>)
//VIS_DEFINE_PROP_LIST_CLASS_AV(VisMatrixExport, CVisMatrix4TC<float>)


#endif // !VIS_ONLY_DECLARE_PROPLIST

#endif // !VIS_NO_LUC_FILES
