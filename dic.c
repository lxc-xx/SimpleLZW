#include "dic.h"
#include "globals.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct NodeType
{
	struct NodeType *child[ 256 ];
	int key;
	unsigned char character;
} TrieNode;


typedef struct RootType
{
	struct NodeType *child[ 256 ];	
	size_t size;
} TrieRoot;


typedef TrieRoot* Trie;

Trie TRIE_TREE = NULL;

unsigned char* Dic[MAX_DIC_SIZE];
size_t Dic_Size = 0;
int FIRST_FLAG = 1;


int ITEM_COUNT = 256;



int trieInit()
{
	TRIE_TREE = ( Trie )malloc ( sizeof( TrieRoot ) );
	if( TRIE_TREE == NULL )
	{
		fprintf( stderr , " error: cannot initiate a trie \n" );
		return 0;
	}
	else
	{
		int i;
		for( i = 0 ; i < 256 ; ++i )
		{
			TRIE_TREE->child[ i ] = NULL;	
		}
		TRIE_TREE->size = 0;
		return 1;
	}
}

int trieDestruct( Trie root )
{
	if( FIRST_FLAG )
	{
		FIRST_FLAG =0;
		memset( (void*)Dic , 0 , MAX_DIC_SIZE );
		trieInit();
	}
	int i;
	for( i = 0 ; i < 256 ; ++i )
	{
		if( root->child[ i ] != NULL )
		{
			if( nodeDestruct( root->child[ i ] ) != 1 )
			{			
				fprintf( stderr , "error: cannot destruct node \n " );
				return 0;
			}
		}
	}
	return 1;
}

int nodeDestruct( TrieNode *node )
{
	if( FIRST_FLAG )
	{
		FIRST_FLAG =0;
		memset( (void*)Dic , 0 , MAX_DIC_SIZE );
		trieInit();
	}
	int i;
	for( i = 0 ; i < 256 ; ++i )
	{	
		if( node->child[ i ] != NULL )
		{
			if( nodeDestruct( node->child[ i ] ) != 1 )
			{
				fprintf( stderr , "error: cannot destruct node \n " );
				return 0;	
			}
			else
			{
				free( node );
				return 1;
			}
		}
	}
}

TrieNode *trieSearch( const unsigned char *name )
{
	if( FIRST_FLAG )
	{
		FIRST_FLAG =0;
		memset( (void*)Dic , 0 , MAX_DIC_SIZE );
		trieInit();
	}
	if( TRIE_TREE == NULL )
	{
		trieInit();
	}
	Trie root = TRIE_TREE;	

	TrieNode * node;

	unsigned char target;
	size_t size = strlen( name );
	
	if( size == 0 || size > MAX_STRLEN )
	{
		return NULL;
	}

	int i;
	int key;
	for( i = 0 ; i < size ; ++i )
	{
		
		target = (unsigned char) ( name[ i ] );
		if( (size_t)target >= 256 || (size_t)target < 0 )
		{
			printf( "bad target: %c\n" , target );
			printf( "name: %s i: %d\n" , name , i );
			exit( 1 );
		}
		if( i == 0 )
		{
			node = root->child[ (size_t)target ];
		}
		else
		{
			node = node->child[ (size_t)target ];	
		}

		if( node == NULL )
		{
			return NULL;	
		}
		
	}
	return node;
}

TrieNode* trieInsert( const unsigned char *name )
{
	/*printf( "start: insert name: %s\n" , name );
	*/
	if( FIRST_FLAG )
	{
		FIRST_FLAG =0;
		memset( (void*)Dic , 0 , MAX_DIC_SIZE );
		trieInit();
	}
	if( TRIE_TREE == NULL )
	{
		trieInit();
	}
	Trie root = TRIE_TREE;	
	TrieNode * node = NULL;

	unsigned char target;
	size_t size = strlen( name );
	if( size == 0 || size > MAX_STRLEN )
	{
		fprintf( stderr, "error: wrong length item\n" );
		return NULL;
	}

	int i;
	for( i = 0 ; i < size ; ++i )
	{
		
		target = name[ i ];
		if( i == 0 )
		{
			node = root->child[ (size_t)target ];
			if( node == NULL )
			{
				root->child[ (size_t)target ] = ( TrieNode* )malloc( sizeof( TrieNode ) );
				node = root->child[ (size_t)target ];
				if( node == NULL )
				{
					fprintf( stderr , "error: cannot create a trie node\n" );
					return NULL;
				}
				int i;
				for( i = 0 ; i < 256 ; ++i )
				{
					node->child[ i ] = NULL;
				}	
				node->key = -1;
				node->character = target;
			}
		}
		else
		{
			if( node->child[ (size_t)target ] == NULL )
			{
				node->child[ (size_t)target ] = ( TrieNode* )malloc( sizeof( TrieNode ) );
				node = node->child[ (size_t)target ];
				if( node == NULL )
				{
					fprintf( stderr , "error: cannot create a trie node\n" );	
					return NULL;
				}
				int i;
				for( i = 0 ; i < 256 ; ++i )
				{
					node->child[ i ] = NULL;
				}		
				node->key = -1;
				node->character = target;
			}
			else
			{
				node = node->child[ (size_t)target ];
			}
		}
		
	}
	/*printf( "node: %x\n" , (size_t)node );
	*/
	if( node->key == -1 )
	{
		node->key = ITEM_COUNT;
		root->size++;
		ITEM_COUNT++;
	}
	/*printf( "done: insert name: %s\n" , name );
	*/
	return node;
}


int trieToFile( const FILE *file )
{
	if( FIRST_FLAG )
	{
		FIRST_FLAG =0;
		memset( (void*)Dic , 0 , MAX_DIC_SIZE );
		trieInit();
	}
	if( TRIE_TREE == NULL )
	{
		trieInit();
	}
	Trie root = TRIE_TREE;
	int i;
	unsigned char name[MAX_STRLEN + 1];
	for( i = 0 ; i < 256 ; ++i )
	{
		if( root->child[ i ] != NULL )
		{
			name[ 0 ] = (char)i;
			name[ 1 ] = '\0';
			if ( nodeToFile( root->child[ i ] , name , file ) == 0)
			{
				fprintf( stderr , "error: cannot print a node\n" );
				return 0;
			}
		}
	}
	return 1;
}

int nodeToFile( TrieNode *node , unsigned char *name , FILE *file )
{
	if( FIRST_FLAG )
	{
		FIRST_FLAG =0;
		memset( (void*)Dic , 0 , MAX_DIC_SIZE );
		trieInit();
	}
	if( node == NULL )
	{
		return 0;
	}
	else
	{
		if( file == NULL )
		{
			file = stdout;	
		}
		if( node->key >= 0 )
		{
			fprintf( file , "string:%s\tkey:%d\n" , name , node->key );
		}
		int i;
		for( i = 0 ; i < 256 ; ++i )
		{
			if( node->child[ i ] != NULL )
			{
				unsigned char tail[2];
				tail[ 0 ] = (unsigned char)i;
				tail[ 1 ] = '\0';
				strcat( name , tail );
				if ( nodeToFile( node->child[ i ] , name , file ) == 0)
				{
					fprintf( stderr , "error: cannot print a node\n" );
					return 0;
				}
				size_t l = strlen( name );
				name[ l - 1 ] = '\0';
			}
		}
	}
	return 1;
}


int dicSearchString( const unsigned char *name )
{
	if( FIRST_FLAG )
	{
		FIRST_FLAG =0;
		memset( (void*)Dic , 0 , MAX_DIC_SIZE );
		trieInit();
	}
	TrieNode *node = trieSearch( name );
	if( node == NULL )
	{
		return -1;
	}
	else
	{
		return node->key;
	}
	
}

int dicInsert( const unsigned char *name )
{	
	if( FIRST_FLAG )
	{
		FIRST_FLAG =0;
		memset((void*)Dic , 0 , MAX_DIC_SIZE );
		trieInit();
	}
	if( Dic_Size < MAX_DIC_SIZE )
	{
		TrieNode *node = trieInsert( name );
		if( node == NULL )
		{
			return -1;
		}
		else
		{
			Dic[ Dic_Size ] = ( unsigned char* )malloc( strlen( name ) + 1 );
			if ( Dic[ Dic_Size ] == NULL )
			{
				
				fprintf( stderr , "error: cannot insert to dictionary\n" );
				return -1;
			}
			strcpy( Dic[ Dic_Size ] , name );
			Dic_Size++;
			return node->key;
		}
	}
	else
	{
		return -1;
	}
}

int dicToFile( const FILE* file )
{
	if( FIRST_FLAG )
	{
		FIRST_FLAG =0;
		memset( (void*)Dic , 0 , MAX_DIC_SIZE );
		trieInit();
	}
	
	return trieToFile( file );
}

unsigned char* dicSearchCode( const unsigned short i )
{	
	if( FIRST_FLAG )
	{
		FIRST_FLAG =0;
		memset( (void*)Dic , 0 , MAX_DIC_SIZE );
		trieInit();
	}
	if( i-256 >= 0 && i-256 < MAX_DIC_SIZE )
	{
		return Dic[ i - 256 ];
	}
	else
	{
		return NULL;
	}
}


size_t dicSize()
{
	if( FIRST_FLAG )
	{
		FIRST_FLAG =0;
		memset( (void*)Dic , 0 , MAX_DIC_SIZE );
		trieInit();
	}
	return TRIE_TREE->size;
}
