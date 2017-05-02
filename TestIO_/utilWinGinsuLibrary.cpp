/*******************************************************************************
* Filename: utilWinConfigLibrary.cpp
* Original Author: Rich Woodland
* File Creation Date: 10/3/2011
* Subsystem: Utility
*
* Copyright Information (c) FMI Medical Systems, Inc. 2011
* Source Control Information: $Id: utilWinGinsuLibrary.cpp 15776 2016-04-07 19:39:15Z rohitb $ 
*******************************************************************************/
#define _AFXDLL 1
#include <afx.h>


#include <stdio.h>

#include "utilWinGinsuLibrary.h"

#include <iostream>

#include <fstream>
using namespace std;


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Global Constant Definitions
// -----------------------------------------------------------------------------

const char* gkVersionInfo = "$Id: utilWinGinsuLibrary.cpp 15776 2016-04-07 19:39:15Z rohitb $";

#define gMaxGinsuNameLength 256


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Global Type Definitions
// -----------------------------------------------------------------------------

struct gtGinsuItem {
	struct gtGinsuItem  *NextItem;
	char		        Filename[gMaxGinsuNameLength];
	HANDLE		        FileHandle;
	LPCTSTR		        BufferPtr;
	int			        SizeInBytes;
};


// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Global Variables
// -----------------------------------------------------------------------------

DWORD gLastErrorNumber;

struct gtGinsuItem *gGinsuListHead = NULL;



/*******************************************************************************
* Module Name: fgsDumpLocalGinsuItems
*
* Description: This function will produce a list of local ginsu names.
* 
* Calling Arguments: 
*  Name       Input/Output  Type   Description 
*  pBuffer    output        char*  Output buffer for the list of ginsu names.
*
*******************************************************************************/

void __stdcall fgsDumpLocalGinsuItems( char *pBuffer ) {

    char myBuffer[1024];

    myBuffer[0] = 0;

	struct gtGinsuItem *myHead = gGinsuListHead;

	while ( myHead ) {

        strcat_s(myBuffer,myHead->Filename);
        strcat_s(myBuffer,",");

		myHead = myHead->NextItem;

	}

    strcpy_s(pBuffer,1024,myBuffer);

}



/*******************************************************************************
* Module Name: FindLocalGinsuItem
*
* Description: This local function will attempt to find a Ginsu with the name
*              specified in the local list of Ginsu Items. If found, the address
*              of the associated Ginsu Item Structure is returned to caller. If
*              not found, a NULL pointer is returned.
* 
* Calling Arguments: 
*  Name       Input/Output  Type   Description 
*  pName      input         char*  The name of the Local Ginsu to be found.
*
* Returns
*  Type        Description
*  void *      Address of the Ginsu Item structure, or NULL if no matching
*              Local Ginsu is found.
*
*******************************************************************************/

struct gtGinsuItem *FindLocalGinsuItem( char *pName ) {

	struct gtGinsuItem *myHead = gGinsuListHead;

	while ( myHead ) {

        if ( strcmp(myHead->Filename,pName) == 0 ) {
			return myHead;
		}
		myHead = myHead->NextItem;

	}

	return NULL;

}



/*******************************************************************************
* Module Name: FindLocalGinsuItem
*
* Description: This local function will attempt to find a Ginsu with the start
*              address specified in the local list of Ginsu Items. If found,
*              the address of the associated Ginsu Item Structure is returned
*              to caller. If not found, a NULL pointer is returned.
* 
* Calling Arguments: 
*  Name         Input/Output  Type           Description 
*  pGinsuStart  input         gtGinsuItem *  The start address of the Local
*                                            Ginsu to be found.
*
* Returns
*  Type        Description
*  void *      Address of the Ginsu Item structure, or NULL if no matching
*              Local Ginsu is found.
*
*******************************************************************************/

struct gtGinsuItem *FindLocalGinsuItem( void *pGinsuStart ) {

	struct gtGinsuItem *myHead = gGinsuListHead;

	while ( myHead ) {

		if ( myHead->BufferPtr == pGinsuStart ) {
			return myHead;
		}

		myHead = myHead->NextItem;

	}

    return NULL;

}



/*******************************************************************************
* Module Name: AddLocalGinsuItem
*
* Description: This local procedure will attempt to create a new Local Ginsu
*              Item, placing the new item at the head of the Local Ginsu List.
* 
* Calling Arguments: 
*  Name         Input/Output  Type           Description 
*  pBufferPtr   input         LPCTSTR        The buffer pointer of the new 
*                                            Ginsu item to be created.
*  pHandle      input         HANDLE         The handle of the new Ginsu item
*                                            to be created.
*  pName        input         char*          The name of the new Ginsu item to 
*                                            be found.
*  pSize        input         int            The size of the new Ginsu item to
*                                            be found.
*
*******************************************************************************/

void AddLocalGinsuItem( LPCTSTR pBufferPtr, HANDLE pHandle, char *pName, int pSize ) {

	struct gtGinsuItem *myGinsuItem;

	myGinsuItem = (gtGinsuItem *) malloc(sizeof(gtGinsuItem));

    if ( myGinsuItem ) {

		myGinsuItem->BufferPtr = pBufferPtr;
		myGinsuItem->FileHandle = pHandle;
		strcpy_s(myGinsuItem->Filename,gMaxGinsuNameLength,pName);
		myGinsuItem->SizeInBytes = pSize;
		myGinsuItem->NextItem = gGinsuListHead;

		gGinsuListHead = myGinsuItem;

	} else {

		gLastErrorNumber = -1;
	}

}



/*******************************************************************************
* Module Name: RemoveLocalGinsuItem
*
* Description: This local procedure will remove the specified Local Ginsu Item
*              from the local list of Ginsu Items, freeing up any resources
*              used by the removed Local Ginsu.
* 
* Calling Arguments: 
*  Name          Input/Output  Type           Description 
*  pTargetItem   Input         gtGinsuItem *  The target Ginsu Item address.
*
*******************************************************************************/

void RemoveLocalGinsuItem( struct gtGinsuItem *pTargetItem ) {

	struct gtGinsuItem *myHead = gGinsuListHead;

	if ( pTargetItem != NULL ) {

		if ( pTargetItem == gGinsuListHead ) {

			gGinsuListHead = pTargetItem->NextItem;

		} else {

			while ( myHead ) {

				if ( pTargetItem == myHead->NextItem ) {
					
					// Found the target.

					myHead->NextItem = (myHead->NextItem)->NextItem;

					break;

				}

				myHead = myHead->NextItem;

			}

		}

		free(pTargetItem);

	}

}



/*******************************************************************************
* Module Name: fgsWhoAmI( )
*
* Description: Returns a string containing the version info
* 
* Returns
*  Type          Description
*  char*         contains the version info
*
*******************************************************************************/

char* __stdcall fgsWhoAmI () {

	gLastErrorNumber = 0;

    return (char*) gkVersionInfo;

}



/*******************************************************************************
* Module Name: fgsMakeGinsu
*
* Description: Create a new Ginsu, with the specified name and size.  
* 
* Calling Arguments: 
*  Name       Input/Output  Type   Description 
*  pName      input         char*  The name of the Ginsu to create.
*  pSize      input         int    The size (bytes) for the new Ginsu.
*
* Returns
*  Type        Description
*  void *      Address of the created Ginsu, or NULL if not created.
*
*******************************************************************************/

void * __stdcall fgsMakeGinsu( char* pName, int pSize ) {

	HANDLE myFileHandle;
	LPCTSTR myBufferPtr;
    char myActualFilename[256];
	//CStringW myFilename;
    char myErrorMessage[512];
	struct gtGinsuItem *myGinsuItem;

	gLastErrorNumber = 0;

    sprintf_s(myActualFilename,sizeof(myActualFilename),"Global\\%s",pName);

    CStringW myFilename(pName);

	HANDLE hFile = CreateFile(myFilename, GENERIC_WRITE | GENERIC_READ, 0, NULL,
		CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);

	myGinsuItem = FindLocalGinsuItem(pName);

	if ( myGinsuItem ) {
        sprintf_s(myErrorMessage,"Ginsu %s already exists! Mapping anyway",pName);
		cout <<myErrorMessage << endl;
		return (void *) myGinsuItem->BufferPtr;
	}

	myFileHandle = CreateFileMapping (
						hFile/*INVALID_HANDLE_VALUE*/,	// use paging file
						NULL,					// default security
						PAGE_READWRITE,			// read/write access
						0,						// maximum object size (high-order DWORD)
						pSize,					// maximum object size (low-order DWORD)
						NULL);			// name of mapping object

	if (myFileHandle == NULL) {
		gLastErrorNumber = GetLastError();
        sprintf_s(myErrorMessage,"Error #%d when creating ginsu %s",gLastErrorNumber,pName);
		cout <<myErrorMessage << endl;
		return NULL;
	}

	myBufferPtr = (LPTSTR) MapViewOfFile(myFileHandle,	// handle to map object
								FILE_MAP_ALL_ACCESS,	// read/write permission
								0,
								0,
								pSize);

	if (myBufferPtr == NULL) {
		gLastErrorNumber = GetLastError();
		CloseHandle(myFileHandle);
        sprintf_s(myErrorMessage,"Error #%d when mapping ginsu view %s",gLastErrorNumber,pName);
		cout <<myErrorMessage << endl;
		return NULL;
	}

	AddLocalGinsuItem(myBufferPtr,myFileHandle,pName,pSize);

	return (void *) myBufferPtr;

}

void * __stdcall fgsMapGinsu( char* pName, int pSize ) {

	HANDLE myFileHandle;
	LPCTSTR myBufferPtr;
    char myActualFilename[256];
	//CStringW myFilename;
    //char myErrorMessage[512];

	struct gtGinsuItem *myGinsuItem;

	gLastErrorNumber = 0;

    sprintf_s(myActualFilename,sizeof(myActualFilename),"Global\\%s",pName);

    CStringW myFilename(pName);

	myGinsuItem = FindLocalGinsuItem(pName);

	if ( myGinsuItem ) {
		//PrintGinsuItem("fgsMapGinsu (already exist)", pName);

		return (void *) myGinsuItem->BufferPtr;
	}


	myFileHandle = OpenFileMapping (
					FILE_MAP_ALL_ACCESS,	// read/write access
					FALSE,					// do not inherit the name
					myFilename);			// name of mapping object

	if(myFileHandle == NULL)
	{
		HANDLE hFile = CreateFile(myFilename, GENERIC_READ, 0, NULL,
			OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
		myFileHandle = CreateFileMapping (
			hFile/*INVALID_HANDLE_VALUE*/,	// use paging file
			NULL,					// default security
			PAGE_READWRITE,			// read/write access
			0,						// maximum object size (high-order DWORD)
			pSize,					// maximum object size (low-order DWORD)
			NULL);			// name of mapping object 
	}

	if (myFileHandle == NULL) {
		gLastErrorNumber = GetLastError();
        //sprintf_s(myErrorMessage,"Error #%d when opening ginsu file %s",gLastErrorNumber,pName);
        //flsInsertWarning("utilWinGinsuLibrary","fgsMapGinsu",myActualFilename,myErrorMessage);
		return NULL;
	}

	myBufferPtr = (LPTSTR) MapViewOfFile(myFileHandle,	// handle to map object
								FILE_MAP_ALL_ACCESS,	// read/write permission
								0,
								0,
								pSize);
	cout <<"fgsMapGinsu:"<<GetLastError()<<endl;
	if (myBufferPtr == NULL) {
		gLastErrorNumber = GetLastError();
		cout <<"myBufferPtr is NULL " << gLastErrorNumber << endl;
		CloseHandle(myFileHandle);
        //sprintf_s(myErrorMessage,"Error #%d when mapping ginsu view %s",gLastErrorNumber,pName);
        //flsInsertWarning("utilWinGinsuLibrary","fgsMapGinsu",myActualFilename,myErrorMessage);
		return NULL;
	}

	AddLocalGinsuItem(myBufferPtr,myFileHandle,pName,pSize);

	//PrintGinsuItem("fgsMapGinsu (new)...", pName);

	printf("MAP(1) '%s' -> %p\n", pName, myBufferPtr);

	return (void *) myBufferPtr;

}

/*******************************************************************************
* Module Name: fgsFreeGinsuByName
*
* Description: Free an existing Ginsu (with the specified name).  
* 
* Calling Arguments: 
*  Name       Input/Output  Type   Description 
*  pName      input         char*  The name of the Ginsu to delete.
* Returns
*  Type        Description
*  int        Status code. If succesed, return 0.
*
*******************************************************************************/ 

int __stdcall fgsFreeGinsuByName( char* pName ) {

    int myStatus = 0;

    //char myErrorMessage[256];

    struct gtGinsuItem *myGinsuItem;

	gLastErrorNumber = 0;

	myGinsuItem = FindLocalGinsuItem(pName);

	if ( myGinsuItem ) {

        if ( UnmapViewOfFile(myGinsuItem->BufferPtr) == false ) {
            gLastErrorNumber = GetLastError();
			//sprintf_s(myErrorMessage,"Unable to unmap ginsu using address %p, Error: %d",myGinsuItem->BufferPtr,gLastErrorNumber);
            //flsInsertWarning("utilWinGinsuLibrary","fgsFreeGinsu","",myErrorMessage);
            return -1;
        };

		CloseHandle(myGinsuItem->FileHandle);

		RemoveLocalGinsuItem(myGinsuItem);
		//printf("Did FreeGinsuByName ginsu: '%s'\n", pName);

	} else {

        //flsInsertWarning("utilWinGinsuLibrary","fgsFreeGinsuByName",pName,"Unable to free ginsu");
		//printf("Unable to FreeGinsuByName ginsu: '%s'\n", pName);
        return -1;
    }

	printf("FreeGinsuByName removed ginsu: '%s'\n", pName);

    return myStatus;

}



/*******************************************************************************
* Module Name: fgsFreeGinsu
*
* Description: Free an existing Ginsu (with the specified start address).  
* 
* Calling Arguments: 
*  Name          Input/Output  Type   Description 
*  pGinsuStart   input         void*  The start address of the Ginsu to delete.
* Returns
*  Type        Description
*  int        Status code. If succesed, return 0.
*
*******************************************************************************/ 

int __stdcall fgsFreeGinsu( void *pGinsuStart ) {

    int myStatus = 0;

    //char myErrorMessage[256];

	struct gtGinsuItem *myGinsuItem;

	gLastErrorNumber = 0;

	myGinsuItem = FindLocalGinsuItem(pGinsuStart);

	if ( myGinsuItem ) {

        if ( UnmapViewOfFile(myGinsuItem->BufferPtr) == false ) {
            //sprintf_s(myErrorMessage,"Unable to unmap ginsu using address %p, Error: %d",myGinsuItem->BufferPtr,GetLastError());
            //flsInsertWarning("utilWinGinsuLibrary","fgsFreeGinsu","",myErrorMessage);
            gLastErrorNumber = GetLastError();
            return -1;
        };

		CloseHandle(myGinsuItem->FileHandle);

		RemoveLocalGinsuItem(myGinsuItem);
		//printf("Did FreeGinsu: '%p'\n", pGinsuStart);

	} else {

        //sprintf_s(myErrorMessage,"Unable to free ginsu using address %p",pGinsuStart);
        //flsInsertWarning("utilWinGinsuLibrary","fgsFreeGinsu","",myErrorMessage);
		printf("Unable to FreeGinsu ginsu: '%p'\n", pGinsuStart);
        return -1;

    }
	
	return myStatus;

}

/*******************************************************************************
* Module Name: fgsLastErrorCode
*
* Description: Return the last error code produced by the library.  
* 
* Returns
*  Type          Description
*  DWORD         The last error code produced by the library.
*
*******************************************************************************/ 

DWORD __stdcall fgsLastErrorCode() {

	return gLastErrorNumber;

}

/*******************************************************************************
* Module Name: fggsMakeGinsu
*
* Description: Create a new Ginsu, with the specified name and size.  
* 
* Calling Arguments: 
*  Name              Input/Output  Type   Description 
*  pName             input         string The name of the Ginsu to create.
*  pSizeInBytes      input         int    The size (bytes) for the new Ginsu.
*
* Returns
*  Type        Description
*  int        Status: 0 = SUCCESS, otherwise ERROR.
*
*******************************************************************************/
int __stdcall fggsMakeGinsu( char* pName,     // input parameter
                             int pSizeInBytes // input parameter
                           )                  // returns status (0: success, otherwise bug)
{
	int myStatus = -1;

	void *buf = fgsMakeGinsu(pName, pSizeInBytes); // creates ginsu, allocate extended memory data, ans insert in 'ginsu list'

	if (buf != NULL)
		myStatus = 0;

	printf("Create '%s' -> %p\n", pName, buf);

	return myStatus;
}

/*******************************************************************************
* Module Name: fggsFreeGinsuByName
*
* Description: Free an existing Ginsu (with the specified name).  
* 
* Calling Arguments: 
*  Name       Input/Output  Type   Description 
*  pName      input         string The name of the Ginsu to delete.
* Returns
*  Type        Description
*  int        Status: 0 = SUCCESS, otherwise ERROR.
*
*******************************************************************************/
int __stdcall fggsFreeGinsuByName( char* pName )
{
	return fgsFreeGinsuByName( pName );
}


int __stdcall fggsFreeAllGinsus( void )
{
	int n = 0;
	struct gtGinsuItem *myHead = gGinsuListHead,
		               *next;

	while ( myHead )
	{
		//printf("Removing: '%s'\n", myHead->Filename);
		next = myHead->NextItem;

		int status = fgsFreeGinsuByName( myHead->Filename );
		//printf("status = %d\n", status);

		//myHead = myHead->NextItem;
		myHead = next;
		n += 1;
	}

	//gGinsuListHead = NULL;
	//printf("fggsFreeAllGinsus removed %d ginsus\n", n);

	return n;
}

/*******************************************************************************
* Module Name: fggsLastErrorCode
*
* Description: Return the last error code produced by the library.  
* 
* Returns
*  Type          Description
*  uint        The last error code produced by the library.
*
*******************************************************************************/
DWORD __stdcall fggsLastErrorCode()
{
	return fgsLastErrorCode();
}

/*******************************************************************************
* Module Name: fggsListGinsus
*
* Description: populate a string buffer that identifies the current local ginsus.
*******************************************************************************/
int __stdcall fggsListGinsus(char *pBuffer)
{
    char myBuffer[1024];
	int n=0;

	//myBuffer[0] = 0;
	memset(myBuffer, 0, 1024);

	struct gtGinsuItem *myHead = gGinsuListHead;

	while ( myHead ) {

        strcat_s(myBuffer, myHead->Filename);
        strcat_s(myBuffer, "\n");
		n += 1;

		myHead = myHead->NextItem;
	}

	//int len = strlen(myBuffer);
	//myBuffer[len] - '\0';

    memcpy(pBuffer, myBuffer, 1024);

	return n;
}

/*******************************************************************************
* Module Name: fggsMapGinsu
*
* Description: Map to a Ginsu having the specified name.  
* 
* Calling Arguments: 
*  Name              Input/Output  Type   Description 
*  pName             input         char*  The name of the Ginsu to map.
*  pSizeInBytes      input         int    The size (bytes) for the mapping (OPTIONAL).
*
* Returns
*  Type        Description
*  int       Number of allocated bytes of the mapped Ginsu.
*
*******************************************************************************/
int __stdcall fggsMapGinsu(char *pName,     // input parameter
	                       int pSizeInBytes // input parameter (optional)
	                      )		            // returns size of ginsu
{
	// This is a modified version of the 'fgsMapGinsu' function...
	HANDLE myFileHandle;
    char myActualFilename[256];
	//CStringW myFilename;
    //char myErrorMessage[512];
	LPCTSTR myBufferPtr;
	int mySizeInBytes = pSizeInBytes;

	struct gtGinsuItem *myGinsuItem;

	gLastErrorNumber = 0;

    sprintf_s(myActualFilename,sizeof(myActualFilename),"Global\\%s",pName);

    CStringW myFilename(myActualFilename);

	myGinsuItem = FindLocalGinsuItem(pName);

	if ( myGinsuItem ) {
		//PrintGinsuItem("fgsMapGinsuUnsafe (already exist)", pName);

		return myGinsuItem->SizeInBytes;
	}

	myFileHandle = OpenFileMapping ( FILE_MAP_ALL_ACCESS,	// read/write access
					                 FALSE,					// do not inherit the name
					                 myFilename);			// name of mapping object

	if (myFileHandle == NULL) {
		gLastErrorNumber = GetLastError();
        //sprintf_s(myErrorMessage,"Error #%d when opening ginsu file %s",gLastErrorNumber,pName);
        //flsInsertWarning("utilWinGinsuLibrary","fgsMapGinsu",myActualFilename,myErrorMessage);
		return 0;
	}

	myBufferPtr = (LPTSTR) MapViewOfFile(myFileHandle,	        // handle to map object
								         FILE_MAP_ALL_ACCESS,	// read/write permission
								         0,
								         0,
								         mySizeInBytes);

	if (myBufferPtr == NULL) {
		gLastErrorNumber = GetLastError();
		CloseHandle(myFileHandle);
        //sprintf_s(myErrorMessage,"Error #%d when mapping ginsu view %s",gLastErrorNumber,pName);
        //flsInsertWarning("utilWinGinsuLibrary","fgsMapGinsu",myActualFilename,myErrorMessage);
		return 0;
	}

	if (mySizeInBytes == 0)
	{
		MEMORY_BASIC_INFORMATION MemInfo;
		int n = VirtualQuery(myBufferPtr, &MemInfo, sizeof(MemInfo));
		mySizeInBytes = (int)MemInfo.RegionSize;
	}
	AddLocalGinsuItem(myBufferPtr, myFileHandle, pName, mySizeInBytes);

	//PrintGinsuItem("fgsMapGinsuUnsafe (new)...", pName);
	printf("MAP(2) '%s' -> %p\n", pName, myBufferPtr);

	return mySizeInBytes;

}

/*******************************************************************************
* Module Name: fggsGinsuPtr
*
* Description: get the ginsu's data pointer.  // NOT part of the framework, ...only used by other C++ applications (...RECON!) 
* 
* Calling Arguments: 
*  Name              Input/Output  Type   Description 
*  pName             input         char*  The name of the Ginsu to map.
*
* Returns
*  Type        Description
*  char *      data pointer of the mapped Ginsu.
*
*******************************************************************************/
void * __stdcall fggsGinsuPtr(char *pName)              // input parameter
{
	void *ptr = NULL;
	struct gtGinsuItem	*myGinsuItem = FindLocalGinsuItem(pName);

	if ( myGinsuItem && myGinsuItem->BufferPtr && (myGinsuItem->SizeInBytes > 0) )
	{
		ptr = (void *)myGinsuItem->BufferPtr;
	}

	return ptr;
}
char * __stdcall fggsGinsuPtrString(char *pName)              // input parameter
{
	static char str[64];

	sprintf(str, "%p", fggsGinsuPtr(pName));

	return str;
}

/*******************************************************************************
* Module Names: fggsGinsuReadXXX & fggsGinsuWriteXXX
*
*     where "XXX" represent the actual data type(s) at play...
*
*     Note: all data sizes are specified in BYTES.
*
* Description: Read/write from/to mapped Ginsu having the specified name.  
* 
* Calling Arguments: 
*  Name                     Input/Output  Type            Description 
*  pName                    input         char*           The name of the Ginsu to map.
*  pOrigin                  input         int*, float*    Data source pointer to copy TO the mapped memeory.
*  pDest                    output        int*, float*    Data destination pointer to copy FROM the mapped memeory.
*  pGinsuOffsetInBytes      input         int             OPTIONAL. The mapped ginsu offset (bytes).
*  pGinsuSizeInBytes        input         int             OPTIONAL. The size (bytes) of data to copy.
*
* Returns
*  Type        Description
*  int        Status: 0 = SUCCESS, otherwise ERROR.
*
*******************************************************************************/
int fggsGinsuRead( char *pName,              // input parameter
	             char *pDest,		       // input/output parameter, ...allocated in calling (C#) function
	             int pReadOffsetInBytes,  // input parameter (optional)
	             int pWriteOffsetInBytes,  // input parameter (optional)
	             int pDataSizeInBytes     // input parameter (optional)
                 )                           // returns 0 if OK, otherwise there's a booboo!
{
	int myStatus = -1;	// booboo by default!
	struct gtGinsuItem	*myGinsuItem = FindLocalGinsuItem(pName);

	if ( myGinsuItem && myGinsuItem->BufferPtr && (myGinsuItem->SizeInBytes > 0) )
	{
		if ((pReadOffsetInBytes + pDataSizeInBytes) <= myGinsuItem->SizeInBytes)
		{
			char *myGinsuPtr = (char *)myGinsuItem->BufferPtr;
			int mySizeInBytes = (pDataSizeInBytes > 0)? pDataSizeInBytes: myGinsuItem->SizeInBytes;

			memcpy(&pDest[ pWriteOffsetInBytes ], &myGinsuPtr[ pReadOffsetInBytes ], mySizeInBytes);
			myStatus = 0;
		}
	}

	return myStatus;
}

int __stdcall fggsGinsuReadByte( char *pName,              // input parameter
							    unsigned char *pDest,     // input/output parameter, ...allocated in calling (C#) function
								 int pReadOffsetInBytes,  // input parameter (optional)
								 int pWriteOffsetInBytes,  // input parameter (optional)
								 int pDataSizeInBytes     // input parameter (optional)
                              )                           // returns 0 if OK, otherwise there's a booboo!
{
	return fggsGinsuRead(pName, (char *)pDest, pReadOffsetInBytes, pWriteOffsetInBytes, pDataSizeInBytes);
}

int __stdcall fggsGinsuReadInt( char *pName,              // input parameter
							    int *pDest,		          // input/output parameter, ...allocated in calling (C#) function
								 int pReadOffsetInBytes,  // input parameter (optional)
								 int pWriteOffsetInBytes,  // input parameter (optional)
								 int pDataSizeInBytes     // input parameter (optional)
                              )                           // returns 0 if OK, otherwise there's a booboo!
{
	return fggsGinsuRead(pName, (char *)pDest, pReadOffsetInBytes, pWriteOffsetInBytes, pDataSizeInBytes);
}

int __stdcall fggsGinsuReadFloat( char *pName,              // input parameter
							      float *pDest,		        // input/output parameter, ...allocated in calling (C#) function
								 int pReadOffsetInBytes,  // input parameter (optional)
								 int pWriteOffsetInBytes,  // input parameter (optional)
								 int pDataSizeInBytes     // input parameter (optional)
							    )                           // returns 0 if OK, otherwise there's a booboo!
{
	return fggsGinsuRead(pName, (char *)pDest, pReadOffsetInBytes, pWriteOffsetInBytes, pDataSizeInBytes);
}

int __stdcall fggsGinsuReadDouble( char *pName,              // input parameter
							       double *pDest,		     // input/output parameter, ...allocated in calling (C#) function
									 int pReadOffsetInBytes,  // input parameter (optional)
									 int pWriteOffsetInBytes,  // input parameter (optional)
									 int pDataSizeInBytes     // input parameter (optional)
							     )                           // returns 0 if OK, otherwise there's a booboo!
{
	return fggsGinsuRead(pName, (char *)pDest, pReadOffsetInBytes, pWriteOffsetInBytes, pDataSizeInBytes);
}

int __stdcall fggsGinsuReadIntToFloat( char *pName,               // input parameter
							           float *pDest,	          // input/output parameter, ...allocated in calling (C#) function
										 int pReadOffsetInBytes,  // input parameter (optional)
										 int pWriteOffsetInBytes,  // input parameter (optional)
										 int pDataSizeInBytes     // input parameter (optional)
							         )                            // returns 0 if OK, otherwise there's a booboo!
{
	int myStatus = -1;	// bug by default!
	struct gtGinsuItem	*myGinsuItem = FindLocalGinsuItem(pName);

	if ( myGinsuItem && myGinsuItem->BufferPtr && (myGinsuItem->SizeInBytes > 0) )
	{
		int *myGinsuPtr = (int *)myGinsuItem->BufferPtr;
		int myOffset = pReadOffsetInBytes / sizeof(int);
		int myGinsuSizeInBytes = (pDataSizeInBytes > 0)? pDataSizeInBytes: myGinsuItem->SizeInBytes;
		int mySize = myGinsuSizeInBytes / sizeof(int);

		int myOutputOffset = pWriteOffsetInBytes / sizeof(float);

		int in1 = myOffset;
		int in2 = myOffset + mySize;
		int out1 = myOutputOffset;

		for (int out=out1,in=in1; in<in2; in++,out++)
		{
			pDest[out] = (float)myGinsuPtr[in];
		}
		
		myStatus = 0;
	}

	return myStatus;
}

int fggsGinsuWrite( char *pName,                // input parameter
			      char *pOrigin,              // input parameter, ...allocated in calling (C#) function
				  int pReadOffsetInBytes,  // input parameter (optional)
			      int pWriteOffsetInBytes,    // input parameter (optional)
			      int pDataSizeInBytes       // input parameter (optional)
                )                             // returns 0 if OK, otherwise there's a booboo!
{
	int myStatus = -1;	// booboo by default!

	if (fggsMapGinsu(pName) > 0)
	{
		struct gtGinsuItem	*myGinsuItem = FindLocalGinsuItem(pName);

		if ( myGinsuItem && myGinsuItem->BufferPtr && (myGinsuItem->SizeInBytes > 0) )
		{
			char *myGinsuPtr = (char *)myGinsuItem->BufferPtr;
			int mySizeInBytes = (pDataSizeInBytes > 0)? pDataSizeInBytes: myGinsuItem->SizeInBytes;

			if ((pWriteOffsetInBytes + mySizeInBytes) <= myGinsuItem->SizeInBytes)
			{
				memcpy(&myGinsuPtr[ pWriteOffsetInBytes ], &pOrigin[ pReadOffsetInBytes ], mySizeInBytes);
				myStatus = 0;
			}
		}
	}

	return myStatus;
}

int __stdcall fggsGinsuWriteByte( char *pName,                // input parameter
						 	       char *pOrigin,     // input parameter, ...allocated in calling (C#) function
   				                   int pReadOffsetInBytes,  // input parameter (optional)
							       int pWriteOffsetInBytes,    // input parameter (optional)
							       int pDataSizeInBytes       // input parameter (optional)
                               )                               // returns 0 if OK, otherwise there's a booboo!
{
	return fggsGinsuWrite(pName, (char *)pOrigin, pReadOffsetInBytes, pWriteOffsetInBytes, pDataSizeInBytes);
}

int __stdcall fggsGinsuWriteInt( char *pName,                // input parameter
						 	       int *pOrigin,             // input parameter, ...allocated in calling (C#) function
   				                   int pReadOffsetInBytes,  // input parameter (optional)
							       int pWriteOffsetInBytes,    // input parameter (optional)
							       int pDataSizeInBytes       // input parameter (optional)
                               )                               // returns 0 if OK, otherwise there's a booboo!
{
	return fggsGinsuWrite(pName, (char *)pOrigin, pReadOffsetInBytes, pWriteOffsetInBytes, pDataSizeInBytes);
}

int __stdcall fggsGinsuWriteFloat( char *pName,                // input parameter
						 	       float *pOrigin,             // input parameter, ...allocated in calling (C#) function
   				                   int pReadOffsetInBytes,  // input parameter (optional)
							       int pWriteOffsetInBytes,    // input parameter (optional)
							       int pDataSizeInBytes       // input parameter (optional)
                               )                               // returns 0 if OK, otherwise there's a booboo!
{
	return fggsGinsuWrite(pName, (char *)pOrigin, pReadOffsetInBytes, pWriteOffsetInBytes, pDataSizeInBytes);
}

int __stdcall fggsGinsuWriteDouble( char *pName,                // input parameter
						 	        double *pOrigin,            // input parameter, ...allocated in calling (C#) function
   				                    int pReadOffsetInBytes,  // input parameter (optional)
							        int pWriteOffsetInBytes,    // input parameter (optional)
							        int pDataSizeInBytes       // input parameter (optional)
                                  )                             // returns 0 if OK, otherwise there's a booboo!
{
	return fggsGinsuWrite(pName, (char *)pOrigin, pReadOffsetInBytes, pWriteOffsetInBytes, pDataSizeInBytes);
}


/*******************************************************************************
* Module Name: fggsFileReadByte
*
* Description: Read a byte array from a file into a buffer
* 
* Calling Arguments: 
*  Name              Input/Output  Type					Description 
*  pName             input         char*				The name of the file to write
*  pBuffer			 output		   char*				The buffer to write
*
* Returns
*
*******************************************************************************/
int __stdcall fggsFileReadByte(char *pFullname, char *pBuffer, int pNumBytes)
{
	int myStatus = -1;
	FILE *fp = NULL;

	if ((fp = fopen(pFullname, "rb")) == NULL)
	{
		cout <<"file open failed: " << pFullname <<endl;
	}
	else
	{
		int i;
		
		i = fread(pBuffer, sizeof(char), pNumBytes, fp);
		if (i == pNumBytes)
			myStatus = 0;
	}

	if (fp != NULL)
		fclose (fp);

	return myStatus;
}

/*******************************************************************************
* Module Name: fggsFileReadUInt32
*
* Description: Reads from a file into a UInt32 array
* 
* Calling Arguments: 
*  Name              Input/Output  Type					Description 
*  pName             input         char*				The name of the file to write
*  pBuffer			 input		   unsigned __int32*	The buffer to read into
*
* Returns
*  Type        Description
*
*******************************************************************************/
int __stdcall fggsFileReadUInt32(char *pFullname, unsigned __int32 *pBuffer, int pNumInts)
{
	int myStatus = -1;
	FILE *fp = NULL;

	if ((fp = fopen(pFullname, "rb")) == NULL)
	{
	}
	else
	{
		int i;
		
		i = fread(pBuffer, sizeof(unsigned __int32), pNumInts, fp);
		if (i == pNumInts)
			myStatus = 0;
	}

	if (fp != NULL)
		fclose (fp);

	return myStatus;
}

/*******************************************************************************
* Module Name: fggsFileReadFloat
*
* Description: Reads from a file into a float array
* 
* Calling Arguments: 
*  Name              Input/Output  Type					Description 
*  pName             input         char*				The name of the file to write
*  pBuffer			 input		   float*				The buffer to read into
*
* Returns
*  Type        Description
*
*******************************************************************************/
int __stdcall fggsFileReadFloat(char *pFullname, float *pBuffer, int pNumFloats, int pOffset)
{
	int myStatus = -1;
	FILE *fp = NULL;

	if ((fp = fopen(pFullname, "rb")) == NULL)
	{
	}
	else
	{
		int i;
		
		int p = fseek(fp, pOffset * sizeof(float), SEEK_SET);

		i = fread(pBuffer, sizeof(float), pNumFloats, fp);
		if (i == pNumFloats)
			myStatus = 0;
	}

	if (fp != NULL)
		fclose (fp);

	return myStatus;
}

/*******************************************************************************
* Module Name: fggsFileReadDoubleToFloat
*
* Description: Reads double from a file and outputs to a float array
* 
* Calling Arguments: 
*  Name              Input/Output  Type					Description 
*  pName             input         char*				The name of the file to write
*
* Returns
*  Type        Description
*
*******************************************************************************/
int __stdcall fggsFileReadDoubleToFloat(char *pFullname, float *pBuffer, int pNumElements, int pOffset)
{
	int myStatus = -1;
	FILE *fp = NULL;
	double *myBuffer = NULL;

	if ((fp = fopen(pFullname, "rb")) == NULL)
	{
	}
	else
	{
		if ((myBuffer = (double *)malloc(pNumElements * sizeof(double))) != NULL)
		{
			int p = fseek(fp, pOffset * sizeof(float), SEEK_SET);

			int i = fread(myBuffer, sizeof(double), pNumElements, fp);
			
			if (i == pNumElements)
			{
				for (int k=0; k<pNumElements; k++)
					pBuffer[k] = (float)myBuffer[k];
			}
			myStatus = 0;
		}
	}

	if (myBuffer)
		free (myBuffer);

	if (fp != NULL)
		fclose (fp);

	return myStatus;
}

/*******************************************************************************
* Module Name: fggsFileWriteByte
*
* Description: Writes a byte array into a file
* 
* Calling Arguments: 
*  Name              Input/Output  Type					Description 
*  pName             input         char*				The name of the file to write
*  pBuffer			 input		   char*				The buffer to write
*  pSizeInBytes      input         int					The size (bytes) for the mapping (OPTIONAL).
*
* Returns
*  Type        Description
*
*******************************************************************************/
int __stdcall fggsFileWriteByte(char *pFullname, char *pBuffer, int pNumBytes)
{
	int myStatus = -1;
	FILE *fp = NULL;

	if ((fp = fopen(pFullname, "wb")) == NULL)
	{
	}
	else
	{
		int i;
		
		i = (int)fwrite(pBuffer, sizeof(char), pNumBytes, fp);
		if (i == pNumBytes)
			myStatus = 0;
	}

	if (fp != NULL)
	{
		fflush (fp);
		fclose (fp);
	}
	return myStatus;
}

/*******************************************************************************
* Module Name: fggsFileWriteFloat
*
* Description: Writes a UInt32 array into a file
* 
* Calling Arguments: 
*  Name              Input/Output  Type					Description 
*  pName             input         char*				The name of the file to write
*  pBuffer			 input		   float*				The buffer to write
*  pSizeInBytes      input         int					The size (bytes) for the mapping (OPTIONAL).
*
* Returns
*  Type        Description
*  int       Number of allocated bytes of the mapped Ginsu.
*
*******************************************************************************/
int __stdcall fggsFileWriteFloat(char *pFullname, float *pBuffer, int pNumFloats)
{
	int myStatus = -1;

	char* myByteBuffer = (char*)pBuffer;

	myStatus = fggsFileWriteByte(pFullname, myByteBuffer, pNumFloats * sizeof(float));		

	return myStatus;
}

/*******************************************************************************
* Module Name: fggsFileWriteInt32
*
* Description: Writes an Int32 array into a file
* 
* Calling Arguments: 
*  Name              Input/Output  Type					Description 
*  pName             input         char*				The name of the file to write
*  pBuffer			 input		    __int32				The buffer to write
*  pSizeInBytes      input         int					The size (bytes) for the mapping (OPTIONAL).
*
* Returns
*  Type        Description
*  int       Number of allocated bytes of the mapped Ginsu.
*
*******************************************************************************/
int __stdcall fggsFileWriteInt32(char *pFullname, __int32 *pBuffer, __int32 pNumInts)
{
	int myStatus = -1;

	char* myByteBuffer = (char*)pBuffer;

	myStatus = fggsFileWriteByte(pFullname, myByteBuffer, pNumInts * sizeof(__int32));		

	return myStatus;
}

/*******************************************************************************
* Module Name: fggsFileWriteUInt32
*
* Description: Writes a UInt32 array into a file
* 
* Calling Arguments: 
*  Name              Input/Output  Type					Description 
*  pName             input         char*				The name of the file to write
*  pBuffer			 input		   unsigned __int32		The buffer to write
*  pSizeInBytes      input         int					The size (bytes) for the mapping (OPTIONAL).
*
* Returns
*  Type        Description
*  int       Number of allocated bytes of the mapped Ginsu.
*
*******************************************************************************/
int __stdcall fggsFileWriteUInt32(char *pFullname, unsigned __int32*pBuffer, __int32 pNumInts, int pOffsetInBytes)
{
	int myStatus = -1;
	
	char *buffer = (char *)pBuffer;
	
	myStatus = fggsFileWriteByte(pFullname, &buffer[pOffsetInBytes], pNumInts * sizeof(unsigned __int32));		

	return myStatus;
}

/*******************************************************************************
* Module Name: fggsBufferReadByteToUInt32
*
* Description: Writes a UInt32 array into a file
* 
* Calling Arguments: 
*  Name              Input/Output  Type					Description 
*  pName             input         char*				The name of the file to write
*  pBuffer			 input		   unsigned __int32		The buffer to write
*  pSizeInBytes      input         int					The size (bytes) for the mapping (OPTIONAL).
*
* Returns
*  Type        Description
*  int       Number of allocated bytes of the mapped Ginsu.
*
*******************************************************************************/
int __stdcall fggsBufferCastByteToFloat(char *pByteBuffer, float *pFloatBuffer, int pReadOffsetInBytes, int pWriteOffsetInBytes, int pBufferSizeInBytes)
{
	int myStatus = -1;

	unsigned __int32* myIntBuffer = (unsigned __int32*)pByteBuffer;
	
	int myOffset = pReadOffsetInBytes / sizeof(unsigned __int32);	
	int mySize = pBufferSizeInBytes / sizeof(unsigned __int32);
	int myOutputOffset = pWriteOffsetInBytes / sizeof(float);

	int in1 = myOffset;
	int in2 = myOffset + mySize;
	int out1 = myOutputOffset;

	for (int out=out1,in=in1; in<in2; in++,out++)
	{
		pFloatBuffer[out] = (float)myIntBuffer[in];
	}

	myStatus = 0;

	return myStatus;
}










