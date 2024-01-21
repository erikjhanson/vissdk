// VisMetBufList.h
//
// Copyright © 1997-2000 Microsoft Corporation, All Rights Reserved

#pragma once
#include <vector>


// UNDONE:  SequenceList code needs to check / set num bufs and copy if not
// enough bufs.  (Meteor code may need to use volatile bufs if enough can't
// be allocated.)

// UNDONE:  CVisMeteor::Release should free 

// UNDONE:  We need a global pointer to the CVisMeteorProvider object and 
// a way to get a buflist from the provider.  (Do we also want a list of
// CVisMeteor objects that are in use?)


// UNDONE:  Other code changes:  Default provider / device key and dlg checkbox,
// flag to use default instead of dlg if default is available.



// Forward Declaration
class CVisMeteor;



class CVisMeteorBufList
{
public:
	CVisMeteorBufList(void);

	~CVisMeteorBufList(void);


	int CBufCur(const CVisMeteor *pMeteor = 0);

	int CBufInUse(const CVisMeteor *pMeteor = 0);

	int CBufUnused(const CVisMeteor *pMeteor = 0);


	class CVisMeteorBuf *PBufGetPMeteor(CVisMeteor *pMeteor);

	void ReleaseBuf(class CVisMeteorBuf *pBuf);

	static void VISAPI StaticReleaseBuf(void *pvData, void *pvBuf);

	void ReallocBufs(CVisMeteor *pMeteor, int cBuf);

	void RemoveBufs(const CVisMeteor *pMeteor);


private:
	CRITICAL_SECTION m_criticalsection;
	std::vector<class CVisMeteorBuf *> m_vectorpMeteorBuf;

	void RemoveBuf(class CVisMeteorBuf *pMeteorBuf);
};



class CVisMeteorBuf
{
public:
	//------------------------------------------------------------------
	// @group Constructor and destructor

	CVisMeteorBuf(void);

	~CVisMeteorBuf(void);


	//------------------------------------------------------------------
	// @group State flags

	bool FIsValid(void) const;

	bool FIsInUse(void) const;

	void SetFIsInUse(bool f);


	//------------------------------------------------------------------
	// @group Properties

	MIL_ID Milid(void) const;

	EVisImEncoding ImEncoding(void) const;

	int Dx(void) const;

	int Dy(void) const;

	SIZE Size(void) const;

	int CbData(void) const;

	void *PvData(void) const;

	class CVisMeteor *PMeteor(void) const;


	//------------------------------------------------------------------
	// @group Buffer allocation

	void Allocate(CVisMeteor *pMeteor);

	void Deallocate(void);


private:
	bool m_fInUse;
	MIL_ID m_milidBuf;
	EVisImEncoding m_evisimencoding;
	SIZE m_size;
	int m_cbData;
	void *m_pvData;
	class CVisMeteor *m_pMeteor;

#ifdef _DEBUG
	friend CVisMeteorBufList::~CVisMeteorBufList(void);
	static int s_cBufInUseCur;
	static int s_cBufInUseHigh;
	static void IncCBufInUse(void);
	static void DecCBufInUse(void);
#endif // _DEBUG
};


#include "VisMetBufList.inl"
