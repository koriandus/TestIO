/*******************************************************************************
* Filename: utilWinGinsuLibrary.h
* Original Author: Rich Woodland
* File Creation Date: 10/3/2011
* Subsystem: Utility
*
* Description: This is the Ginsu library for the system 
*
* Copyright Information (c) FMI Medical Systems, Inc. 2011
* Source Control Information: $Id: utilWinGinsuLibrary.h 15049 2016-02-09 16:25:36Z laurentl $ 
*******************************************************************************/

#include <Windows.h>


/*******************************************************************************
* Module Name: fgsWhoAmI( )
*
* Description: Returns a string containing the version info
* 
* Returns
*  Type          Description
*  char*         contains the verion info
*
*******************************************************************************/

char * __stdcall fgsWhoAmI ();



/*******************************************************************************
* Module Name: fgsDumpLocalGinsuItems
*
* Description: This local function will produce a list of local ginsu names.
* 
* Calling Arguments: 
*  Name       Input/Output  Type   Description 
*  pBuffer    output        char*  Output buffer for the list of ginsu names.
*
*******************************************************************************/

void __stdcall fgsDumpLocalGinsuItems( char *pBuffer );


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

void * __stdcall fgsMakeGinsu( char* pName, int pSize );



/*******************************************************************************
* Module Name: fgsMapGinsu
*
* Description: Map to a Ginsu having the specified name.  
* 
* Calling Arguments: 
*  Name       Input/Output  Type   Description 
*  pName      input         char*  The name of the Ginsu to map.
*  pSize      input         int    The size (bytes) for the mapping.
*
* Returns
*  Type        Description
*  void *      Address of the mapped Ginsu, or NULL if not mapped.
*
*******************************************************************************/ 

void * __stdcall fgsMapGinsu( char* pName, int pSize );

/*******************************************************************************
* Module Name: fgsFreeGinsuByName
*
* Description: Free an existing Ginsu (with the specified name).  
* 
* Calling Arguments: 
*  Name       Input/Output  Type   Description 
*  pName      input         char*  The name of the Ginsu to delete.
*
*******************************************************************************/ 

int __stdcall fgsFreeGinsuByName( char* pName );



/*******************************************************************************
* Module Name: fgsFreeGinsu
*
* Description: Free an existing Ginsu (with the specified start address).  
* 
* Calling Arguments: 
*  Name          Input/Output  Type   Description 
*  pGinsuStart   input         void*  The start address of the Ginsu to delete.
*
*******************************************************************************/ 

int __stdcall fgsFreeGinsu( void *pGinsuStart );



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

DWORD __stdcall fgsLastErrorCode();




/******************************************************************************
*   Component Name: IDL_XXXXXX
*            Scope: PUBLIC
*  Original Author: Aaron Zhang
*    Creation Date: November 22, 2011.
*
*  Description: This is the IDL GLUE.
*******************************************************************************/
void IDL_fgsMakeGinsu(int argc, void *argv[]);

void IDL_fgsFreeGinsu(int argc, void *argv[]);

void IDL_fgsFreeGinsuByName(int argc, void *argv[]);

void IDL_fgsMapGinsu(int argc, void *argv[]);

int IDL_fgsRead(int argc, void *argv[]);

int IDL_fgsWrite(int argc, void *argv[]);





// =================================================================================
int __stdcall fggsMakeGinsu(char *pName, int pSizeInBytes);
int __stdcall fggsFreeGinsuByName( char* pName );
int __stdcall fggsFreeAllGinsus( void );
DWORD __stdcall fggsLastErrorCode();
int __stdcall fggsListGinsus(unsigned char *pBuffer);

int __stdcall fggsMapGinsu(char *pName, int pSizeInBytes = 0);
void * __stdcall fggsGinsuPtr(char *pName);
char * __stdcall fggsGinsuPtrString(char *pName);

//int fggsGinsuRead(char *pName, char *pDest, int pGinsuOffsetInBytes = 0, int pGinsuSizeInBytes = 0);
int __stdcall fggsGinsuReadByte(char *pName, unsigned char *pDest, int pReadOffsetInBytes = 0, int pWriteOffsetInBytes = 0, int pDataSizeInBytes = 0);
int __stdcall fggsGinsuReadInt(char *pName, int *pDest, int pReadOffsetInBytes = 0, int pWriteOffsetInBytes = 0, int pDataSizeInBytes = 0);
int __stdcall fggsGinsuReadFloat(char *pName, float *pDest, int pReadOffsetInBytes = 0, int pWriteOffsetInBytes = 0, int pDataSizeInBytes = 0);
int __stdcall fggsGinsuReadDouble(char *pName, double *pDest, int pReadOffsetInBytes = 0, int pWriteOffsetInBytes = 0, int pDataSizeInBytes = 0);
int __stdcall fggsGinsuReadIntToFloat(char *pName, float *pDest, int pReadOffsetInBytes = 0, int pWriteOffsetInBytes = 0, int pDataSizeInBytes = 0);

//int fggsGinsuWrite(char *pName, char *pOrigin, int pGinsuOffsetInBytes = 0, int pGinsuSizeInBytes = 0);
int __stdcall fggsGinsuWriteByte(char *pName, unsigned char *pOrigin, int pReadOffsetInBytes = 0, int pWriteOffsetInBytes = 0, int pDataSizeInBytes = 0);
int __stdcall fggsGinsuWriteInt(char *pName, int *pOrigin, int pReadOffsetInBytes = 0, int pWriteOffsetInBytes = 0, int pDataSizeInBytes = 0);
int __stdcall fggsGinsuWriteFloat(char *pName, float *pOrigin, int pReadOffsetInBytes = 0, int pWriteOffsetInBytes = 0, int pDataSizeInBytes = 0);
int __stdcall fggsGinsuWriteDouble(char *pName, double *pOrigin, int pReadOffsetInBytes = 0, int pWriteOffsetInBytes = 0, int pDataSizeInBytes = 0);

// File Read Functions
int __stdcall fggsFileReadByte(char *pFullname, char *pBuffer, int pNumBytes);
int __stdcall fggsFileReadUInt32(char *pFullname, unsigned __int32 *pBuffer, int pNumInts);
int __stdcall fggsFileReadFloat(char *pFullname, float *pBuffer, int pNumFloats, int pOffset=0);

int __stdcall fggsFileReadDoubleToFloat(char *pFullname, float *pBuffer, int pNumElements, int pOffset=0);

// File Write Functions
int __stdcall fggsFileWriteByte(char *pFullname, char *pBuffer, int pNumBytes);
int __stdcall fggsFileWriteFloat(char *pFullname, float *pBuffer, int pNumFloats);
int __stdcall fggsFileWriteInt32(char *pFullname, __int32 *pBuffer, int pNumInts);
int __stdcall fggsFileWriteUInt32(char *pFullname, unsigned __int32 *pBuffer, int pNumInts, int pOffsetInBytes=0);

// Buffer Functions
int __stdcall fggsBufferCastByteToFloat(char *pByteBuffer, float *pFloatBuffer, int pReadOffsetInBytes, int pWriteOffsetInBytes, int pBufferSizeInBytes);