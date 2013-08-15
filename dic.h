#ifndef _dic_H_
#define _dic_H_
#include <stdio.h>
unsigned char* dicSearchCode( unsigned short );
int dicSearchString( const unsigned char* );
int dicInsert( const unsigned char* );
int dicToFile( const FILE* );
size_t dicSize();
#endif
