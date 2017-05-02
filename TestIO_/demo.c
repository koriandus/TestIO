/* 
Public domain by Lasse Reinhold, 2010
No copyright pending

Reads a large from disk to illustrate buffered and unbuffered disk I/O.
*/

#include <stdlib.h>
#include "windows.h"
#include "unbuf.h"
#include "time.h"
#include "tchar.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "MemoryMapWriter.h"
#include "MemoryMapReader.h"
#include <string>
#include <sstream>
#include "utilWinGinsuLibrary.h"
//#define NORMAL // Comment away this to make it use unbuffered disk I/O
using namespace std; 

const char* large_file_Z_prefix = ("Z:\\Test\\z");
const char* large_file_prefix = ("C:\\Test\\l");

const size_t WR_SIZE = 1024*1024*10;//每次读写的大小,此处为10M
#ifdef NORMAL
	#define CreateFile_NB CreateFile
	#define ReadFile_NB ReadFile
	#define WriteFile_NB WriteFile
	#define CloseHandle_NB CloseHandle
#endif

TCHAR* string2TCHAR(string res)
{
	TCHAR* wc = new TCHAR[1024]; 
#ifdef UNICODE  
	_stprintf_s(wc, 1024, _T("%S"), res.c_str());//%S宽字符  
#else  
	_stprintf_s(wc, 1024, _T("%s"), res.c_str());//%s单字符  
#endif
	return wc;
}
/*
TCHAR* char2TCHAR(char* chars)
{	
	int iLength ;  

	iLength = MultiByteToWideChar (CP_ACP, 0, chars, strlen (chars) + 1, NULL, 0) ;  
		TCHAR tchar[iLength];
	MultiByteToWideChar (CP_ACP, 0, chars, strlen (chars) + 1, tchar, iLength) ;  
}*/
char* TCHAR2char(TCHAR* tchStr) 
{ 
	int iLen = 2*wcslen(tchStr);	//CString,TCHAR汉字算一个字符，因此不用普通计算长度 
	char* chRtn = new char[iLen+1];
	wcstombs(chRtn,tchStr,iLen+1);	//转换成功返回为非负值 
	return chRtn; 
}

TCHAR* char2TCHAR(char *src)
{	
	if(src == NULL)return NULL;
	int size = 0;
	size = strlen(src);
	TCHAR* unicode_string = new TCHAR[size+1]; 
	mbstowcs(unicode_string, src, size+1);
	return unicode_string;
}
int MemoryMapWrite(char* fr)
{
	DWORD start_time = 0;
	DWORD end_time = 0;
	CMemoryMapWriter writer; 

	if (false == writer.Open(string2TCHAR(fr), true))
	{
		printf("open fail \n");
		return 1;
	}
	long file_len = 1024*1024*1024;
	char* writeData = (char *)malloc(file_len);
	memset(writeData,0,file_len);
	start_time = GetTickCount();  
	writer.WriteData(writeData, file_len);	
	writer.Close();
	end_time = GetTickCount();
	cout << "The run time of write use MemoryMap is:" << (end_time - start_time) << "ms!" << endl; 
	delete []writeData;
	return 0;
}
int MemoryMapRead(char* fr,bool isDeletedCache)
{
	DWORD start_time = 0;
	DWORD end_time = 0;
	long file_len = 1024*1024*1024;

	CMemoryMapReader reader;
	if (false == reader.Open(string2TCHAR(fr)))
	{
		printf("open fail \n");
		return 1;
	}
	char* readData = (char *)malloc(file_len);
	start_time = GetTickCount();  
	reader.ReadData(readData,file_len);
	reader.Close();
	end_time = GetTickCount();
	cout << "The run time of read use MemoryMap is:" << (end_time - start_time) << "ms!" << endl; 
	delete []readData;

	//及时删除，不然缓存住的内存，会影响其他进程对内存的申请，使变慢
	if(isDeletedCache)
	{
		start_time = GetTickCount();
		if (remove(fr) == 0) 
			printf("Removed %s.\n",fr); 
		else 
			perror("remove"); 
		end_time = GetTickCount();
		cout<<"The run time of delete file is: "<< (end_time - start_time) <<"ms!"<<endl;
		cout<<endl;
	}
	return 0;
}

void DirectIORead(char* fr,bool isDeletedCache)
{
	DWORD read = 0;
	DWORD written = 0;
	DWORD o = 0;
	DWORD file_len = 0;
	DWORD start_time = 0;
	DWORD end_time =0;
	file_len = 1024*1024*1024;

	HANDLE ifileHandle = CreateFile_NB(string2TCHAR(fr), GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	char *buf = (char *)malloc(file_len);
	start_time = GetTickCount();  
	do
	{
		ReadFile_NB(ifileHandle, buf, WR_SIZE, &read, 0);
	} while (read > 0);
	CloseHandle_NB(ifileHandle);
	end_time = GetTickCount();
	cout << "The run time of read use unbuf is:" << (end_time - start_time) << "ms!" << endl; 
	delete []buf;

	if(isDeletedCache)
	{
		start_time = GetTickCount();	
		if (remove(fr) == 0) 
			printf("Removed %s.\n",fr); 
		else 
			perror("remove"); 
		end_time = GetTickCount();
		cout<<"The run time of delete file is: "<< (end_time - start_time) <<"ms!"<<endl;
		cout<<endl;
	}
}
void DirectIOWrite(char* fr)
{
	DWORD read = 0;
	DWORD written = 0;
	DWORD o = 0;
	DWORD file_len = 0;
	DWORD start_time = 0;
	DWORD end_time =0;
	file_len = 1024*1024*1024;

	HANDLE ofileHandle = CreateFile_NB(string2TCHAR(fr), GENERIC_WRITE, FILE_SHARE_WRITE , 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	char* writeBuff = (char *)malloc(file_len);
	memset(writeBuff,0,file_len);
	start_time = GetTickCount();  
	while(o < file_len)
	{
		WriteFile_NB(ofileHandle, writeBuff, WR_SIZE, &written, 0);
		o += written;
	}
	CloseHandle_NB(ofileHandle);
	end_time = GetTickCount();
	cout << "The run time of write use unbuf is:" << (end_time - start_time) << "ms!" << endl; 
	delete []writeBuff;
}

void bufferRead(char* fr,bool isDeletedCache)
{
	FILE* stream_fd;
	size_t num_read;
	int rw_cnt = 0;
	long long offset = 0;
	DWORD dStart = 0;
	DWORD dEnd = 0;
	DWORD file_len = 0;
	file_len = 1024*1024*1024;

	////////////////////////////////////////////////////////
	dStart = GetTickCount();
	char *buf = new char[WR_SIZE];
	if(0 == fopen_s(&stream_fd,fr,"rb")){
		while(!feof(stream_fd)){
			rw_cnt++;
			num_read = fread(buf,sizeof(char),WR_SIZE,stream_fd);
			if(0==num_read) break;
			offset += num_read;//文件偏移
			_fseeki64(stream_fd,offset,SEEK_SET);			
		}
	}
	dEnd = GetTickCount();
	cout<<"The run time of read use buffer is: "<< (dEnd - dStart) <<"ms!"<<endl;
	delete []buf;
	fclose(stream_fd); 

	//delete file ,cache release, Importance to delete
	//及时删除，不然缓存住的内存，会影响其他进程对内存的申请，使变慢
	if(isDeletedCache)
	{
		dStart = GetTickCount();
		if (remove(fr) == 0) 
			printf("Removed %s.\n",fr); 
		else 
			perror("remove"); 
		dEnd = GetTickCount();
		cout<<"The run time of delete file is: "<< (dEnd - dStart) <<"ms!"<<endl;
		cout<<endl;
	}
}
void bufferWrite(char* fr)
{
	FILE* stream_fd;
	size_t num_written;
	int rw_cnt = 0;
	long long offset = 0;
	DWORD dStart = 0;
	DWORD dEnd = 0;
	DWORD file_len = 0;
	file_len = 1024*1024*1024;
	//if(0 != fopen_s(&stream_wr,fw,"wb"))
	if((stream_fd=fopen(fr, "wb"))==NULL)
	{
		return;
	}
	dStart = GetTickCount();
	char *writeBuff = new char[file_len];
	memset(writeBuff,0,file_len);
	offset = 0;
	dStart = GetTickCount();
	while(offset < file_len){
		num_written = fwrite(writeBuff,sizeof(char),WR_SIZE,stream_fd);
		offset += num_written;
		_fseeki64(stream_fd,offset,SEEK_SET);
	}
	dEnd = GetTickCount();
	cout<<"The run time of write use buffer is: "<< (dEnd - dStart) <<"ms!"<<endl;
	delete []writeBuff;
	fclose(stream_fd); 
}
void TestDirectIONRamdisk(int iterator_,bool isDeleted)
{
	cout<<"####using DirectIO and Ramdisk:#####"<<endl;
	for(int i=0;i<iterator_;i++)
	{
		char tmp[1024];
		sprintf(tmp,"%s%d%s",large_file_Z_prefix,i,".bin");	
		DirectIOWrite(tmp);
	}
	for(int i=0;i<iterator_;i++)
	{
		char tmp[1024];
		sprintf(tmp,"%s%d%s",large_file_Z_prefix,i,".bin");	
		DirectIORead(tmp,isDeleted);
	}
}
void TestDirectIO(int iterator_,bool isDeleted)
{

	cout<<"#########using DirectIO:############"<<endl;
	for(int i=0;i<iterator_;i++)
	{
		char tmp[1024];
		sprintf_s(tmp,"%s%d%s",large_file_prefix,i,".bin");		
		DirectIOWrite(tmp);
	}
	for(int i=0;i<iterator_;i++)
	{
		char tmp[1024];
		sprintf(tmp,"%s%d%s",large_file_prefix,i,".bin");	
		DirectIORead(tmp,isDeleted);
	}
}
void TestBufferIONRamdisk(int iterator_,bool isDeleted)
{	
	cout<<"####using Buffer IO and Ramdisk:###"<<endl;
	for(int i=0;i<iterator_;i++)
	{
		char tmp[1024];
		sprintf(tmp,"%s%d%s",large_file_Z_prefix,i,".bin");	
		bufferWrite(tmp);
	}
	for(int i=0;i<iterator_;i++)
	{
		char tmp[1024];
		sprintf(tmp,"%s%d%s",large_file_Z_prefix,i,".bin");	
		bufferRead(tmp,isDeleted);
	}
}
void TestBufferIO(int iterator_,bool isDeleted)
{
	///////////////////////////////////////////
	cout<<"#########using BufferIO:############"<<endl;
	for(int i=0;i<iterator_;i++)
	{
		char tmp[1024];
		sprintf(tmp,"%s%d%s",large_file_prefix,i,".bin");	
		bufferWrite(tmp);
	}
	for(int i=0;i<iterator_;i++)
	{
		char tmp[1024];
		sprintf(tmp,"%s%d%s",large_file_prefix,i,".bin");	
		bufferRead(tmp,isDeleted);
	}
}
void TestMemMapIONRamdisk(int iterator_,bool isDeleted)
{
	cout<<"####using Buffer IO and Ramdisk:###"<<endl;
	for(int i=0;i<2;i++)
	{
		char tmp[1024];
		sprintf(tmp,"%s%d%s",large_file_Z_prefix,i,".bin");	
		MemoryMapWrite(tmp);
	}
	for(int i=0;i<2;i++)
	{
		char tmp[1024];
		sprintf(tmp,"%s%d%s",large_file_Z_prefix,i,".bin");	
		MemoryMapRead(tmp,isDeleted);
	}
}
void TestMemMapIO(int iterator_,bool isDeleted)
{
	///////////////////////////////////////////
	cout<<"#########using BufferIO:############"<<endl;
	for(int i=0;i<iterator_;i++)
	{
		char tmp[1024];
		sprintf(tmp,"%s%d%s",large_file_prefix,i,".bin");	
		MemoryMapWrite(tmp);
	}
	for(int i=0;i<iterator_;i++)
	{
		char tmp[1024];
		sprintf(tmp,"%s%d%s",large_file_prefix,i,".bin");	
		MemoryMapRead(tmp,isDeleted);
	}
}

void TestFileMappingWrite(char* fr)
{
	DWORD dStart = 0;
	DWORD dEnd = 0;
	DWORD file_len = 1024*1024*1024;
	void *myOutBuffer = NULL;
	unsigned char *myDataOutBuffer = NULL;

	dStart = GetTickCount();

	unsigned char *writeBuff = new unsigned char[file_len];
	unsigned char *pStart = writeBuff;
	memset(writeBuff,0,file_len);

	myOutBuffer = fgsMakeGinsu(fr,file_len);
	myDataOutBuffer = (unsigned char *)myOutBuffer;

	if(myOutBuffer == NULL) 
	{
		printf("Can't create Ginsu.");
		return;
	}
	////////////////////////////////
	long myBytesRemaining = file_len;

	int myGinsuPosition = 0;
	int myChunkSize = 1024 * 1024;

	// Write 1MB of data at a time into the Ginsu
	while (myBytesRemaining > 0)
	{
		if (myChunkSize > myBytesRemaining)
		{
			myChunkSize = myBytesRemaining;
		}

		memcpy(myDataOutBuffer,writeBuff,myChunkSize);
		writeBuff += myChunkSize;
		myDataOutBuffer +=  myChunkSize;

		myBytesRemaining = myBytesRemaining - myChunkSize;
	}
	writeBuff = writeBuff - file_len;
	delete []pStart;
	dEnd = GetTickCount();
	cout<<"The run time of write use buffer is: "<< (dEnd - dStart) <<"ms!" <<endl;
	//fgsFreeGinsuByName(fr);
}
void TestFileMappingRead(char* fr)
{
	DWORD dStart = 0;
	DWORD dEnd = 0;

	// Ginsu buffer
	void *myInBuffer = NULL;
	unsigned char *myDataIntBuffer = NULL;
	char myDump[1024];

	HANDLE hFile = CreateFile(char2TCHAR(fr), GENERIC_WRITE | GENERIC_READ, 0, NULL,
		CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	DWORD dwFileSize = GetFileSize(hFile, NULL);

	if(hFile == NULL)
	{
		printf("hFile is NULL.\n");
	}
	dStart = GetTickCount();
	myInBuffer = fgsMapGinsu(fr,dwFileSize);

	if(myInBuffer == NULL)
	{
		printf("Can't Read Ginsu in myInBuffer.\n");
		return;
	}
	myDataIntBuffer = (unsigned char *) myInBuffer;	
	// If no Ginsu is made, go to the top of the loop to create another Ginsu
	if(myDataIntBuffer == NULL) 
	{
		printf("Can't Read Ginsu in myDataIntBuffer.\n");
		return;
	}
	// Dump the list of Ginsus
	//fgsDumpLocalGinsuItems(myDump);
	//printf("Ginsu List: %s\n",myDump);
	dEnd = GetTickCount();
	cout<<"The run time of read use FileMapping is: "<< (dEnd - dStart) <<"ms!"<<endl;
	fgsFreeGinsuByName(fr);
}

void TestMemMappingInDisk(int iterator_)
{
	for(int i=0;i<iterator_;i++)
	{
		char tmp[1024];
		sprintf(tmp,"%s%d%s",large_file_prefix,i,".bin");	
		TestFileMappingWrite(tmp);
	}
	for(int i=0;i<iterator_;i++)
	{
		char tmp[1024];
		sprintf(tmp,"%s%d%s",large_file_prefix,i,".bin");	
		TestFileMappingRead(tmp);
	}
}
void TestIOWithDeleteCache()
{
	//TestDirectIO(3,true);
	//TestBufferIO(3,true);
	TestMemMapIO(3,true);
}

void TestIOWithoutDeleteCache()
{
	//TestDirectIO(2,false);
	//TestBufferIO(2,false);
	//TestMemMapIO(2,false);
}
int _tmain(int argc, _TCHAR **argv)
{
	TestIOWithDeleteCache();
	//TestMemMappingInDisk(3);
	//TestIOWithoutDeleteCache();
	getchar();
	return 0;
}

