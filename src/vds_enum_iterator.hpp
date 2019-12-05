#pragma once

#include <atlbase.h>
#include <dumbnose/windows_exception.hpp>
#include <boost/iterator_adaptors.hpp>
#include <boost/noncopyable.hpp>
#include <comip.h>


namespace dumbnose {

template<class T, const IID* piid = &__uuidof(T), class P = _com_ptr_t<_com_IIID<T, &__uuidof(T)> > >
class vds_enum_iterator
	:	public boost::iterator_facade<
				vds_enum_iterator<T,piid>,
				P,
				boost::forward_traversal_tag> ,
		boost::noncopyable
{
public:

	// default constructor (creates an iterator equal to end)
	vds_enum_iterator()
	{
	}

	vds_enum_iterator(CComQIPtr<IEnumVdsObject>& it)
	{
		enumerator_ = it;

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

    bool equal(vds_enum_iterator<T,piid> const& other) const
    {
        return this->value_ == other.value_;
    }

    P& dereference() const { return value_; }

	_com_ptr_t<IEnumVdsObject> enumerator_;
	mutable P value_;
};


} // namespace dumbnose
