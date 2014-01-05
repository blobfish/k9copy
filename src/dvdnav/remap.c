/*
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
 * $Id: remap.c,v 1.4 2003/08/25 21:51:40 f1rmb Exp $
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifndef _MSC_VER 
#include <sys/param.h>
#include <sys/fcntl.h>
#else
#ifndef MAXPATHLEN
#define MAXPATHLEN 255
#endif
#endif /* _MSC_VER */

#include <assert.h>
#include "remap.h"
#include "dvdnav_internal.h"

struct block_s {
    int domain;
    int title;
    int program;
    unsigned long start_block;
    unsigned long end_block;
};

struct remap_s {
    char *title;
    int maxblocks;
    int nblocks;
    int debug;
    struct block_s *blocks;
};

#define remap_new(a) ___remap_new(a)
static remap_t* ___remap_new( char *title) {
    remap_t *map = malloc( sizeof(remap_t));
    map->title = strdup(title);
    map->maxblocks = 0;
    map->nblocks = 0;
    map->blocks = NULL;
    map->debug = 0;
    return map;
}

#define compare_block(a,b) ___compare_block(a,b)
static int ___compare_block( block_t *a, block_t *b) {
    /* returns -1 if a precedes b, 1 if a follows b, and 0 if a and b overlap */
    if (a->domain < b->domain) {
	return -1;
    } else if (a->domain > b->domain) {
	return 1;
    }

    if (a->title < b->title) {
	return -1;
    } else if (a->title > b->title) {
	return 1;
    }

    if (a->program < b->program) {
	return -1;
    } else if (a->program > b->program) {
	return 1;
    }

    if (a->end_block < b->start_block) {
	return -1;
    } else if (a->start_block > b->end_block) {
	/*
	 * if a->start_block == b->end_block then the two regions
	 * aren't strictly overlapping, but they should be merged 
	 * anyway since there are zero blocks between them
	 */
	return 1;
    }

    return 0;
}

#define findblock(a,b) ___findblock(a,b)
static block_t *___findblock( remap_t *map, block_t *key) {
    int lb = 0;
    int ub = map->nblocks - 1;
    int mid;
    int res;

    while (lb <= ub) {
	mid = lb + (ub - lb)/2;
	res = compare_block( key, &map->blocks[mid]);
	if (res < 0) {
	    ub = mid-1;
	} else if (res > 0) {
	    lb = mid+1;
	} else {
	    return &map->blocks[mid];
	}
    }
    return NULL;
}

#define mergeblock(a,b) ___mergeblock(a,b)
static void ___mergeblock( block_t *b, block_t tmp) {
    if (tmp.start_block < b->start_block) b->start_block = tmp.start_block;
    if (tmp.end_block > b->end_block) b->end_block = tmp.end_block;
}

#define remap_add_node(a,b) ___remap_add_node(a,b)
static void ___remap_add_node( remap_t *map, block_t block) {
    block_t *b;
    int n;
    b = findblock( map, &block);
    if (b) {
	/* overlaps an existing block */
	mergeblock( b, block);
    } else {
        /* new block */
	if (map->nblocks >= map->maxblocks) {
	    map->maxblocks += 20;
	    map->blocks = realloc( map->blocks, sizeof( block_t)*map->maxblocks);
	}
	n = map->nblocks++;
	while (n > 0 && compare_block( &block, &map->blocks[ n-1]) < 0) {
	    map->blocks[ n] = map->blocks[ n-1];
	    n--;
	}
	map->blocks[ n] = block;
    }
}

#define parseblock(a,b,c,d,e,f) ___parseblock(a,b,c,d,e,f)
static int ___parseblock(char *buf, int *dom, int *tt, int *pg,
		      unsigned long *start, unsigned long *end) {
    long tmp;
    char *tok;
    char *epos;
    char *marker[]={"domain", "title", "program", "start", "end"};
    int st = 0;
    tok = strtok( buf, " ");
    while (st < 5) {
        if (strcmp(tok, marker[st])) return -st-1000;
        tok = strtok( NULL, " ");
        if (!tok) return -st-2000;
        tmp = strtol( tok, &epos, 0);
        if (*epos != 0 && *epos != ',') return -st-3000;
        switch (st) {
	    case 0:
		*dom = (int)tmp;
		break;
	    case 1:
		*tt = (int)tmp;
		break;
	    case 2:
		*pg = (int)tmp;
		break;
	    case 3:
		*start = tmp;
		break;
	    case 4:
		*end = tmp;
		break;
	} 
	st++;
        tok = strtok( NULL, " ");
    }
    return st;
}

remap_t* ___remap_loadmap( char *title) {
    char buf[160];
    char *fname;
    char *home;
    int res;
    int fname_len=0;
    FILE *fp;
    block_t tmp;
    remap_t *map;

    /* Build the map filename */
    home = getenv("HOME"); assert(home);
    /*strncpy(fname, home, sizeof(fname));
    strncat(fname, "/.dvdnav/", sizeof(fname));
    strncat(fname, title, sizeof(fname));
    strncat(fname, ".map", sizeof(fname));
    */
    fname_len = strlen(home)+strlen("/.dvdnav/")+strlen(title)+strlen(".map")+1;
    fname = calloc(fname_len, sizeof(char));
    snprintf(fname, fname_len, "%s%s%s%s", home, "/.dvdnav/", title, ".map");
 
    /* Open the map file */
    fp = fopen( fname, "r");
    if (!fp) {
	fprintf(MSG_OUT, "libdvdnav: Unable to find map file '%s'\n", fname);
	return NULL;
    }

    /* Load the map file */
    map = remap_new( title);
    while (fgets( buf, sizeof(buf), fp) != NULL) {
        if (buf[0] == '\n' || buf[0] == '#' || buf[0] == 0) continue;
        if (strncasecmp( buf, "debug", 5) == 0) {
	    map->debug = 1;
	} else {
	    res = parseblock( buf, 
		&tmp.domain, &tmp.title, &tmp.program, &tmp.start_block, &tmp.end_block);
	    if (res != 5) {
		fprintf(MSG_OUT, "libdvdnav: Ignoring map line (%d): %s\n", res, buf);
		continue;
	    }
	    remap_add_node( map, tmp);
	}
    }

    free(fname);
    if (map->nblocks == 0 && map->debug == 0) return NULL;
    return map;
}

unsigned long ___remap_block(
	remap_t *map, int domain, int title, int program, 
	unsigned long cblock, unsigned long offset) 
{
    block_t key;
    block_t *b;

    if (map->debug) {
	fprintf(MSG_OUT, "libdvdnav: %s: domain %d, title %d, program %d, start %lx, next %lx\n",
	    map->title, domain, title, program, cblock, cblock+offset);
    }

    key.domain = domain;
    key.title = title;
    key.program = program;
    key.start_block = key.end_block = cblock + offset;
    b = findblock( map, &key);
    
    if (b) {
       if (map->debug) {
	   fprintf(MSG_OUT, "libdvdnav: Redirected to %lx\n", b->end_block);
       }
       return b->end_block - cblock;
    }
    return offset;
}
