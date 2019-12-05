#pragma once

#include "has_member.hpp"
#include <boost/mpl/identity.hpp>

namespace dumbnose { namespace aux { 


//
//	determines if the type has a member function named swap with the 
//	supplied signature
//
template<typename type_t, typename signature_t, template<signature_t> class wrap_t>
struct swap_member
{
	template<typename dummy_t>
	static boost::mpl::aux::yes_tag check(wrap_t<&dummy_t::swap>*);
};


// nice-to-have helper template
template<typename type_t, typename signature_t>
class has_swap : public has_member<type_t,signature_t,swap_member>{};


}} // namespace dumbnose::aux