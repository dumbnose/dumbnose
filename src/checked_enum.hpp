#pragma once

namespace dumbnose {


enum checked_enum
{
};

checked_enum& assign(checked_enum& ce, const checked_enum& val)
{
	return ce;
}


} // namespace dumbnose
