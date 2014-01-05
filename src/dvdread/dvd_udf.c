/* -*- c-basic-offset: 2; indent-tabs-mode: nil -*- */
/*
 * This code is based on dvdudf by:
 *   Christian Wolff <scarabaeus@convergence.de>.
 *
 * Modifications by:
 *   Billy Biggs <vektor@dumbterm.net>.
 *   Bj�rn Englund <d4bjorn@dtek.chalmers.se>.
 *
 * dvdudf: parse and read the UDF volume information of a DVD Video
 * Copyright (C) 1999 Christian Wolff for convergence integrated media
 * GmbH The author can be reached at scarabaeus@convergence.de, the
 * project's page is at http://linuxtv.org/dvd/
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.  Or, point your browser to
 * http://www.gnu.org/copyleft/gpl.html
 */
 
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#ifndef __WIN32__
#include <fnmatch.h>
#endif

#if defined(HAVE_INTTYPES_H)
#include <inttypes.h>
#elif defined(HAVE_STDINT_H)
#include <stdint.h>
#endif

#include "dvd_reader.h"
#include "dvd_udf.h"
#include "dvdread_internal.h"

#include "ifo_types.h"
#include "ifo_read.h"

#ifndef EMEDIUMTYPE
#define EMEDIUMTYPE ENOENT
#endif

#ifndef HAVE_UINTPTR_T
#warning "Assuming that (unsigned long) can hold (void *)"
/*typedef unsigned long uintptr_t;*/
#endif

#define DVD_ALIGN(ptr) (void *)((((uintptr_t)(ptr)) + (DVD_VIDEO_LB_LEN-1)) \
                                / DVD_VIDEO_LB_LEN * DVD_VIDEO_LB_LEN)

typedef struct {
  void *start;
  void *aligned;
} dvdalign_ptrs_t;

typedef struct {
  dvdalign_ptrs_t *ptrs;
  uint32_t ptrs_in_use;
  uint32_t ptrs_max;
} dvdalign_t;

extern void *___GetAlignHandle(dvd_reader_t *device);
#define GetAlignHandle(a) ___GetAlignHandle(a)
extern void ___SetAlignHandle(dvd_reader_t *device, void *align);
#define SetAlignHandle(a,b) ___SetAlignHandle(a,b)
/**
 * Allocates aligned memory (for use with reads from raw/O_DIRECT devices).
 * This memory must be freed with dvdalign_free()
 * The size of the memory that is allocate is num_lbs*2048 bytes.
 * The memory will be suitably aligned for use with
 * block reads from raw/O_DIRECT device.
 * @param num_lbs Number of logical blocks (2048 bytes) to allocate.
 * @return Returns pointer to allocated memory, or NULL on failure
 * This isn't supposed to be fast/efficient, if that is needed
 * this function should be rewritten to use posix_memalign or similar.
 * It's just needed for aligning memory for small block reads from
 * raw/O_DIRECT devices. 
 * We assume that 2048 is enough alignment for all systems at the moment.
 * Not thread safe. Only use this from one thread.
 * Depends on sizeof(unsigned long) being at least as large as sizeof(void *)
 */
#define dvdalign_lbmalloc(a,b) ___dvdalign_lbmalloc(a,b)
static void *___dvdalign_lbmalloc(dvd_reader_t *device, uint32_t num_lbs)
{
  void *m;
  int n;
  dvdalign_t *a;
  
  m = malloc((num_lbs+1)*DVD_VIDEO_LB_LEN);
  if(m == NULL) {
    return m;
  }
  a = (dvdalign_t *)GetAlignHandle(device);
  if(a == NULL) {
    a = malloc(sizeof(dvdalign_t));
    if(a == NULL) {
      return a;
    }
    a->ptrs = NULL;
    a->ptrs_in_use = 0;
    a->ptrs_max = 0;
    SetAlignHandle(device, (void *)a);
  }
  
  if(a->ptrs_in_use >= a->ptrs_max) {
    a->ptrs = realloc(a->ptrs, (a->ptrs_max+10)*sizeof(dvdalign_ptrs_t));
    if(a->ptrs == NULL) {
      free(m);
      return NULL;
    }
    a->ptrs_max+=10;
    for(n = a->ptrs_in_use; n < a->ptrs_max; n++) {
      a->ptrs[n].start = NULL;
      a->ptrs[n].aligned = NULL;
    }
    n = a->ptrs_in_use;
  } else {
    for(n = 0; n < a->ptrs_max; n++) {
      if(a->ptrs[n].start == NULL) {
        break;
      }
    }
  }

  a->ptrs[n].start = m;
  a->ptrs[n].aligned = DVD_ALIGN(m);

  a->ptrs_in_use++;

  /* If this function starts to be used too much print a warning.
     Either there is a memory leak somewhere or we need to rewrite this to
     a more efficient version.
  */
  if(a->ptrs_in_use > 50) {
    if(dvdread_verbose(device) >= 0) {
      fprintf(stderr, "libdvdread: dvdalign_lbmalloc(), more allocs than supposed: %u\n", a->ptrs_in_use);
    }
  }

  return  a->ptrs[n].aligned;
}

/**
 * Frees memory allocated with dvdalign_lbmemory() 
 * @param ptr Pointer to memory space to free
 * Not thread safe.
 */
#define dvdalign_lbfree(a,b) ___dvdalign_lbfree(a,b)
static void ___dvdalign_lbfree(dvd_reader_t *device, void *ptr)
{
  int n;
  dvdalign_t *a;

  a = (dvdalign_t *)GetAlignHandle(device);
  if(a && a->ptrs) {
    for(n = 0; n < a->ptrs_max; n++) {
      if(a->ptrs[n].aligned == ptr) {
        free(a->ptrs[n].start);
        a->ptrs[n].start = NULL;
        a->ptrs[n].aligned = NULL;
        a->ptrs_in_use--;
        if(a->ptrs_in_use == 0) {
          free(a->ptrs);
          a->ptrs = NULL;
          a->ptrs_max = 0;
          free(a);
          a = NULL;
          SetAlignHandle(device, (void *)a);
        }
        return;
      }
    }
  }
  if(dvdread_verbose(device) >= 0) {
    fprintf(stderr, "libdvdread: dvdalign_lbfree(), error trying to free mem: %08lx (%u)\n", (unsigned long)ptr, a ? a->ptrs_in_use : 0);
  }
}


/* Private but located in/shared with dvd_reader.c */
extern int ___UDFReadBlocksRaw( dvd_reader_t *device, uint32_t lb_number,
                             size_t block_count, unsigned char *data, 
                             int encrypted );
#define UDFReadBlocksRaw(a,b,c,d,e) ___UDFReadBlocksRaw(a,b,c,d,e)

/** @internal
 * Its required to either fail or deliver all the blocks asked for. 
 *
 * @param data Pointer to a buffer where data is returned. This must be large
 *   enough to hold lb_number*2048 bytes.
 *   It must be aligned to system specific (2048) logical blocks size when
 *   reading from raw/O_DIRECT device.
 */
#define DVDReadLBUDF(a,b,c,d,e) ___DVDReadLBUDF(a,b,c,d,e)
static int ___DVDReadLBUDF( dvd_reader_t *device, uint32_t lb_number,
                         size_t block_count, unsigned char *data, 
                         int encrypted )
{
  int ret;
  size_t count = block_count;
  
  while(count > 0) {
    
    ret = UDFReadBlocksRaw(device, lb_number, count, data, encrypted);
        
    if(ret <= 0) {
      /* One of the reads failed or nothing more to read, too bad.
       * We won't even bother returning the reads that went ok. */
      return ret;
    }
    
    count -= (size_t)ret;
    lb_number += (uint32_t)ret;
  }

  return block_count;
}


#ifndef NULL
#define NULL ((void *)0)
#endif

struct Partition {
  int valid;
  char VolumeDesc[128];
  uint16_t Flags;
  uint16_t Number;
  char Contents[32];
  uint32_t AccessType;
  uint32_t Start;
  uint32_t Length;
};

struct AD {
  uint32_t Location;
  uint32_t Length;
  uint8_t  Flags;
  uint16_t Partition;
};

struct extent_ad {
  uint32_t location;
  uint32_t length;
};

struct avdp_t {
  struct extent_ad mvds;
  struct extent_ad rvds;
};

struct pvd_t {
  uint8_t VolumeIdentifier[32];
  uint8_t VolumeSetIdentifier[128];
};

struct lbudf {
  uint32_t lb;
  uint8_t *data;
};

struct icbmap {
  uint32_t lbn;
  struct AD file;
  uint8_t filetype;
};

struct udf_cache {
  int avdp_valid;
  struct avdp_t avdp;
  int pvd_valid;
  struct pvd_t pvd;
  int partition_valid;
  struct Partition partition;
  int rooticb_valid;
  struct AD rooticb;
  int lb_num;
  struct lbudf *lbs;
  int map_num;
  struct icbmap *maps;
};

typedef enum {
  PartitionCache, RootICBCache, LBUDFCache, MapCache, AVDPCache, PVDCache
} UDFCacheType;

extern void *___GetUDFCacheHandle(dvd_reader_t *device);
#define GetUDFCacheHandle(a) ___GetUDFCacheHandle(a)
extern void ___SetUDFCacheHandle(dvd_reader_t *device, void *cache);
#define SetUDFCacheHandle(a,b) ___SetUDFCacheHandle(a,b);


#define FreeUDFCache(a,b) ___FreeUDFCache(a,b)
void ___FreeUDFCache(dvd_reader_t *device, void *cache)
{
  int n;
  
  struct udf_cache *c = (struct udf_cache *)cache;
  if(c == NULL) {
    return;
  }

  for(n = 0; n < c->lb_num; n++) {
    if(c->lbs[n].data) {
      /* free data */
      dvdalign_lbfree(device, c->lbs[n].data);
    }
  }
  c->lb_num = 0;

  if(c->lbs) {
    free(c->lbs);
  }
  if(c->maps) {
    free(c->maps);
  }
  free(c);
}

#define GetUDFCache(a,b,c,d) ___GetUDFCache(a,b,c,d)
static int ___GetUDFCache(dvd_reader_t *device, UDFCacheType type,
                       uint32_t nr, void *data)
{
  int n;
  struct udf_cache *c;

  if(DVDUDFCacheLevel(device, -1) <= 0) {
    return 0;
  }
  
  c = (struct udf_cache *)GetUDFCacheHandle(device);
  
  if(c == NULL) {
    return 0;
  }
  
  switch(type) {
  case AVDPCache:
    if(c->avdp_valid) {
      *(struct avdp_t *)data = c->avdp;
      return 1;
    }    
    break;
  case PVDCache:
    if(c->pvd_valid) {
      *(struct pvd_t *)data = c->pvd;
      return 1;
    }    
    break;
  case PartitionCache:
    if(c->partition_valid) {
      *(struct Partition *)data = c->partition;
      return 1;
    }
    break;
  case RootICBCache:
    if(c->rooticb_valid) {
      *(struct AD *)data = c->rooticb;
      return 1;
    }
    break;
  case LBUDFCache:
    for(n = 0; n < c->lb_num; n++) {
      if(c->lbs[n].lb == nr) {
        *(uint8_t **)data = c->lbs[n].data;
        return 1;
      }
    }
    break;
  case MapCache:
    for(n = 0; n < c->map_num; n++) {
      if(c->maps[n].lbn == nr) {
        *(struct icbmap *)data = c->maps[n];
        return 1;
      }
    }
    break;
  default:
    break;
  }
  
  return 0;
}

#define SetUDFCache(a,b,c,d) ___SetUDFCache(a,b,c,d)
static int ___SetUDFCache(dvd_reader_t *device, UDFCacheType type,
                       uint32_t nr, void *data)
{
  int n;
  struct udf_cache *c;

  if(DVDUDFCacheLevel(device, -1) <= 0) {
    return 0;
  }

  c = (struct udf_cache *)GetUDFCacheHandle(device);
  
  if(c == NULL) {
    c = calloc(1, sizeof(struct udf_cache));    
    /*    fprintf(stderr, "calloc: %d\n", sizeof(struct udf_cache));    */
    if(c == NULL) {
      return 0;
    }
    SetUDFCacheHandle(device, c);
  }
  
  
  switch(type) {
  case AVDPCache:
    c->avdp = *(struct avdp_t *)data; 
    c->avdp_valid = 1;
    break;
  case PVDCache:
    c->pvd = *(struct pvd_t *)data; 
    c->pvd_valid = 1;
    break;
  case PartitionCache:
    c->partition = *(struct Partition *)data; 
    c->partition_valid = 1;
    break;
  case RootICBCache:
    c->rooticb = *(struct AD *)data; 
    c->rooticb_valid = 1;
    break;
  case LBUDFCache:
    for(n = 0; n < c->lb_num; n++) {
      if(c->lbs[n].lb == nr) {
        /* replace with new data */
        c->lbs[n].data = *(uint8_t **)data;
        c->lbs[n].lb = nr;
        return 1;
      }
    }
    c->lb_num++;
    c->lbs = realloc(c->lbs, c->lb_num * sizeof(struct lbudf));
    /*
      fprintf(stderr, "realloc lb: %d * %d = %d\n",
      c->lb_num, sizeof(struct lbudf),
      c->lb_num * sizeof(struct lbudf));
    */
    if(c->lbs == NULL) {
      c->lb_num = 0;
      return 0;
    }
    c->lbs[n].data = *(uint8_t **)data;
    c->lbs[n].lb = nr;
    break;
  case MapCache:
    for(n = 0; n < c->map_num; n++) {
      if(c->maps[n].lbn == nr) {
        /* replace with new data */
        c->maps[n] = *(struct icbmap *)data;
        c->maps[n].lbn = nr;
        return 1;
      }
    }
    c->map_num++;
    c->maps = realloc(c->maps, c->map_num * sizeof(struct icbmap));
    /*
      fprintf(stderr, "realloc maps: %d * %d = %d\n",
      c->map_num, sizeof(struct icbmap),
      c->map_num * sizeof(struct icbmap));
    */
    if(c->maps == NULL) {
      c->map_num = 0;
      return 0;
    }
    c->maps[n] = *(struct icbmap *)data;
    c->maps[n].lbn = nr;
    break;
  default:
    return 0;
  }
    
  return 1;
}


/* For direct data access, LSB first */
#define GETN1(p) ((uint8_t)data[p])
#define GETN2(p) ((uint16_t)data[p] | ((uint16_t)data[(p) + 1] << 8))
#define GETN3(p) ((uint32_t)data[p] | ((uint32_t)data[(p) + 1] << 8)    \
                  | ((uint32_t)data[(p) + 2] << 16))
#define GETN4(p) ((uint32_t)data[p]                     \
                  | ((uint32_t)data[(p) + 1] << 8)      \
                  | ((uint32_t)data[(p) + 2] << 16)     \
                  | ((uint32_t)data[(p) + 3] << 24))
/* This is wrong with regard to endianess */
#define GETN(p, n, target) memcpy(target, &data[p], n)

#define Unicodedecode(a,b,c) ___Unicodedecode(a,b,c)
static int ___Unicodedecode( uint8_t *data, int len, char *target )
{
  int p = 1, i = 0;

  if( ( data[ 0 ] == 8 ) || ( data[ 0 ] == 16 ) ) do {
    if( data[ 0 ] == 16 ) p++;  /* Ignore MSB of unicode16 */
    if( p < len ) {
      target[ i++ ] = data[ p++ ];
    }
  } while( p < len );

  target[ i ] = '\0';
  return 0;
}

#define UDFDescriptor(a,b) ___UDFDescriptor(a,b)
static int ___UDFDescriptor( uint8_t *data, uint16_t *TagID )
{
  *TagID = GETN2(0);
  return 0;
}
#define UDFExtentAD(a,b,c) ___UDFExtentAD(a,b,c)
static int ___UDFExtentAD( uint8_t *data, uint32_t *Length, uint32_t *Location )
{
  *Length   = GETN4(0);
  *Location = GETN4(4);
  return 0;
}

#define UDFShortAD(a,b,c) ___UDFShortAD(a,b,c)
static int ___UDFShortAD( uint8_t *data, struct AD *ad,
                       struct Partition *partition ) 
{
  ad->Length = GETN4(0);
  ad->Flags = ad->Length >> 30;
  ad->Length &= 0x3FFFFFFF;
  ad->Location = GETN4(4);
  ad->Partition = partition->Number; /* use number of current partition*/
  return 0;
}

#define UDFLongAD(a,b) ___UDFLongAD(a,b)
static int ___UDFLongAD( uint8_t *data, struct AD *ad )
{
  ad->Length = GETN4(0);
  ad->Flags = ad->Length >> 30;
  ad->Length &= 0x3FFFFFFF;
  ad->Location = GETN4(4);
  ad->Partition = GETN2(8);
  /*GETN(10, 6, Use);*/
  return 0;
}

#define UDFExtAD(a,b) ___UDFExtAD(a,b)
static int ___UDFExtAD( uint8_t *data, struct AD *ad )
{
  ad->Length = GETN4(0);
  ad->Flags = ad->Length >> 30;
  ad->Length &= 0x3FFFFFFF;
  ad->Location = GETN4(12);
  ad->Partition = GETN2(16);
  /*GETN(10, 6, Use);*/
  return 0;
}

#define UDFICB(a,b,c) ___UDFICB(a,b,c)
static int ___UDFICB( uint8_t *data, uint8_t *FileType, uint16_t *Flags )
{
  *FileType = GETN1(11);
  *Flags = GETN2(18);
  return 0;
}

#define UDFPartition(a,b,c,d,e,f) ___UDFPartition(a,b,c,d,e,f)
static int ___UDFPartition( uint8_t *data, uint16_t *Flags, uint16_t *Number,
                         char *Contents, uint32_t *Start, uint32_t *Length )
{
  *Flags = GETN2(20);
  *Number = GETN2(22);
  GETN(24, 32, Contents);
  *Start = GETN4(188);
  *Length = GETN4(192);
  return 0;
}

/**
 * Reads the volume descriptor and checks the parameters.  Returns 0 on OK, 1
 * on error.
 */
#define UDFLogVolume(a,b) ___UDFLogVolume(a,b)
static int ___UDFLogVolume( uint8_t *data, char *VolumeDescriptor )
{
  uint32_t lbsize, MT_L, N_PM;
  Unicodedecode(&data[84], 128, VolumeDescriptor);
  lbsize = GETN4(212);  /* should be 2048*/
  MT_L = GETN4(264);    /* should be 6  */
  N_PM = GETN4(268);    /* should be 1 */
  if (lbsize != DVD_VIDEO_LB_LEN) return 1;
  return 0;
}

#define UDFFileEntry(a,b,c,d) ___UDFFileEntry(a,b,c,d)
static int ___UDFFileEntry( uint8_t *data, uint8_t *FileType,
                         struct Partition *partition, struct AD *ad )
{
  uint16_t flags;
  uint32_t L_EA, L_AD;
  unsigned int p;

  UDFICB( &data[ 16 ], FileType, &flags );
   
  /* Init ad for an empty file (i.e. there isn't a AD, L_AD == 0 ) */
  ad->Length = GETN4( 60 ); /* Really 8 bytes a 56*/
  ad->Flags = 0;
  ad->Location = 0; /* what should we put here? */
  ad->Partition = partition->Number; /* use number of current partition*/

  L_EA = GETN4( 168 );
  L_AD = GETN4( 172 );
  p = 176 + L_EA;
  while( p < 176 + L_EA + L_AD ) {
    switch( flags & 0x0007 ) {
    case 0: UDFShortAD( &data[ p ], ad, partition ); p += 8;  break;
    case 1: UDFLongAD( &data[ p ], ad );  p += 16; break;
    case 2: UDFExtAD( &data[ p ], ad );   p += 20; break;
    case 3:
      switch( L_AD ) {
      case 8:  UDFShortAD( &data[ p ], ad, partition ); break;
      case 16: UDFLongAD( &data[ p ], ad );  break;
      case 20: UDFExtAD( &data[ p ], ad );   break;
      }
      p += L_AD;
      break;
    default:
      p += L_AD; break;
    }
  }
  return 0;
}

#define UDFFileIdentifier(a,b,c,d) ___UDFFileIdentifier(a,b,c,d)
static int ___UDFFileIdentifier( uint8_t *data, uint8_t *FileCharacteristics,
                              char *FileName, struct AD *FileICB )
{
  uint8_t L_FI;
  uint16_t L_IU;

  *FileCharacteristics = GETN1(18);
  L_FI = GETN1(19);
  UDFLongAD(&data[20], FileICB);
  L_IU = GETN2(36);
  if (L_FI) Unicodedecode(&data[38 + L_IU], L_FI, FileName);
  else FileName[0] = '\0';
  return 4 * ((38 + L_FI + L_IU + 3) / 4);
}

/**
 * Maps ICB to FileAD
 * ICB: Location of ICB of directory to scan
 * FileType: Type of the file
 * File: Location of file the ICB is pointing to
 * return 1 on success, 0 on error;
 */
#define UDFMapICB(a,b,c,d,e) ___UDFMapICB(a,b,c,d,e)
static int ___UDFMapICB( dvd_reader_t *device, struct AD ICB, uint8_t *FileType,
                      struct Partition *partition, struct AD *File ) 
{
  uint8_t *LogBlock;
  uint32_t lbnum;
  uint16_t TagID;
  struct icbmap tmpmap;

  lbnum = partition->Start + ICB.Location;
  tmpmap.lbn = lbnum;
  if(GetUDFCache(device, MapCache, lbnum, &tmpmap)) {
    *FileType = tmpmap.filetype;
    *File = tmpmap.file;
    return 1;
  }

  LogBlock = dvdalign_lbmalloc(device, 1);
  if(!LogBlock) {
    return 0;
  }
    
  do {
    if( DVDReadLBUDF( device, lbnum++, 1, LogBlock, 0 ) <= 0 ) {
      TagID = 0;
    } else {
      UDFDescriptor( LogBlock, &TagID );
    }

    if( TagID == 261 ) {
      UDFFileEntry( LogBlock, FileType, partition, File );
      tmpmap.file = *File;
      tmpmap.filetype = *FileType;
      SetUDFCache(device, MapCache, tmpmap.lbn, &tmpmap);
      dvdalign_lbfree(device, LogBlock);
      return 1;
    };
  } while( ( lbnum <= partition->Start + ICB.Location + ( ICB.Length - 1 )
             / DVD_VIDEO_LB_LEN ) && ( TagID != 261 ) );

  dvdalign_lbfree(device, LogBlock);
  return 0;
}

/**
 * Dir: Location of directory to scan
 * FileName: Name of file to look for
 * FileICB: Location of ICB of the found file
 * return 1 on success, 0 on error;
 */
#define UDFScanDir(a,b,c,d,e,f) ___UDFScanDir(a,b,c,d,e,f)
static int ___UDFScanDir( dvd_reader_t *device, struct AD Dir, char *FileName,
                       struct Partition *partition, struct AD *FileICB,
                       int cache_file_info) 
{
  char filename[ MAX_UDF_FILE_NAME_LEN ];
  uint8_t *directory;
  uint32_t lbnum;
  uint16_t TagID;
  uint8_t filechar;
  unsigned int p;
  uint8_t *cached_dir = NULL;
  uint32_t dir_lba;
  struct AD tmpICB;
  int found = 0;
  int in_cache = 0;

  /* Scan dir for ICB of file */
  lbnum = partition->Start + Dir.Location;
    
  if(DVDUDFCacheLevel(device, -1) > 0) {
    /* caching */
      
    if(!GetUDFCache(device, LBUDFCache, lbnum, &cached_dir)) {
      dir_lba = (Dir.Length + DVD_VIDEO_LB_LEN) / DVD_VIDEO_LB_LEN;
      if((cached_dir = dvdalign_lbmalloc(device, dir_lba)) == NULL) {
        return 0;
      }
      if( DVDReadLBUDF( device, lbnum, dir_lba, cached_dir, 0) <= 0 ) {
        dvdalign_lbfree(device, cached_dir);
        cached_dir = NULL;
      }
      SetUDFCache(device, LBUDFCache, lbnum, &cached_dir);
    } else {
      in_cache = 1;
    }
      
    if(cached_dir == NULL) {
      return 0;
    }
      
    p = 0;
      
    while( p < Dir.Length ) {
      UDFDescriptor( &cached_dir[ p ], &TagID );
      if( TagID == 257 ) {
        p += UDFFileIdentifier( &cached_dir[ p ], &filechar,
                                filename, &tmpICB );
        if(cache_file_info && !in_cache) {
          uint8_t tmpFiletype;
          struct AD tmpFile;
            
          if( !strcasecmp( FileName, filename ) ) {
            *FileICB = tmpICB;
            found = 1;
              
          }
          UDFMapICB(device, tmpICB, &tmpFiletype,
                    partition, &tmpFile);
        } else {
          if( !strcasecmp( FileName, filename ) ) {
            *FileICB = tmpICB;
            return 1;
          }
        }
      } else {
        if(cache_file_info && (!in_cache) && found) {
          return 1;
        }
        return 0;
      }
    }
    if(cache_file_info && (!in_cache) && found) {
      return 1;
    }
    return 0;
  }

  directory = dvdalign_lbmalloc(device, 2);
  if(!directory) {
    return 0;
  }
  if( DVDReadLBUDF( device, lbnum, 2, directory, 0 ) <= 0 ) {
    dvdalign_lbfree(device, directory);
    return 0;
  }

  p = 0;
  while( p < Dir.Length ) {
    if( p > DVD_VIDEO_LB_LEN ) {
      ++lbnum;
      p -= DVD_VIDEO_LB_LEN;
      Dir.Length -= DVD_VIDEO_LB_LEN;
      if( DVDReadLBUDF( device, lbnum, 2, directory, 0 ) <= 0 ) {
        dvdalign_lbfree(device, directory);
        return 0;
      }
    }
    UDFDescriptor( &directory[ p ], &TagID );
    if( TagID == 257 ) {
      p += UDFFileIdentifier( &directory[ p ], &filechar,
                              filename, FileICB );
      if( !strcasecmp( FileName, filename ) ) {
        dvdalign_lbfree(device, directory);
        return 1;
      }
    } else {
      dvdalign_lbfree(device, directory);
      return 0;
    }
  }

  dvdalign_lbfree(device, directory);
  return 0;
}

#define UDFGetAVDP(a,b) ___UDFGetAVDP(a,b)
static int ___UDFGetAVDP( dvd_reader_t *device,
                       struct avdp_t *avdp)
{
  uint8_t *Anchor;
  uint32_t lbnum, MVDS_location, MVDS_length;
  uint16_t TagID;
  uint32_t lastsector;
  int terminate;
  struct avdp_t; 
  
  if(GetUDFCache(device, AVDPCache, 0, avdp)) {
    return 1;
  }
  
  /* Find Anchor */
  lastsector = 0;
  lbnum = 256;   /* Try #1, prime anchor */
  terminate = 0;
  
  Anchor = dvdalign_lbmalloc(device, 1);
  if(!Anchor) {
    return 0;
  }
  for(;;) {
    if( DVDReadLBUDF( device, lbnum, 1, Anchor, 0 ) > 0 ) {
      UDFDescriptor( Anchor, &TagID );
    } else {
      TagID = 0;
    }
    if (TagID != 2) {
      /* Not an anchor */
      if( terminate ) {
        dvdalign_lbfree(device, Anchor);
        errno = EMEDIUMTYPE;
        return 0; /* Final try failed */
      } 
      
      if( lastsector ) {
        /* We already found the last sector.  Try #3, alternative
         * backup anchor.  If that fails, don't try again.
         */
        lbnum = lastsector;
        terminate = 1;
      } else {
        /* TODO: Find last sector of the disc (this is optional). */
        if( lastsector ) {
          /* Try #2, backup anchor */
          lbnum = lastsector - 256;
        } else {
          /* Unable to find last sector */
          dvdalign_lbfree(device, Anchor);
          errno = EMEDIUMTYPE;
          return 0;
        }
      }
    } else {
      /* It's an anchor! We can leave */
      break;
    }
  }
  /* Main volume descriptor */
  UDFExtentAD( &Anchor[ 16 ], &MVDS_length, &MVDS_location );
  avdp->mvds.location = MVDS_location;
  avdp->mvds.length = MVDS_length;
  
  /* Backup volume descriptor */
  UDFExtentAD( &Anchor[ 24 ], &MVDS_length, &MVDS_location );
  avdp->rvds.location = MVDS_location;
  avdp->rvds.length = MVDS_length;
  
  SetUDFCache(device, AVDPCache, 0, avdp);
  
  dvdalign_lbfree(device, Anchor);
  return 1;
}

/**
 * Looks for partition on the disc.  Returns 1 if partition found, 0 on error.
 *   partnum: Number of the partition, starting at 0.
 *   part: structure to fill with the partition information
 */
#define UDFFindPartition(a,b,c) ___UDFFindPartition(a,b,c)
static int ___UDFFindPartition( dvd_reader_t *device, int partnum,
                             struct Partition *part ) 
{
  uint8_t *LogBlock;
  uint32_t lbnum, MVDS_location, MVDS_length;
  uint16_t TagID;
  int i, volvalid;
  struct avdp_t avdp;

    
  if(!UDFGetAVDP(device, &avdp)) {
    return 0;
  }

  LogBlock = dvdalign_lbmalloc(device, 1);
  if(!LogBlock) {
    return 0;
  }
  /* Main volume descriptor */
  MVDS_location = avdp.mvds.location;
  MVDS_length = avdp.mvds.length;

  part->valid = 0;
  volvalid = 0;
  part->VolumeDesc[ 0 ] = '\0';
  i = 1;
  do {
    /* Find Volume Descriptor */
    lbnum = MVDS_location;
    do {

      if( DVDReadLBUDF( device, lbnum++, 1, LogBlock, 0 ) <= 0 ) {
        TagID = 0;
      } else {
        UDFDescriptor( LogBlock, &TagID );
      }

      if( ( TagID == 5 ) && ( !part->valid ) ) {
        /* Partition Descriptor */
        UDFPartition( LogBlock, &part->Flags, &part->Number,
                      part->Contents, &part->Start, &part->Length );
        part->valid = ( partnum == part->Number );
      } else if( ( TagID == 6 ) && ( !volvalid ) ) {
        /* Logical Volume Descriptor */
        if( UDFLogVolume( LogBlock, part->VolumeDesc ) ) {  
          /* TODO: sector size wrong! */
        } else {
          volvalid = 1;
        }
      }

    } while( ( lbnum <= MVDS_location + ( MVDS_length - 1 )
               / DVD_VIDEO_LB_LEN ) && ( TagID != 8 )
             && ( ( !part->valid ) || ( !volvalid ) ) );

    if( ( !part->valid) || ( !volvalid ) ) {
      /* Backup volume descriptor */
      MVDS_location = avdp.mvds.location;
      MVDS_length = avdp.mvds.length;
    }
  } while( i-- && ( ( !part->valid ) || ( !volvalid ) ) );

  dvdalign_lbfree(device, LogBlock);
  /* We only care for the partition, not the volume */
  return part->valid;
}

#define UDFFindFileReal(a,b,c) ___UDFFindFileReal(a,b,c)
static uint32_t ___UDFFindFileReal( dvd_reader_t *device, char *filename,
                      uint32_t *filesize )
{
  uint8_t *LogBlock;
  uint32_t lbnum;
  uint16_t TagID;
  struct Partition partition;
  struct AD RootICB, File, ICB;
  char tokenline[ MAX_UDF_FILE_NAME_LEN ];
  char *token;
  uint8_t filetype;
  
  if(filesize) {
    *filesize = 0;
  }
  tokenline[0] = '\0';
  strcat( tokenline, filename );

    
  if(!(GetUDFCache(device, PartitionCache, 0, &partition) &&
       GetUDFCache(device, RootICBCache, 0, &RootICB))) {
    /* Find partition, 0 is the standard location for DVD Video.*/
    if( !UDFFindPartition( device, 0, &partition ) ) {
      return 0;
    }
    SetUDFCache(device, PartitionCache, 0, &partition);
    
    LogBlock = dvdalign_lbmalloc(device, 1);
    if(!LogBlock) {
      return 0;
    }
    /* Find root dir ICB */
    lbnum = partition.Start;
    do {
      if( DVDReadLBUDF( device, lbnum++, 1, LogBlock, 0 ) <= 0 ) {
        TagID = 0;
      } else {
        UDFDescriptor( LogBlock, &TagID );
      }

      /* File Set Descriptor */
      if( TagID == 256 ) {  /* File Set Descriptor*/
        UDFLongAD( &LogBlock[ 400 ], &RootICB );
      }
    } while( ( lbnum < partition.Start + partition.Length )
             && ( TagID != 8 ) && ( TagID != 256 ) );

    dvdalign_lbfree(device, LogBlock);
      
    /* Sanity checks. */
    if( TagID != 256 ) {
      return 0;
    }
    if( RootICB.Partition != 0 ) {
      return 0;
    }
    SetUDFCache(device, RootICBCache, 0, &RootICB);
  }

  /* Find root dir */
  if( !UDFMapICB( device, RootICB, &filetype, &partition, &File ) ) {
    return 0;
  }
  if( filetype != 4 ) {
    return 0;  /* Root dir should be dir */
  }
  {
    int cache_file_info = 0;
    /* Tokenize filepath */
    token = strtok(tokenline, "/");
    
    while( token != NULL ) {
      
      if( !UDFScanDir( device, File, token, &partition, &ICB,
                       cache_file_info)) {
        return 0;
      }
      if( !UDFMapICB( device, ICB, &filetype, &partition, &File ) ) {
        return 0;
      }
      if(!strcmp(token, "VIDEO_TS")) {
        cache_file_info = 1;
      }
      token = strtok( NULL, "/" );
    }
  } 

  /* Sanity check. */
  if( File.Partition != 0 ) {
    return 0;
  }

  if(filesize) {
    *filesize = File.Length;
  }
  /* Hack to not return partition.Start for empty files. */
  if( !File.Location ) {
    return 0;
  } else {
    return partition.Start + File.Location;
  }
}

/**
 * Get the offset from the ifo files to allow playback of DVDs
 * with a deliberately broken UDF file system (aka DVD-Movie-Prxxxx).
 * When the file is not an IFO or VOB, it calls the real UDF routine.
 */
#define UDFFindFile(a,b,c) ___UDFFindFile(a,b,c)
uint32_t ___UDFFindFile( dvd_reader_t *device, char *filename,
		      uint32_t *filesize )
{
#ifndef __WIN32__
  if (!fnmatch("/VIDEO_TS/VTS_[0-9][0-9]_[0-9].???", filename, FNM_PATHNAME)) {
#else
  if (strlen("/VIDEO_TS/VTS_01_1.VOB") == strlen(filename)
      && !strncmp(filename, "/VIDEO_TS/VTS_", strlen("/VIDEO_TS/VTS_"))	) {
#endif
		size_t len = strlen(filename);
		char *extension = &filename[len-3];
		if (!strcmp(extension, "IFO") || !strcmp(extension, "VOB")) {
			int title = atoi(&filename[len-8]);
			int part = atoi(&filename[len-5]);

			ifo_handle_t *ifo_handle = ifoOpen(device, 0);
			if (0 == ifo_handle)
				return 0;

			uint32_t tmp_filesize;
			uint32_t offset = UDFFindFileReal(device, "/VIDEO_TS/VIDEO_TS.IFO", &tmp_filesize);

			int i;
			for (i=0; i<ifo_handle->tt_srpt->nr_of_srpts; i++)
				if (title == ifo_handle->tt_srpt->title[i].title_set_nr)
					break;

			if (i == ifo_handle->tt_srpt->nr_of_srpts) {
				/* not found */
				ifoClose(ifo_handle);
				return 0;
			}
			offset += ifo_handle->tt_srpt->title[i].title_set_sector;
			ifoClose(ifo_handle);

			if (!strcmp(extension, "VOB")) {
				ifo_handle = ifoOpen(device, title);
				if (0 == ifo_handle)
					return 0;

				switch(part) {
					case 0:
						if (0 == ifo_handle->vtsi_mat->vtsm_vobs) {
							ifoClose(ifo_handle);
							return 0;
						}
						offset += ifo_handle->vtsi_mat->vtsm_vobs;
						break;
					case 1:
						if (0 == ifo_handle->vtsi_mat->vtstt_vobs) {
							ifoClose(ifo_handle);
							return 0;
						}
						offset += ifo_handle->vtsi_mat->vtstt_vobs;
						break;
					default: /* can't get other parts (also no need to) */
						offset = 0;
						break;
				}

				ifoClose(ifo_handle);
			}


			*filesize = 1000000; /* File size unknown */
			if (offset != 0)
				return offset;
		}
	}

	return UDFFindFileReal( device, filename, filesize);
}


/**
 * Gets a Descriptor .
 * Returns 1 if descriptor found, 0 on error.
 * id, tagid of descriptor
 * bufsize, size of BlockBuf (must be >= DVD_VIDEO_LB_LEN)
 * and aligned for raw/O_DIRECT read.
 */
#define UDFGetDescriptor(a,b,c,d) ___UDFGetDescriptor(a,b,c,d)
static int ___UDFGetDescriptor( dvd_reader_t *device, int id,
                             uint8_t *descriptor, int bufsize) 
{
  uint32_t lbnum, MVDS_location, MVDS_length;
  struct avdp_t avdp;
  uint16_t TagID;
  uint32_t lastsector;
  int i, terminate;
  int desc_found = 0;
  /* Find Anchor */
  lastsector = 0;
  lbnum = 256;   /* Try #1, prime anchor */
  terminate = 0;
  if(bufsize < DVD_VIDEO_LB_LEN) {
    return 0;
  }
  
  if(!UDFGetAVDP(device, &avdp)) {
    return 0;
  }

  /* Main volume descriptor */
  MVDS_location = avdp.mvds.location;
  MVDS_length = avdp.mvds.length;
  
  i = 1;
  do {
    /* Find  Descriptor */
    lbnum = MVDS_location;
    do {
      
      if( DVDReadLBUDF( device, lbnum++, 1, descriptor, 0 ) <= 0 ) {
        TagID = 0;
      } else {
        UDFDescriptor( descriptor, &TagID );
      }
      
      if( (TagID == id) && ( !desc_found ) ) {
        /* Descriptor */
        desc_found = 1;
      }
    } while( ( lbnum <= MVDS_location + ( MVDS_length - 1 )
               / DVD_VIDEO_LB_LEN ) && ( TagID != 8 )
             && ( !desc_found) );
    
    if( !desc_found ) {
      /* Backup volume descriptor */
      MVDS_location = avdp.rvds.location;
      MVDS_length = avdp.rvds.length;
    }
  } while( i-- && ( !desc_found )  );

  
  return desc_found;
}

#define UDFGetPVD(a,b) ___UDFGetPVD(a,b)
static int ___UDFGetPVD(dvd_reader_t *device, struct pvd_t *pvd)
{
  uint8_t *pvd_buf;
  
  if(GetUDFCache(device, PVDCache, 0, pvd)) {
    return 1;
  }
  
  pvd_buf = dvdalign_lbmalloc(device, 1);
  if(!pvd_buf) {
    return 0;
  }
  if(!UDFGetDescriptor( device, 1, pvd_buf, 1*DVD_VIDEO_LB_LEN)) {
    dvdalign_lbfree(device, pvd_buf);
    return 0;
  }
  
  memcpy(pvd->VolumeIdentifier, &pvd_buf[24], 32);
  memcpy(pvd->VolumeSetIdentifier, &pvd_buf[72], 128);
  SetUDFCache(device, PVDCache, 0, pvd);
  
  dvdalign_lbfree(device, pvd_buf);

  return 1;
}

/**
 * Gets the Volume Identifier string, in 8bit unicode (latin-1)
 * volid, place to put the string
 * volid_size, size of the buffer volid points to
 * returns the size of buffer needed for all data
 */
#define UDFGetVolumeIdentifier(a,b,c) ___UDFGetVolumeIdentifier(a,b,c)
int UDFGetVolumeIdentifier(dvd_reader_t *device, char *volid,
                           unsigned int volid_size)
{
  struct pvd_t pvd;
  unsigned int volid_len;

  /* get primary volume descriptor */
  if(!UDFGetPVD(device, &pvd)) {
    return 0;
  }

  volid_len = pvd.VolumeIdentifier[31];
  if(volid_len > 31) {
    /* this field is only 32 bytes something is wrong */
    volid_len = 31;
  }
  if(volid_size > volid_len) {
    volid_size = volid_len;
  }
  Unicodedecode(pvd.VolumeIdentifier, volid_size, volid);
  
  return volid_len;
}

/**
 * Gets the Volume Set Identifier, as a 128-byte dstring (not decoded)
 * WARNING This is not a null terminated string
 * volsetid, place to put the data
 * volsetid_size, size of the buffer volsetid points to 
 * the buffer should be >=128 bytes to store the whole volumesetidentifier
 * returns the size of the available volsetid information (128)
 * or 0 on error
 */
#define UDFGetVolumeSetIdentifier(a,b,c) ___UDFGetVolumeSetIdentifier(a,b,c)
int ___UDFGetVolumeSetIdentifier(dvd_reader_t *device, uint8_t *volsetid,
                              unsigned int volsetid_size)
{
  struct pvd_t pvd;

  /* get primary volume descriptor */
  if(!UDFGetPVD(device, &pvd)) {
    return 0;
  }


  if(volsetid_size > 128) {
    volsetid_size = 128;
  }
  
  memcpy(volsetid, pvd.VolumeSetIdentifier, volsetid_size);
  
  return 128;
}
