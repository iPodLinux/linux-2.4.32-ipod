/*
 * YAFFS: Yet another FFS. A NAND-flash specific file system. 
 * yaffs_guts.h: Configuration etc for yaffs_guts
 *
 * Copyright (C) 2002 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1 as
 * published by the Free Software Foundation.
 *
 *
 * Note: Only YAFFS headers are LGPL, YAFFS C code is covered by GPL.
 *
 * $Id: yaffs_guts.h,v 1.1 2005/08/11 12:53:51 claudio Exp $
 */

#ifndef __YAFFS_GUTS_H__
#define __YAFFS_GUTS_H__

#include "yportenv.h"

#define YAFFS_OK	1
#define YAFFS_FAIL  0

// Give us a Y=0x59, 
// Give us an A=0x41, 
// Give us an FF=0xFF 
// Give us an S=0x53
// And what have we got... 
#define YAFFS_MAGIC					0x5941FF53

#define YAFFS_NTNODES_LEVEL0	  	16
#define YAFFS_TNODES_LEVEL0_BITS	4
#define YAFFS_TNODES_LEVEL0_MASK	0xf

#define YAFFS_NTNODES_INTERNAL 		(YAFFS_NTNODES_LEVEL0 / 2)
#define YAFFS_TNODES_INTERNAL_BITS 	(YAFFS_TNODES_LEVEL0_BITS - 1)
#define YAFFS_TNODES_INTERNAL_MASK	0x7
#define YAFFS_TNODES_MAX_LEVEL		6
		
#define YAFFS_BYTES_PER_SPARE		16

#define YAFFS_BYTES_PER_CHUNK		512
//#define YAFFS_CHUNK_SIZE_SHIFT		9


#define YAFFS_CHUNKS_PER_BLOCK		32
#define YAFFS_BYTES_PER_BLOCK		(YAFFS_CHUNKS_PER_BLOCK*YAFFS_BYTES_PER_CHUNK)

#define YAFFS_MAX_CHUNK_ID			0x000FFFFF

#define YAFFS_UNUSED_OBJECT_ID		0x0003FFFF

#define YAFFS_ALLOCATION_NOBJECTS	100
#define YAFFS_ALLOCATION_NTNODES	100
#define YAFFS_ALLOCATION_NLINKS		100

#define YAFFS_NOBJECT_BUCKETS		256


#define YAFFS_OBJECT_SPACE			0x40000
#define YAFFS_MAX_NAME_LENGTH		255
#define YAFFS_SHORT_NAME_LENGTH		15

#define YAFFS_MAX_ALIAS_LENGTH		159

#define YAFFS_OBJECTID_ROOT			1
#define YAFFS_OBJECTID_LOSTNFOUND	2
#define YAFFS_OBJECTID_UNLINKED		3

#define YAFFS_MAX_SHORT_OP_CACHES	20


// ChunkCache is used for short read/write operations.
typedef struct
{
	struct yaffs_ObjectStruct *object;
	int chunkId;
	int lastUse;
	int dirty;	
	int nBytes;	// Only valid if the cache is dirty
	__u8 data[YAFFS_BYTES_PER_CHUNK];
} yaffs_ChunkCache;

// Tags structures in RAM
// NB This uses bitfield. Bitfields should not straddle a u32 boundary otherwise
// the structure size will get blown out.

typedef struct
{
    unsigned chunkId:20;
    unsigned serialNumber:2;
    unsigned byteCount:10;
    unsigned objectId:18;
    unsigned ecc:12;
    unsigned unusedStuff:2;
} yaffs_Tags;

typedef union
{
    yaffs_Tags asTags;
    __u8       asBytes[8];
} yaffs_TagsUnion;


// Spare structure
typedef struct
{
    __u8  tagByte0;
    __u8  tagByte1;
    __u8  tagByte2;
    __u8  tagByte3;
    __u8  pageStatus; 	// set to 0 to delete the chunk
    __u8  blockStatus;
    __u8  tagByte4;
    __u8  tagByte5;
    __u8  ecc1[3];
    __u8  tagByte6;
    __u8  tagByte7;
    __u8  ecc2[3];
} yaffs_Spare;

//Special structure for passing through to mtd
struct yaffs_NANDSpare {
	yaffs_Spare	spare;
	int		eccres1;
	int		eccres2;
};

// Block data in RAM

typedef enum {
	YAFFS_BLOCK_STATE_UNKNOWN	= 0,
	YAFFS_BLOCK_STATE_SCANNING,		// Used while the block is being scanned.
									// NB Don't erase blocks while they're being scanned
	
	YAFFS_BLOCK_STATE_EMPTY,		// This block is empty
	
	YAFFS_BLOCK_STATE_ALLOCATING,	// This block is partially allocated. 
									// This is the one currently being used for page
									// allocation. Should never be more than one of these
							

	YAFFS_BLOCK_STATE_FULL,			// All the pages in this block have been allocated.
									// At least one page holds valid data.
							 
	YAFFS_BLOCK_STATE_DIRTY,		// All pages have been allocated and deleted. 
									// Erase me, reuse me.
							
	YAFFS_BLOCK_STATE_DEAD 			// This block has failed and is not in use

} yaffs_BlockState;




typedef struct
{
#ifndef CONFIG_YAFFS_NO_YAFFS2
	__u32 sequenceNumber;	// block sequence number for yaffs2
#endif
	int   softDeletions:12; // number of soft deleted pages
    int   pagesInUse:12;	// number of pages in use
    __u32 blockState:4; 	// One of the above block states
    __u32 needsRetiring:1;	// Data has failed on this block, need to get valid data off
    						// and retire the block.
} yaffs_BlockInfo;


//////////////////// Object structure ///////////////////////////
// This is the object structure as stored on NAND

typedef enum
{
	YAFFS_OBJECT_TYPE_UNKNOWN,
	YAFFS_OBJECT_TYPE_FILE,
	YAFFS_OBJECT_TYPE_SYMLINK,
	YAFFS_OBJECT_TYPE_DIRECTORY,
	YAFFS_OBJECT_TYPE_HARDLINK,
	YAFFS_OBJECT_TYPE_SPECIAL
} yaffs_ObjectType;

typedef struct
{
	yaffs_ObjectType type;

	// Apply to everything	
	int   parentObjectId;
	__u16 sum__NoLongerUsed;	// checksum of name. Calc this off the name to prevent inconsistencies
	char  name[YAFFS_MAX_NAME_LENGTH + 1];

	// Thes following apply to directories, files, symlinks - not hard links
	__u32 st_mode;  // protection

#ifdef CONFIG_YAFFS_WINCE
	__u32 notForWinCE[5];
#else
	__u32 st_uid;   // user ID of owner
	__u32 st_gid;    // group ID of owner 
	__u32 st_atim; // time of last access
	__u32 st_mtim; // time of last modification
	__u32 st_ctim; // time of last change
#endif

	// File size  applies to files only
	int fileSize; 
		
	// Equivalent object id applies to hard links only.
	int  equivalentObjectId;
	
	// Alias is for symlinks only.
	char alias[YAFFS_MAX_ALIAS_LENGTH + 1];
	
	__u32 st_rdev;  // device stuff for block and char devices (maj/min)
	
#ifdef CONFIG_YAFFS_WINCE
	__u32 win_ctime[2];
	__u32 win_atime[2];
	__u32 win_mtime[2];
	__u32 roomToGrow[6];
#else
	__u32 roomToGrow[12];
#endif
	
} yaffs_ObjectHeader;



////////////////////  Tnode ///////////////////////////

union yaffs_Tnode_union
{
#ifdef CONFIG_YAFFS_TNODE_LIST_DEBUG
	union yaffs_Tnode_union *internal[YAFFS_NTNODES_INTERNAL+1];
#else
	union yaffs_Tnode_union *internal[YAFFS_NTNODES_INTERNAL];
#endif
	__u16 level0[YAFFS_NTNODES_LEVEL0];
	
};

typedef union yaffs_Tnode_union yaffs_Tnode;

struct yaffs_TnodeList_struct
{
	struct yaffs_TnodeList_struct *next;
	yaffs_Tnode *tnodes;
};

typedef struct yaffs_TnodeList_struct yaffs_TnodeList;



///////////////////  Object ////////////////////////////////
// An object can be one of:
// - a directory (no data, has children links
// - a regular file (data.... not prunes :->).
// - a symlink [symbolic link] (the alias).
// - a hard link


typedef struct 
{
	__u32 fileSize;
	__u32 scannedFileSize;
	int   topLevel;
	yaffs_Tnode *top;
} yaffs_FileStructure;

typedef struct
{
	struct list_head children; // list of child links
} yaffs_DirectoryStructure;

typedef struct
{
	char *alias;
} yaffs_SymLinkStructure;

typedef struct
{
	struct yaffs_ObjectStruct *equivalentObject;
	__u32	equivalentObjectId;
} yaffs_HardLinkStructure;

typedef union
{
	yaffs_FileStructure fileVariant;
	yaffs_DirectoryStructure directoryVariant;
	yaffs_SymLinkStructure symLinkVariant;
	yaffs_HardLinkStructure hardLinkVariant;
} yaffs_ObjectVariant;


struct  yaffs_ObjectStruct
{
	__u8 deleted: 1;		// This should only apply to unlinked files.
	__u8 softDeleted: 1;	// it has also been soft deleted
	__u8 unlinked: 1;		// An unlinked file. The file should be in the unlinked pseudo directory.
	__u8 fake:1;			// A fake object has no presence on NAND.
	__u8 renameAllowed:1;
	__u8 unlinkAllowed:1;
	__u8 dirty:1;			// the object needs to be written to flash
	__u8 valid:1;			// When the file system is being loaded up, this 
							// object might be created before the data
							// is available (ie. file data records appear before the header).
	__u8 serial;			// serial number of chunk in NAND. Store here so we don't have to
							// read back the old one to update.
	__u16 sum;				// sum of the name to speed searching
	
	struct yaffs_DeviceStruct *myDev; // The device I'm on
	
								
	struct list_head hashLink;	// list of objects in this hash bucket
							

	struct list_head hardLinks; // all the equivalent hard linked objects
								// live on this list
	// directory structure stuff
	struct yaffs_ObjectStruct  *parent;	//my parent directory
	struct list_head siblings;	// siblings in a directory
								// also used for linking up the free list
		
	// Where's my data in NAND?
	int chunkId;		// where it lives

	int nDataChunks;	
	
	__u32 objectId;		// the object id value
	
	
	__u32 st_mode;  	// protection

#ifdef CONFIG_YAFFS_SHORT_NAMES_IN_RAM
	char shortName[YAFFS_SHORT_NAME_LENGTH+1];
#endif

#ifndef __KERNEL__
	__u32 inUse;
#endif

#ifdef CONFIG_YAFFS_WINCE
	__u32 win_ctime[2];
	__u32 win_mtime[2];
	__u32 win_atime[2];
#else
	__u32 st_uid;   	// user ID of owner
	__u32 st_gid;    	// group ID of owner 
	__u32 st_atim; 	// time of last access
	__u32 st_mtim; 	// time of last modification
	__u32 st_ctim; 	// time of last change
#endif

	__u32 st_rdev; 	    // device stuff for block and char devices



#ifdef __KERNEL__
	struct inode *myInode;
	__u8  deferedFree;   // YAFFS has removed the object from NAND, but it is being kept
			     // Alive until the inode is cleared to prevent inode inconsistencies.
#endif


	
	yaffs_ObjectType variantType;
	
	yaffs_ObjectVariant variant;
	
};



typedef struct yaffs_ObjectStruct yaffs_Object;


struct yaffs_ObjectList_struct
{
	yaffs_Object *objects;
	struct yaffs_ObjectList_struct *next;
};

typedef struct yaffs_ObjectList_struct yaffs_ObjectList;

typedef struct
{
	struct list_head list;
	int count;
} yaffs_ObjectBucket;


//////////////////// Device ////////////////////////////////

struct yaffs_DeviceStruct
{
	// Entry parameters set up way early. Yaffs sets up the rest.
	int   nBytesPerChunk; 	 // Should be a power of 2 >= 512
	int   nChunksPerBlock;	 // does not need to be a power of 2
	int   startBlock;		 // Start block we're allowed to use
	int   endBlock;			 // End block we're allowed to use
	int   nReservedBlocks;	 // We want this tuneable so that we can reduce
							 // reserved blocks on NOR and RAM.
	
	int   useNANDECC;		// Flag to decide whether or not to use NANDECC
	int   nShortOpCaches;	// If <= 0, then short op caching is disabled, else
							// the number of short op caches (don't use too many).
	
	
	void *genericDevice; // Pointer to device context
						 // On an mtd this holds the mtd pointer.

	// NAND access functions (Must be set before calling YAFFS)
	
	int (*writeChunkToNAND)(struct yaffs_DeviceStruct *dev,int chunkInNAND, const __u8 *data, yaffs_Spare *spare);
	int (*readChunkFromNAND)(struct yaffs_DeviceStruct *dev,int chunkInNAND, __u8 *data, yaffs_Spare *spare);
	int (*eraseBlockInNAND)(struct yaffs_DeviceStruct *dev,int blockInNAND);	
	int (*initialiseNAND)(struct yaffs_DeviceStruct *dev);

	// Runtime parameters. Set up by YAFFS.
	int   internalStartBlock;		 // Internal version of startBlock
	int   internalEndBlock;			 // End block we're allowed to use
	int   blockOffset;
	int   chunkOffset;
	
	
	__u16 chunkGroupBits; // 0 for devices <= 32MB. else log2(nchunks) - 16
	__u16 chunkGroupSize; // == 2^^chunkGroupBits
	
#ifdef __KERNEL__

	struct semaphore sem;// Semaphore for waiting on erasure.
	struct semaphore grossLock; // Gross locking semaphore

#endif	
#ifdef __KERNEL__
	void (*putSuperFunc)(struct super_block *sb);
#endif

	int isMounted;
	
	// Block Info
	yaffs_BlockInfo *blockInfo;
	__u8 *chunkBits;   // bitmap of chunks in use
	int   chunkBitmapStride; // Number of bytes of chunkBits per block. 
							 //	Must be consistent with nChunksPerBlock.


	int   nErasedBlocks;
	int   allocationBlock;			// Current block being allocated off
	__u32 allocationPage;
	int   allocationBlockFinder;	// Used to search for next allocation block
	
	// Runtime state
	int   nTnodesCreated;	
	yaffs_Tnode *freeTnodes;
	int  nFreeTnodes;
	yaffs_TnodeList *allocatedTnodeList;


	int   nObjectsCreated;
	yaffs_Object *freeObjects;
	int   nFreeObjects;

	yaffs_ObjectList *allocatedObjectList;

	yaffs_ObjectBucket objectBucket[YAFFS_NOBJECT_BUCKETS];

	int	  nFreeChunks;
		
	int   currentDirtyChecker;	// Used to find current dirtiest block
	
	
	// Operations since mount
	int nPageWrites;
	int nPageReads;
	int nBlockErasures;
	int nGCCopies;
	int garbageCollections;
	int passiveGarbageCollections;
	int nRetriedWrites;
	int nRetiredBlocks;
	int eccFixed;
	int eccUnfixed;
	int tagsEccFixed;
	int tagsEccUnfixed;
	int nDeletions;
	int nUnmarkedDeletions;
	
	yaffs_Object *rootDir;
	yaffs_Object *lostNFoundDir;
	
	// Buffer areas for storing data to recover from write failures
//	__u8 		bufferedData[YAFFS_CHUNKS_PER_BLOCK][YAFFS_BYTES_PER_CHUNK];
//	yaffs_Spare bufferedSpare[YAFFS_CHUNKS_PER_BLOCK];
	int bufferedBlock;	// Which block is buffered here?
	int doingBufferedBlockRewrite;

	yaffs_ChunkCache *srCache;
	int srLastUse;

	int cacheHits;

	// Stuff for background deletion and unlinked files.
	yaffs_Object *unlinkedDir;		// Directory where unlinked and deleted files live.
	yaffs_Object *unlinkedDeletion;	// Current file being background deleted.
	int nDeletedFiles;				// Count of files awaiting deletion;
	int nUnlinkedFiles;				// Count of unlinked files. 
	int nBackgroundDeletions;			// Count of background deletions.	
	
	__u8 *localBuffer;
	
};

typedef struct yaffs_DeviceStruct yaffs_Device;



//////////// YAFFS Functions //////////////////

int yaffs_GutsInitialise(yaffs_Device *dev);
void yaffs_Deinitialise(yaffs_Device *dev);

int yaffs_GetNumberOfFreeChunks(yaffs_Device *dev);


// Rename
int yaffs_RenameObject(yaffs_Object *oldDir, const char *oldName, yaffs_Object *newDir, const char *newName);

// generic Object functions
int yaffs_Unlink(yaffs_Object *dir, const char *name);
int yaffs_DeleteFile(yaffs_Object *obj);

// Object access functions.
int yaffs_GetObjectName(yaffs_Object *obj,char *name,int buffSize);
int yaffs_GetObjectFileLength(yaffs_Object *obj);
int yaffs_GetObjectInode(yaffs_Object *obj);
unsigned yaffs_GetObjectType(yaffs_Object *obj);
int yaffs_GetObjectLinkCount(yaffs_Object *obj);

// Change inode attributes
int yaffs_SetAttributes(yaffs_Object *obj, struct iattr *attr);
int yaffs_GetAttributes(yaffs_Object *obj, struct iattr *attr);

// File operations
int yaffs_ReadDataFromFile(yaffs_Object *obj, __u8 *buffer, __u32 offset, int nBytes);
int yaffs_WriteDataToFile(yaffs_Object *obj, const __u8 *buffer, __u32 offset, int nBytes);
int yaffs_ResizeFile(yaffs_Object *obj, int newSize);

yaffs_Object *yaffs_MknodFile(yaffs_Object *parent,const char *name, __u32 mode, __u32 uid, __u32 gid);
int yaffs_FlushFile(yaffs_Object *obj,int updateTime);


// Directory operations
yaffs_Object *yaffs_MknodDirectory(yaffs_Object *parent,const char *name, __u32 mode, __u32 uid, __u32 gid);
yaffs_Object *yaffs_FindObjectByName(yaffs_Object *theDir,const char *name);
int yaffs_ApplyToDirectoryChildren(yaffs_Object *theDir,int (*fn)(yaffs_Object *));

yaffs_Object *yaffs_FindObjectByNumber(yaffs_Device *dev,__u32 number);

// Link operations
yaffs_Object *yaffs_Link(yaffs_Object *parent, const char *name, yaffs_Object *equivalentObject);

yaffs_Object *yaffs_GetEquivalentObject(yaffs_Object *obj);

// Symlink operations
yaffs_Object *yaffs_MknodSymLink(yaffs_Object *parent, const char *name, __u32 mode,  __u32 uid, __u32 gid, const char *alias);
char *yaffs_GetSymlinkAlias(yaffs_Object *obj);

// Special inodes (fifos, sockets and devices)
yaffs_Object *yaffs_MknodSpecial(yaffs_Object *parent,const char *name, __u32 mode, __u32 uid, __u32 gid,__u32 rdev);


// Special directories
yaffs_Object *yaffs_Root(yaffs_Device *dev);
yaffs_Object *yaffs_LostNFound(yaffs_Device *dev);

#ifdef CONFIG_YAFFS_WINCE
// CONFIG_YAFFS_WINCE special stuff
void  yfsd_WinFileTimeNow(__u32 target[2]);
#endif

#ifdef __KERNEL__
void yaffs_HandleDeferedFree(yaffs_Object *obj);
#endif

// Debug dump 
int yaffs_DumpObject(yaffs_Object *obj);


void yaffs_GutsTest(yaffs_Device *dev);


#endif
