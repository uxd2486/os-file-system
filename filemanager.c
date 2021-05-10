/**
**
** @file filemanager.h
**
** @author Utkarsh Dayal CSCI-452 class of 20205
**
** Functions for handling files and directories
*/

#define	SP_KERNEL_SRC

#include "common.h"
#include "filemanager.h"
#include "ulib.h"
#include "file.h"

/*
** PRIVATE DEFINITIONS
*/

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/
static int file_id_assigner;
static StrIntMap *file_name_map;
static file_count;

/*
** PUBLIC GLOBAL VARIABLES
*/

/*
** PRIVATE FUNCTIONS
*/
char *get_file_name( int file_id ){
    for ( int i = 0; i < file_count; i++ ){
        if ( file_name_map[i].id == file_id ){
            return file_name_map.name;
	}
    }
    return NULL;
}

/*
** PUBLIC FUNCTIONS
*/


void init_filesystem(){
    // start at 0
    file_id_assigner = 0;
}

int create_file( char *filename, int current_dir ){
    
    // check if file already exists
    for ( int i = 0; i < dir->num_files; i++ ){
        char *name = get_file_name( dir->files[i] );
	if ( strcmp( name, filename ) == 0 ){ 
            return ERROR;
	}
    }

    // make file in storage first
    int file_id = create_file( file_id_assigner );
    file_id_assigner++;
    // TODO check return

    // add file to filename list
    StrIntMap new_map = { filename, id };
    file_name_map[file_count] = new_map;
    file_count++;

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
