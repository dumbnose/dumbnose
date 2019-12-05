/*  -------------------------------------------------------  *\
    Softricity, Inc.
    332 Congress Street
    Boston, MA 02210

    File: SEHToCPPExceptionTranslator.h

    Author: Kris Reierson

    Description:  

	Lifted from Jeffrey Richter's 'Applications for Windows'.
	This simple class allows you to catch SEH and C++ at the 
	same scope.  To use it :
	*	Call SEHToCPPExceptionTranslator::SetTranslator
	on the thread before any guarded code.  
	*   Then catch(SEHToCPPExceptionTranslator)

\*  -------------------------------------------------------  */

#pragma once

#include <eh.h>

namespace dumbnose
{


//
// The translator must be installed on every thread.
// They are not chained and there is only one per thread.
//  
 
class SEHToCPPExceptionTranslator
{
public:
	static void SetTranslator(){_set_se_translator(TranslateSEHTOCPPE);}

	EXCEPTION_RECORD m_ExceptionRecord;
	CONTEXT m_Context;
	UINT m_Cread;

	operator DWORD() { return (m_ExceptionRecord.ExceptionCode); }

protected:
	SEHToCPPExceptionTranslator(UINT uiEC,PEXCEPTION_POINTERS peps)
	{
		m_ExceptionRecord = *peps->ExceptionRecord;
		m_Context = *peps->ContextRecord;
		m_Cread = uiEC;
	}

	static void _cdecl TranslateSEHTOCPPE(UINT uiEC,PEXCEPTION_POINTERS peps)
	{
		throw SEHToCPPExceptionTranslator(uiEC,peps);
	}

};


} // namespace dumbnose