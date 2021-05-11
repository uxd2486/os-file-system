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

/*
** PRIVATE DEFINITIONS
*/

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/
static file_map *file_to_block;
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
    file_to_block = ( FileMap * ) _km_alloc_page( 2 );
    file_count = 0;

    // call the block init
    _blk_init();
}


int create_file( int id ){

    // initialize file
    File *file = ( File * ) _km_slice_alloc( 1 );
    file->id = id;
    file->bytes = 0;
    file->block = alloc_blocks( NUM_BLOCKS );

    // alloc block to store i-node
    int file_block = alloc_blocks( 1 );
    file_to_block[file_count] = file_block;
    file_count++;

    // save i-node to disk
    int result = save_file( file, file_block );
    if ( result < 0 ){
        return E_FAILURE; // something went wrong
    }

    // free memory
    _km_slice_free( file );

    return SUCCESS;
}

File *open_file( int id ){
    
    // get the block the i-node is in
    int block_id = get_block_id( id );
    if ( block_id == -1 ){
        return E_FAILURE; // file i-node not found
    }

    File *file = NULL;
    // TODO load the file i-node from disk

    return file;
}

int delete_file( int id ){
    
    // get the block that the i-node is in
    int block_id = get_block_id( id );
    if ( block_id == -1 ){
        return E_FAILURE; // file i-node not found
    }

    // TODO load the file i-node from disk

    // free file blocks
    int start_block = file->block;
    for( int i = 0; i < NUM_BLOCKS; i++ ){
        free_block( start_block + i );
    }

    // free the file i-node block
    free_block( block_id );

    return SUCCESS;
}

int close_file( File *file ){
    
    int block_id = get_block_id( file->id );
    if ( block_id == -1 ){
        return E_FAILURE; // file i-node not found
    }

    // TODO write the file i-node to the disk
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
