/* -*- c-basic-offset: 2; indent-tabs-mode: nil -*- */
#ifndef IFO_READ_H_INCLUDED
#define IFO_READ_H_INCLUDED

/*
 * Copyright (C) 2000, 2001, 2002 Bj�rn Englund <d4bjorn@dtek.chalmers.se>,
 *                                H�kan Hjort <d95hjort@dtek.chalmers.se>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "ifo_types.h"
#include "dvd_reader.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * handle = ifoOpen(dvd, title);
 *
 * Opens an IFO and reads in all the data for the IFO file corresponding to the
 * given title.  If title 0 is given, the video manager IFO file is read.
 * Returns a handle to a completely parsed structure.
 */
ifo_handle_t *___ifoOpen(dvd_reader_t *, int );
#define ifoOpen(a,b) ___ifoOpen(a,b)

/**
 * handle = ifoOpenVMGI(dvd);
 *
 * Opens an IFO and reads in _only_ the vmgi_mat data.  This call can be used
 * together with the calls below to read in each segment of the IFO file on
 * demand.
 */
ifo_handle_t *___ifoOpenVMGI(dvd_reader_t *);
#define ifoOpenVMGI(a) ___ifoOpenVMGI(a)
/**
 * handle = ifoOpenVTSI(dvd, title);
 *
 * Opens an IFO and reads in _only_ the vtsi_mat data.  This call can be used
 * together with the calls below to read in each segment of the IFO file on
 * demand.
 */
ifo_handle_t *___ifoOpenVTSI(dvd_reader_t *, int);
#define ifoOpenVTSI(a,b) ___ifoOpenVTSI(a,b)
/**
 * ifoClose(ifofile);
 * Cleans up the IFO information.  This will free all data allocated for the
 * substructures.
 */
void ___ifoClose(ifo_handle_t *);
#define ifoClose(a) ___ifoClose(a)
/**
 * The following functions are for reading only part of the VMGI/VTSI files.
 * Returns 1 if the data was successfully read and 0 on error.
 */

/**
 * okay = ifoRead_PLT_MAIT(ifofile);
 *
 * Read in the Parental Management Information table, filling the
 * ifofile->ptl_mait structure and its substructures.  This data is only
 * located in the video manager information file.  This fills the
 * ifofile->ptl_mait structure and all its substructures.
 */
int ___ifoRead_PTL_MAIT(ifo_handle_t *);
#define ifoRead_PTL_MAIT(a) ___ifoRead_PTL_MAIT(a)
/**
 * okay = ifoRead_VTS_ATRT(ifofile);
 *
 * Read in the attribute table for the main menu vob, filling the
 * ifofile->vts_atrt structure and its substructures.  Only located in the
 * video manager information file.  This fills in the ifofile->vts_atrt
 * structure and all its substructures.
 */
int ___ifoRead_VTS_ATRT(ifo_handle_t *);
#define ifoRead_VTS_ATRT(a) ___ifoRead_VTS_ATRT(a)
/**
 * okay = ifoRead_TT_SRPT(ifofile);
 *
 * Reads the title info for the main menu, filling the ifofile->tt_srpt
 * structure and its substructures.  This data is only located in the video
 * manager information file.  This structure is mandatory in the IFO file.
 */
int ___ifoRead_TT_SRPT(ifo_handle_t *);
#define ifoRead_TT_SRPT(a) ___ifoRead_TT_SRPT(a)
/**
 * okay = ifoRead_VTS_PTT_SRPT(ifofile);
 *
 * Reads in the part of title search pointer table, filling the
 * ifofile->vts_ptt_srpt structure and its substructures.  This data is only
 * located in the video title set information file.  This structure is
 * mandatory, and must be included in the VTSI file.
 */
int ___ifoRead_VTS_PTT_SRPT(ifo_handle_t *);
#define ifoRead_VTS_PTT_SRPT(a) ___ifoRead_VTS_PTT_SRPT(a)
/**
 * okay = ifoRead_FP_PGC(ifofile);
 *
 * Reads in the first play program chain data, filling the
 * ifofile->first_play_pgc structure.  This data is only located in the video
 * manager information file (VMGI).  This structure is optional.
 */
int ___ifoRead_FP_PGC(ifo_handle_t *);
#define ifoRead_FP_PGC(a) ___ifoRead_FP_PGC(a)
/**
 * okay = ifoRead_PGCIT(ifofile);
 *
 * Reads in the program chain information table for the video title set.  Fills
 * in the ifofile->vts_pgcit structure and its substructures, which includes
 * the data for each program chain in the set.  This data is only located in
 * the video title set information file.  This structure is mandatory, and must
 * be included in the VTSI file.
 */
int ___ifoRead_PGCIT(ifo_handle_t *);
#define ifoRead_PGCIT(a) ___ifoRead_PGCIT(a)
/**
 * okay = ifoRead_PGCI_UT(ifofile);
 *
 * Reads in the menu PGCI unit table for the menu VOB.  For the video manager,
 * this corresponds to the VIDEO_TS.VOB file, and for each title set, this
 * corresponds to the VTS_XX_0.VOB file.  This data is located in both the
 * video manager and video title set information files.  For VMGI files, this
 * fills the ifofile->vmgi_pgci_ut structure and all its substructures.  For
 * VTSI files, this fills the ifofile->vtsm_pgci_ut structure.
 */
int ___ifoRead_PGCI_UT(ifo_handle_t *);
#define ifoRead_PGCI_UT(a) ___ifoRead_PGCI_UT(a)
/**
 * okay = ifoRead_VTS_TMAPT(ifofile);
 *
 * Reads in the VTS Time Map Table, this data is only located in the video
 * title set information file.  This fills the ifofile->vts_tmapt structure
 * and all its substructures.  When pressent enables VOBU level time-based
 * seeking for One_Sequential_PGC_Titles.
 */
int ___ifoRead_VTS_TMAPT(ifo_handle_t *);
#define ifoRead_VTS_TMAPT(a) ___ifoRead_VTS_TMAPT(a)
/**
 * okay = ifoRead_C_ADT(ifofile);
 *
 * Reads in the cell address table for the menu VOB.  For the video manager,
 * this corresponds to the VIDEO_TS.VOB file, and for each title set, this
 * corresponds to the VTS_XX_0.VOB file.  This data is located in both the
 * video manager and video title set information files.  For VMGI files, this
 * fills the ifofile->vmgm_c_adt structure and all its substructures.  For VTSI
 * files, this fills the ifofile->vtsm_c_adt structure.
 */
int ___ifoRead_C_ADT(ifo_handle_t *);
#define ifoRead_C_ADT(a) ___ifoRead_C_ADT(a)
/**
 * okay = ifoRead_TITLE_C_ADT(ifofile);
 *
 * Reads in the cell address table for the video title set corresponding to
 * this IFO file.  This data is only located in the video title set information
 * file.  This structure is mandatory, and must be included in the VTSI file.
 * This call fills the ifofile->vts_c_adt structure and its substructures.
 */
int ___ifoRead_TITLE_C_ADT(ifo_handle_t *);
#define ifoRead_TITLE_C_ADT(a) ___ifoRead_TITLE_C_ADT(a)
/**
 * okay = ifoRead_VOBU_ADMAP(ifofile);
 *
 * Reads in the VOBU address map for the menu VOB.  For the video manager, this
 * corresponds to the VIDEO_TS.VOB file, and for each title set, this
 * corresponds to the VTS_XX_0.VOB file.  This data is located in both the
 * video manager and video title set information files.  For VMGI files, this
 * fills the ifofile->vmgm_vobu_admap structure and all its substructures.  For
 * VTSI files, this fills the ifofile->vtsm_vobu_admap structure.
 */
int ___ifoRead_VOBU_ADMAP(ifo_handle_t *);
#define ifoRead_VOBU_ADMAP(a) ___ifoRead_VOBU_ADMAP(a)
/**
 * okay = ifoRead_TITLE_VOBU_ADMAP(ifofile);
 *
 * Reads in the VOBU address map for the associated video title set.  This data
 * is only located in the video title set information file.  This structure is
 * mandatory, and must be included in the VTSI file.  Fills the
 * ifofile->vts_vobu_admap structure and its substructures.
 */
int ___ifoRead_TITLE_VOBU_ADMAP(ifo_handle_t *);
#define ifoRead_TITLE_VOBU_ADMAP(a) ___ifoRead_TITLE_VOBU_ADMAP(a)
/**
 * okay = ifoRead_TXTDT_MGI(ifofile);
 *
 * Reads in the text data strings for the DVD.  Fills the ifofile->txtdt_mgi
 * structure and all its substructures.  This data is only located in the video
 * manager information file.  This structure is mandatory, and must be included
 * in the VMGI file.
 */
int ___ifoRead_TXTDT_MGI(ifo_handle_t *);
#define ifoRead_TXTDT_MGI(a) ___ifoRead_TXTDT_MGI(a)
/**
 * The following functions are used for freeing parsed sections of the
 * ifo_handle_t structure and the allocated substructures.  The free calls
 * below are safe:  they will not mind if you attempt to free part of an IFO
 * file which was not read in or which does not exist.
 */
void ___ifoFree_PTL_MAIT(ifo_handle_t *);
#define ifoFree_PTL_MAIT(a) ___ifoFree_PTL_MAIT(a)
void ___ifoFree_VTS_ATRT(ifo_handle_t *);
#define ifoFree_VTS_ATRT(a) ___ifoFree_VTS_ATRT(a)
void ___ifoFree_TT_SRPT(ifo_handle_t *);
#define ifoFree_TT_SRPT(a) ___ifoFree_TT_SRPT(a)
void ___ifoFree_VTS_PTT_SRPT(ifo_handle_t *);
#define ifoFree_VTS_PTT_SRPT(a) ___ifoFree_VTS_PTT_SRPT(a)
void ___ifoFree_FP_PGC(ifo_handle_t *);
#define ifoFree_FP_PGC(a) ___ifoFree_FP_PGC(a)
void ___ifoFree_PGCIT(ifo_handle_t *);
#define ifoFree_PGCIT(a) ___ifoFree_PGCIT(a)
void ___ifoFree_PGCI_UT(ifo_handle_t *);
#define ifoFree_PGCI_UT(a) ___ifoFree_PGCI_UT(a)
void ___ifoFree_VTS_TMAPT(ifo_handle_t *);
#define ifoFree_VTS_TMAPT(a) ___ifoFree_VTS_TMAPT(a)
void ___ifoFree_C_ADT(ifo_handle_t *);
#define ifoFree_C_ADT(a) ___ifoFree_C_ADT(a)
void ___ifoFree_TITLE_C_ADT(ifo_handle_t *);
#define ifoFree_TITLE_C_ADT(a) ___ifoFree_TITLE_C_ADT(a)
void ___ifoFree_VOBU_ADMAP(ifo_handle_t *);
#define ifoFree_VOBU_ADMAP(a) ___ifoFree_VOBU_ADMAP(a)
void ___ifoFree_TITLE_VOBU_ADMAP(ifo_handle_t *);
#define ifoFree_TITLE_VOBU_ADMAP(a) ___ifoFree_TITLE_VOBU_ADMAP(a)
void ___ifoFree_TXTDT_MGI(ifo_handle_t *);
#define ifoFree_TXTDT_MGI(a) ___ifoFree_TXTDT_MGI(a)
#ifdef __cplusplus
};
#endif
#endif /* IFO_READ_H_INCLUDED */
