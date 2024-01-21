// VisMeteorProvider.h
//
// Copyright © 1997-2000 Microsoft Corporation, All Rights Reserved

#include <vector>


// Structure containing the information needed (along with m_listszDig)
// to specify a device to be created.
struct SVisSDC
{
	int iSys, iDig, iChannel;
	inline SVisSDC(void) {}
	inline bool operator==(const struct SVisSDC& refsdc) const
		{ return (memcmp(this, &refsdc, sizeof(SVisSDC)) == 0); }
	inline bool operator!=(const struct SVisSDC& refsdc) const
		{ return (memcmp(this, &refsdc, sizeof(SVisSDC)) != 0); }
	inline bool operator<(const struct SVisSDC& refsdc) const
		{ return (memcmp(this, &refsdc, sizeof(SVisSDC)) < 0); }
};


class CVisMeteorProvider : public IVisImSrcProvider
{
public:
	//------------------------------------------------------------------
	// @group Getting a pointer to the provider object.

    static CVisMeteorProvider *PMeteorProviderGet(void);


	//------------------------------------------------------------------
	// @group IUnknown methods

    STDMETHOD(QueryInterface) (THIS_ REFIID riid, void **ppvObject);
    STDMETHOD_(ULONG, AddRef) (THIS);
    STDMETHOD_(ULONG, Release) (THIS);


	//------------------------------------------------------------------
	// @group IVisImSrcProvider methods

    STDMETHOD(HasConnectedDevices) (THIS);
    STDMETHOD(HasSelectionDialog) (THIS);
    STDMETHOD_(LONG, DoSelectionDialog) (THIS_
			HWND hwndParent VISDEFAULT(0));
    STDMETHOD(GetConnectedDeviceList) (THIS_ IVisListSz **ppIVisListSz);
    STDMETHOD(GetDeviceSz) (THIS_ IVisImSrcDevice **ppIVisImSrcDevice,
			LPCSTR sz);
    STDMETHOD(GetDeviceI) (THIS_ IVisImSrcDevice **ppIVisImSrcDevice,
			LONG i);
    STDMETHOD(GetSzID) (THIS_ LPSTR sz, LONG *pcb);
    STDMETHOD(GetSzName) (THIS_ LPSTR sz, LONG *pcb);


	//------------------------------------------------------------------
	// @group CVisMeteorProvider methods

	// Helper function used by devices when they are created.
	const char *SzNameGetIDevice(int iDevice) const;
	SVisSDC SdcGetIDevice(int iDevice) const;
	long LSysGetISys(int iSys) const;
	char *SzDigGetIDig(int iDig) const;
	char *SzChannelGetIChannel(int iChannel) const;
	const CVisMeteorBufList& MeteorBufList(void) const;
	CVisMeteorBufList& MeteorBufList(void);


private:
	// Note:  Meteor documentation says that we can check for 16 devices
	// (systems), but we crash if we try to check for devices 11-16.
	enum { e_iMeteorDevLim = 10 };

	// UNDONE:  It might be okay to keep the provider ID, but we should
	// probably get the name from the registry.
	static const char *s_szID;		// UNDONE:  Replace global string
	static const char *s_szName;	// UNDONE:  Replace global string

	static long s_rglSysNum[16];
	static char *s_rgszDigFormat[15];
	static char *s_rgszChannel[5];

	unsigned long m_cRef;
	MIL_ID m_milidApp;
	CVisListSz *m_plistszConnectedDevices;
	CVisListSz m_listszDig;
	std::vector< SVisSDC > m_mpisdcConnected;

	CVisMeteorBufList m_meteorbuflist;


	static CVisMeteorProvider *s_pMeteorProvider;


	//------------------------------------------------------------------
	// @group Constructor and destructor

    CVisMeteorProvider(void);
	virtual ~CVisMeteorProvider(void);


	//------------------------------------------------------------------
	// @group Private helper methods

	void  FindConnectedDevices(void);
    HRESULT FindDeviceSz(IVisImSrcDevice **ppIVisImSrcDevice, LPCSTR sz);
};


#include "VisMeteorProvider.inl"
