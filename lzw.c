#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "dic.h"
#include "lzw.h"
#include "globals.h"
#define BUF_SIZE (4096)

unsigned short BUF_IN[ BUF_SIZE ];
size_t POS_IN = BUF_SIZE;
FILE *IN;
unsigned short EMPTY_FLAG = 0;

unsigned short BUF_OUT[ BUF_SIZE ];
size_t POS_OUT = 0;
FILE *OUT;

void writeInt( unsigned short i )
{
	if( POS_OUT >= BUF_SIZE )
	{
		fwrite( BUF_OUT , sizeof( unsigned short int ) , BUF_SIZE , OUT );
		POS_OUT = 0;
	}
	BUF_OUT[ POS_OUT++ ] = i;

}

unsigned short readInt()
{
	static size_t SIZE_NOW = 0;
	if( POS_IN >= SIZE_NOW )
	{
		SIZE_NOW = fread( BUF_IN , sizeof( unsigned short int ) , BUF_SIZE , IN );
		POS_IN = 0;
		if( SIZE_NOW == 0 )
		{
			EMPTY_FLAG = 1;
		}
	}
	return BUF_IN[ POS_IN++ ];
}

void synFile()
{
	fwrite( BUF_OUT , sizeof( unsigned short int ) , POS_OUT, OUT );
}



int lzw()
{

	unsigned char str[MAX_STRLEN + 1];
	unsigned char ch;
	int ich = 0;	
	str[ 0 ] = '\0';
	
	while( ( ich = getc( IN ) ) != EOF )
	{
		ch = ( unsigned char ) ich;
		int length = strlen( str );
		int insert_code;
		int out_code;
		if( length == 0 )
		{
			str[ 0 ] = ch;
			str[ 1 ] = '\0';
			continue;
		}

		if( length == 1 )
		{
			out_code = (int)( str[ 0 ] );
		}
		else
		{
			out_code = dicSearchString( str );
		}

		if( length < MAX_STRLEN && length > 0 )
		{
			str[ length ] = ch; 
			str[ length + 1 ] = '\0';
			if( dicSearchString( str ) == -1 && ( length != 0 ) )
			{	
				if( dicSize() < MAX_DIC_SIZE)
				{
				
					if ( ( insert_code = dicInsert( str ) ) != -1 )
					{
					
						writeInt( out_code );
						/*fprintf( stdout , "code:%x [insert]string:%s key:%x\n" , out_code , str , insert_code );
						*/
						str[ 0 ] = ch;
						str[ 1 ] = '\0';
					}
					else
					{
						fprintf( stderr , "error: cannot insert to dictionary\n" );
						return 0;
					}
				}
			}
		}
		else
		{
			writeInt( out_code );
			str[ 0 ] = ch; 
			str[ 1 ] = '\0';
		}
	}
	return 1;
}

	
int dlzw()
{
	unsigned short k;
	unsigned char entry[MAX_STRLEN + 1];
	unsigned char s[MAX_STRLEN + 1];
	entry [ 0 ] = '\0';
	s [ 0 ] = '\0';
	unsigned char *search_rel = NULL;
	while( 1 )
	{
		k = readInt();
		if( EMPTY_FLAG )
		{
			break;
		}
		if( k < 256 )
		{
			entry[ 0 ] = (unsigned char)k;
			entry[ 1 ] = '\0'; 
		}
		else
		{
			
			/*printf( "entry address : %d\n" , (int)&entry );
			printf( "k:%d\n" , k );
			*/
			search_rel = dicSearchCode( k );
			if( search_rel != NULL )
			{
				/*printf( "befor cpy: entry:%s search_rel:%s\n" , entry , search_rel );
				*/
				strcpy( entry , search_rel );
				/*printf( "after cpy: entry:%s search_rel:%s\n" , entry , search_rel );
				*/
			}
			else
			{
				strcpy( entry , s );
				size_t entry_l = strlen( entry );
				if( entry_l < MAX_STRLEN )
				{
					entry[ entry_l ] = s[ 0 ];
					entry[ entry_l + 1 ] = '\0';
				}
			}
			
		}
		/*printf( "[input] %x [output] %s\n" , k , entry );
		*/
		fputs( entry , OUT );
		size_t s_l = strlen( s );
		if( s_l < MAX_STRLEN && s_l > 0 && dicSize() < MAX_DIC_SIZE )
		{
			s[ s_l ] = entry[ 0 ];
			s[ s_l + 1 ] = '\0';
			int insert_rel = dicInsert( s );
			if ( insert_rel == -1 )
			{
				fprintf( stderr , "error: cannot insert to dictionary\n" );
				return 0;
			}
			else
			{
				/*printf( "[insert] string: %s key: %x\n" , s , insert_rel );
			*/
			}
		}	
		strcpy( s , entry );
	}
	fputs( "\n" , OUT );
	return 1;
}






unsigned char * help = "lzw: A program to compress files\n-o Specify the output file name. If it is omited, {input file name}.lzw is selected\n-d Decompress the file\n-v See the version information \n-h See the help information \n";
unsigned char * version = "version 1.0 by Li, Xuanchong\nWarning!Lots of bugs with it!\n";



int main( int argc , char **argv )
{
	unsigned char *in_file_name = NULL;
	unsigned char *out_file_name = NULL;
	int result = 0;
	int decom_flag = 0;

	if( argc >= 2 )
	{
		in_file_name = argv[ 1 ];
	}
	else
	{
		printf( "%s" , help );
		exit( 1 );
	}

	if( in_file_name == NULL )
	{
		fprintf( stderr , "error: cannot get the input file\n" );
	}
	while ( (result = getopt( argc , argv , "o:hvd" ) ) != -1 )
	{
		switch ( result )
		{
			case 'h':
				printf( "%s" , help );
				exit( 1 );
			case 'v':
				printf( "%s" , version );
				exit( 1 );
			case 'o':
				out_file_name = optarg;
				break;
			case 'd':
				decom_flag = 1;
				break;
		}
	}
	

	if( out_file_name == NULL )
	{
		out_file_name = (unsigned char *)malloc( strlen( in_file_name ) + 1 + 4 );
		if( out_file_name == NULL )
		{
			fprintf( stderr , "error: memory error\n" );
			exit(1);
		}
		
		strcpy( out_file_name , in_file_name );
		strcat( out_file_name , ".lzw" );
	}
	IN = fopen( in_file_name , "r" );
	OUT = fopen( out_file_name , "w" );
	if( decom_flag )
	{
		dlzw();
	}
	else
	{
		lzw();
		synFile();
	}
	
	
	fclose( IN );
	fclose( OUT );

	
	return 1;
}

