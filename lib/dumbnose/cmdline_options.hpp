#pragma once

#include <windows.h>
#include <map>
#include <list>
#include <vector>
#include <string>
#include <boost/format.hpp>
#include <boost/lexical_cast/lexical_cast_old.hpp>
#include <stdlib.h>
#include <dumbnose/string_utils.hpp>
#include <dumbnose\windows_exception.hpp>

namespace dumbnose {

#ifdef UNICODE

typedef wchar_t char_t;
typedef std::wstring string_t;
typedef std::string alt_string_t;
#define argv__ __wargv

#else

typedef char char_t;
typedef std::string string_t;
typedef std::wstring alt_string_t;
#define argv__ __argv

#endif

class cmdline_options
{
public:

	typedef std::map<string_t,string_t,string_utils::case_insensitive> option_map;
	typedef std::vector<string_t> arg_list;

	static cmdline_options& instance(){
		// Singleton instance
		static cmdline_options instance;

		return instance;
	};

    cmdline_options(int argc, char_t** argv) { initialize(argc, argv); };
    ~cmdline_options() {};

    bool exists(string_t name){return options_.count(name)>0;}
	string_t option(string_t name){return options_[name];}
	const arg_list& args(){return args_;}
    const option_map& options() { return options_; }
    static bool is_option(const char_t* param)
	{
		return (param[0]=='/') || (param[0]=='-');
	}

private:
	cmdline_options(){initialize(__argc, argv__);};

	void initialize(int argc, char_t** argv)
	{
		for(int lcv=1 ; lcv<argc ; lcv++)
		{
			const char_t* item = argv[lcv];

			if(is_option(item))
			{
				string_t option(&item[1]);
				size_t equal_loc = option.find_first_of(L':');

				string_t name = option.substr(0,equal_loc);
				string_t value;

				if(equal_loc!=option.npos)
				{
					value=option.substr(equal_loc+1);
				}

				options_[name] = value;
			}
			else
			{
				args_.push_back(item);
			}
		}
	}

	// Data
	option_map options_;
	arg_list args_;
};


class cmdline_option
{
public:
	cmdline_option(string_t name):name_(name){};

	template<typename result_t>
	result_t as(){
		return boost::lexical_cast<result_t>(cmdline_options::instance().option(name_));
	};

    template<>
    std::string as<std::string>()
    {
        auto woption = (boost::wformat(L"%S") % cmdline_options::instance().option(name_)).str();
        std::vector<char> buffer(woption.length() + 1);

        int num_chars = WideCharToMultiByte(CP_ACP, 0, woption.c_str(), static_cast<int>(woption.size()), buffer.data(), static_cast<int>(buffer.size()), nullptr, nullptr);
        if (num_chars == 0) throw dumbnose::windows_exception("WideCharToMultiByte failed");

        return std::string(buffer.data(), woption.length());
    };

    template<typename result_t>
	operator result_t(){
		return this->as<result_t>();
	};

	operator bool(){return cmdline_options::instance().exists(name_);};

	operator alt_string_t(){
		return string_utils::convert(cmdline_options::instance().option(name_));
	};

private:
	string_t name_;
};


} // namespace dumbnose
