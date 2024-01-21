// VisPixelOps.inl
//
// Copyright © 2000 Microsoft Corporation, All Rights Reserved
//
// The following functions are defined to work with the standard C numeric
// types, CVisRGBA classes using the standard C numeric types, and images
// using these types:
//
// VisAbs
// VisAvg
// VisAvg
// VisMin
//
// The following functions take CVisRGBA classes using the standard C
// numeric types and return the scalar values based on the R, G, and B
// components.
//
// VisAvgOfRGB
// VisAvgOfRGB
// VisMinOfRGB
// VisSumOfRGB
// VisSumOfSquaresOfRGB
// VisBrightnessFromRGBA
//
// The standard C numeric types are:
//
// signed char
// short
// int
// long
// __int64
// unsigned char
// unsigned char
// unsigned short
// unsigned int
// unsigned long
// unsigned __int64
// float
// double
//
// LATER:  Put large functions that aren't templated in a CPP file?


//---------------------------------------------------------------------

inline signed char VisAbs(const signed char& refnum)
{
	return ((refnum >= 0) ? refnum : - refnum);
}

inline short VisAbs(const short& refnum)
{
	return ((refnum >= 0) ? refnum : - refnum);
}

inline int VisAbs(const int& refnum)
{
	return ((refnum >= 0) ? refnum : - refnum);
}

inline long VisAbs(const long& refnum)
{
	return ((refnum >= 0) ? refnum : - refnum);
}

inline __int64 VisAbs(const __int64& refnum)
{
	return ((refnum >= 0) ? refnum : - refnum);
}

inline unsigned char VisAbs(const unsigned char& refnum)
{
	return ((refnum >= 0) ? refnum : - refnum);
}

inline unsigned short VisAbs(const unsigned short& refnum)
{
	return ((refnum >= 0) ? refnum : - refnum);
}

inline unsigned int VisAbs(const unsigned int& refnum)
{
	return ((refnum >= 0) ? refnum : - refnum);
}

inline unsigned long VisAbs(const unsigned long& refnum)
{
	return ((refnum >= 0) ? refnum : - refnum);
}

inline unsigned __int64 VisAbs(const unsigned __int64& refnum)
{
	return ((refnum >= 0) ? refnum : - refnum);
}

inline float VisAbs(const float& refnum)
{
	return ((refnum >= 0) ? refnum : - refnum);
}

inline double VisAbs(const double& refnum)
{
	return ((refnum >= 0) ? refnum : - refnum);
}


inline CVisRGBA<signed char> VisAbs(const CVisRGBA<signed char>& refrgba)
{
	return CVisRGBA<signed char>(VisAbs(refrgba.R()), VisAbs(refrgba.G()),
			VisAbs(refrgba.B()), refrgba.A());
}

inline CVisRGBA<short> VisAbs(const CVisRGBA<short>& refrgba)
{
	return CVisRGBA<short>(VisAbs(refrgba.R()), VisAbs(refrgba.G()),
			VisAbs(refrgba.B()), refrgba.A());
}

inline CVisRGBA<int> VisAbs(const CVisRGBA<int>& refrgba)
{
	return CVisRGBA<int>(VisAbs(refrgba.R()), VisAbs(refrgba.G()),
			VisAbs(refrgba.B()), refrgba.A());
}

inline CVisRGBA<long> VisAbs(const CVisRGBA<long>& refrgba)
{
	return CVisRGBA<long>(VisAbs(refrgba.R()), VisAbs(refrgba.G()),
			VisAbs(refrgba.B()), refrgba.A());
}

inline CVisRGBA<__int64> VisAbs(const CVisRGBA<__int64>& refrgba)
{
	return CVisRGBA<__int64>(VisAbs(refrgba.R()), VisAbs(refrgba.G()),
			VisAbs(refrgba.B()), refrgba.A());
}

inline CVisRGBA<unsigned char> VisAbs(const CVisRGBA<unsigned char>& refrgba)
{
	return CVisRGBA<unsigned char>(VisAbs(refrgba.R()), VisAbs(refrgba.G()),
			VisAbs(refrgba.B()), refrgba.A());
}

inline CVisRGBA<unsigned short> VisAbs(const CVisRGBA<unsigned short>& refrgba)
{
	return CVisRGBA<unsigned short>(VisAbs(refrgba.R()), VisAbs(refrgba.G()),
			VisAbs(refrgba.B()), refrgba.A());
}

inline CVisRGBA<unsigned int> VisAbs(const CVisRGBA<unsigned int>& refrgba)
{
	return CVisRGBA<unsigned int>(VisAbs(refrgba.R()), VisAbs(refrgba.G()),
			VisAbs(refrgba.B()), refrgba.A());
}

inline CVisRGBA<unsigned long> VisAbs(const CVisRGBA<unsigned long>& refrgba)
{
	return CVisRGBA<unsigned long>(VisAbs(refrgba.R()), VisAbs(refrgba.G()),
			VisAbs(refrgba.B()), refrgba.A());
}

inline CVisRGBA<unsigned __int64> VisAbs(const CVisRGBA<unsigned __int64>& refrgba)
{
	return CVisRGBA<unsigned __int64>(VisAbs(refrgba.R()), VisAbs(refrgba.G()),
			VisAbs(refrgba.B()), refrgba.A());
}

inline CVisRGBA<float> VisAbs(const CVisRGBA<float>& refrgba)
{
	return CVisRGBA<float>(VisAbs(refrgba.R()), VisAbs(refrgba.G()),
			VisAbs(refrgba.B()), refrgba.A());
}

inline CVisRGBA<double> VisAbs(const CVisRGBA<double>& refrgba)
{
	return CVisRGBA<double>(VisAbs(refrgba.R()), VisAbs(refrgba.G()),
			VisAbs(refrgba.B()), refrgba.A());
}


template<class TPixel>
inline CVisImage<TPixel> VisAbs(const CVisImage<TPixel>& refimage)
{
	assert(&refimage != 0);

	CVisImage<TPixel> imageRet;

	if (refimage.IsValid())
	{
		CVisShape shape = refimage.Shape();
		int dipixelWidth = shape.Width() * shape.NBands();
		imageRet.Allocate(shape);
		for (int y = shape.top; y < shape.bottom; ++y)
		{
			const TPixel *ppixelSrc = refimage.PixelAddress(shape.left, y, 0);
			TPixel *ppixelDest = imageRet.PixelAddress(shape.left, y, 0);
			for (int x = 0; i < dipixelWidth; ++i)
			{
				ppixelDest[i] = VisAbs(ppixelSrc[i]);
			}
		}

	}

	return imageRet;
}

//---------------------------------------------------------------------

inline signed char VisAvg(const signed char& refnum1,
		const signed char& refnum2)
{
	return (signed char) (((unsigned int) (refnum1 + refnum2)) >> 1);
}

inline short VisAvg(const short& refnum1, const short& refnum2)
{
	return (short) (((unsigned int) (refnum1 + refnum2)) >> 1);
}

inline int VisAvg(const int& refnum1, const int& refnum2)
{
	return (int) (((unsigned int) (refnum1 + refnum2)) >> 1);
}

inline long VisAvg(const long& refnum1, const long& refnum2)
{
	return (long) (((unsigned ) (refnum1 + refnum2)) >> 1);
}

inline __int64 VisAvg(const __int64& refnum1, const __int64& refnum2)
{
	return (__int64) (((unsigned ) (refnum1 + refnum2)) >> 1);
}

inline unsigned char VisAvg(const unsigned char& refnum1,
		const unsigned char& refnum2)
{
	return (unsigned char) ((refnum1 + refnum2) >> 1);
}

inline unsigned short VisAvg(const unsigned short& refnum1,
		const unsigned short& refnum2)
{
	return (unsigned short) ((refnum1 + refnum2) >> 1);
}

inline unsigned int VisAvg(const unsigned int& refnum1,
		const unsigned int& refnum2)
{
	return (unsigned int) ((refnum1 + refnum2) >> 1);
}

inline unsigned long VisAvg(const unsigned long& refnum1,
		const unsigned long& refnum2)
{
	return (unsigned long) ((refnum1 + refnum2) >> 1);
}

inline unsigned __int64 VisAvg(const unsigned __int64& refnum1,
		const unsigned __int64& refnum2)
{
	return (unsigned __int64) ((refnum1 + refnum2) >> 1);
}

inline float VisAvg(const float& refnum1, const float& refnum2)
{
	return (float) ((refnum1 + refnum2) * 0.5);
}

inline double VisAvg(const double& refnum1, const double& refnum2)
{
	return (double) ((refnum1 + refnum2) * 0.5);
}


inline CVisRGBA<signed char> VisAvg(const CVisRGBA<signed char>& refrgba1,
		const CVisRGBA<signed char>& refrgba2)
{
	return CVisRGBA<signed char>(VisAvg(refrgba1.R(), refrgba2.R()),
			VisAvg(refrgba1.G(), refrgba2.G()),
			VisAvg(refrgba1.B(), refrgba2.B()),
			VisAvg(refrgba1.A(), refrgba2.A()));
}

inline CVisRGBA<short> VisAvg(const CVisRGBA<short>& refrgba1,
		const CVisRGBA<short>& refrgba2)
{
	return CVisRGBA<short>(VisAvg(refrgba1.R(), refrgba2.R()),
			VisAvg(refrgba1.G(), refrgba2.G()),
			VisAvg(refrgba1.B(), refrgba2.B()),
			VisAvg(refrgba1.A(), refrgba2.A()));
}

inline CVisRGBA<int> VisAvg(const CVisRGBA<int>& refrgba1,
		const CVisRGBA<int>& refrgba2)
{
	return CVisRGBA<int>(VisAvg(refrgba1.R(), refrgba2.R()),
			VisAvg(refrgba1.G(), refrgba2.G()),
			VisAvg(refrgba1.B(), refrgba2.B()),
			VisAvg(refrgba1.A(), refrgba2.A()));
}

inline CVisRGBA<long> VisAvg(const CVisRGBA<long>& refrgba1,
		const CVisRGBA<long>& refrgba2)
{
	return CVisRGBA<long>(VisAvg(refrgba1.R(), refrgba2.R()),
			VisAvg(refrgba1.G(), refrgba2.G()),
			VisAvg(refrgba1.B(), refrgba2.B()),
			VisAvg(refrgba1.A(), refrgba2.A()));
}

inline CVisRGBA<__int64> VisAvg(const CVisRGBA<__int64>& refrgba1,
		const CVisRGBA<__int64>& refrgba2)
{
	return CVisRGBA<__int64>(VisAvg(refrgba1.R(), refrgba2.R()),
			VisAvg(refrgba1.G(), refrgba2.G()),
			VisAvg(refrgba1.B(), refrgba2.B()),
			VisAvg(refrgba1.A(), refrgba2.A()));
}

inline CVisRGBA<unsigned char> VisAvg(const CVisRGBA<unsigned char>& refrgba1,
		const CVisRGBA<unsigned char>& refrgba2)
{
	return CVisRGBA<unsigned char>(VisAvg(refrgba1.R(), refrgba2.R()),
			VisAvg(refrgba1.G(), refrgba2.G()),
			VisAvg(refrgba1.B(), refrgba2.B()),
			VisAvg(refrgba1.A(), refrgba2.A()));
}

inline CVisRGBA<unsigned short> VisAvg(const CVisRGBA<unsigned short>& refrgba1,
		const CVisRGBA<unsigned short>& refrgba2)
{
	return CVisRGBA<unsigned short>(VisAvg(refrgba1.R(), refrgba2.R()),
			VisAvg(refrgba1.G(), refrgba2.G()),
			VisAvg(refrgba1.B(), refrgba2.B()),
			VisAvg(refrgba1.A(), refrgba2.A()));
}

inline CVisRGBA<unsigned int> VisAvg(const CVisRGBA<unsigned int>& refrgba1,
		const CVisRGBA<unsigned int>& refrgba2)
{
	return CVisRGBA<unsigned int>(VisAvg(refrgba1.R(), refrgba2.R()),
			VisAvg(refrgba1.G(), refrgba2.G()),
			VisAvg(refrgba1.B(), refrgba2.B()),
			VisAvg(refrgba1.A(), refrgba2.A()));
}

inline CVisRGBA<unsigned long> VisAvg(const CVisRGBA<unsigned long>& refrgba1,
		const CVisRGBA<unsigned long>& refrgba2)
{
	return CVisRGBA<unsigned long>(VisAvg(refrgba1.R(), refrgba2.R()),
			VisAvg(refrgba1.G(), refrgba2.G()),
			VisAvg(refrgba1.B(), refrgba2.B()),
			VisAvg(refrgba1.A(), refrgba2.A()));
}

inline CVisRGBA<unsigned __int64> VisAvg(const CVisRGBA<unsigned __int64>& refrgba1,
		const CVisRGBA<unsigned __int64>& refrgba2)
{
	return CVisRGBA<unsigned __int64>(VisAvg(refrgba1.R(), refrgba2.R()),
			VisAvg(refrgba1.G(), refrgba2.G()),
			VisAvg(refrgba1.B(), refrgba2.B()),
			VisAvg(refrgba1.A(), refrgba2.A()));
}

inline CVisRGBA<float> VisAvg(const CVisRGBA<float>& refrgba1,
		const CVisRGBA<float>& refrgba2)
{
	return CVisRGBA<float>(VisAvg(refrgba1.R(), refrgba2.R()),
			VisAvg(refrgba1.G(), refrgba2.G()),
			VisAvg(refrgba1.B(), refrgba2.B()),
			VisAvg(refrgba1.A(), refrgba2.A()));
}

inline CVisRGBA<double> VisAvg(const CVisRGBA<double>& refrgba1,
		const CVisRGBA<double>& refrgba2)
{
	return CVisRGBA<double>(VisAvg(refrgba1.R(), refrgba2.R()),
			VisAvg(refrgba1.G(), refrgba2.G()),
			VisAvg(refrgba1.B(), refrgba2.B()),
			VisAvg(refrgba1.A(), refrgba2.A()));
}


template<class TPixel>
inline CVisImage<TPixel> VisAvg(const CVisImage<TPixel>& refimage1,
		const CVisImage<TPixel>& refimage2)
{
	assert(&refimage1 != 0);
	assert(&refimage2 != 0);

	CVisImage<TPixel> imageRet;

	CVisShape shape = (refimage1.Shape() & refimage2.Shape());
	int dipixelWidth = shape.Width() * shape.NBands();
	if ((dipixelWidth > 0) && (shape.Height() > 0))
	{
		imageRet.Allocate(shape);
		for (int y = shape.top; y < shape.bottom; ++y)
		{
			const TPixel *ppixelSrc1 = refimage1.PixelAddress(shape.left, y, 0);
			const TPixel *ppixelSrc2 = refimage2.PixelAddress(shape.left, y, 0);
			TPixel *ppixelDest = imageRet.PixelAddress(shape.left, y, 0);
			for (int x = 0; i < dipixelWidth; ++i)
			{
				ppixelDest[i] = VisAvg(ppixelSrc1[i], ppixelSrc2[i]);
			}
		}

	}

	return imageRet;
}

// Also defined using three scalars for use in VisAvgOfRGB.

inline signed char VisAvg(const signed char& refnum1,
		const signed char& refnum2, const signed char& refnum3)
{
	return (signed char) (((unsigned int) (refnum1 + refnum2 + refnum3)) / 3);
}

inline short VisAvg(const short& refnum1, const short& refnum2,
		const short& refnum3)
{
	return (short) (((unsigned int) (refnum1 + refnum2 + refnum3)) / 3);
}

inline int VisAvg(const int& refnum1, const int& refnum2,
		const int& refnum3)
{
	return (int) (((unsigned int) (refnum1 + refnum2 + refnum3)) / 3);
}

inline long VisAvg(const long& refnum1, const long& refnum2,
		const long& refnum3)
{
	return (long) (((unsigned ) (refnum1 + refnum2 + refnum3)) / 3);
}

inline __int64 VisAvg(const __int64& refnum1, const __int64& refnum2,
		const __int64& refnum3)
{
	return (__int64) (((unsigned ) (refnum1 + refnum2 + refnum3)) / 3);
}

inline unsigned char VisAvg(const unsigned char& refnum1,
		const unsigned char& refnum2, const unsigned char& refnum3)
{
	return (unsigned char) ((refnum1 + refnum2 + refnum3 + 1) / 3);
}

inline unsigned short VisAvg(const unsigned short& refnum1,
		const unsigned short& refnum2, const unsigned short& refnum3)
{
	return (unsigned short) ((refnum1 + refnum2 + refnum3 + 1) / 3);
}

inline unsigned int VisAvg(const unsigned int& refnum1,
		const unsigned int& refnum2, const unsigned int& refnum3)
{
	return (unsigned int) ((refnum1 + refnum2 + refnum3 + 1) / 3);
}

inline unsigned long VisAvg(const unsigned long& refnum1,
		const unsigned long& refnum2, const unsigned long& refnum3)
{
	return (unsigned long) ((refnum1 + refnum2 + refnum3 + 1) / 3);
}

inline unsigned __int64 VisAvg(const unsigned __int64& refnum1,
		const unsigned __int64& refnum2, const unsigned __int64& refnum3)
{
	return (unsigned __int64) ((refnum1 + refnum2 + refnum3 + 1) / 3);
}

inline float VisAvg(const float& refnum1, const float& refnum2,
		const float& refnum3)
{
	return (float) ((refnum1 + refnum2 + refnum3) / 3.0);
}

inline double VisAvg(const double& refnum1, const double& refnum2,
		const double& refnum3)
{
	return (double) ((refnum1 + refnum2 + refnum3) / 3.0);
}

//---------------------------------------------------------------------

inline signed char VisMax(const signed char& refnum1,
		const signed char& refnum2)
{
	return ((refnum1 >= refnum2) ? refnum1 : refnum2);
}

inline short VisMax(const short& refnum1, const short& refnum2)
{
	return ((refnum1 >= refnum2) ? refnum1 : refnum2);
}

inline int VisMax(const int& refnum1, const int& refnum2)
{
	return ((refnum1 >= refnum2) ? refnum1 : refnum2);
}

inline long VisMax(const long& refnum1, const long& refnum2)
{
	return ((refnum1 >= refnum2) ? refnum1 : refnum2);
}

inline __int64 VisMax(const __int64& refnum1, const __int64& refnum2)
{
	return ((refnum1 >= refnum2) ? refnum1 : refnum2);
}

inline unsigned char VisMax(const unsigned char& refnum1,
		const unsigned char& refnum2)
{
	return ((refnum1 >= refnum2) ? refnum1 : refnum2);
}

inline unsigned short VisMax(const unsigned short& refnum1,
		const unsigned short& refnum2)
{
	return ((refnum1 >= refnum2) ? refnum1 : refnum2);
}

inline unsigned int VisMax(const unsigned int& refnum1,
		const unsigned int& refnum2)
{
	return ((refnum1 >= refnum2) ? refnum1 : refnum2);
}

inline unsigned long VisMax(const unsigned long& refnum1,
		const unsigned long& refnum2)
{
	return ((refnum1 >= refnum2) ? refnum1 : refnum2);
}

inline unsigned __int64 VisMax(const unsigned __int64& refnum1,
		const unsigned __int64& refnum2)
{
	return ((refnum1 >= refnum2) ? refnum1 : refnum2);
}

inline float VisMax(const float& refnum1, const float& refnum2)
{
	return ((refnum1 >= refnum2) ? refnum1 : refnum2);
}

inline double VisMax(const double& refnum1, const double& refnum2)
{
	return ((refnum1 >= refnum2) ? refnum1 : refnum2);
}


inline CVisRGBA<signed char> VisMax(const CVisRGBA<signed char>& refrgba1,
		const CVisRGBA<signed char>& refrgba2)
{
	return CVisRGBA<signed char>(VisMax(refrgba1.R(), refrgba2.R()),
			VisMax(refrgba1.G(), refrgba2.G()),
			VisMax(refrgba1.B(), refrgba2.B()),
			VisAvg(refrgba1.A(), refrgba2.A()));
}

inline CVisRGBA<short> VisMax(const CVisRGBA<short>& refrgba1,
		const CVisRGBA<short>& refrgba2)
{
	return CVisRGBA<short>(VisMax(refrgba1.R(), refrgba2.R()),
			VisMax(refrgba1.G(), refrgba2.G()),
			VisMax(refrgba1.B(), refrgba2.B()),
			VisAvg(refrgba1.A(), refrgba2.A()));
}

inline CVisRGBA<int> VisMax(const CVisRGBA<int>& refrgba1,
		const CVisRGBA<int>& refrgba2)
{
	return CVisRGBA<int>(VisMax(refrgba1.R(), refrgba2.R()),
			VisMax(refrgba1.G(), refrgba2.G()),
			VisMax(refrgba1.B(), refrgba2.B()),
			VisAvg(refrgba1.A(), refrgba2.A()));
}

inline CVisRGBA<long> VisMax(const CVisRGBA<long>& refrgba1,
		const CVisRGBA<long>& refrgba2)
{
	return CVisRGBA<long>(VisMax(refrgba1.R(), refrgba2.R()),
			VisMax(refrgba1.G(), refrgba2.G()),
			VisMax(refrgba1.B(), refrgba2.B()),
			VisAvg(refrgba1.A(), refrgba2.A()));
}

inline CVisRGBA<__int64> VisMax(const CVisRGBA<__int64>& refrgba1,
		const CVisRGBA<__int64>& refrgba2)
{
	return CVisRGBA<__int64>(VisMax(refrgba1.R(), refrgba2.R()),
			VisMax(refrgba1.G(), refrgba2.G()),
			VisMax(refrgba1.B(), refrgba2.B()),
			VisAvg(refrgba1.A(), refrgba2.A()));
}

inline CVisRGBA<unsigned char> VisMax(const CVisRGBA<unsigned char>& refrgba1,
		const CVisRGBA<unsigned char>& refrgba2)
{
	return CVisRGBA<unsigned char>(VisMax(refrgba1.R(), refrgba2.R()),
			VisMax(refrgba1.G(), refrgba2.G()),
			VisMax(refrgba1.B(), refrgba2.B()),
			VisAvg(refrgba1.A(), refrgba2.A()));
}

inline CVisRGBA<unsigned short> VisMax(const CVisRGBA<unsigned short>& refrgba1,
		const CVisRGBA<unsigned short>& refrgba2)
{
	return CVisRGBA<unsigned short>(VisMax(refrgba1.R(), refrgba2.R()),
			VisMax(refrgba1.G(), refrgba2.G()),
			VisMax(refrgba1.B(), refrgba2.B()),
			VisAvg(refrgba1.A(), refrgba2.A()));
}

inline CVisRGBA<unsigned int> VisMax(const CVisRGBA<unsigned int>& refrgba1,
		const CVisRGBA<unsigned int>& refrgba2)
{
	return CVisRGBA<unsigned int>(VisMax(refrgba1.R(), refrgba2.R()),
			VisMax(refrgba1.G(), refrgba2.G()),
			VisMax(refrgba1.B(), refrgba2.B()),
			VisAvg(refrgba1.A(), refrgba2.A()));
}

inline CVisRGBA<unsigned long> VisMax(const CVisRGBA<unsigned long>& refrgba1,
		const CVisRGBA<unsigned long>& refrgba2)
{
	return CVisRGBA<unsigned long>(VisMax(refrgba1.R(), refrgba2.R()),
			VisMax(refrgba1.G(), refrgba2.G()),
			VisMax(refrgba1.B(), refrgba2.B()),
			VisAvg(refrgba1.A(), refrgba2.A()));
}

inline CVisRGBA<unsigned __int64> VisMax(const CVisRGBA<unsigned __int64>& refrgba1,
		const CVisRGBA<unsigned __int64>& refrgba2)
{
	return CVisRGBA<unsigned __int64>(VisMax(refrgba1.R(), refrgba2.R()),
			VisMax(refrgba1.G(), refrgba2.G()),
			VisMax(refrgba1.B(), refrgba2.B()),
			VisAvg(refrgba1.A(), refrgba2.A()));
}

inline CVisRGBA<float> VisMax(const CVisRGBA<float>& refrgba1,
		const CVisRGBA<float>& refrgba2)
{
	return CVisRGBA<float>(VisMax(refrgba1.R(), refrgba2.R()),
			VisMax(refrgba1.G(), refrgba2.G()),
			VisMax(refrgba1.B(), refrgba2.B()),
			VisAvg(refrgba1.A(), refrgba2.A()));
}

inline CVisRGBA<double> VisMax(const CVisRGBA<double>& refrgba1,
		const CVisRGBA<double>& refrgba2)
{
	return CVisRGBA<double>(VisMax(refrgba1.R(), refrgba2.R()),
			VisMax(refrgba1.G(), refrgba2.G()),
			VisMax(refrgba1.B(), refrgba2.B()),
			VisAvg(refrgba1.A(), refrgba2.A()));
}


template<class TPixel>
inline CVisImage<TPixel> VisMax(const CVisImage<TPixel>& refimage1,
		const CVisImage<TPixel>& refimage2)
{
	assert(&refimage1 != 0);
	assert(&refimage2 != 0);

	CVisImage<TPixel> imageRet;

	CVisShape shape = (refimage1.Shape() & refimage2.Shape());
	int dipixelWidth = shape.Width() * shape.NBands();
	if ((dipixelWidth > 0) && (shape.Height() > 0))
	{
		imageRet.Allocate(shape);
		for (int y = shape.top; y < shape.bottom; ++y)
		{
			const TPixel *ppixelSrc1 = refimage1.PixelAddress(shape.left, y, 0);
			const TPixel *ppixelSrc2 = refimage2.PixelAddress(shape.left, y, 0);
			TPixel *ppixelDest = imageRet.PixelAddress(shape.left, y, 0);
			for (int x = 0; i < dipixelWidth; ++i)
			{
				ppixelDest[i] = VisMax(ppixelSrc1[i], ppixelSrc2[i]);
			}
		}

	}

	return imageRet;
}

// Also defined using three scalars for use in VisMaxOfRGB.

inline signed char VisMax(const signed char& refnum1,
		const signed char& refnum2, const signed char& refnum3)
{
	return ((refnum1 >= refnum2)
			? ((refnum1 >= refnum3) ? refnum1 : refnum3)
			: ((refnum2 >= refnum3) ? refnum2 : refnum3));
}

inline short VisMax(const short& refnum1, const short& refnum2,
		const short& refnum3)
{
	return ((refnum1 >= refnum2)
			? ((refnum1 >= refnum3) ? refnum1 : refnum3)
			: ((refnum2 >= refnum3) ? refnum2 : refnum3));
}

inline int VisMax(const int& refnum1, const int& refnum2,
		const int& refnum3)
{
	return ((refnum1 >= refnum2)
			? ((refnum1 >= refnum3) ? refnum1 : refnum3)
			: ((refnum2 >= refnum3) ? refnum2 : refnum3));
}

inline long VisMax(const long& refnum1, const long& refnum2,
		const long& refnum3)
{
	return ((refnum1 >= refnum2)
			? ((refnum1 >= refnum3) ? refnum1 : refnum3)
			: ((refnum2 >= refnum3) ? refnum2 : refnum3));
}

inline __int64 VisMax(const __int64& refnum1, const __int64& refnum2,
		const __int64& refnum3)
{
	return ((refnum1 >= refnum2)
			? ((refnum1 >= refnum3) ? refnum1 : refnum3)
			: ((refnum2 >= refnum3) ? refnum2 : refnum3));
}

inline unsigned char VisMax(const unsigned char& refnum1,
		const unsigned char& refnum2, const unsigned char& refnum3)
{
	return ((refnum1 >= refnum2)
			? ((refnum1 >= refnum3) ? refnum1 : refnum3)
			: ((refnum2 >= refnum3) ? refnum2 : refnum3));
}

inline unsigned short VisMax(const unsigned short& refnum1,
		const unsigned short& refnum2, const unsigned short& refnum3)
{
	return ((refnum1 >= refnum2)
			? ((refnum1 >= refnum3) ? refnum1 : refnum3)
			: ((refnum2 >= refnum3) ? refnum2 : refnum3));
}

inline unsigned int VisMax(const unsigned int& refnum1,
		const unsigned int& refnum2, const unsigned int& refnum3)
{
	return ((refnum1 >= refnum2)
			? ((refnum1 >= refnum3) ? refnum1 : refnum3)
			: ((refnum2 >= refnum3) ? refnum2 : refnum3));
}

inline unsigned long VisMax(const unsigned long& refnum1,
		const unsigned long& refnum2, const unsigned long& refnum3)
{
	return ((refnum1 >= refnum2)
			? ((refnum1 >= refnum3) ? refnum1 : refnum3)
			: ((refnum2 >= refnum3) ? refnum2 : refnum3));
}

inline unsigned __int64 VisMax(const unsigned __int64& refnum1,
		const unsigned __int64& refnum2, const unsigned __int64& refnum3)
{
	return ((refnum1 >= refnum2)
			? ((refnum1 >= refnum3) ? refnum1 : refnum3)
			: ((refnum2 >= refnum3) ? refnum2 : refnum3));
}

inline float VisMax(const float& refnum1, const float& refnum2,
		const float& refnum3)
{
	return ((refnum1 >= refnum2)
			? ((refnum1 >= refnum3) ? refnum1 : refnum3)
			: ((refnum2 >= refnum3) ? refnum2 : refnum3));
}

inline double VisMax(const double& refnum1, const double& refnum2,
		const double& refnum3)
{
	return ((refnum1 >= refnum2)
			? ((refnum1 >= refnum3) ? refnum1 : refnum3)
			: ((refnum2 >= refnum3) ? refnum2 : refnum3));
}

//---------------------------------------------------------------------

inline signed char VisMin(const signed char& refnum1,
		const signed char& refnum2)
{
	return ((refnum1 <= refnum2) ? refnum1 : refnum2);
}

inline short VisMin(const short& refnum1, const short& refnum2)
{
	return ((refnum1 <= refnum2) ? refnum1 : refnum2);
}

inline int VisMin(const int& refnum1, const int& refnum2)
{
	return ((refnum1 <= refnum2) ? refnum1 : refnum2);
}

inline long VisMin(const long& refnum1, const long& refnum2)
{
	return ((refnum1 <= refnum2) ? refnum1 : refnum2);
}

inline __int64 VisMin(const __int64& refnum1, const __int64& refnum2)
{
	return ((refnum1 <= refnum2) ? refnum1 : refnum2);
}

inline unsigned char VisMin(const unsigned char& refnum1,
		const unsigned char& refnum2)
{
	return ((refnum1 <= refnum2) ? refnum1 : refnum2);
}

inline unsigned short VisMin(const unsigned short& refnum1,
		const unsigned short& refnum2)
{
	return ((refnum1 <= refnum2) ? refnum1 : refnum2);
}

inline unsigned int VisMin(const unsigned int& refnum1,
		const unsigned int& refnum2)
{
	return ((refnum1 <= refnum2) ? refnum1 : refnum2);
}

inline unsigned long VisMin(const unsigned long& refnum1,
		const unsigned long& refnum2)
{
	return ((refnum1 <= refnum2) ? refnum1 : refnum2);
}

inline unsigned __int64 VisMin(const unsigned __int64& refnum1,
		const unsigned __int64& refnum2)
{
	return ((refnum1 <= refnum2) ? refnum1 : refnum2);
}

inline float VisMin(const float& refnum1, const float& refnum2)
{
	return ((refnum1 <= refnum2) ? refnum1 : refnum2);
}

inline double VisMin(const double& refnum1, const double& refnum2)
{
	return ((refnum1 <= refnum2) ? refnum1 : refnum2);
}


inline CVisRGBA<signed char> VisMin(const CVisRGBA<signed char>& refrgba1,
		const CVisRGBA<signed char>& refrgba2)
{
	return CVisRGBA<signed char>(VisMin(refrgba1.R(), refrgba2.R()),
			VisMin(refrgba1.G(), refrgba2.G()),
			VisMin(refrgba1.B(), refrgba2.B()),
			VisAvg(refrgba1.A(), refrgba2.A()));
}

inline CVisRGBA<short> VisMin(const CVisRGBA<short>& refrgba1,
		const CVisRGBA<short>& refrgba2)
{
	return CVisRGBA<short>(VisMin(refrgba1.R(), refrgba2.R()),
			VisMin(refrgba1.G(), refrgba2.G()),
			VisMin(refrgba1.B(), refrgba2.B()),
			VisAvg(refrgba1.A(), refrgba2.A()));
}

inline CVisRGBA<int> VisMin(const CVisRGBA<int>& refrgba1,
		const CVisRGBA<int>& refrgba2)
{
	return CVisRGBA<int>(VisMin(refrgba1.R(), refrgba2.R()),
			VisMin(refrgba1.G(), refrgba2.G()),
			VisMin(refrgba1.B(), refrgba2.B()),
			VisAvg(refrgba1.A(), refrgba2.A()));
}

inline CVisRGBA<long> VisMin(const CVisRGBA<long>& refrgba1,
		const CVisRGBA<long>& refrgba2)
{
	return CVisRGBA<long>(VisMin(refrgba1.R(), refrgba2.R()),
			VisMin(refrgba1.G(), refrgba2.G()),
			VisMin(refrgba1.B(), refrgba2.B()),
			VisAvg(refrgba1.A(), refrgba2.A()));
}

inline CVisRGBA<__int64> VisMin(const CVisRGBA<__int64>& refrgba1,
		const CVisRGBA<__int64>& refrgba2)
{
	return CVisRGBA<__int64>(VisMin(refrgba1.R(), refrgba2.R()),
			VisMin(refrgba1.G(), refrgba2.G()),
			VisMin(refrgba1.B(), refrgba2.B()),
			VisAvg(refrgba1.A(), refrgba2.A()));
}

inline CVisRGBA<unsigned char> VisMin(const CVisRGBA<unsigned char>& refrgba1,
		const CVisRGBA<unsigned char>& refrgba2)
{
	return CVisRGBA<unsigned char>(VisMin(refrgba1.R(), refrgba2.R()),
			VisMin(refrgba1.G(), refrgba2.G()),
			VisMin(refrgba1.B(), refrgba2.B()),
			VisAvg(refrgba1.A(), refrgba2.A()));
}

inline CVisRGBA<unsigned short> VisMin(const CVisRGBA<unsigned short>& refrgba1,
		const CVisRGBA<unsigned short>& refrgba2)
{
	return CVisRGBA<unsigned short>(VisMin(refrgba1.R(), refrgba2.R()),
			VisMin(refrgba1.G(), refrgba2.G()),
			VisMin(refrgba1.B(), refrgba2.B()),
			VisAvg(refrgba1.A(), refrgba2.A()));
}

inline CVisRGBA<unsigned int> VisMin(const CVisRGBA<unsigned int>& refrgba1,
		const CVisRGBA<unsigned int>& refrgba2)
{
	return CVisRGBA<unsigned int>(VisMin(refrgba1.R(), refrgba2.R()),
			VisMin(refrgba1.G(), refrgba2.G()),
			VisMin(refrgba1.B(), refrgba2.B()),
			VisAvg(refrgba1.A(), refrgba2.A()));
}

inline CVisRGBA<unsigned long> VisMin(const CVisRGBA<unsigned long>& refrgba1,
		const CVisRGBA<unsigned long>& refrgba2)
{
	return CVisRGBA<unsigned long>(VisMin(refrgba1.R(), refrgba2.R()),
			VisMin(refrgba1.G(), refrgba2.G()),
			VisMin(refrgba1.B(), refrgba2.B()),
			VisAvg(refrgba1.A(), refrgba2.A()));
}

inline CVisRGBA<unsigned __int64> VisMin(const CVisRGBA<unsigned __int64>& refrgba1,
		const CVisRGBA<unsigned __int64>& refrgba2)
{
	return CVisRGBA<unsigned __int64>(VisMin(refrgba1.R(), refrgba2.R()),
			VisMin(refrgba1.G(), refrgba2.G()),
			VisMin(refrgba1.B(), refrgba2.B()),
			VisAvg(refrgba1.A(), refrgba2.A()));
}

inline CVisRGBA<float> VisMin(const CVisRGBA<float>& refrgba1,
		const CVisRGBA<float>& refrgba2)
{
	return CVisRGBA<float>(VisMin(refrgba1.R(), refrgba2.R()),
			VisMin(refrgba1.G(), refrgba2.G()),
			VisMin(refrgba1.B(), refrgba2.B()),
			VisAvg(refrgba1.A(), refrgba2.A()));
}

inline CVisRGBA<double> VisMin(const CVisRGBA<double>& refrgba1,
		const CVisRGBA<double>& refrgba2)
{
	return CVisRGBA<double>(VisMin(refrgba1.R(), refrgba2.R()),
			VisMin(refrgba1.G(), refrgba2.G()),
			VisMin(refrgba1.B(), refrgba2.B()),
			VisAvg(refrgba1.A(), refrgba2.A()));
}


template<class TPixel>
inline CVisImage<TPixel> VisMin(const CVisImage<TPixel>& refimage1,
		const CVisImage<TPixel>& refimage2)
{
	assert(&refimage1 != 0);
	assert(&refimage2 != 0);

	CVisImage<TPixel> imageRet;

	CVisShape shape = (refimage1.Shape() & refimage2.Shape());
	int dipixelWidth = shape.Width() * shape.NBands();
	if ((dipixelWidth > 0) && (shape.Height() > 0))
	{
		imageRet.Allocate(shape);
		for (int y = shape.top; y < shape.bottom; ++y)
		{
			const TPixel *ppixelSrc1 = refimage1.PixelAddress(shape.left, y, 0);
			const TPixel *ppixelSrc2 = refimage2.PixelAddress(shape.left, y, 0);
			TPixel *ppixelDest = imageRet.PixelAddress(shape.left, y, 0);
			for (int x = 0; i < dipixelWidth; ++i)
			{
				ppixelDest[i] = VisMin(ppixelSrc1[i], ppixelSrc2[i]);
			}
		}

	}

	return imageRet;
}

// Also defined using three scalars for use in VisMinOfRGB.

inline signed char VisMin(const signed char& refnum1,
		const signed char& refnum2, const signed char& refnum3)
{
	return ((refnum1 <= refnum2)
			? ((refnum1 <= refnum3) ? refnum1 : refnum3)
			: ((refnum2 <= refnum3) ? refnum2 : refnum3));
}

inline short VisMin(const short& refnum1, const short& refnum2,
		const short& refnum3)
{
	return ((refnum1 <= refnum2)
			? ((refnum1 <= refnum3) ? refnum1 : refnum3)
			: ((refnum2 <= refnum3) ? refnum2 : refnum3));
}

inline int VisMin(const int& refnum1, const int& refnum2,
		const int& refnum3)
{
	return ((refnum1 <= refnum2)
			? ((refnum1 <= refnum3) ? refnum1 : refnum3)
			: ((refnum2 <= refnum3) ? refnum2 : refnum3));
}

inline long VisMin(const long& refnum1, const long& refnum2,
		const long& refnum3)
{
	return ((refnum1 <= refnum2)
			? ((refnum1 <= refnum3) ? refnum1 : refnum3)
			: ((refnum2 <= refnum3) ? refnum2 : refnum3));
}

inline __int64 VisMin(const __int64& refnum1, const __int64& refnum2,
		const __int64& refnum3)
{
	return ((refnum1 <= refnum2)
			? ((refnum1 <= refnum3) ? refnum1 : refnum3)
			: ((refnum2 <= refnum3) ? refnum2 : refnum3));
}

inline unsigned char VisMin(const unsigned char& refnum1,
		const unsigned char& refnum2, const unsigned char& refnum3)
{
	return ((refnum1 <= refnum2)
			? ((refnum1 <= refnum3) ? refnum1 : refnum3)
			: ((refnum2 <= refnum3) ? refnum2 : refnum3));
}

inline unsigned short VisMin(const unsigned short& refnum1,
		const unsigned short& refnum2, const unsigned short& refnum3)
{
	return ((refnum1 <= refnum2)
			? ((refnum1 <= refnum3) ? refnum1 : refnum3)
			: ((refnum2 <= refnum3) ? refnum2 : refnum3));
}

inline unsigned int VisMin(const unsigned int& refnum1,
		const unsigned int& refnum2, const unsigned int& refnum3)
{
	return ((refnum1 <= refnum2)
			? ((refnum1 <= refnum3) ? refnum1 : refnum3)
			: ((refnum2 <= refnum3) ? refnum2 : refnum3));
}

inline unsigned long VisMin(const unsigned long& refnum1,
		const unsigned long& refnum2, const unsigned long& refnum3)
{
	return ((refnum1 <= refnum2)
			? ((refnum1 <= refnum3) ? refnum1 : refnum3)
			: ((refnum2 <= refnum3) ? refnum2 : refnum3));
}

inline unsigned __int64 VisMin(const unsigned __int64& refnum1,
		const unsigned __int64& refnum2, const unsigned __int64& refnum3)
{
	return ((refnum1 <= refnum2)
			? ((refnum1 <= refnum3) ? refnum1 : refnum3)
			: ((refnum2 <= refnum3) ? refnum2 : refnum3));
}

inline float VisMin(const float& refnum1, const float& refnum2,
		const float& refnum3)
{
	return ((refnum1 <= refnum2)
			? ((refnum1 <= refnum3) ? refnum1 : refnum3)
			: ((refnum2 <= refnum3) ? refnum2 : refnum3));
}

inline double VisMin(const double& refnum1, const double& refnum2,
		const double& refnum3)
{
	return ((refnum1 <= refnum2)
			? ((refnum1 <= refnum3) ? refnum1 : refnum3)
			: ((refnum2 <= refnum3) ? refnum2 : refnum3));
}

//---------------------------------------------------------------------

inline signed char VisAvgOfRGB(const CVisRGBA<signed char>& refrgba)
{
	return VisAvg(refrgba.R(), refrgba.G(), refrgba.B());
}

inline short VisAvgOfRGB(const CVisRGBA<short>& refrgba)
{
	return VisAvg(refrgba.R(), refrgba.G(), refrgba.B());
}

inline int VisAvgOfRGB(const CVisRGBA<int>& refrgba)
{
	return VisAvg(refrgba.R(), refrgba.G(), refrgba.B());
}

inline long VisAvgOfRGB(const CVisRGBA<long>& refrgba)
{
	return VisAvg(refrgba.R(), refrgba.G(), refrgba.B());
}

inline __int64 VisAvgOfRGB(const CVisRGBA<__int64>& refrgba)
{
	return VisAvg(refrgba.R(), refrgba.G(), refrgba.B());
}

inline unsigned char VisAvgOfRGB(const CVisRGBA<unsigned char>& refrgba)
{
	return VisAvg(refrgba.R(), refrgba.G(), refrgba.B());
}

inline unsigned short VisAvgOfRGB(const CVisRGBA<unsigned short>& refrgba)
{
	return VisAvg(refrgba.R(), refrgba.G(), refrgba.B());
}

inline unsigned int VisAvgOfRGB(const CVisRGBA<unsigned int>& refrgba)
{
	return VisAvg(refrgba.R(), refrgba.G(), refrgba.B());
}

inline unsigned long VisAvgOfRGB(const CVisRGBA<unsigned long>& refrgba)
{
	return VisAvg(refrgba.R(), refrgba.G(), refrgba.B());
}

inline unsigned __int64 VisAvgOfRGB(const CVisRGBA<unsigned __int64>& refrgba)
{
	return VisAvg(refrgba.R(), refrgba.G(), refrgba.B());
}

inline float VisAvgOfRGB(const CVisRGBA<float>& refrgba)
{
	return VisAvg(refrgba.R(), refrgba.G(), refrgba.B());
}

inline double VisAvgOfRGB(const CVisRGBA<double>& refrgba)
{
	return VisAvg(refrgba.R(), refrgba.G(), refrgba.B());
}


template<class TPixel>
inline CVisImage<TPixel> VisAvgOfRGB(const CVisImage<CVisRGBA<TPixel> >& refimage)
{
	assert(&refimage != 0);

	CVisImage<TPixel> imageRet;

	if (refimage.IsValid())
	{
		CVisShape shape = refimage.Shape();
		int dipixelWidth = shape.Width() * shape.NBands();
		imageRet.Allocate(shape);
		for (int y = shape.top; y < shape.bottom; ++y)
		{
			const CVisRGBA<TPixel> *ppixelSrc = refimage.PixelAddress(shape.left, y, 0);
			TPixel *ppixelDest = imageRet.PixelAddress(shape.left, y, 0);
			for (int x = 0; i < dipixelWidth; ++i)
			{
				ppixelDest[i] = VisAvgOfRGB(ppixelSrc[i]);
			}
		}

	}

	return imageRet;
}

//---------------------------------------------------------------------

inline signed char VisMaxOfRGB(const CVisRGBA<signed char>& refrgba)
{
	return VisMax(refrgba.R(), refrgba.G(), refrgba.B());
}

inline short VisMaxOfRGB(const CVisRGBA<short>& refrgba)
{
	return VisMax(refrgba.R(), refrgba.G(), refrgba.B());
}

inline int VisMaxOfRGB(const CVisRGBA<int>& refrgba)
{
	return VisMax(refrgba.R(), refrgba.G(), refrgba.B());
}

inline long VisMaxOfRGB(const CVisRGBA<long>& refrgba)
{
	return VisMax(refrgba.R(), refrgba.G(), refrgba.B());
}

inline __int64 VisMaxOfRGB(const CVisRGBA<__int64>& refrgba)
{
	return VisMax(refrgba.R(), refrgba.G(), refrgba.B());
}

inline unsigned char VisMaxOfRGB(const CVisRGBA<unsigned char>& refrgba)
{
	return VisMax(refrgba.R(), refrgba.G(), refrgba.B());
}

inline unsigned short VisMaxOfRGB(const CVisRGBA<unsigned short>& refrgba)
{
	return VisMax(refrgba.R(), refrgba.G(), refrgba.B());
}

inline unsigned int VisMaxOfRGB(const CVisRGBA<unsigned int>& refrgba)
{
	return VisMax(refrgba.R(), refrgba.G(), refrgba.B());
}

inline unsigned long VisMaxOfRGB(const CVisRGBA<unsigned long>& refrgba)
{
	return VisMax(refrgba.R(), refrgba.G(), refrgba.B());
}

inline unsigned __int64 VisMaxOfRGB(const CVisRGBA<unsigned __int64>& refrgba)
{
	return VisMax(refrgba.R(), refrgba.G(), refrgba.B());
}

inline float VisMaxOfRGB(const CVisRGBA<float>& refrgba)
{
	return VisMax(refrgba.R(), refrgba.G(), refrgba.B());
}

inline double VisMaxOfRGB(const CVisRGBA<double>& refrgba)
{
	return VisMax(refrgba.R(), refrgba.G(), refrgba.B());
}


template<class TPixel>
inline CVisImage<TPixel> VisMaxOfRGB(const CVisImage<CVisRGBA<TPixel> >& refimage)
{
	assert(&refimage != 0);

	CVisImage<TPixel> imageRet;

	if (refimage.IsValid())
	{
		CVisShape shape = refimage.Shape();
		int dipixelWidth = shape.Width() * shape.NBands();
		imageRet.Allocate(shape);
		for (int y = shape.top; y < shape.bottom; ++y)
		{
			const CVisRGBA<TPixel> *ppixelSrc = refimage.PixelAddress(shape.left, y, 0);
			TPixel *ppixelDest = imageRet.PixelAddress(shape.left, y, 0);
			for (int x = 0; i < dipixelWidth; ++i)
			{
				ppixelDest[i] = VisMaxOfRGB(ppixelSrc[i]);
			}
		}

	}

	return imageRet;
}

//---------------------------------------------------------------------

inline signed char VisMinOfRGB(const CVisRGBA<signed char>& refrgba)
{
	return VisMin(refrgba.R(), refrgba.G(), refrgba.B());
}

inline short VisMinOfRGB(const CVisRGBA<short>& refrgba)
{
	return VisMin(refrgba.R(), refrgba.G(), refrgba.B());
}

inline int VisMinOfRGB(const CVisRGBA<int>& refrgba)
{
	return VisMin(refrgba.R(), refrgba.G(), refrgba.B());
}

inline long VisMinOfRGB(const CVisRGBA<long>& refrgba)
{
	return VisMin(refrgba.R(), refrgba.G(), refrgba.B());
}

inline __int64 VisMinOfRGB(const CVisRGBA<__int64>& refrgba)
{
	return VisMin(refrgba.R(), refrgba.G(), refrgba.B());
}

inline unsigned char VisMinOfRGB(const CVisRGBA<unsigned char>& refrgba)
{
	return VisMin(refrgba.R(), refrgba.G(), refrgba.B());
}

inline unsigned short VisMinOfRGB(const CVisRGBA<unsigned short>& refrgba)
{
	return VisMin(refrgba.R(), refrgba.G(), refrgba.B());
}

inline unsigned int VisMinOfRGB(const CVisRGBA<unsigned int>& refrgba)
{
	return VisMin(refrgba.R(), refrgba.G(), refrgba.B());
}

inline unsigned long VisMinOfRGB(const CVisRGBA<unsigned long>& refrgba)
{
	return VisMin(refrgba.R(), refrgba.G(), refrgba.B());
}

inline unsigned __int64 VisMinOfRGB(const CVisRGBA<unsigned __int64>& refrgba)
{
	return VisMin(refrgba.R(), refrgba.G(), refrgba.B());
}

inline float VisMinOfRGB(const CVisRGBA<float>& refrgba)
{
	return VisMin(refrgba.R(), refrgba.G(), refrgba.B());
}

inline double VisMinOfRGB(const CVisRGBA<double>& refrgba)
{
	return VisMin(refrgba.R(), refrgba.G(), refrgba.B());
}


template<class TPixel>
inline CVisImage<TPixel> VisMinOfRGB(const CVisImage<CVisRGBA<TPixel> >& refimage)
{
	assert(&refimage != 0);

	CVisImage<TPixel> imageRet;

	if (refimage.IsValid())
	{
		CVisShape shape = refimage.Shape();
		int dipixelWidth = shape.Width() * shape.NBands();
		imageRet.Allocate(shape);
		for (int y = shape.top; y < shape.bottom; ++y)
		{
			const CVisRGBA<TPixel> *ppixelSrc = refimage.PixelAddress(shape.left, y, 0);
			TPixel *ppixelDest = imageRet.PixelAddress(shape.left, y, 0);
			for (int x = 0; i < dipixelWidth; ++i)
			{
				ppixelDest[i] = VisMinOfRGB(ppixelSrc[i]);
			}
		}

	}

	return imageRet;
}

//---------------------------------------------------------------------

inline int VisSumOfRGB(const CVisRGBA<signed char>& refrgba)
{
	return (refrgba.R() + refrgba.G() + refrgba.B());
}

inline int VisSumOfRGB(const CVisRGBA<short>& refrgba)
{
	return (refrgba.R() + refrgba.G() + refrgba.B());
}

inline int VisSumOfRGB(const CVisRGBA<int>& refrgba)
{
	return (refrgba.R() + refrgba.G() + refrgba.B());
}

inline long VisSumOfRGB(const CVisRGBA<long>& refrgba)
{
	return (refrgba.R() + refrgba.G() + refrgba.B());
}

inline __int64 VisSumOfRGB(const CVisRGBA<__int64>& refrgba)
{
	return (refrgba.R() + refrgba.G() + refrgba.B());
}

inline unsigned int VisSumOfRGB(const CVisRGBA<unsigned char>& refrgba)
{
	return (refrgba.R() + refrgba.G() + refrgba.B());
}

inline unsigned int VisSumOfRGB(const CVisRGBA<unsigned short>& refrgba)
{
	return (refrgba.R() + refrgba.G() + refrgba.B());
}

inline unsigned int VisSumOfRGB(const CVisRGBA<unsigned int>& refrgba)
{
	return (refrgba.R() + refrgba.G() + refrgba.B());
}

inline unsigned long VisSumOfRGB(const CVisRGBA<unsigned long>& refrgba)
{
	return (refrgba.R() + refrgba.G() + refrgba.B());
}

inline unsigned __int64 VisSumOfRGB(const CVisRGBA<unsigned __int64>& refrgba)
{
	return (refrgba.R() + refrgba.G() + refrgba.B());
}

inline float VisSumOfRGB(const CVisRGBA<float>& refrgba)
{
	return (float) (refrgba.R() + refrgba.G() + refrgba.B());
}

inline double VisSumOfRGB(const CVisRGBA<double>& refrgba)
{
	return (refrgba.R() + refrgba.G() + refrgba.B());
}


template<class TPixel>
inline CVisImage<TPixel> VisSumOfRGB(const CVisImage<CVisRGBA<TPixel> >& refimage)
{
	assert(&refimage != 0);

	CVisImage<TPixel> imageRet;

	if (refimage.IsValid())
	{
		CVisShape shape = refimage.Shape();
		int dipixelWidth = shape.Width() * shape.NBands();
		imageRet.Allocate(shape);
		for (int y = shape.top; y < shape.bottom; ++y)
		{
			const CVisRGBA<TPixel> *ppixelSrc = refimage.PixelAddress(shape.left, y, 0);
			TPixel *ppixelDest = imageRet.PixelAddress(shape.left, y, 0);
			for (int x = 0; i < dipixelWidth; ++i)
			{
				ppixelDest[i] = VisSumOfRGB(ppixelSrc[i]);
			}
		}

	}

	return imageRet;
}

//---------------------------------------------------------------------

inline int VisSumOfSquaresOfRGB(const CVisRGBA<signed char>& refrgba)
{
	return (refrgba.R() * refrgba.R() + refrgba.G() * refrgba.G()
			+ refrgba.B() * refrgba.B());
}

inline int VisSumOfSquaresOfRGB(const CVisRGBA<short>& refrgba)
{
	return (refrgba.R() * refrgba.R() + refrgba.G() * refrgba.G()
			+ refrgba.B() * refrgba.B());
}

inline int VisSumOfSquaresOfRGB(const CVisRGBA<int>& refrgba)
{
	return (refrgba.R() * refrgba.R() + refrgba.G() * refrgba.G()
			+ refrgba.B() * refrgba.B());
}

inline long VisSumOfSquaresOfRGB(const CVisRGBA<long>& refrgba)
{
	return (refrgba.R() * refrgba.R() + refrgba.G() * refrgba.G()
			+ refrgba.B() * refrgba.B());
}

inline __int64 VisSumOfSquaresOfRGB(const CVisRGBA<__int64>& refrgba)
{
	return (refrgba.R() * refrgba.R() + refrgba.G() * refrgba.G()
			+ refrgba.B() * refrgba.B());
}

inline unsigned int VisSumOfSquaresOfRGB(const CVisRGBA<unsigned char>& refrgba)
{
	return (refrgba.R() * refrgba.R() + refrgba.G() * refrgba.G()
			+ refrgba.B() * refrgba.B());
}

inline unsigned int VisSumOfSquaresOfRGB(const CVisRGBA<unsigned short>& refrgba)
{
	return (refrgba.R() * refrgba.R() + refrgba.G() * refrgba.G()
			+ refrgba.B() * refrgba.B());
}

inline unsigned int VisSumOfSquaresOfRGB(const CVisRGBA<unsigned int>& refrgba)
{
	return (refrgba.R() * refrgba.R() + refrgba.G() * refrgba.G()
			+ refrgba.B() * refrgba.B());
}

inline unsigned long VisSumOfSquaresOfRGB(const CVisRGBA<unsigned long>& refrgba)
{
	return (refrgba.R() * refrgba.R() + refrgba.G() * refrgba.G()
			+ refrgba.B() * refrgba.B());
}

inline unsigned __int64 VisSumOfSquaresOfRGB(const CVisRGBA<unsigned __int64>& refrgba)
{
	return (refrgba.R() * refrgba.R() + refrgba.G() * refrgba.G()
			+ refrgba.B() * refrgba.B());
}

inline float VisSumOfSquaresOfRGB(const CVisRGBA<float>& refrgba)
{
	return (refrgba.R() * refrgba.R() + refrgba.G() * refrgba.G()
			+ refrgba.B() * refrgba.B());
}

inline double VisSumOfSquaresOfRGB(const CVisRGBA<double>& refrgba)
{
	return (refrgba.R() * refrgba.R() + refrgba.G() * refrgba.G()
			+ refrgba.B() * refrgba.B());
}


template<class TPixel>
inline CVisImage<TPixel> VisSumOfSquaresOfRGB(const CVisImage<CVisRGBA<TPixel> >& refimage)
{
	assert(&refimage != 0);

	CVisImage<TPixel> imageRet;

	if (refimage.IsValid())
	{
		CVisShape shape = refimage.Shape();
		int dipixelWidth = shape.Width() * shape.NBands();
		imageRet.Allocate(shape);
		for (int y = shape.top; y < shape.bottom; ++y)
		{
			const CVisRGBA<TPixel> *ppixelSrc = refimage.PixelAddress(shape.left, y, 0);
			TPixel *ppixelDest = imageRet.PixelAddress(shape.left, y, 0);
			for (int x = 0; i < dipixelWidth; ++i)
			{
				ppixelDest[i] = VisSumOfSquaresOfRGB(ppixelSrc[i]);
			}
		}

	}

	return imageRet;
}

//---------------------------------------------------------------------

inline signed char VisBrightnessFromRGBA(const CVisRGBA<signed char>& refrgba)
{
	const unsigned int nYR8 = (((unsigned int) (0x2020202 * .299 + 1)) >> 1);
	const unsigned int nYG8 = (((unsigned int) (0x2020202 * .587 + 1)) >> 1);
	const unsigned int nYB8 = (((unsigned int) (0x2020202 * .114 + 1)) >> 1);
	return (signed char)
			((nYR8 * ((unsigned int) refrgba.R())
			+ nYG8 * ((unsigned int) refrgba.G())
			+ nYB8 * ((unsigned int) refrgba.B()))
					>> 24);
}

inline short VisBrightnessFromRGBA(const CVisRGBA<short>& refrgba)
{
	const unsigned int nYR16 = (((unsigned int) (0x20002 * .299 + 1)) >> 1);
	const unsigned int nYG16 = (((unsigned int) (0x20002 * .587 + 1)) >> 1);
	const unsigned int nYB16 = (((unsigned int) (0x20002 * .114 + 1)) >> 1);
	return (signed char)
			((nYR16 * ((unsigned int) refrgba.R())
			+ nYG16 * ((unsigned int) refrgba.G())
			+ nYB16 * ((unsigned int) refrgba.B()))
					>> 24);
}

inline int VisBrightnessFromRGBA(const CVisRGBA<int>& refrgba)
{
	return (int)
			(.299 * refrgba.R() + .587 * refrgba.G() + .114 * refrgba.B());
}

inline long VisBrightnessFromRGBA(const CVisRGBA<long>& refrgba)
{
	return (long)
			(.299 * refrgba.R() + .587 * refrgba.G() + .114 * refrgba.B());
}

inline __int64 VisBrightnessFromRGBA(const CVisRGBA<__int64>& refrgba)
{
	return (__int64)
			(.299 * refrgba.R() + .587 * refrgba.G() + .114 * refrgba.B());
}

inline unsigned char VisBrightnessFromRGBA(const CVisRGBA<unsigned char>& refrgba)
{
	const unsigned int nYR8 = (((unsigned int) (0x2020202 * .299 + 1)) >> 1);
	const unsigned int nYG8 = (((unsigned int) (0x2020202 * .587 + 1)) >> 1);
	const unsigned int nYB8 = (((unsigned int) (0x2020202 * .114 + 1)) >> 1);
	return (unsigned char)
			((nYR8 * refrgba.R() + nYG8 * refrgba.G() + nYB8 * refrgba.B())
					>> 24);
}

inline unsigned short VisBrightnessFromRGBA(const CVisRGBA<unsigned short>& refrgba)
{
	const unsigned int nYR16 = (((unsigned int) (0x20002 * .299 + 1)) >> 1);
	const unsigned int nYG16 = (((unsigned int) (0x20002 * .587 + 1)) >> 1);
	const unsigned int nYB16 = (((unsigned int) (0x20002 * .114 + 1)) >> 1);
	return (unsigned short)
			((nYR16 * refrgba.R() + nYG16 * refrgba.G() + nYB16 * refrgba.B())
					>> 16);
}

inline unsigned int VisBrightnessFromRGBA(const CVisRGBA<unsigned int>& refrgba)
{
	return (unsigned int)
			(.299 * refrgba.R() + .587 * refrgba.G() + .114 * refrgba.B());
}

inline unsigned long VisBrightnessFromRGBA(const CVisRGBA<unsigned long>& refrgba)
{
	return (unsigned long)
			(.299 * refrgba.R() + .587 * refrgba.G() + .114 * refrgba.B());
}

inline unsigned __int64 VisBrightnessFromRGBA(const CVisRGBA<unsigned __int64>& refrgba)
{
	return (unsigned __int64)
			(.299 * refrgba.R() + .587 * refrgba.G() + .114 * refrgba.B());
}

inline float VisBrightnessFromRGBA(const CVisRGBA<float>& refrgba)
{
	return (float)
			(.299 * refrgba.R() + .587 * refrgba.G() + .114 * refrgba.B());
}

inline double VisBrightnessFromRGBA(const CVisRGBA<double>& refrgba)
{
	return (.299 * refrgba.R() + .587 * refrgba.G() + .114 * refrgba.B());
}


template<class TPixel>
inline CVisImage<TPixel> VisBrightnessFromRGBA(const CVisImage<CVisRGBA<TPixel> >& refimage)
{
	assert(&refimage != 0);

	CVisImage<TPixel> imageRet;

	if (refimage.IsValid())
	{
		CVisShape shape = refimage.Shape();
		int dipixelWidth = shape.Width() * shape.NBands();
		imageRet.Allocate(shape);
		for (int y = shape.top; y < shape.bottom; ++y)
		{
			const CVisRGBA<TPixel> *ppixelSrc = refimage.PixelAddress(shape.left, y, 0);
			TPixel *ppixelDest = imageRet.PixelAddress(shape.left, y, 0);
			for (int x = 0; i < dipixelWidth; ++i)
			{
				ppixelDest[i] = VisBrightnessFromRGBA(ppixelSrc[i]);
			}
		}

	}

	return imageRet;
}

// Specialization.
VisCoreExport CVisImage<BYTE> VisBrightnessFromRGBA(
		const CVisImage<CVisRGBA<BYTE> >& refimageRGBA);

// Old name.  (Use "brightness" in new code, as it better
// decribes the formula used.)
#define VisIntensityFromRGBA VisBrightnessFromRGBA

