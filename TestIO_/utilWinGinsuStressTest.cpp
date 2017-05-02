/*******************************************************************************
* Filename: utilWinGinsuStressTest.cpp
* Original Author: {authorName}
* File Creation Date: 
* Subsystem: utilWinGinsuStressTest
*
* Description: {fileDescription} 
*
* Copyright Information (c) FMI Medical Systems, Inc. {year}
* Source Control Information: $Id$ 
*******************************************************************************/

/****************************************
    built from Generic  root.cpp
****************************************/
#include <tchar.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include "utilWinGinsuLibrary.h"

using namespace std; 
/*
// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Global Constant Definitions
// -----------------------------------------------------------------------------
const char* large_file_Z_prefix = ("Z:\\Test\\z");
const char* large_file_prefix = ("C:\\Test\\l");
const char* gkProgramName = "utilWinGinsuStressTest";

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Global Variables
// -----------------------------------------------------------------------------


char kStartup[] = "Program Starting";
char kShutdown[] = "Program Ending";
char* FILE_RD_1 = "i1.bin";
char* FILE_WR_1 = "Test\\o1.bin";
const size_t WR_SIZE = 1024*1024*10;

TCHAR* char2TCHAR(char *src)
{	
	int size = strlen(src); 
	TCHAR unicode_string[1024]; 
	mbstowcs(unicode_string, src, size+1);
	return unicode_string;
}

void FileMappingWrite(char* fr)
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
void FileMappingRead(char* fr)
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
		FileMappingWrite(tmp);
	}
	for(int i=0;i<iterator_;i++)
	{
		char tmp[1024];
		sprintf(tmp,"%s%d%s",large_file_prefix,i,".bin");	
		FileMappingRead(tmp);
	}
}*/
