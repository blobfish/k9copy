//
// C++ Interface: k9requant
//
// Description: A transcription from m2vrequantizer in C++
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9REQUANT_H
#define K9REQUANT_H
#include "k9common.h"


#include <qthread.h>
#include <qmutex.h>
#include <qwaitcondition.h>


// user defined types
//typedef unsigned int		uint;
typedef unsigned char		uint8;
typedef unsigned short		uint16;
typedef unsigned int		uint32;
typedef unsigned long long	uint64;

typedef char				int8;
typedef short				int16;
typedef int					int32;
typedef long long			int64;


typedef signed int			sint;
typedef signed char			sint8;
typedef signed short		sint16;
typedef signed int			sint32;
#ifdef WIN
typedef __int64				sint64;
#else
typedef signed long long	sint64;
#endif

#define BITS_IN_BUF (8)

// block data
typedef struct {
    uint8 run;
    short level;
}
RunLevel;
/*
#define BUF_SIZE (16*1024*1024)
#define MIN_READ (4*1024*1024)
#define MIN_WRITE (8*1024*1024)
#define MAX_READ (10*1024*1024)
*/


// keep gcc happy
#define WRITE      \
  orbuf  = orbuf;  \
  mloka1 = mloka1; \
  mloka2 = mloka2; \
  eof    = eof;

// meaningless
#define MIN_WRITE 0
#define MAX_READ  0
#define MOV_READ

// this is where we switch threads
#define LOCK(x)   if (! lock(x)) {rqt_run=false;return;}

#define COPY(x)           \
  memcpy (wbuf, cbuf, x); \
  cbuf += x;              \
  wbuf += x;

#define SEEKR(x) cbuf += x;

#define SEEKW(x) wbuf += x;


/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/
class k9requant : public QThread {
private:

    int		inbitcnt, outbitcnt;
    uint32	inbitbuf, outbitbuf;
    uint64	inbytecnt, outbytecnt;
    float	fact_x;
    int		mloka1, mloka2, eof;

    int64 orim2vsize;
    int64 bytediff;
    double stress_factor;	//	from 0.0 to 1.0

    int i_factor;
    int p_factor;
    int b_factor;
    double i_min_stress;
    double p_min_stress;
    double b_min_stress;

    short quant_table_id_data[4096];
    short *quant_table_id ;

#ifdef USE_FD

    FILE *ifd, *ofd;
#endif

#ifdef STAT

    uint64 ori_i, ori_p, ori_b;
    uint64 new_i, new_p, new_b;
    uint64 cnt_i, cnt_p, cnt_b;
    uint64 cnt_p_i, cnt_p_ni;
    uint64 cnt_b_i, cnt_b_ni;
#endif

#ifdef DEMO

    int gopCount;
#endif

#ifdef LOG_RATE_CONTROL

    FILE* LOG_FILE;
#endif

#ifdef CHANGE_BRIGHTNESS

    int delta_bright;
    int dc_reset;
    int old_dc_pred, new_dc_pred;
#endif

    // mpeg2 state
    // seq header
    uint horizontal_size_value;
    uint vertical_size_value;

    // pic header
    uint picture_coding_type;

    // pic code ext
    uint f_code[2][2];
    uint intra_dc_precision;
    uint picture_structure;
    uint frame_pred_frame_dct;
    uint concealment_motion_vectors;
    uint q_scale_type;
    uint intra_vlc_format;
    uint alternate_scan;

    // error
    int validPicHeader;
    int validSeqHeader;
    int validExtHeader;
    int sliceError;

    // slice or mb
    uint quantizer_scale;
    uint new_quantizer_scale;
    uint last_coded_scale;
    int  h_offset, v_offset;
    int  mb_skip, mb_add;
    int  mb_out;

    int  mb_sav_run, mb_sav_lev, mb_sav_c;
    short *curTable;

    RunLevel block[6][65]; // terminated by level = 0, so we need 64+1
private:
    void putbits(uint val, int n);
    void Refill_bits(void);

    void Flush_Bits(uint n);
    uint Show_Bits(uint n);
    uint Get_Bits(uint n);

    uint Copy_Bits(uint n);
    void flush_read_buffer();
    void flush_write_buffer();
    int scale_quant(double quant );
    int increment_quant(int quant);
    int intmax( register int x, register int y );

    int intmin( register int x, register int y );
    void putmbtype(int mb_type);

    int getNewQuant(int curQuant, int intra);

    int isNotEmpty(RunLevel *blk);


    // return != 0 if error
    int putAC(int run, int signed_level, int vlcformat);
    // return != 0 if error
    int putACfirst(int run, int val);
    void putnonintrablk(RunLevel *blk);
    void putcbp(int cbp);

    int get_macroblock_modes ();
    int get_quantizer_scale ();
    void get_motion_delta (const int f_code);
    void get_dmv ();
    int get_coded_block_pattern ();
    int get_luma_dc_dct_diff ();
    int get_chroma_dc_dct_diff ();
    void get_intra_block_B14 ();
    void get_intra_block_B15 ();
    int get_non_intra_block_rq (RunLevel *blk);
    int get_non_intra_block_sav (RunLevel *blk, int cc);

#ifdef P_FRAME_NON_INTRA_DROP

    int get_non_intra_block_drop (RunLevel *blk, int cc);
#endif

#ifdef CHANGE_BRIGHTNESS

    void putDC(const sVLCtable *tab, int val);
#endif

    void slice_intra_DCT (const int cc);
    void slice_non_intra_DCT (int cur_block);
    void motion_fr_frame ( uint f_code[2] );
    void motion_fr_field ( uint f_code[2] );
    void motion_fr_dmv ( uint f_code[2] );
    void motion_fr_conceal ( );
    void motion_fi_field ( uint f_code[2] );
    void motion_fi_16x8 ( uint f_code[2] );
    void motion_fi_dmv ( uint f_code[2] );
    void motion_fi_conceal ();
    void putmbdata(int macroblock_modes);
    void put_quantiser(int quantiser);
    void putaddrinc(int addrinc);
    int slice_init (int code);
    void mpeg2_slice ( const int code );
    void initRequant();
    bool lock(int64 x);

protected:
    void run ();

public:
    uint8	*cbuf, *rbuf, *wbuf, *orbuf, *owbuf;
    // global data for inter thread com
    float           rqt_fact;
    uint32_t     rqt_rcnt;
    uint32_t     rqt_wcnt;
    uint64_t     rqt_inbytes;
    uint64_t     rqt_outbytes;
    uint64_t     rqt_visize;
    uchar          *rqt_rptr;
    uchar          *rqt_wptr;
    QWaitCondition condr;
    QWaitCondition  condw;
    QMutex mutr;
    QMutex mutw;
    bool 		rqt_stop;
    bool 		rqt_run;


public:
    k9requant();
    void initvar();
};



#endif
