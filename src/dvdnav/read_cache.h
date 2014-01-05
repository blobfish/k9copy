/* 
 * Copyright (C) 2000 Rich Wareham <richwareham@users.sourceforge.net>
 * 
 * This file is part of libdvdnav, a DVD navigation library.
 * 
 * libdvdnav is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * libdvdnav is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA
 *
 * $Id: read_cache.h,v 1.1 2002/08/08 17:49:21 richwareham Exp $
 *
 */

#ifndef __DVDNAV_READ_CACHE_H
#define __DVDNAV_READ_CACHE_H

#include "dvdnav_internal.h"

/* Opaque cache type -- defined in dvdnav_internal.h */
/* typedef struct read_cache_s read_cache_t; */

/* EXPERIMENTAL: Setting the following to 1 will use an experimental multi-threaded
 *               read-ahead cache. 
 */
#define _MULTITHREAD_ 0

/* Constructor/destructors */
#define dvdnav_read_cache_new(a) ___dvdnav_read_cache_new(a)
read_cache_t *___dvdnav_read_cache_new(dvdnav_t* dvd_self);
#define dvdnav_read_cache_free(a) ___dvdnav_read_cache_free(a)
void ___dvdnav_read_cache_free(read_cache_t* self);

/* This function MUST be called whenever self->file changes. */
#define dvdnav_read_cache_clear(a) ___dvdnav_read_cache_clear(a)
void ___dvdnav_read_cache_clear(read_cache_t *self);
/* This function is called just after reading the NAV packet. */
#define dvdnav_pre_cache_blocks(a,b,c) ___dvdnav_pre_cache_blocks(a,b,c)
void ___dvdnav_pre_cache_blocks(read_cache_t *self, int sector, size_t block_count);
/* This function will do the cache read.
 * The buffer handed in must be malloced to take one dvd block.
 * On a cache hit, a different buffer will be returned though.
 * Those buffers must _never_ be freed. */
#define dvdnav_read_cache_block(a,b,c,d) ___dvdnav_read_cache_block(a,b,c,d)
int ___dvdnav_read_cache_block(read_cache_t *self, int sector, size_t block_count, uint8_t **buf);

#endif /* __DVDNAV_READ_CACHE_H */
