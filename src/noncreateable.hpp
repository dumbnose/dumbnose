#pragma once

namespace dumbnose
{

// Ensures the class can only be created by itself or decendants
class noncreateable
{
protected:
	noncreateable(){}
	~noncreateable(){}
};


}