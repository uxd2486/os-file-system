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
int create_file( int id ){
    File *file; // TODO malloc properly
    file->id = id;
    for( int i = 0; i < NUM_BLOCKS; i++ ){
        // get a block and save its id in the blocks list
    }
    // set the indirect block here

    // add to file_to_block

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
