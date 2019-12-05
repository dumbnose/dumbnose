/*  -------------------------------------------------------  *\
    Softricity, Inc.
    332 Congress Street
    Boston, MA 02210


    Highly Confidential Information
    Copyright © 2001 Softricity, Inc.
    Property of Softricity, Inc.
    All rights reserved

    File: tclassfactory.h

    Author: John Sheehan

    Description: Factory for COM Server COM object
\*  -------------------------------------------------------  */

#pragma once


#include <unknwn.h>
#include "com_unknown.hpp"
#include "boost/mpl/list.hpp"

namespace mpl = boost::mpl;

namespace dumbnose {

/*  -------------------------------------------------------  *\
	Implements the Class Factory interface

	This is a basic implementation for an OutOfProcess COM
	Server that doesn't implement IClassFactory2 or utilize
	aggregation.  Once time is allotted, we can either 
	implement the rest of the functionality or use some 
	other existing implementation.
\*  -------------------------------------------------------  */
template<class C>
class class_factory_t : public unknown_t<mpl::list<IClassFactory> >
{
public:
	//
	// IClassFactory
	//
	STDMETHOD(CreateInstance)(IUnknown *pUnkOuter, REFIID riid, void **ppvObject)
	{
		// Make sure they are not trying to aggregate us
		if(pUnkOuter) {
			return CLASS_E_NOAGGREGATION;
		}

		// Create the object and return a pointer to the interface requested
		C* comServer = new C();
		if(!comServer) {
			return E_OUTOFMEMORY;
		}

		// Try to get the interface from the object.
		HRESULT hr = comServer->QueryInterface(riid, ppvObject);

		// If QueryInterface failed, the object will delete itself, because
		// the refcount wasn't incremented.  Otherwise, it won't.
		comServer->Release();

		return hr;

	}

	// Not implementing at this point, because it is only useful for InProc
	// servers
	STDMETHOD(LockServer)(BOOL fLock)
	{
		return S_OK;
	}

	// Construction / destruction
	class_factory_t<C>() : m_regToken(0){};
	virtual ~class_factory_t<C>() {};

	HRESULT RegisterClassObject(REFCLSID clsid,DWORD ctx,DWORD flags)
	{
		// Need an IUnknown pointer to the factory to register with COM.
		IUnknown* unk;
		HRESULT hr = this->QueryInterface(IID_IUnknown,reinterpret_cast<void**>(&unk));
		if (FAILED(hr)) {
			return hr;
		}

		// Register the actual object, specifying that it will handle requests from multiple
		// clients.
		hr = ::CoRegisterClassObject(clsid,unk,ctx,flags,&m_regToken);

		return hr;
	}

private:
	DWORD m_regToken;

};


} // namespace dumbnose
