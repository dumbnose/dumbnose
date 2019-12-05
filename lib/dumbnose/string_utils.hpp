#pragma once


#include <boost/shared_array.hpp>


namespace dumbnose {


struct string_utils
{
// disable warning "conversion from 'size_t' to 'int', possible loss of data"
#pragma warning(push)
#pragma warning(disable:4267)

	static std::string convert(const std::wstring& from)
	{
		size_t buflen = from.length()+1;
		boost::shared_array<char> buffer(new char[buflen]);
		int bytes_written = WideCharToMultiByte(CP_ACP,0,from.c_str(),-1,&buffer[0],buflen,NULL,NULL);
		if(bytes_written==0) {
			throw GetLastError();
		}

		return &buffer[0];
	}

	static std::wstring convert(const std::string& from)
	{
		size_t buflen = from.length()+1;
		boost::shared_array<wchar_t> buffer(new wchar_t[buflen]);
		int bytes_written = MultiByteToWideChar(CP_ACP,0,from.c_str(),-1,&buffer[0],buflen);
		if(bytes_written==0) {
			throw GetLastError();
		}

		return &buffer[0];
	}
#pragma warning(pop)

	template<typename char_t>
	static char_t toupper(char_t ch)
	{

	};

	template<>
	static wchar_t toupper<wchar_t>(wchar_t ch)
	{
		return towupper(ch);
	};

	template<>
	static char toupper<char>(char ch)
	{
		return toupper(ch);
	};

	// Functor class for enabling case insensitivity in an STL map
	class case_insensitive
	{
	public:
		template<class string_t>
		bool operator()(const string_t& lhs, const string_t& rhs) const
		{
			return _stricmp(lhs.c_str(),rhs.c_str())<0;
		}

		template<>
		bool operator()<std::wstring>(const std::wstring& lhs, const std::wstring& rhs) const
		{
			return _wcsicmp(lhs.c_str(),rhs.c_str())<0;
		}
	};





};


} // namespace dumbnose 
