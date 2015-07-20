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
	 
	 
    _ASSERTE(create_very_big_file(L"big.txt",goalsize));//��ǥũ�� 6GB��ŭ ���� ����


    StopWatch sw; 
    sw.Start();
    _ASSERTE(file_copy_using_read_write(L"big.txt", L"big2.txt"));
    sw.Stop();   
    print("info] (Read Write) time elapsed = %f", sw.GetDurationSecond());


	LARGE_INTEGER _Filesize; 
	_Filesize.QuadPart = (LONGLONG)(1024*1024)*(LONGLONG)goalsize;//������ ũ�� 6GB �����ϱ����� LARGE_INTEGER
	DWORD bufS = 4096; // read.write���� buf�� 4096�̹Ƿ� 4096����
	PUCHAR buf = (PUCHAR)malloc(bufS);

	LARGE_INTEGER _Offset;
	_Offset.QuadPart = 0; //������ �Ű��ִ� ����� �ϴµ� ����Ѵ�.



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

