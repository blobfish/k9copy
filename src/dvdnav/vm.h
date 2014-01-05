/*
 * Copyright (C) 2000, 2001 H�an Hjort
 * Copyright (C) 2001 Rich Wareham <richwareham@users.sourceforge.net>
 * 
 * This file is part of libdvdnav, a DVD navigation library. It is modified
 * from a file originally part of the Ogle DVD player.
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
 * $Id: vm.h,v 1.11 2004/10/05 19:07:10 hadess Exp $
 *
 */

#ifndef VM_H_INCLUDED
#define VM_H_INCLUDED

#include "remap.h"
#include "dvdnav_internal.h"

/* DOMAIN enum */

typedef enum {
  FP_DOMAIN   = 1,
  VTS_DOMAIN  = 2,
  VMGM_DOMAIN = 4,
  VTSM_DOMAIN = 8
} domain_t;  

/**
 * State: SPRM, GPRM, Domain, pgc, pgN, cellN, ?
 */
typedef struct {
  registers_t registers;
  
  domain_t  domain;
  int       vtsN;         /* 0 is vmgm? */
  pgc_t    *pgc;          /* either this or 'int pgcN' is enough? */
  int       pgcN;         /* but provide pgcN for quick lookup */
  int       pgN;          /* is this needed? can allways fid pgN from cellN? */
  int       cellN;
  int32_t   cell_restart; /* get cell to restart */
  int       blockN;
  
  /* Resume info */
  int      rsm_vtsN;
  int      rsm_blockN;    /* of nav_packet */
  uint16_t rsm_regs[5];   /* system registers 4-8 */
  int      rsm_pgcN;
  int      rsm_cellN;
} dvd_state_t;

typedef struct vm_position_s {
  int16_t  button;        /* Button highlighted */
  int32_t  vts;           /* vts number to use */
  domain_t domain;        /* domain to use */
  int32_t  spu_channel;   /* spu channel to use */
  int32_t  angle_channel; /* angle channel to use */
  int32_t  audio_channel; /* audio channel to use */
  int32_t  hop_channel;   /* channel hopping. E.g menu button pressed */
#if 0
  /* currently unused */
  int32_t  title;         /* title number */
  int32_t  chapter;       /* chapter number */
#endif
  int32_t  cell;          /* cell number */
  int32_t  cell_restart;  /* get cell to restart */
  int32_t  cell_start;    /* sector number of start of current cell in use */
  int32_t  still;         /* is cell still */
  int32_t  block;         /* block number within cell in use */
} vm_position_t;

typedef struct {
  dvd_reader_t *dvd,*openedDvd;
  ifo_handle_t *vmgi;
  ifo_handle_t *vtsi;
  dvd_state_t   state;
  int32_t       hop_channel;
  char          dvd_name[50];
  remap_t      *map;
  int           stopped;
} vm_t;

/* magic number for seeking hops */
#define HOP_SEEK 0x1000


/*  Audio stream number */
#define AST_REG      registers.SPRM[1]
/*  Subpicture stream number */
#define SPST_REG     registers.SPRM[2]
/*  Angle number */
#define AGL_REG      registers.SPRM[3]
/*  Title Track Number */
#define TTN_REG      registers.SPRM[4]
/*  VTS Title Track Number */
#define VTS_TTN_REG  registers.SPRM[5]
/*  PGC Number for this Title Track */
#define TT_PGCN_REG  registers.SPRM[6]
/*  Current Part of Title (PTT) number for (One_Sequential_PGC_Title) */
#define PTTN_REG     registers.SPRM[7]
/*  Highlighted Button Number (btn nr 1 == value 1024) */
#define HL_BTNN_REG  registers.SPRM[8]
/*  Parental Level */
#define PTL_REG      registers.SPRM[13]

/* Initialisation & destruction */
#define vm_new_vm() ___vm_new_vm()
vm_t *___vm_new_vm(void);
#define vm_free_vm(a) ___vm_free_vm(a)
void  ___vm_free_vm(vm_t *vm);

/* IFO access */
#define vm_get_vmgi(a) ___vm_get_vmgi(a)
ifo_handle_t *___vm_get_vmgi(vm_t *vm);

#define vm_get_vtsi(a) ___vm_get_vtsi(a)
ifo_handle_t *___vm_get_vtsi(vm_t *vm);

/* Reader Access */
#define vm_get_dvd_reader(a) ___vm_get_dvd_reader(a)
dvd_reader_t *___vm_get_dvd_reader(vm_t *vm);

/* Basic Handling */
#define vm_start(a) ___vm_start(a)
int  ___vm_start(vm_t *vm);
#define vm_stop(a) ___vm_stop(a)
void ___vm_stop(vm_t *vm);
#define vm_reset(a,b) ___vm_reset(a,b)
int  ___vm_reset(vm_t *vm, const char *dvdroot);

/* copying and merging  - useful for try-running an operation */
#define vm_new_copy(a) ___vm_new_copy(a)
vm_t *___vm_new_copy(vm_t *vm);
#define vm_merge(a,b) ___vm_merge(a,b)
void  ___vm_merge(vm_t *target, vm_t *source);
#define vm_free_copy(a) ___vm_free_copy(a)
void  ___vm_free_copy(vm_t *vm);

/* regular playback */
#define vm_position_get(a,b) ___vm_position_get(a,b)
void ___vm_position_get(vm_t *vm, vm_position_t *position);
#define vm_get_next_cell(a) ___vm_get_next_cell(a)
void ___vm_get_next_cell(vm_t *vm);

/* Jumping - all these return 1, if a hop has been performed */
#define vm_jump_pg(a,b) ___vm_jump_pg(a,b)
int ___vm_jump_pg(vm_t *vm, int pg);
#define vm_jump_cell_block(a,b,c) ___vm_jump_cell_block(a,b,c)
int ___vm_jump_cell_block(vm_t *vm, int cell, int block);
#define vm_jump_title_part(a,b,c) ___vm_jump_title_part(a,b,c)
int ___vm_jump_title_part(vm_t *vm, int title, int part);
#define vm_jump_top_pg(a) ___vm_jump_top_pg(a)
int ___vm_jump_top_pg(vm_t *vm);
#define vm_jump_next_pg(a) ___vm_jump_next_pg(a)
int ___vm_jump_next_pg(vm_t *vm);
#define vm_jump_prev_pg(a) ___vm_jump_prev_pg(a)
int ___vm_jump_prev_pg(vm_t *vm);
#define vm_jump_up(a) ___vm_jump_up(a)
int ___vm_jump_up(vm_t *vm);
#define vm_jump_menu(a,b) ___vm_jump_menu(a,b)
int ___vm_jump_menu(vm_t *vm, DVDMenuID_t menuid);
#define vm_jump_resume(a) ___vm_jump_resume(a)
int ___vm_jump_resume(vm_t *vm);
#define vm_exec_cmd(a,b) ___vm_exec_cmd(a,b)
int ___vm_exec_cmd(vm_t *vm, vm_cmd_t *cmd);

/* getting information */
#define vm_get_current_menu(a,b) ___vm_get_current_menu(a,b)
int ___vm_get_current_menu(vm_t *vm, int *menuid);
#define vm_get_current_title_part(a,b,c) ___vm_get_current_title_part(a,b,c)
int ___vm_get_current_title_part(vm_t *vm, int *title_result, int *part_result);
#define vm_get_audio_stream(a,b) ___vm_get_audio_stream(a,b)
int ___vm_get_audio_stream(vm_t *vm, int audioN);
#define vm_get_subp_stream(a,b,c) ___vm_get_subp_stream(a,b,c)
int ___vm_get_subp_stream(vm_t *vm, int subpN, int mode);
#define vm_get_audio_active_stream(a) ___vm_get_audio_active_stream(a)
int ___vm_get_audio_active_stream(vm_t *vm);
#define vm_get_subp_active_stream(a,b) ___vm_get_subp_active_stream(a,b)
int ___vm_get_subp_active_stream(vm_t *vm, int mode);
#define vm_get_angle_info(a,b,c) ___vm_get_angle_info(a,b,c)
void ___vm_get_angle_info(vm_t *vm, int *current, int *num_avail);
#if 0
/* currently unused */
#define vm_get_audio_info(a,b,c) ___vm_get_audio_info(a,b,c)
void ___vm_get_audio_info(vm_t *vm, int *current, int *num_avail);
#define vm_get_subp_info(a,b,c) ___vm_get_subp_info(a,b,c)
void ___vm_get_subp_info(vm_t *vm, int *current, int *num_avail);
#define vm_get_video_res(a,b,c) ___vm_get_video_res(a,b,c)
void ___vm_get_video_res(vm_t *vm, int *width, int *height);
#endif
#define vm_get_video_aspect(a) ___vm_get_video_aspect(a)
int  ___vm_get_video_aspect(vm_t *vm);
#define vm_get_video_scale_permission(a) ___vm_get_video_scale_permission(a)
int  ___vm_get_video_scale_permission(vm_t *vm);
#define vm_get_video_attr(a) ___vm_get_video_attr(a)
video_attr_t ___vm_get_video_attr(vm_t *vm);
#define vm_get_audio_attr(a,b) ___vm_get_audio_attr(a,b)
audio_attr_t ___vm_get_audio_attr(vm_t *vm, int streamN);
#define vm_get_subp_attr(a,b) ___vm_get_subp_attr(a,b)
subp_attr_t  ___vm_get_subp_attr(vm_t *vm, int streamN);

#ifdef TRACE
/* Debug */
#define vm_position_print(a,b) ___vm_position_print(a,b)
void ___vm_position_print(vm_t *vm, vm_position_t *position);
#endif


#endif /* VM_HV_INCLUDED */
