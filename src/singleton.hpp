#pragma once

#include "windows_exception.hpp"
#include <boost/noncopyable.hpp>


namespace dumbnose
{


template<class T>
class singleton : boost::noncopyable
{
public:
	static T& instance(){return *get_instance();}

protected:
	singleton(){}

	static BOOL WINAPI init(PINIT_ONCE intOncePtr, PVOID Parameter, PVOID* context)
	{
		T* inst = new T;
		*context = inst;

		return TRUE;
	}

	static T* get_instance()
	{
		static INIT_ONCE init_once = {nullptr};
		static T* context = nullptr;
		if(context != NULL) return context;

		BOOL status = InitOnceExecuteOnce(&init_once, init, NULL, reinterpret_cast<void**>(&context));
		if(!status) throw windows_exception("Could not create singleton");

		return context;
	}
};


} // namespace dumbnose
