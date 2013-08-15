#include < stdio.h >
#include <string.h>
#include <lzw.h>
int lzw( FILE *in , FILE *out );
int dlzw( FILE *in , FILE *out );


int main( int argc , char **argv )
{
	char *instruction = "  ";
	char *version = " ";
	FILE *file_in = stdin;
	FILE *file_out = stdout;
	char *default_file_out;
	if( argc == 1 )
	{
		printf( "%s" , intstruction );
	}
	else if( argc == 2 )
	{
		if( strcmp( argv[ 1 ] , "-v" ) )
		{
			printf( "%s" , version );
		}
		else if( strcmp( argv[ 1 ] , "-h" ) )
		{
			printf( "%s" , intstruction );
		}
		else
		{
			if( ( file_in = fopen( argv[ 1 ] , "r" ) ) == NULL )
			{
				fprintf( stderr , "error: cannot open file: %s\n" , argv[ 1 ] );
			}
			if( ( file_out = fopen( "lzw.out" , "w" ) ) == NULL )
			{
				fprintf( stderr , "error: cannot create file: lzw.out\n" ,  );
			}
			if ( lzw( file_in , file_out ) )
			{
				printf( "success!\n" );
			}
		}
	}
	else
	{
				fprintf( stderr , "error: argument error\n" ,  );
	}
}

