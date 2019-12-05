#pragma once

template<typename func_sig>
class dynamic_call : public func_sig
{
public:
	dynamic_call(char* dll_name, char* func_name)
	{
		HANDLE lib = LoadLibrary(dll_name);
		func_sig = GetProcAddress(lib,func_name);
	}

	func_sig* call(){return func_;}

private:
	func_sig* func_;
};
