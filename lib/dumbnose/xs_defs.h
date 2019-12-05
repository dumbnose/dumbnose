// Copyright (c) 2001 John Panzer
// Permission is granted to use this code without restriction as
// long as this copyright notice appears in all source files.

#ifndef XS_DEFS_H_INCLUDED
#define XS_DEFS_H_INCLUDED

#include <cassert>

#ifdef XSTRING_DEBUG
#define xs_assert(x) assert(x)
#else
#define xs_assert(x)
#endif

#ifdef NO_NAMESPACES
#define XS_NAMESPACE(NS)
#define XS_END_NAMESPACE
#else
#define XS_NAMESPACE(NS) namespace NS {
#define XS_END_NAMESPACE }
#endif


#endif