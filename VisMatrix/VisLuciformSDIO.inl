// VisLuciformSDIO.inl
//
// Copyright © 2000 Microsoft Corporation, All Rights Reserved


// SDStream I/O helper methods
template<class TCoord>
inline void CVisHLine2TC<TCoord>::BuildPropList(CVisPropList& refproplist)
{
	refproplist.AddPropReference("a", (*this)[0], false);
	refproplist.AddPropReference("b", (*this)[1], false);
	refproplist.AddPropReference("c", (*this)[2], false);
};

template<class TCoord>
inline void CVisHPoint2TC<TCoord>::BuildPropList(CVisPropList& refproplist)
{
	refproplist.AddPropReference("x", (*this)[0], false);
	refproplist.AddPropReference("y", (*this)[1], false);
	refproplist.AddPropReference("w", (*this)[2], false);
};

template<class TCoord>
inline void CVisHPoint3TC<TCoord>::BuildPropList(CVisPropList& refproplist)
{
	refproplist.AddPropReference("x", (*this)[0], false);
	refproplist.AddPropReference("y", (*this)[1], false);
	refproplist.AddPropReference("z", (*this)[2], false);
	refproplist.AddPropReference("w", (*this)[3], false);
};

template<class TCoord>
inline void CVisHPlane3TC<TCoord>::BuildPropList(CVisPropList& refproplist)
{
	refproplist.AddPropReference("x", (*this)[0], false);
	refproplist.AddPropReference("y", (*this)[1], false);
	refproplist.AddPropReference("z", (*this)[2], false);
	refproplist.AddPropReference("w", (*this)[3], false);
};

template<class TCoord>
inline void CVisVector2TC<TCoord>::BuildPropList(CVisPropList& refproplist)
{
	refproplist.AddPropReference("dx", (*this)[0], false);
	refproplist.AddPropReference("dy", (*this)[1], false);
};

template<class TCoord>
inline void CVisCoVector2TC<TCoord>::BuildPropList(CVisPropList& refproplist)
{
	refproplist.AddPropReference("nx", (*this)[0], false);
	refproplist.AddPropReference("ny", (*this)[1], false);
};

template<class TCoord>
inline void CVisMatrix2TC<TCoord>::BuildPropList(CVisPropList& refproplist)
{
	refproplist.AddPropReference("m0",
			*((CVisCoVector2TC<TCoord> *) &((*this)[0])), false);
	refproplist.AddPropReference("m1",
			*((CVisCoVector2TC<TCoord> *) &((*this)[1])), false);
};

template<class TCoord>
inline void CVisPoint2TC<TCoord>::BuildPropList(CVisPropList& refproplist)
{
	refproplist.AddPropReference("x", (*this)[0], false);
	refproplist.AddPropReference("y", (*this)[1], false);
};

template<class TCoord>
inline void CVisVector3TC<TCoord>::BuildPropList(CVisPropList& refproplist)
{
	refproplist.AddPropReference("dx", (*this)[0], false);
	refproplist.AddPropReference("dy", (*this)[1], false);
	refproplist.AddPropReference("dz", (*this)[2], false);
};

template<class TCoord>
inline void CVisCoVector3TC<TCoord>::BuildPropList(CVisPropList& refproplist)
{
	refproplist.AddPropReference("nx", (*this)[0], false);
	refproplist.AddPropReference("ny", (*this)[1], false);
	refproplist.AddPropReference("nz", (*this)[2], false);
};

template<class TCoord>
inline void CVisMatrix3TC<TCoord>::BuildPropList(CVisPropList& refproplist)
{
	refproplist.AddPropReference("m0",
			*((CVisCoVector3TC<TCoord> *) &((*this)[0])), false);
	refproplist.AddPropReference("m1",
			*((CVisCoVector3TC<TCoord> *) &((*this)[1])), false);
	refproplist.AddPropReference("m2",
			*((CVisCoVector3TC<TCoord> *) &((*this)[2])), false);
};

template<class TCoord>
inline void CVisPoint3TC<TCoord>::BuildPropList(CVisPropList& refproplist)
{
	refproplist.AddPropReference("x", (*this)[0], false);
	refproplist.AddPropReference("y", (*this)[1], false);
	refproplist.AddPropReference("z", (*this)[2], false);
};

template<class TCoord>
inline void CVisVector4TC<TCoord>::BuildPropList(CVisPropList& refproplist)
{
	refproplist.AddPropReference("dx", (*this)[0], false);
	refproplist.AddPropReference("dy", (*this)[1], false);
	refproplist.AddPropReference("dz", (*this)[2], false);
	refproplist.AddPropReference("dw", (*this)[3], false);
};

template<class TCoord>
inline void CVisCoVector4TC<TCoord>::BuildPropList(CVisPropList& refproplist)
{
	refproplist.AddPropReference("nx", (*this)[0], false);
	refproplist.AddPropReference("ny", (*this)[1], false);
	refproplist.AddPropReference("nz", (*this)[2], false);
	refproplist.AddPropReference("nw", (*this)[3], false);
};

template<class TCoord>
inline void CVisMatrix4TC<TCoord>::BuildPropList(CVisPropList& refproplist)
{
	refproplist.AddPropReference("m0",
			*((CVisCoVector4TC<TCoord> *) &((*this)[0])), false);
	refproplist.AddPropReference("m1",
			*((CVisCoVector4TC<TCoord> *) &((*this)[1])), false);
	refproplist.AddPropReference("m2",
			*((CVisCoVector4TC<TCoord> *) &((*this)[2])), false);
	refproplist.AddPropReference("m3",
			*((CVisCoVector4TC<TCoord> *) &((*this)[3])), false);
};

template<class TCoord>
inline void CVisPoint4TC<TCoord>::BuildPropList(CVisPropList& refproplist)
{
	refproplist.AddPropReference("x", (*this)[0], false);
	refproplist.AddPropReference("y", (*this)[1], false);
	refproplist.AddPropReference("z", (*this)[2], false);
	refproplist.AddPropReference("w", (*this)[3], false);
};



#define VIS_PROP_LIST_AND_SD_IO_FOR_TEMPLATED_POINT_OR_VECT(TPoint)	\
	template<class TCoord>											\
	VIS_PROP_LIST_CLASS_TYPE_INFO_WITH_IO_PL(TPoint<TCoord>, true)	\
	template<class TCoord>											\
	VIS_PROP_LIST_CLASS_TYPE_INFO_LOOKUP(TPoint<TCoord>)			\
	template<class TCoord>											\
	VIS_PROP_LIST_GET_PROP_TYPE_INFO(TPoint<TCoord>)				\
	template<class TCoord>											\
	VIS_SDI_FIND_TYPES_PL(TPoint<TCoord>)							\
	template<class TCoord>											\
	VIS_SDI_READ_VALUE_USING_CLASS_PL(TPoint<TCoord>)				\
	template<class TCoord>											\
	VIS_SDI_READ_OBJ_DEFAULT(TPoint<TCoord>)						\
	template<class TCoord>											\
	VIS_SDI_READ_OP_DEFAULT(TPoint<TCoord>)							\
	template<class TCoord>											\
	VIS_SDO_FIND_TYPES_PL(TPoint<TCoord>)							\
	template<class TCoord>											\
	VIS_SDO_WRITE_VALUE_USING_CLASS_PL(TPoint<TCoord>)				\
	template<class TCoord>											\
	VIS_SDO_WRITE_OBJ_DEFAULT(TPoint<TCoord>)						\
	template<class TCoord>											\
	VIS_SDO_WRITE_OP_DEFAULT(TPoint<TCoord>)

VIS_PROP_LIST_AND_SD_IO_FOR_TEMPLATED_POINT_OR_VECT(CVisHLine2TC)
VIS_PROP_LIST_AND_SD_IO_FOR_TEMPLATED_POINT_OR_VECT(CVisHPoint2TC)
VIS_PROP_LIST_AND_SD_IO_FOR_TEMPLATED_POINT_OR_VECT(CVisHPoint3TC)
VIS_PROP_LIST_AND_SD_IO_FOR_TEMPLATED_POINT_OR_VECT(CVisHPlane3TC)
VIS_PROP_LIST_AND_SD_IO_FOR_TEMPLATED_POINT_OR_VECT(CVisVector2TC)
VIS_PROP_LIST_AND_SD_IO_FOR_TEMPLATED_POINT_OR_VECT(CVisCoVector2TC)
VIS_PROP_LIST_AND_SD_IO_FOR_TEMPLATED_POINT_OR_VECT(CVisPoint2TC)
VIS_PROP_LIST_AND_SD_IO_FOR_TEMPLATED_POINT_OR_VECT(CVisVector3TC)
VIS_PROP_LIST_AND_SD_IO_FOR_TEMPLATED_POINT_OR_VECT(CVisCoVector3TC)
VIS_PROP_LIST_AND_SD_IO_FOR_TEMPLATED_POINT_OR_VECT(CVisPoint3TC)
VIS_PROP_LIST_AND_SD_IO_FOR_TEMPLATED_POINT_OR_VECT(CVisVector4TC)
VIS_PROP_LIST_AND_SD_IO_FOR_TEMPLATED_POINT_OR_VECT(CVisCoVector4TC)
VIS_PROP_LIST_AND_SD_IO_FOR_TEMPLATED_POINT_OR_VECT(CVisPoint4TC)


#define VIS_PROP_LIST_AND_SD_IO_FOR_TEMPLATED_MATRIX(TMatrix)		\
	template<class TCoord>											\
	VIS_PROP_LIST_CLASS_TYPE_INFO_WITH_IO_PL(TMatrix<TCoord>, true)	\
	template<class TCoord>											\
	VIS_PROP_LIST_CLASS_TYPE_INFO_LOOKUP(TMatrix<TCoord>)			\
	template<class TCoord>											\
	VIS_PROP_LIST_GET_PROP_TYPE_INFO(TMatrix<TCoord>)				\
	template<class TCoord>											\
	VIS_DECLARE_SDI_FIND_TYPES(inline, TMatrix<TCoord>)				\
	template<class TCoord>											\
	VIS_SDI_READ_VALUE_USING_CLASS_PL(TMatrix<TCoord>)				\
	template<class TCoord>											\
	VIS_SDI_READ_OBJ_DEFAULT(TMatrix<TCoord>)						\
	template<class TCoord>											\
	VIS_SDI_READ_OP_DEFAULT(TMatrix<TCoord>)						\
	template<class TCoord>											\
	VIS_DECLARE_SDO_FIND_TYPES(inline, TMatrix<TCoord>)				\
	template<class TCoord>											\
	VIS_SDO_WRITE_VALUE_USING_CLASS_PL(TMatrix<TCoord>)				\
	template<class TCoord>											\
	VIS_SDO_WRITE_OBJ_DEFAULT(TMatrix<TCoord>)						\
	template<class TCoord>											\
	VIS_SDO_WRITE_OP_DEFAULT(TMatrix<TCoord>)

VIS_PROP_LIST_AND_SD_IO_FOR_TEMPLATED_MATRIX(CVisMatrix2TC)
VIS_PROP_LIST_AND_SD_IO_FOR_TEMPLATED_MATRIX(CVisMatrix3TC)
VIS_PROP_LIST_AND_SD_IO_FOR_TEMPLATED_MATRIX(CVisMatrix4TC)


template<class TCoord>
inline void VisSDIFindTypes(CVisSDIStream& refsdistream,
		CVisMatrix2TC<TCoord> const& refmat)
{
	// LATER:  If we allow non-standard coordinates, we should tell the stream
	// about the coordinate type and add FindType functions for the point and
	// vector classes.
	// refsdistream.UseObjType(TCoord);
	refsdistream.UseObjType(CVisCoVector2TC<TCoord>());
	refsdistream.UseObjType(refmat);
}

template<class TCoord>
inline void VisSDOFindTypes(CVisSDOStream& refsdostream,
		CVisMatrix2TC<TCoord> const& refmat)
{
	// LATER:  If we allow non-standard coordinates, we should tell the stream
	// about the coordinate type and add FindType functions for the point and
	// vector classes.
	// refsdostream.UseObjType(TCoord);
	refsdostream.UseObjType(CVisCoVector2TC<TCoord>());
	refsdostream.UseObjType(refmat);
}

template<class TCoord>
inline void VisSDIFindTypes(CVisSDIStream& refsdistream,
		CVisMatrix3TC<TCoord> const& refmat)
{
	// LATER:  If we allow non-standard coordinates, we should tell the stream
	// about the coordinate type and add FindType functions for the point and
	// vector classes.
	// refsdistream.UseObjType(TCoord);
	refsdistream.UseObjType(CVisCoVector3TC<TCoord>());
	refsdistream.UseObjType(refmat);
}

template<class TCoord>
inline void VisSDOFindTypes(CVisSDOStream& refsdostream,
		CVisMatrix3TC<TCoord> const& refmat)
{
	// LATER:  If we allow non-standard coordinates, we should tell the stream
	// about the coordinate type and add FindType functions for the point and
	// vector classes.
	// refsdostream.UseObjType(TCoord);
	refsdostream.UseObjType(CVisCoVector3TC<TCoord>());
	refsdostream.UseObjType(refmat);
}

template<class TCoord>
inline void VisSDIFindTypes(CVisSDIStream& refsdistream,
		CVisMatrix4TC<TCoord> const& refmat)
{
	// LATER:  If we allow non-standard coordinates, we should tell the stream
	// about the coordinate type and add FindType functions for the point and
	// vector classes.
	// refsdistream.UseObjType(TCoord);
	refsdistream.UseObjType(CVisCoVector4TC<TCoord>());
	refsdistream.UseObjType(refmat);
}

template<class TCoord>
inline void VisSDOFindTypes(CVisSDOStream& refsdostream,
		CVisMatrix4TC<TCoord> const& refmat)
{
	// LATER:  If we allow non-standard coordinates, we should tell the stream
	// about the coordinate type and add FindType functions for the point and
	// vector classes.
	// refsdostream.UseObjType(TCoord);
	refsdostream.UseObjType(CVisCoVector4TC<TCoord>());
	refsdostream.UseObjType(refmat);
}


#ifdef LATER
// Allow vector and covector types with float coordinates to be used in
// arrays and vectors and allow point types to be used in arrays, vectors,
// and tables.
// LATER:  We could add more types or support for double coordinates, but
// that could use up a lot of the compiler's heap space.
VIS_DECLARE_PROP_LIST_CLASS_AVT(VisMatrixExport, CVisPoint2TC<float>)
VIS_DECLARE_PROP_LIST_CLASS_AVT(VisMatrixExport, CVisPoint3TC<float>)
VIS_DECLARE_PROP_LIST_CLASS_AVT(VisMatrixExport, CVisPoint4TC<float>)

VIS_DECLARE_PROP_LIST_CLASS_AVT(VisMatrixExport, CVisHPoint2TC<float>)
VIS_DECLARE_PROP_LIST_CLASS_AVT(VisMatrixExport, CVisHPoint3TC<float>)

//VIS_DECLARE_PROP_LIST_CLASS_AV(VisMatrixExport, CVisHPlane3TC<float>)

VIS_DECLARE_PROP_LIST_CLASS_AV(VisMatrixExport, CVisVector2TC<float>)
VIS_DECLARE_PROP_LIST_CLASS_AV(VisMatrixExport, CVisVector3TC<float>)
VIS_DECLARE_PROP_LIST_CLASS_AV(VisMatrixExport, CVisVector4TC<float>)

VIS_DECLARE_PROP_LIST_CLASS_AV(VisMatrixExport, CVisCoVector2TC<float>)
VIS_DECLARE_PROP_LIST_CLASS_AV(VisMatrixExport, CVisCoVector3TC<float>)
VIS_DECLARE_PROP_LIST_CLASS_AV(VisMatrixExport, CVisCoVector4TC<float>)

//VIS_DECLARE_PROP_LIST_CLASS_AV(VisMatrixExport, CVisMatrix2TC<float>)
//VIS_DECLARE_PROP_LIST_CLASS_AV(VisMatrixExport, CVisMatrix3TC<float>)
//VIS_DECLARE_PROP_LIST_CLASS_AV(VisMatrixExport, CVisMatrix4TC<float>)
#endif // LATER

// UNDONE:  Need to add SDStream I/O functions for line segments
