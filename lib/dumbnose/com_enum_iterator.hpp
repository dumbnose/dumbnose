#pragma once

#include <atlbase.h>
#include <dumbnose/windows_exception.hpp>
#include <boost/iterator_adaptors.hpp>

#pragma warning(push)
#pragma warning(disable:4244)
#include <comip.h>
#pragma warning(pop)


namespace dumbnose {

template<class T, const IID* piid = &__uuidof(T), class P = _com_ptr_t<_com_IIID<T, &__uuidof(T)> > >
class com_enum_iterator 
	:	public boost::iterator_facade<
				com_enum_iterator<T,piid>,
				P,
				boost::forward_traversal_tag>
{
public:
	// @todo:  copy constructor

	// default constructor (creates an iterator equal to end)
	com_enum_iterator()
	{
	}

	template<class container_t>
	com_enum_iterator(container_t container)
	{
		_com_ptr_t<_com_IIID<IUnknown, &__uuidof(IUnknown)> > enum_unk;
		HRESULT hr = container->get__newEnum(&enum_unk);
		if(FAILED(hr)) throw windows_exception("Could not get enumerator", hr);

		enumerator_ = enum_unk;
		if(enumerator_==NULL) throw windows_exception("Could not cast enumerator", -1);

		increment();
	}

private:
	friend class boost::iterator_core_access;

	void increment()
	{
		if(value_!=NULL) value_.Release();
		unsigned long fetched=0;
		_variant_t temp;
		HRESULT hr = enumerator_->Next(1,&temp,&fetched);
		if(temp.vt!=VT_EMPTY) value_ = temp.punkVal;
	}

    bool equal(com_enum_iterator<T,piid> const& other) const
    {
        return this->value_ == other.value_;
    }

    P& dereference() const { return value_; }

	_com_ptr_t<_com_IIID<IEnumVARIANT, &IID_IEnumVARIANT> > enumerator_;
	mutable P value_;
};


} // namespace dumbnose
