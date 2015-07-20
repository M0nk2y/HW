#include "stdafx.h"
#include "util.h"
#include <memory>


/**
 * @brief	
 * @param	
 * @see		
 * @remarks	
 * @code		
 * @endcode	
 * @return	
**/
void write_to_console(_In_ DWORD log_level, _In_ wchar_t* function, _In_ wchar_t* format, ...)
{
	static HANDLE	_stdout_handle = INVALID_HANDLE_VALUE;
	static WORD		_old_color = 0x0000;
	
	//> initialization for console text color manipulation.
	if (INVALID_HANDLE_VALUE == _stdout_handle)
	{
		CONSOLE_SCREEN_BUFFER_INFO csbi;

		_stdout_handle = GetStdHandle(STD_OUTPUT_HANDLE);
		GetConsoleScreenBufferInfo(_stdout_handle, &csbi);
		_old_color = csbi.wAttributes;
	}

	std::wstringstream strm_prefix;
	switch (log_level)
	{
	case LL_DEBG: 
		{
			strm_prefix << L"[DEBG] " << function << L", "; 
			break;
		}
	case LL_INFO: 
		{
			strm_prefix << L"[INFO] " << function << L", "; 
			SetConsoleTextAttribute(_stdout_handle, FOREGROUND_GREEN | FOREGROUND_INTENSITY);
			break;
		}
	case LL_ERRR: 
		{
			strm_prefix << L"[ERR ] " << function << L", "; 
			SetConsoleTextAttribute(_stdout_handle, FOREGROUND_RED | FOREGROUND_INTENSITY);
			break;
		}
	case LL_NONE:
	default:
		{
			//strm_prefix << function << L", ";
			break;
		}
	}

	DWORD len;
	va_list args;
	WCHAR msg[4096];	
	
	va_start(args, format);
	if(TRUE != SUCCEEDED(StringCchVPrintf(msg, sizeof(msg), format, args))){ return; }
	va_end(args);

	WriteConsole(_stdout_handle, strm_prefix.str().c_str(), (DWORD)strm_prefix.str().size(), &len, NULL);
	WriteConsole(_stdout_handle, msg, (DWORD)wcslen(msg), &len, NULL);	
	WriteConsole(_stdout_handle, L"\n", (DWORD)wcslen(L"\n"), &len, NULL);
	
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), _old_color);

#ifdef _USE_AHNTRACE_
	switch (log_level)
	{
	case LL_DEBG: DEBUGLOG(msg); break;
	case LL_INFO: INFOLOG(msg); break;	
	case LL_ERRR: ERRORLOG(msg); break;
	default:
		break;		
	}
#endif//_USE_AHNTRACE_
}

/**
 * @brief
 * @param
 * @see
 * @remarks
 * @code
 * @endcode
 * @return
**/
bool is_file_existsW(_In_ const wchar_t* file_path)
{
	_ASSERTE(NULL != file_path);
	_ASSERTE(TRUE != IsBadStringPtrW(file_path, MAX_PATH));
	if ((NULL == file_path) || (TRUE == IsBadStringPtrW(file_path, MAX_PATH))) return false;

	WIN32_FILE_ATTRIBUTE_DATA info = {0};

	if (GetFileAttributesExW(file_path, GetFileExInfoStandard, &info)==0)
		return false;
	else
		return true;
}

/**
 * @brief
 * @param
 * @see
 * @remarks
 * @code
 * @endcode
 * @return
**/
bool is_file_existsA(_In_ const char* file_path)
{
    WCHAR* wcs=MbsToWcs(file_path);
    if(NULL==wcs) { return false;}

	bool ret = is_file_existsW(wcs); free(wcs);
	return ret;
}


/**
 * @brief	ASCII(Multibyte) --> WIDE CHAR 로 변환, caller 는 리턴되는 포인터를 소멸시켜주어야 함
 * @param
 * @see
 * @remarks
 * @code
 * @endcode
 * @return
**/
wchar_t* MbsToWcs(_In_ const char* mbs)
{
    _ASSERTE(NULL!=mbs);
    if(NULL==mbs) return NULL;

    int outLen=MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, mbs, -1, NULL, 0);
    if(0==outLen) return NULL;

    wchar_t* outWchar=(wchar_t*) malloc(outLen * (sizeof(wchar_t)));  // outLen contains NULL char
    if(NULL==outWchar) return NULL;
    RtlZeroMemory(outWchar, outLen);

    if(0==MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, mbs, -1, outWchar, outLen))
    {
        log_err L"MultiByteToWideChar() failed, errcode=0x%08x", GetLastError() log_end

        free(outWchar);
        return NULL;
    }

    return outWchar;
}

/**
 * @brief	WIDE CHAR --> ASCII(Multibyte) 로 변환, caller 는 리턴되는 포인터를 소멸시켜주어야 함
 * @param
 * @see
 * @remarks
 * @code
 * @endcode
 * @return
**/
char* WcsToMbs(_In_ const wchar_t* wcs)
{
    _ASSERTE(NULL!=wcs);
    if(NULL==wcs) return NULL;

    int outLen=WideCharToMultiByte(CP_ACP, 0, wcs, -1, NULL, 0, NULL, NULL);
    if(0==outLen) return NULL;

    char* outChar=(char*) malloc(outLen * sizeof(char));
    if(NULL==outChar) return NULL;
    RtlZeroMemory(outChar, outLen);

    if(0==WideCharToMultiByte(CP_ACP, 0, wcs, -1, outChar, outLen, NULL, NULL))
    {
        log_err L"WideCharToMultiByte() failed, errcode=0x%08x", GetLastError() log_end
        free(outChar);
        return NULL;
    }

    return outChar;
}

/**
 * @brief	wide char -> utf8 변환, caller 는 리턴되는 포인터를 소멸시켜주어야 함
 * @param
 * @see
 * @remarks
 * @code
 * @endcode
 * @return
**/
char* WcsToMbsUTF8(_In_ const wchar_t* wcs)
{
    _ASSERTE(NULL!=wcs);
    if(NULL==wcs) return NULL;

    int outLen=WideCharToMultiByte(CP_UTF8, 0, wcs, -1, NULL, 0, NULL, NULL);
    if(0==outLen) return NULL;

    char* outChar=(char*) malloc(outLen * sizeof(char));
    if(NULL==outChar) return NULL;
    RtlZeroMemory(outChar, outLen);

    if(0==WideCharToMultiByte(CP_UTF8, 0, wcs, -1, outChar, outLen, NULL, NULL))
    {
        log_err L"WideCharToMultiByte() failed, errcode=0x%08x", GetLastError() log_end

        free(outChar);
        return NULL;
    }

    return outChar;
}

/**
* @brief
* @param
* @see
* @remarks
* @code
* @endcode
* @return
*/
std::wstring MbsToWcsEx(_In_ const char *mbs)
{
    std::auto_ptr<wchar_t> tmp( MbsToWcs(mbs) );
    if (NULL == tmp.get())
    {
        return _null_stringw;
    }
    else
    {
        return std::wstring(tmp.get());
    }

}

/**
* @brief
* @param
* @see
* @remarks
* @code
* @endcode
* @return
*/
std::string WcsToMbsEx(_In_ const wchar_t *wcs)
{
    std::auto_ptr<char> tmp( WcsToMbs(wcs) );
    if (NULL == tmp.get())
    {
        return _null_stringa;
    }
    else
    {
        return std::string(tmp.get());
    }
}

/**
* @brief
* @param
* @see
* @remarks
* @code
* @endcode
* @return
*/
std::string WcsToMbsUTF8Ex(_In_ const wchar_t *wcs)
{
    std::auto_ptr<char> tmp( WcsToMbsUTF8(wcs) );
    if (NULL == tmp.get())
    {
        return _null_stringa;
    }
    else
    {
        return std::string(tmp.get());
    }
}