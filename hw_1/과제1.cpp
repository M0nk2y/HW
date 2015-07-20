// ConsoleApplication3.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdarg.h>
#include <Strsafe.h>
#include <stdint.h>
#include <Windows.h>
#include "util.h"


    HANDLE fp; 
    DWORD readn; 

/**
 * @brief
 * @param      
 * @see        
 * @remarks    
 * @code               
 * @endcode    
 * @return     
**/



void print(_In_ const char* fmt, _In_ ...)
{
        char log_buffer[2048];
    va_list args;
 
    va_start(args,fmt);
    HRESULT hRes = StringCbVPrintfA(log_buffer, sizeof(log_buffer), fmt, args);
    if (S_OK != hRes)
    {
        fprintf(
            stderr,
            "%s, StringCbVPrintfA() failed. res = 0x%08x",
            __FUNCTION__,
            hRes
            );
        return;
    }
 
    OutputDebugStringA(log_buffer);
    fprintf(stdout, "%s \n", log_buffer);
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
bool create_bob_txt()
{
    // current directory 를 구한다.
    wchar_t *buf=NULL;
    uint32_t buflen = 0;
    buflen = GetCurrentDirectoryW(buflen, buf);
        if (0 == buflen)
        {
        print("err, GetCurrentDirectoryW() failed. gle = 0x%08x", GetLastError());
        return false;
        }
 
        buf = (PWSTR) malloc(sizeof(WCHAR) * buflen);
        if (0 == GetCurrentDirectoryW(buflen, buf))
        {
                print("err, GetCurrentDirectoryW() failed. gle = 0x%08x", GetLastError());
                free(buf);
                return false;
        }
 
    // current dir \\ bob.txt 파일명 생성
    wchar_t file_name[260];
    if (!SUCCEEDED(StringCbPrintfW(
                            file_name,
                            sizeof(file_name),
                            L"%ws\\bob.txt",
                            buf)))
    {  
        print("err, can not create file name");
        free(buf);
        return false;
    }
    free(buf); buf = NULL;
 
    if (true == is_file_existsW(file_name))
    {
        ::DeleteFileW(file_name);
    }
 
    // 파일 생성
    HANDLE file_handle = CreateFileW(
                                file_name,
                                                        GENERIC_WRITE,
                                                        FILE_SHARE_READ,
                                                        NULL,
                                                        CREATE_NEW,
                                                        FILE_ATTRIBUTE_NORMAL,
                                                        NULL); 
        if(file_handle == INVALID_HANDLE_VALUE)
        {                                              
        print("err, CreateFile(path=%ws), gle=0x%08x", file_name, GetLastError());
        return false;
        }

    // 파일에 데이터 쓰기
    DWORD bytes_written = 0;
    wchar_t string_buf[1024];
	char string_buf_utf8[1024] ={0,};
    if (!SUCCEEDED(StringCbPrintfW(
                        string_buf,
                        sizeof(string_buf),
                        L"가갸거겨고교구규그기 ABCDEFGHIJKLMNOPQRSTUVWXYZ")))
    {
        print("err, can not create data to write.");
        CloseHandle(file_handle);
        return false;
    }
 
	//유니코드-> UTF-8

	int nLen = WideCharToMultiByte(CP_UTF8, 0, string_buf, lstrlenW(string_buf), NULL, 0, NULL, NULL);
	WideCharToMultiByte (CP_UTF8, 0, string_buf, lstrlenW(string_buf), string_buf_utf8, nLen, NULL, NULL);


    if (!WriteFile(file_handle, string_buf_utf8, strlen(string_buf_utf8), &bytes_written, NULL))
    {
        print("err, WriteFile() failed. gle = 0x%08x", GetLastError());
        CloseHandle(file_handle);
        return false;
    }

 
    // 파일 닫기
    CloseHandle(file_handle);
    return true;
 }


bool read_file_using_memory_map()
{
    // current directory 를 구한다.
        wchar_t *buf=NULL;
    uint32_t buflen = 0;
    buflen = GetCurrentDirectoryW(buflen, buf);
        if (0 == buflen)
        {
        print("err, GetCurrentDirectoryW() failed. gle = 0x%08x", GetLastError());
        return false;
        }
 
        buf = (PWSTR) malloc(sizeof(WCHAR) * buflen);
        if (0 == GetCurrentDirectoryW(buflen, buf))
        {
                print("err, GetCurrentDirectoryW() failed. gle = 0x%08x", GetLastError());
                free(buf);
                return false;
        }
 
    // current dir \\ test.txt 파일명 생성
    wchar_t file_name[260];
    if (!SUCCEEDED(StringCbPrintfW(
                            file_name,
                            sizeof(file_name),
                            L"%ws\\bob.txt",
                            buf)))
    {  
        print("err, can not create file name");
        free(buf);
        return false;
    }
    free(buf); buf = NULL;
 
    /*if (true == is_file_existsW(file_name))
    {
        ::DeleteFileW(file_name);
    }*/
 
 
 
 
    HANDLE file_handle = CreateFileW(
                            (LPCWSTR)file_name,
                            GENERIC_READ,
                            NULL,
                            NULL,
                            OPEN_EXISTING,
                            FILE_ATTRIBUTE_NORMAL,
                            NULL
                            );
    if (INVALID_HANDLE_VALUE == file_handle)
    {
        print("err, CreateFile(%ws) failed, gle = %u", file_name, GetLastError());
        return false;
    }
 
    // check file size
    //
    LARGE_INTEGER fileSize;
    if (TRUE != GetFileSizeEx(file_handle, &fileSize))
    {
        print("err, GetFileSizeEx(%ws) failed, gle = %u", file_name, GetLastError());
        CloseHandle(file_handle);
        return false;
    }
 
    // [ WARN ]
    //
    // 4Gb 이상의 파일의 경우 MapViewOfFile()에서 오류가 나거나
    // 파일 포인터 이동이 문제가 됨
    // FilIoHelperClass 모듈을 이용해야 함
    //
    _ASSERTE(fileSize.HighPart == 0);
        if (fileSize.HighPart > 0)
        {
        print("file size = %I64d (over 4GB) can not handle. use FileIoHelperClass",
            fileSize.QuadPart);
                CloseHandle(file_handle);
        return false;
        }
 
    DWORD file_size = (DWORD)fileSize.QuadPart;
    HANDLE file_map = CreateFileMapping(
                            file_handle,
                            NULL,
                            PAGE_READONLY,
                            0,
                            0,
                            NULL
                            );
    if (NULL == file_map)
    {
        print("err, CreateFileMapping(%ws) failed, gle = %u", file_name, GetLastError());
        CloseHandle(file_handle);
        return false;
    }
 
    PCHAR file_view = (PCHAR) MapViewOfFile(
                                    file_map,
                                    FILE_MAP_READ,
                                    0,
                                    0,
                                    0
                                    );
    if(file_view == NULL)
    {
        print("err, MapViewOfFile(%ws) failed, gle = %u", file_name, GetLastError());
       
        CloseHandle(file_map);
        CloseHandle(file_handle);
        return false;
    }  
	///---------------//
 
    // do some io
    int uniLen = MultiByteToWideChar(CP_UTF8, 0, file_view, -1, NULL, NULL);
	 wchar_t *unistr = (wchar_t*)malloc((uniLen+1) * sizeof(wchar_t));
	 memset ( unistr, '\x00', uniLen+2 );
	 //유니코드로 변환
	 MultiByteToWideChar(CP_UTF8, 0, file_view, -1, unistr, uniLen);

	 int  multilen = WideCharToMultiByte(CP_ACP, 0, unistr, -1, NULL, 0, NULL, NULL);
	 char *multistr = (char*)malloc((multilen+1) * sizeof(char));
	 memset ( multistr, '\x00', multilen+2 );
	 //멀티바이트로 변환
	 WideCharToMultiByte(CP_ACP, 0, unistr, -1, multistr, multilen, NULL, NULL);
	 //multistr[multilen] = '\x00\x00';
	 printf("%s\n", multistr);
	 
    
 
    // close all
    UnmapViewOfFile(file_view);
    CloseHandle(file_map);
    CloseHandle(file_handle);
    return true;
}


bool copy_bob_txt()
{
    // current directory 를 구한다.
    wchar_t *buf=NULL;
    uint32_t buflen = 0;
    buflen = GetCurrentDirectoryW(buflen, buf);
        if (0 == buflen)
        {
        print("err, GetCurrentDirectoryW() failed. gle = 0x%08x", GetLastError());
        return false;
        }
 
        buf = (PWSTR) malloc(sizeof(WCHAR) * buflen);
        if (0 == GetCurrentDirectoryW(buflen, buf))
        {
                print("err, GetCurrentDirectoryW() failed. gle = 0x%08x", GetLastError());
                free(buf);
                return false;
        }
 
    // current dir \\ bob.txt 파일명 생성
    wchar_t file_name[260];
    if (!SUCCEEDED(StringCbPrintfW(
                            file_name,
                            sizeof(file_name),
                            L"%ws\\bob.txt",
                            buf)))
    {  
        print("err, can not create file name");
        free(buf);
        return false;
    }

	wchar_t newfile_name[260];
    if (!SUCCEEDED(StringCbPrintfW(
                            newfile_name,
                            sizeof(newfile_name),
                            L"%ws\\bob2.txt",
                            buf)))
    {  
        print("err, can not create file name");
        free(buf);
        return false;
    }

	CopyFile(file_name,newfile_name,false); 


    free(buf); buf = NULL;

 }


	bool readapi_bob2()
{
    // current directory 를 구한다.

	
    wchar_t *buf=NULL;
    uint32_t buflen = 0;
    buflen = GetCurrentDirectoryW(buflen, buf);
        if (0 == buflen)
        {
        print("err, GetCurrentDirectoryW() failed. gle = 0x%08x", GetLastError());
        return false;
        }
 
        buf = (PWSTR) malloc(sizeof(WCHAR) * buflen);
        if (0 == GetCurrentDirectoryW(buflen, buf))
        {
                print("err, GetCurrentDirectoryW() failed. gle = 0x%08x", GetLastError());
                free(buf);
                return false;
        }
 
    // current dir \\ bob.txt 파일명 생성
    wchar_t bob2_name[260];
    if (!SUCCEEDED(StringCbPrintfW(
                            bob2_name,
                            sizeof(bob2_name),
                            L"%ws\\bob2.txt",
                            buf)))
    {  
        print("err, can not create file name");
        free(buf);
        return false;
    }
    

	HANDLE fileHandle = CreateFile(  
		bob2_name,   
        GENERIC_READ,  
        FILE_SHARE_READ,  
        NULL,  
        OPEN_EXISTING,  
        FILE_ATTRIBUTE_NORMAL,  
        NULL); 


	 char *strtemp;
	 strtemp = (char*)malloc(1000);
	 DWORD endlen;
	 //파일을 오픈
	 ReadFile(fileHandle, strtemp, 1000, &endlen, NULL);
	 strtemp[endlen] = '\x00\x00';

	 int uniLen = MultiByteToWideChar(CP_UTF8, 0, strtemp, strlen(strtemp), NULL, NULL);
	 wchar_t *unistr = (wchar_t*)malloc((uniLen+1) * sizeof(wchar_t));
	 memset ( unistr, '\x00', uniLen+2 );
	 //유니코드로 변환
	 MultiByteToWideChar(CP_UTF8, 0, strtemp, strlen(strtemp), unistr, uniLen);
	 int  multilen = WideCharToMultiByte(CP_ACP, 0, unistr, -1, NULL, 0, NULL, NULL);
	 char *multistr = (char*)malloc((multilen+1) * sizeof(char));
	 memset ( multistr, '\x00', multilen+2 );
	 //멀티바이트로 변환
	 WideCharToMultiByte(CP_ACP, 0, unistr, uniLen, multistr, multilen+1, NULL, NULL);

	 multistr[multilen] = '\x00\x00';
	 printf("%s\n", multistr);

    free(buf); buf = NULL;


	CloseHandle(fileHandle);
 }




bool delete_bob_txt(){
   // current directory 를 구한다.
   wchar_t *buf = NULL;
   uint32_t buflen = 0;
   buflen = GetCurrentDirectoryW(buflen, buf);
   if (0 == buflen)
   {
      print("err, GetCurrentDirectoryW() failed. gle = 0x%08x", GetLastError());
      return false;
   }

   buf = (PWSTR)malloc(sizeof(WCHAR) * buflen);
   if (0 == GetCurrentDirectoryW(buflen, buf))
   {
      print("err, GetCurrentDirectoryW() failed. gle = 0x%08x", GetLastError());
      free(buf);
      return false;
   }

   // current dir \\ bob.txt 파일명 생성
   wchar_t file_name[260];
   if (!SUCCEEDED(StringCbPrintfW(
      file_name,
      sizeof(file_name),
      L"%ws\\bob.txt",
      buf)))
   {
      print("err, can not create file name");
      free(buf);
      return false;
   }
   wchar_t file_name2[260];
   if (!SUCCEEDED(StringCbPrintfW(
      file_name2,
      sizeof(file_name2),
      L"%ws\\bob2.txt",
      buf)))
   {
      print("err, can not create file name");
      free(buf);
      return false;
   }
   free(buf);
   buf = NULL;
   //삭제
   ::DeleteFileW(file_name);
   ::DeleteFileW(file_name2);
}

	



int _tmain(int argc, _TCHAR* argv[])
{
	create_bob_txt();
	copy_bob_txt();
	printf("ReadFile() api\n");
	readapi_bob2();
	printf("Memory Mapped I/O\n");
	read_file_using_memory_map();
	delete_bob_txt();

	return 0;
}
