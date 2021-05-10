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
static file_map *file_map;
static map_count;
static File *open_files;
static int open_files_count;

/*
** PUBLIC GLOBAL VARIABLES
*/

/*
** PRIVATE FUNCTIONS
*/
char *get_file_name( int file_id ){
    for ( int i = 0; i < map_count; i++ ){
        if ( file_map[i].id == file_id ){
            return file_map.name;
	}
    }
    return NULL;
}

/*
** PUBLIC FUNCTIONS
*/


void _fs_init(){
    // start at 0
    file_id_assigner = 0;

    // allocate stuff, set counts to zero
    file_map = ( file_map * ) _km_page_alloc( 2 );
    map_count = 0;

    open_files = ( File * ) _km_page_alloc( 4 );
    open_files_count = 0;
}

int create_file( char *filename ){

    // check if name is correct length
    if ( strlen( filename) >= 16 ){
        return ERROR;
    }
    
    // check if file already exists
    for ( int i = 0; i < map_count; i++ ){
        if ( strcmp( file_map[i].name, filename ) == 0 ){
            return ERROR;
	}
    }

    // make file in storage first
    int file_id = create_file( file_id_assigner );
    if ( file_id < 0 ){
        return ERROR;
    }
    file_id_assigner++;

    // add file to filename list
    file_map new_map = { filename, id };
    file_map[file_count] = new_map;
    file_count++;

    return SUCCESS;
}

int delete_file( char *filename ){
    
    // find the file first
    int id = -1;
    int index = -1;
    for ( int i = 0; i < map_count; i++ ){
        if ( strcmp( file_map[i].name, filename ) == 0 ){
            id = file_map[i].id;
	    index = i;
	    break;
	}
    }

    if ( id == -1 || index == -1){
        return ERROR; // file doesn't exist
    }
    
    // delete file from disk
    int result = delete_file( id );
    if ( result < 0 ){
        return ERROR; // unable to delete file
    }

    // delete file from the map
    for ( int i = index; i < map_count; i++ ){
        if ( i + 1 < map_count ){
             file_map temp = file_map[i+1];
	     file_map[i].id = temp.id;
	     strcpy( file_map[i].name, temp.name );
	}
    }
    map_count--;

    return SUCCESS;
}

int open_file( char *filename ){
    
    // find the file first
    int id = -1;
    for ( int i = 0; i < map_count; i++ ){
        if ( strcmp( file_map[i].name, filename ) == 0 ){
            id = file_map[i].id;
	    break;
	}
    }

    if ( id == -1){
        return ERROR; // file doesn't exist
    }

    // load the file
    File *file = open_file( id );
    if ( file == NULL ){
        return ERROR;
    }
    
    // Add it to the open files list
    open_files[open_files_count] = *file;
    open_files_count++;

    return SUCCESS;
}

int close_file( char *filename ){
    
    // find the file first
    int id = -1;
    int index = -1;
    for ( int i = 0; i < map_count; i++ ){
        if ( strcmp( file_map[i].name, filename ) == 0 ){
            id = file_map[i].id;
	    index = i;
	    break;
	}
    }

    if ( id == -1 || index == -1){
        return ERROR; // file doesn't exist
    }

    // find the file in the open file list
    File *file = NULL;
    for ( int i = 0; i < open_files_count; i++ ){
        if ( open_files[i].id == id ){
            file = &open_files[i];
	}
    }

    if ( file == NULL ){
        return ERROR; // file isn't in the open list
    }

    // close the file
    int result = close_file( file );
    if ( result < 0 ){
        return ERROR; // something went wrong
    }

    // remove the file from the open list

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
