// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__401C6083_B7B9_11D2_B681_D0BCED681808__INCLUDED_)
#define AFX_STDAFX_H__401C6083_B7B9_11D2_B681_D0BCED681808__INCLUDED_

#define _HAS_ITERATOR_DEBUGGING 0 // 由于是对vector进行reserve space，如果不undef这个宏的话，使用[]访问元素，会报下表越界。 所以，必须在#include <vector>之前，将这个宏关闭


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include <time.h>

// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__401C6083_B7B9_11D2_B681_D0BCED681808__INCLUDED_)
