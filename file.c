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
static FileMap *file_to_block;
static int file_count;

/*
** PUBLIC GLOBAL VARIABLES
*/

/*
** PRIVATE FUNCTIONS
*/

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
    // TODO allocate blocks and save ids

    // alloc block to store i-node
    int file_block = alloc_block();
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

/**
** Name:  ?
**
** ?
**
** @param ?    ?
**
** @return ?
*/
