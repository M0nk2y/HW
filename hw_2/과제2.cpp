// ic.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdint.h>
#include <conio.h>
#include "StopWatch.h"
#include "mmio.h"
#include "FileIoHelperClass.h"


int _tmain(int argc, _TCHAR* argv[])
{
	 uint32_t goalsize = 6144; // 6GB
	 
	 
    _ASSERTE(create_very_big_file(L"big.txt",goalsize));//목표크기 6GB만큼 파일 생성


    StopWatch sw; 
    sw.Start();
    _ASSERTE(file_copy_using_read_write(L"big.txt", L"big2.txt"));
    sw.Stop();   
    print("info] (Read Write) time elapsed = %f", sw.GetDurationSecond());


	LARGE_INTEGER _Filesize; 
	_Filesize.QuadPart = (LONGLONG)(1024*1024)*(LONGLONG)goalsize;//파일의 크기 6GB 설정하기위해 LARGE_INTEGER
	DWORD bufS = 4096; // read.write에서 buf가 4096이므로 4096설정
	PUCHAR buf = (PUCHAR)malloc(bufS);

	LARGE_INTEGER _Offset;
	_Offset.QuadPart = 0; //파일을 옮겨주는 계산을 하는데 사용한다.



    StopWatch sw2;
	sw2.Start();
    
	FileIoHelper FileHelp;
	FileHelp.FIOpenForRead(L"big.txt");
	FileHelp.FIOCreateFile(L"big3.txt", _Filesize);

	while(_Filesize.QuadPart > _Offset.QuadPart)         
	{
		if((_Filesize.QuadPart - _Offset.QuadPart) > (LONGLONG)bufS)
			bufS = 4096;
		
		else
			bufS = (DWORD)(_Filesize.QuadPart - _Offset.QuadPart);

		FileHelp.FIOReadFromFile(_Offset, bufS, buf);
		FileHelp.FIOWriteToFile(_Offset, bufS, buf);
		_Offset.QuadPart += (LONGLONG)bufS;
	}
    sw2.Stop();
    print("info] (Memory Map) time elapsed = %f", sw2.GetDurationSecond());


    return 0;
}

