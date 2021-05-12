/**
**
** @file file.h
**
** @author Utkarsh Dayal CSCI-452 class of 20205
**
** Functions for reading/writing one file 
*/

#define	SP_KERNEL_SRC

#include "common.h"
#include "kmem.h"

/*
** PRIVATE DEFINITIONS
*/

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/
static filemap_t *file_to_block;
static int file_count;

/*
** PUBLIC GLOBAL VARIABLES
*/

/*
** PRIVATE FUNCTIONS
*/
int get_block_id( int file_id ){
    
    int block_id = -1;
    for ( int i = 0; i < file_count; i++ ){
        if ( file_to_block[i]->file_id == file_id ){
	    block_id = file_to_block[i]->block_id;
	    break;
	}
    }
    return block_id;
}

/*
** PUBLIC FUNCTIONS
*/
void _fl_init(){
    // Initilize the globals
    file_to_block = ( filemap_t * ) _km_alloc_page( 2 );
    file_count = 0;

    // call the block init
    _blk_init();
}


int _fl_create( int id ){

    // initialize file
    file_t *file = ( file_t * ) _km_slice_alloc( 1 );
    file->id = id;
    file->bytes = 0;
    file->block = _blk_alloc( NUM_BLOCKS );

    // alloc block to store i-node
    int file_block = _blk_alloc( 1 );
    file_to_block[file_count] = file_block;
    file_count++;

    // save i-node to disk
    int result = _blk_save_file( file_block, file );
    if ( result < 0 ){
        return E_FAILURE; // something went wrong
    }

    // free memory
    _km_slice_free( file );

    return SUCCESS;
}

file_t *_fl_open( int id ){
    
    // get the block the i-node is in
    int block_id = get_block_id( id );
    if ( block_id == -1 ){
        return E_FAILURE; // file i-node not found
    }

    file_t *file = NULL;
    // load the file i-node from disk
    int result = _blk_load_file( block_id, file );
    if ( result < 0 || file == NULL){
        return E_FAILURE; // something went wrong
    }

    return file;
}

int _fl_delete( int id ){
    
    // get the block that the i-node is in
    int block_id = get_block_id( id );
    if ( block_id == -1 ){
        return E_FAILURE; // file i-node not found
    }

    // load the file i-node from disk
    int result = _blk_load_file( block_id, file );
    if ( result < 0 || file == NULL){
        return E_FAILURE; // something went wrong
    }

    // free file blocks
    int start_block = file->block;
    for( int i = 0; i < NUM_BLOCKS; i++ ){
        _blk_free( start_block + i );
    }

    // free the file i-node block
    _blk_free( block_id );

    // remove the file from the file_to_block
    int index = -1;
    for ( int i = 0; i < file_count; i++ ){
        if ( file_to_block[i]->block_id == block_id ){
	    index = -1;
	    break;
	}
    }
    if ( index == -1 ){
        return E_FAILURE; // something went wrong
    }
    for ( int i = index; i < file_count; i++ ){
        if ( i + 1 < file_count ){
            file_to_block[i]->file_id = file_to_block[i+1]->file_id;
            file_to_block[i]->block_id = file_to_block[i+1]->block_id;
	}
    }

    return SUCCESS;
}

int _fl_close( file_t *file ){
    
    int block_id = get_block_id( file->id );
    if ( block_id == -1 ){
        return E_FAILURE; // file i-node not found
    }

    // write the file i-node to the disk
    int result = _blk_save_file( file_block, file );
    if ( result < 0 ){
        return E_FAILURE; // something went wrong
    }

    // free the file
    _km_slice_free( file );

    return SUCCESS;
}

int _fl_read( file_t *file, char *buf){
    
    // get the number of blocks to read
    int num_blocks = ( file->bytes / BLOCK_SIZE ) + \
    ( ( file->bytes % BLOCK_SIZE ) != 0 );

    // allocate memory in the buffer
    // slices and blocks are the same size so this
    // works really well
    buf = ( char * ) _km_slice_alloc( num_blocks );

    // read file contents from disk
    int result = _blk_load_filecontents( file->block, buf, num_blocks );

    // check result
    if ( result < 0 ){
        return E_FAILURE;
    }

    // nul-terminate buffer so it can be a string
    buf[file->bytes] = '\0';

    // return the number of characters read (includes NULL-terminator)
    return file->bytes + 1;
}

int _fl_write( file_t *file, char *buf, int buf_size ){
    
    // stores the current contents of the file
    char *contents;

    // get the current contents of the file
    int result = _fl_read( file, contents );
    if ( result < 0 ){
        return E_FAILURE; // could not load file contents
    }

    // append the new stuff
    int num_bytes = file->bytes + buf_size;
    int num_blocks = ( num_bytes / BLOCK_SIZE ) + \
    ( ( num_bytes % BLOCK_SIZE ) != 0;
    char *new_contents = _km_slice_alloc( num_blocks );
    new_contents = strcpy( new_contents, contents );
    new_contents = strcat( new_contents, buf );

    // write new contents to the disk
    int result = _blk_save_filecontents( file->block, new_contents, num_blocks );

    // check result
    if ( result < 0 ){
        return E_FAILURE;
    }

    // update file i-node
    file->bytes += buf_size;

    return SUCCESS;
}

/**
** Name:  ?
**
** ?
**
** @param ?    ?
**
** @return ?
*/
