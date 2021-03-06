/**
** @file block.c
**
** @author Utkarsh Dayal CSCI-452 class of 20205
**
** File to handle block allocation via bitmap. This also calls
** functions in ahci.h to read/write sectors from the disk.
*/

#define	SP_KERNEL_SRC

#include "common.h"
#include "kmem.h"
#include "block.h"
#include "ahci.h"

/*
** PRIVATE DEFINITIONS
*/
// size of file inode in bytes
#define FILE_INODE_SIZE 12

/*
** PRIVATE DATA TYPES
*/

/*
** PRIVATE GLOBAL VARIABLES
*/

// the bit-map, used to keep track of free and allocated blocks
uint32_t *bit_map;

// list of block data structs
block_t *block_list;

// number of blocks
int block_count;

/*
** PUBLIC GLOBAL VARIABLES
*/

/*
** PRIVATE FUNCTIONS
*/

/**
** Name:  is_allocated
**
** Given an id, returns true if the block is allocated, false if not
**
** @param index   The id of the block to be checked
**
** @return  1 if allocated, 0 if free 
*/
int is_allocated( int index ){
    return (bit_map[index / 32] & (1 << (index % 32))) != 0;
}

/**
** Name:  alloc_block
**
** Allocates a single block in the bit-map
**
** @param index  The id of the block to be allocated
*/
void alloc_block( int index ){
    bit_map[index / 32] |= 1 << (index % 32);
}

/*
** PUBLIC FUNCTIONS
*/

/**
** Name:  _blk_init
**
** Initializes all the blocks in the disk
**
*/
void _blk_init( void ){
    
    // get the hdd devices
    hddDeviceList_t list = _get_device_list();

    // go through devices and count the sectors
    int sector_count = 0;
    for ( int i = 0; i < list.count; i++ ){
        hddDevice_t device = list.devices[i];
	sector_count += device.sector_count;
    }

    // calculate number of blocks that can be made
    block_count = sector_count / NUM_SECTORS;

    // allocate memory to store block info
    int block_mem = block_count * sizeof( block_t );
    int num_pages = block_mem / PAGE_SIZE;
    block_list = ( block_t * ) _km_page_alloc( num_pages );

    // assign sectors to blocks
    int count = 0;
    // iterate through the devices
    for ( int i = 0; i < list.count; i++ ){
        hddDevice_t device = list.devices[i];

	// iterate through the sectors in each device
        for ( int j = 0; j < device.sector_count; j += NUM_SECTORS ){
	    // create the block
	    block_t *block = ( block_t * ) _km_slice_alloc();
	    block->id = count;
	    block->device = (uint32_t) i;
	    block->startl = (uint32_t) (j & 0xff);
	    block->starth = (uint32_t) (j >> 8);

	    // put the block in the list
	    block_list[count] = *block;
	    count++;

	    // free the memory used
	    _km_slice_free( block );
	}
    }

    // calculate size of the bitmap
    int map_mem = ( ( block_count / 32 ) + ( ( block_count % 32 ) != 0 ) * 4);
    // calculate number of pages for the bitmap
    int map_pages = ( map_mem / PAGE_SIZE ) + ( ( map_mem % PAGE_SIZE ) != 0);
    // allocate the bitmap
    bit_map = ( uint32_t * ) _km_page_alloc( map_pages );
}
    

/**
** Name:  _blk_free
**
** Frees a single disk block using the bit-map, given the id
**
** @param index    The id of the block to be freed
**
*/
void _blk_free( int index ){
    bit_map[index / 32] &= ~(1 << (index % 32));
}

/**
** Name:  _blk_alloc
**
** Allocates a given number of continous disk blocks using the bit-map
**
** @param num    The number of disk blocks to be allocated
**
** @return id of the first disk block 
*/
int _blk_alloc( int num ){
    
    // index of block being processed
    int idx = 0;
    // number of consecutive free blocks found
    int free_blocks = 0; 

    // go through all the blocks
    while ( idx < block_count ){
    
        // check if block is allocated
        if ( !is_allocated( idx ) ){
	    free_blocks++;

	    // check if we've found enough consecutive free blocks
	    if ( free_blocks == block_count ){

	        // index at which the consecutive blocks start
	        int start_index = idx - block_count + 1;
		// allocate the blocks
		int count = 0;
		while( count < block_count ){
		    alloc_block( start_index + count );
		    count++;
		}
		// return the block at which the blocks start
		return start_index;
	    }
	} else {
	    // consecutive free blocks go down to zero
	    free_blocks = 0;
	}
	idx++;
    }

    // Out of blocks?????????
    __cio_printf( "Ran out of blocks?????\n");
    return E_FAILURE;
}

/**
** Name:  _blk_save_file
**
** Given the i-node of a file and the block it, saves it to the disk
**
** @param id          The id of the i-node block of the file
** @param file        Pointer where i-node needs to is stored
**
** @return 0 if successful, -1 if not
*/
int _blk_save_file( int id, file_t *file ){
    
    // get the block
    block_t block = block_list[id];

    // get the device
    hddDeviceList_t list = _get_device_list();
    hddDevice_t device = list.devices[block.device];

    // write it to the disk
    bool_t result = _write_disk( device, block.startl, block.starth, NUM_SECTORS, (uint16_t *) file );

    // check result of write
    if ( !result ){
        __cio_printf( "Unable to write to disk\n");
        return E_FAILURE;
    }

    return SUCCESS; 
}

/**
** Name:  _blk_load_file
**
** Given the block of the i-node, loads the file i-node from the disk
**
** @param id          The id of the i-node block of the file
** @param file        Pointer where i-node needs to be stored
**
** @return 0 if successful, -1 if not
*/
int _blk_load_file( int id, file_t *file ){
    
    // get the block
    block_t block = block_list[id];
    
    // get the device
    hddDeviceList_t list = _get_device_list();
    hddDevice_t device = list.devices[block.device];

    uint16_t *buf = _km_slice_alloc();

    // read it from the disk
    bool_t result = _read_disk( device, block.startl, block.starth,\
    NUM_SECTORS, buf );

    // check result of read
    if ( !result ){
        __cio_printf( "Unable to read from disk\n");
        return E_FAILURE;
    }

    file = ( file_t *) buf;

    return SUCCESS; 
}

/**
** Name:  _blk_load_filecontents
**
** Given the contents of a file and the starting block, loads the file 
** contents from the disk
**
** @param id          The id of the starting block of the file
** @param contents    Buffer where contents are to be written
** @param num_blocks  The number of blocks to be read
**
** @return 0 if successful, -1 if not
*/
int _blk_load_filecontents( int id, char *buf, int num_blocks ){
    
    // for passing in to the disk driver
    char *buf_ptr = buf;

    // go through each block
    for( int i = id; i < id + num_blocks; i++ ){
        
	// get the block
        block_t block = block_list[i];
        
	// get the device
        hddDeviceList_t list = _get_device_list();
        hddDevice_t device = list.devices[block.device];

	// read block from the disk
        bool_t result = _read_disk( device, block.startl, block.starth,\
        NUM_SECTORS, ( uint16_t *) buf_ptr );
        
	// check result of read
        if ( !result ){
            __cio_printf( "Unable to read from disk\n");
            return E_FAILURE;
        }
	
	// this now points to the next part of buffer to be filled
	buf_ptr += BLOCK_SIZE;
    }

    return SUCCESS;
}

/**
** Name:  _blk_save_filecontents
**
** Given the contents of a file and the starting block, stores the file 
** contents onto the disk
**
** @param id          The id of the starting block of the file
** @param contents    Buffer containing contents of the file
** @param num_blocks  The number of blocks to be written
**
** @return 0 if successful, -1 if not
*/
int _blk_save_filecontents( int id, char *contents, int num_blocks ){
    
    // for passing in to the disk driver
    char *buf_ptr = contents;

    // go through each block
    for( int i = id; i < id + num_blocks; i++ ){
        
	// get the block
        block_t block = block_list[i];
        
	// get the device
        hddDeviceList_t list = _get_device_list();
        hddDevice_t device = list.devices[block.device];

	// write block to the disk
        bool_t result = _write_disk( device, block.startl, block.starth,\
        NUM_SECTORS, ( uint16_t *) buf_ptr );
        
	// check result of write
        if ( !result ){
            __cio_printf( "Unable to write to disk\n");
            return E_FAILURE;
        }
	
	// this now points to the next part of buffer to be written
	buf_ptr += BLOCK_SIZE;
    }

    return SUCCESS;
}


