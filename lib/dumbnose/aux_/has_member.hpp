#pragma once

#include "boost/mpl/aux_/yes_no.hpp"

namespace dumbnose { namespace aux {


template<
	typename type_t, 
	typename signature_t,
	template<typename,typename,template<signature_t> class> class member_extractor_t
>
class has_member
{
	template<signature_t> class wrap_t{};

	typedef member_extractor_t<type_t,signature_t,wrap_t> base_t;

	// if this nested struct is not used, it causes the compiler to generate
	// an INTERNAL COMPILER ERROR
	struct has_member_impl
		: public base_t
	{
		// this matches if the base class one doesn't
		template<typename dummy_t> 
		static boost::mpl::aux::no_tag check(...);

		// bring the specific member check into this scope
		using base_t::check;
	};

public:
	// do the check
	static const bool value = sizeof(has_member_impl::check<type_t>(0)) == 
							  sizeof(boost::mpl::aux::yes_tag);
};


}} // namespace dumbnose::aux
