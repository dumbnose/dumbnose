/*  -------------------------------------------------------  *\
    Softricity, Inc.
    332 Congress Street
    Boston, MA 02210


    Highly Confidential Information
    Copyright © 2001 Softricity, Inc.
    Property of Softricity, Inc.
    All rights reserved

    File: tunknown.h

    Author: John Sheehan

    Description: Implements IUnknown Interface for COM
	objects.
\*  -------------------------------------------------------  */

#pragma once

#pragma warning(disable: 4786)

#include <unknwn.h>
#include "boost/mpl/inherit_linearly.hpp"
#include "boost/mpl/int.hpp"
#include "boost/mpl/list.hpp"

namespace mpl = boost::mpl;
using namespace mpl::placeholders;

// TODO:  Remove.  For debugging
#include <sstream>

namespace dumbnose {

/*  -------------------------------------------------------  *\
	Implements IUnknown for each Interface in the list.
\*  -------------------------------------------------------  */
template<class base_t,class type_t>
class query_interface_impl :	public base_t,
								public type_t
{
public:
	virtual STDMETHODIMP
	QueryInterface(REFIID riid, void **ppvObject)
	{
		if((riid != IID_IUnknown) && (riid != __uuidof(type_t))) {
			return base_t::QueryInterface(riid, ppvObject);
		}

		*ppvObject = static_cast<type_t*>(this);
		static_cast<IUnknown*>(*ppvObject)->AddRef();

		return S_OK;
	}
};

/*  -------------------------------------------------------  *\
	Handle termination case
\*  -------------------------------------------------------  */
class empty_query_interface_impl
{
public:
	virtual STDMETHODIMP
	QueryInterface(REFIID riid, void **ppvObject)
	{
		*ppvObject = NULL;
		return E_NOINTERFACE;
	}
};

/*  -------------------------------------------------------  *\
	Implements the IUnknown interface
\*  -------------------------------------------------------  */
template<class interface_list>
class unknown_t : 
	public mpl::inherit_linearly<
				interface_list,
				query_interface_impl<_,_>,
				empty_query_interface_impl
			>::type
{
public:
	// Need to initilize the count to 1.  The ClassFactory will call release
	// once it is done QueryInterfacing it.
	unknown_t() : m_refCount(1){}

	virtual ULONG STDMETHODCALLTYPE AddRef()
	{
		return ::InterlockedIncrement(&m_refCount);
	}

	virtual ULONG STDMETHODCALLTYPE Release()
	{
		::InterlockedDecrement(&m_refCount);

		if (m_refCount == 0)
		{
			delete this;
			return 0;
		}
		return m_refCount;
	}

protected:
	long m_refCount;

};

} // namespace dumbnose
