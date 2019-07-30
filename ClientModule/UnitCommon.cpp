//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include "UnitCommon.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
//---------------------------------------------------------------------------
char win_dir[MAX_PATH];
double SystemTime;
//---------------------------------------------------------------------------
void BasicCode(char *Data_, int DataSize_, unsigned int Code_)
{
    for ( int i=0, j=DataSize_-sizeof(unsigned int); i<=j; i++ ) *((unsigned int*)(Data_+i))+=Code_;
}
//---------------------------------------------------------------------------
void BasicEncode(char *Data_, int DataSize_, unsigned int Code_)
{
    for ( int i=DataSize_-sizeof(unsigned int); i>=0; i-- ) *((unsigned int*)(Data_+i))-=Code_;
}
//---------------------------------------------------------------------------

