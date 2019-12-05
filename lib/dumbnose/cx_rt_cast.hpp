#pragma once

#include <winrt\base.h>

namespace dumbnose {


template <typename T>
T^ to_cx_cast(winrt::Windows::Foundation::IUnknown const& from)
{
	return safe_cast<T^>(reinterpret_cast<Platform::Object^>(winrt::get_abi(from)));
}

template <typename T>
T from_cx_cast(Platform::Object^ from)
{
	T to{ nullptr };
	winrt::check_hresult(reinterpret_cast<::IUnknown*>(from)->QueryInterface(winrt::put_abi(to)));
	return to;
}


}