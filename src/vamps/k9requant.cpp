//
// C++ Implementation: k9requant
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9requant.h"
#include "getvlc.h"
#include "putvlc.h"

// Code from libmpeg2 and mpeg2enc copyright by their respective owners
// New code and modifications copyright Antoine Missout
// Thanks to Sven Goethel for error resilience patches
// Released under GPL license, see gnu.org

// toggles:

#define THREAD
// #define LOG_RATE_CONTROL // some stats
// #define DEMO // demo mode
// #define STAT // print stats on exit
// #define USE_FD // use 2 lasts args for input/output paths

#define NDEBUG // turns off asserts
#define REMOVE_BYTE_STUFFING	// removes series of 0x00
// #define USE_GLOBAL_REGISTER // assign registers to bit buffers
#define MAX_ERRORS 0 // if above copy slice

//#define CHANGE_BRIGHTNESS //add a param to command line, changing brightness: _will_not_recompress_, disables max_errors
//#define WIN // for windows fixes, use with USE_FD

// params:

// if not defined, non intra block in p frames are requantised
// if defined and >= 0, we keep coeff. in pos 0..n-1 in scan order
// and coeff which would have been non-null if requantised
// if defined and < 0 we drop max 1/x coeffs.
// experimental, looks better when undefined
// #define P_FRAME_NON_INTRA_DROP 8

// params for fact = 1.0, fact = 3.0 and fact = 10.0
// we'll make a linear interpolation between
static const int i_factors[3] = {  5, 15,  65 };
static const int p_factors[3] = {  5, 25,  85 };
static const int b_factors[3] = { 25, 45, 105 };


static const double i_min_stresses[3] = { 0.70, 0.40, 0.00 };
static const double p_min_stresses[3] = { 0.60, 0.35, 0.00 };
static const double b_min_stresses[3] = { 0.00, 0.00, 0.00 };


// factor up to which alt table will be used
// (though alt_table gives better psnr up to factor around ~2.5
// the result is less pleasing to watch than normal table
// so this is disabled)
static const double max_alt_table = 0.0;

// includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

#ifndef USE_FD
#include <unistd.h>
#include <fcntl.h>
#endif

// quant table
#include "qTable.h"

// useful constants
#define I_TYPE 1
#define P_TYPE 2
#define B_TYPE 3

// gcc
#ifdef HAVE_BUILTIN_EXPECT
	#define likely(x) __builtin_expect ((x) != 0, 1)
	#define unlikely(x) __builtin_expect ((x) != 0, 0)
#else
	#define likely(x) (x)
	#define unlikely(x) (x)
#endif

#ifndef NDEBUG
	#define DEB(msg) fprintf (stderr, "%s:%d " msg, __FILE__, __LINE__)
	#define DEBF(format, args...) fprintf (stderr, "%s:%d " format, __FILE__, __LINE__, args)
#else
	#define DEB(msg)
	#ifdef WIN
		#define DEBF(format, args)
	#else
		#define DEBF(format, args...)
	#endif
#endif

#ifndef THREAD
#define LOG(msg) fprintf (stderr, msg)
#ifdef WIN
	#define LOGF(format, arg1) fprintf (stderr, format, arg1)
#else
	#define LOGF(format, args...) fprintf (stderr, format, args)
#endif
#endif



/*#define MOV_READ \
	mloka1 = rbuf - cbuf; if (mloka1) memmove(orbuf, cbuf, mloka1);\
	cbuf = rbuf = orbuf; rbuf += mloka1;
*/

#ifdef STAT

#define RETURN \
		assert(rbuf >= cbuf);\
		mloka1 = rbuf - cbuf;\
		if (mloka1) { COPY(mloka1); }\
		WRITE \
		free(orbuf); \
		free(owbuf); \
		\
		LOG("Stats:\n");\
		\
		LOGF("Wanted fact_x: %.1f\n", fact_x);\
		\
		LOGF("cnt_i: %.0f ", (float)cnt_i); \
		if (cnt_i) LOGF("ori_i: %.0f new_i: %.0f fact_i: %.1f\n", (float)ori_i, (float)new_i, (float)ori_i/(float)new_i); \
		else LOG("\n");\
		\
		LOGF("cnt_p: %.0f ", (float)cnt_p); \
		if (cnt_p) LOGF("ori_p: %.0f new_p: %.0f fact_p: %.1f cnt_p_i: %.0f cnt_p_ni: %.0f propor: %.1f i\n", \
			(float)ori_p, (float)new_p, (float)ori_p/(float)new_p, (float)cnt_p_i, (float)cnt_p_ni, (float)cnt_p_i/((float)cnt_p_i+(float)cnt_p_ni)); \
		else LOG("\n");\
		\
		LOGF("cnt_b: %.0f ", (float)cnt_b); \
		if (cnt_b) LOGF("ori_b: %.0f new_b: %.0f fact_b: %.1f cnt_b_i: %.0f cnt_b_ni: %.0f propor: %.1f i\n", \
			(float)ori_b, (float)new_b, (float)ori_b/(float)new_b, (float)cnt_b_i, (float)cnt_b_ni, (float)cnt_b_i/((float)cnt_b_i+(float)cnt_b_ni)); \
		else LOG("\n");\
		\
		LOGF("Final fact_x: %.1f\n", (float)inbytecnt/(float)outbytecnt);\
		exit(0);

#else

#define RETURN \
		assert(rbuf >= cbuf);\
		mloka1 = rbuf - cbuf;\
		if (mloka1) { COPY(mloka1); }\
		WRITE \
		free(orbuf); \
		free(owbuf); \
		exit(0);

#endif
	#define MOTION_CALL(routine,direction) 						\
do {														\
    if ((direction) & MACROBLOCK_MOTION_FORWARD)			\
		routine (f_code[0]);								\
    if ((direction) & MACROBLOCK_MOTION_BACKWARD)			\
		routine (f_code[1]);								\
} while (0)

#define NEXT_MACROBLOCK											\
do {															\
    h_offset += 16;												\
    if (h_offset == horizontal_size_value) 						\
	{															\
		v_offset += 16;											\
		if (v_offset > (vertical_size_value - 16)) return;		\
		h_offset = 0;											\
    }															\
} while (0)

#ifdef P_FRAME_NON_INTRA_DROP
	#if (P_FRAME_NON_INTRA_DROP < 0)
		#undef UPDATE_VAL
		#define UPDATE_VAL
		#define SAVE_VAL
		#define WRITE_VAL \
			blk->level = val; \
			blk->run = i - li - 1; \
			li = i; \
			blk++;
	#else
		#define SAVE_VAL oval = val;
		#define WRITE_VAL \
			if ((val) || (i < P_FRAME_NON_INTRA_DROP)) \
			{ \
				blk->level = oval; \
				blk->run = i - li - 1; \
				li = i; \
				blk++; \
			}
	#endif
#else
	#define SAVE_VAL
	#define WRITE_VAL \
		if (val) \
		{ \
			blk->level = val; \
			blk->run = i - li - 1; \
			li = i; \
			blk++; \
		}
#endif

#define UPDATE_VAL \
	val = curTable[val];

int quantisers[42] =
  {
    1, 2, 3, 4, 5, 6, 7, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30, 32,
    34, 36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64, 72, 80,
    88, 96, 104, 112
  };

int non_linear_quantizer_scale [] =
  {
    0,  1,  2,  3,  4,  5,   6,   7,
    8, 10, 12, 14, 16, 18,  20,  22,
    24, 28, 32, 36, 40, 44,  48,  52,
    56, 64, 72, 80, 88, 96, 104, 112
  };


const uint8 non_linear_mquant_table[32] =
  {
    0, 1, 2, 3, 4, 5, 6, 7,
    8,10,12,14,16,18,20,22,
    24,28,32,36,40,44,48,52,
    56,64,72,80,88,96,104,112
  };
const uint8 map_non_linear_mquant[113] =
  {
    0,1,2,3,4,5,6,7,8,8,9,9,10,10,11,11,12,12,13,13,14,14,15,15,16,16,
    16,17,17,17,18,18,18,18,19,19,19,19,20,20,20,20,21,21,21,21,22,22,
    22,22,23,23,23,23,24,24,24,24,24,24,24,25,25,25,25,25,25,25,26,26,
    26,26,26,26,26,26,27,27,27,27,27,27,27,27,28,28,28,28,28,28,28,29,
    29,29,29,29,29,29,29,29,29,30,30,30,30,30,30,30,31,31,31,31,31
  };


k9requant::k9requant()
{

  cbuf=rbuf=orbuf=wbuf=NULL;
  quant_table_id = &quant_table_id_data[2048];
  rqt_run=false;
  initvar();
}
void k9requant::putbits(uint val, int n)
{
  assert(n < 32);
  assert(!(val & (0xffffffffU << n)));

  while (unlikely(n >= outbitcnt))
  {
    wbuf[0] = (outbitbuf << outbitcnt ) | (val >> (n - outbitcnt));
    SEEKW(1);
    n -= outbitcnt;
    outbitbuf = 0;
    val &= ~(0xffffffffU << n);
    outbitcnt = BITS_IN_BUF;
  }

  if (likely(n))
  {
    outbitbuf = (outbitbuf << n) | val;
    outbitcnt -= n;
  }

  assert(outbitcnt > 0);
  assert(outbitcnt <= BITS_IN_BUF);
}

void k9requant::Refill_bits(void)
{
  assert((rbuf - cbuf) >= 1);
  inbitbuf |= cbuf[0] << (24 - inbitcnt);
  inbitcnt += 8;
  SEEKR(1)
}

void k9requant::Flush_Bits(uint n)
{
  assert(inbitcnt >= n);

  inbitbuf <<= n;
  inbitcnt -= n;

  assert( (!n) || ((n>0) && !(inbitbuf & 0x1)) );

  while (unlikely(inbitcnt < 24)) Refill_bits();
}

uint k9requant::Show_Bits(uint n)
{
  return ((unsigned int)inbitbuf) >> (32 - n);
}

uint k9requant::Get_Bits(uint n)
{
  uint Val = Show_Bits(n);
  Flush_Bits(n);
  return Val;
}

uint k9requant::Copy_Bits(uint n)
{
  uint Val = Get_Bits(n);
  putbits(Val, n);
  return Val;
}

void k9requant::flush_read_buffer()
{
  int i = inbitcnt & 0x7;
  if (i)
  {
    if (inbitbuf >> (32 - i))
    {
      DEBF("illegal inbitbuf: 0x%08X, %i, 0x%02X, %i\n", inbitbuf, inbitcnt, (inbitbuf >> (32 - i)), i);
      sliceError++;
    }

    inbitbuf <<= i;
    inbitcnt -= i;
  }
  SEEKR(-1 * (inbitcnt >> 3));
  inbitcnt = 0;
}

void k9requant::flush_write_buffer()
{
  if (outbitcnt != 8) putbits(0, outbitcnt);
}

/////---- begin ext mpeg code
int k9requant::scale_quant(double quant )
{
  int iquant;
#ifdef DEMO
  if ((gopCount & 0x7F) < 10) // gop is ~ 0.5 sec, so 5 sec every ~minute (127 * 0.5 = 63.5 sec)
  {
    if (q_scale_type) return 112;
    else return 62;
  }
#endif
  if (q_scale_type)
  {
    iquant = (int) floor(quant+0.5);
    /* clip mquant to legal (linear) range */
    if (iquant<1) iquant = 1;
    if (iquant>112) iquant = 112;
    iquant = non_linear_mquant_table[map_non_linear_mquant[iquant]];
  }
  else
  {
    /* clip mquant to legal (linear) range */
    iquant = (int)floor(quant+0.5);
    if (iquant<2) iquant = 2;
    if (iquant>62) iquant = 62;
    iquant = (iquant/2)*2; // Must be *even*
  }
  return iquant;
}

int k9requant::increment_quant(int quant)
{
#ifdef DEMO
  if ((gopCount & 0x7F) < 10)
  {
    if (q_scale_type) return 112;
    else return 62;
  }
#endif
  if (q_scale_type)
  {
    if (quant < 1 || quant > 112)
    {
      DEBF("illegal quant: %d\n", quant);
      if (quant > 112) quant = 112;
      else if (quant < 1) quant = 1;
      DEBF("illegal quant changed to : %d\n", quant);
      sliceError++;
    }
    quant = map_non_linear_mquant[quant] + 1;
    if (quant > 31) quant = 31;
    quant = non_linear_mquant_table[quant];
  }
  else
  {
    if ((quant & 1) || (quant < 2) || (quant > 62))
    {
      DEBF("illegal quant: %d\n", quant);
      if (quant & 1) quant--;
      if (quant > 62) quant = 62;
      else if (quant < 2) quant = 2;
      DEBF("illegal quant changed to : %d\n", quant);
      sliceError++;
    }
    quant += 2;
    if (quant > 62) quant = 62;
  }
  return quant;
}

int k9requant::intmax( register int x, register int y )
{ return x < y ? y : x; }

int k9requant::intmin( register int x, register int y )
{ return x < y ? x : y; }


int k9requant::getNewQuant(int curQuant, int intra)
{
#ifdef CHANGE_BRIGHTNESS
  return curQuant;
#else
  int mquant = 0;
  double cStress;

  switch (picture_coding_type)
  {
  case I_TYPE:
    cStress = (stress_factor - i_min_stress) / (1.0 - i_min_stress);
    mquant = intmax(scale_quant(curQuant + i_factor*cStress), increment_quant(curQuant));
    break;

  case P_TYPE:
    cStress = (stress_factor - p_min_stress) / (1.0 - p_min_stress);
    if (intra) // since it might be used as a ref, treat it as an I frame block
      mquant = intmax(scale_quant(curQuant + i_factor*cStress), increment_quant(curQuant));
    else
      mquant = intmax(scale_quant(curQuant + p_factor*cStress), increment_quant(curQuant));
    break;

  case B_TYPE:
    cStress = (stress_factor - b_min_stress) / (1.0 - b_min_stress);
    mquant = intmax(scale_quant(curQuant + b_factor*cStress), increment_quant(curQuant));
    break;

  default:
    assert(0);
    break;
  }

  assert(mquant >= curQuant);

  return mquant;
#endif
}

int k9requant::isNotEmpty(RunLevel *blk)
{
  return (blk->level);
}


// return != 0 if error
int k9requant::putAC(int run, int signed_level, int vlcformat)
{
  int level, len;
  const VLCtable *ptab = NULL;

  level = (signed_level<0) ? -signed_level : signed_level; /* abs(signed_level) */

  // assert(!(run<0 || run>63 || level==0 || level>2047));
  if(run<0 || run>63)
  {
    DEBF("illegal run: %d\n", run);
    sliceError++;
    return 1;
  }
  if(level==0 || level>2047)
  {
    DEBF("illegal level: %d\n", level);
    sliceError++;
    return 1;
  }

  len = 0;

  if (run<2 && level<41)
  {
    if (vlcformat)  ptab = &dct_code_tab1a[run][level-1];
    else ptab = &dct_code_tab1[run][level-1];
    len = ptab->len;
  }
  else if (run<32 && level<6)
  {
    if (vlcformat) ptab = &dct_code_tab2a[run-2][level-1];
    else ptab = &dct_code_tab2[run-2][level-1];
    len = ptab->len;
  }

  if (len) /* a VLC code exists */
  {
    putbits(ptab->code, len);
    putbits(signed_level<0, 1); /* sign */
  }
  else
  {
    putbits(1l, 6); /* Escape */
    putbits(run, 6); /* 6 bit code for run */
    putbits(((uint)signed_level) & 0xFFF, 12);
  }

  return 0;
}

// return != 0 if error
int k9requant::putACfirst(int run, int val)
{
  if (run==0 && (val==1 || val==-1))
  {
    putbits(2|((val<0) ? 1 : 0), 2);
    return 0;
  }
  else return putAC(run,val,0);
}

void k9requant::putnonintrablk(RunLevel *blk)
{
  assert(blk->level);

  if (putACfirst(blk->run, blk->level)) return;
  blk++;

  while(blk->level)
  {
    if (putAC(blk->run, blk->level, 0)) return;
    blk++;
  }

  putbits(2,2);
}

void k9requant::putcbp(int cbp)
{
  assert(cbp > 0 && cbp < 64);
  putbits(cbptable[cbp].code,cbptable[cbp].len);
}

void k9requant::putmbtype(int mb_type)
{
  putbits(mbtypetab[picture_coding_type-1][mb_type].code,
          mbtypetab[picture_coding_type-1][mb_type].len);
}

int k9requant::get_macroblock_modes ()
{
  int macroblock_modes;
  const MBtab * tab;

  switch (picture_coding_type)
  {
  case I_TYPE:

    tab = MB_I + UBITS (bit_buf, 1);
    DUMPBITS (bit_buf, bits, tab->len);
    macroblock_modes = tab->modes;

    if ((! (frame_pred_frame_dct)) && (picture_structure == FRAME_PICTURE))
    {
      macroblock_modes |= UBITS (bit_buf, 1) * DCT_TYPE_INTERLACED;
      DUMPBITS (bit_buf, bits, 1);
    }

    return macroblock_modes;

  case P_TYPE:

    tab = MB_P + UBITS (bit_buf, 5);
    DUMPBITS (bit_buf, bits, tab->len);
    macroblock_modes = tab->modes;

    if (picture_structure != FRAME_PICTURE)
    {
      if (macroblock_modes & MACROBLOCK_MOTION_FORWARD)
      {
        macroblock_modes |= UBITS (bit_buf, 2) * MOTION_TYPE_BASE;
        DUMPBITS (bit_buf, bits, 2);
      }
      return macroblock_modes;
    }
    else if (frame_pred_frame_dct)
    {
      if (macroblock_modes & MACROBLOCK_MOTION_FORWARD)
        macroblock_modes |= MC_FRAME;
      return macroblock_modes;
    }
    else
    {
      if (macroblock_modes & MACROBLOCK_MOTION_FORWARD)
      {
        macroblock_modes |= UBITS (bit_buf, 2) * MOTION_TYPE_BASE;
        DUMPBITS (bit_buf, bits, 2);
      }
      if (macroblock_modes & (MACROBLOCK_INTRA | MACROBLOCK_PATTERN))
      {
        macroblock_modes |= UBITS (bit_buf, 1) * DCT_TYPE_INTERLACED;
        DUMPBITS (bit_buf, bits, 1);
      }
      return macroblock_modes;
    }

  case B_TYPE:

    tab = MB_B + UBITS (bit_buf, 6);
    DUMPBITS (bit_buf, bits, tab->len);
    macroblock_modes = tab->modes;

    if (picture_structure != FRAME_PICTURE)
    {
      if (! (macroblock_modes & MACROBLOCK_INTRA))
      {
        macroblock_modes |= UBITS (bit_buf, 2) * MOTION_TYPE_BASE;
        DUMPBITS (bit_buf, bits, 2);
      }
      return macroblock_modes;
    }
    else if (frame_pred_frame_dct)
    {
      /* if (! (macroblock_modes & MACROBLOCK_INTRA)) */
      macroblock_modes |= MC_FRAME;
      return macroblock_modes;
    }
    else
    {
      if (macroblock_modes & MACROBLOCK_INTRA) goto intra;
      macroblock_modes |= UBITS (bit_buf, 2) * MOTION_TYPE_BASE;
      DUMPBITS (bit_buf, bits, 2);
      if (macroblock_modes & (MACROBLOCK_INTRA | MACROBLOCK_PATTERN))
      {
      intra:
        macroblock_modes |= UBITS (bit_buf, 1) * DCT_TYPE_INTERLACED;
        DUMPBITS (bit_buf, bits, 1);
      }
      return macroblock_modes;
    }

  default:
    return 0;
  }

}

int k9requant::get_quantizer_scale ()
{
  int quantizer_scale_code;

  quantizer_scale_code = UBITS (bit_buf, 5);
  DUMPBITS (bit_buf, bits, 5);

  if (!quantizer_scale_code)
  {
    DEBF("illegal quant scale code: %d\n", quantizer_scale_code);
    sliceError++;
    quantizer_scale_code++;
  }

  if (q_scale_type) return non_linear_quantizer_scale[quantizer_scale_code];
  else return quantizer_scale_code << 1;
}

void k9requant::get_motion_delta (const int f_code)
{
#define bit_buf (inbitbuf)
  const MVtab * tab;

  if (bit_buf & 0x80000000)
  {
    COPYBITS (bit_buf, bits, 1);
    return;
  }
  else if (bit_buf >= 0x0c000000)
  {

    tab = MV_4 + UBITS (bit_buf, 4);
    COPYBITS (bit_buf, bits, tab->len + 1);
    if (f_code) COPYBITS (bit_buf, bits, f_code);
    return;
  }
  else
  {

    tab = MV_10 + UBITS (bit_buf, 10);
    COPYBITS (bit_buf, bits, tab->len + 1);
    if (f_code) COPYBITS (bit_buf, bits, f_code);
    return;
  }
}


void k9requant::get_dmv ()
{
  const DMVtab * tab;
  tab = DMV_2 + UBITS (bit_buf, 2);
  COPYBITS (bit_buf, bits, tab->len);
  return;
}

int k9requant::get_coded_block_pattern ()
{
#define bit_buf (inbitbuf)
  const CBPtab * tab;

  if (bit_buf >= 0x20000000)
  {
    tab = CBP_7 + (UBITS (bit_buf, 7) - 16);
    DUMPBITS (bit_buf, bits, tab->len);
    return tab->cbp;
  }
  else
  {
    tab = CBP_9 + UBITS (bit_buf, 9);
    DUMPBITS (bit_buf, bits, tab->len);
    return tab->cbp;
  }
}

int k9requant::get_luma_dc_dct_diff ()
{
#define bit_buf (inbitbuf)
#ifdef CHANGE_BRIGHTNESS
	#define DOBITS(x, y, z) DUMPBITS(x, y, z)
#else
	#define DOBITS(x, y, z) COPYBITS(x, y, z)
#endif
  const DCtab * tab;
  int size;
  int dc_diff;

  if (bit_buf < 0xf8000000)
  {
    tab = DC_lum_5 + UBITS (bit_buf, 5);
    size = tab->size;
    if (size)
    {
      DOBITS (bit_buf, bits, tab->len);
      //dc_diff = UBITS (bit_buf, size) - UBITS (SBITS (~bit_buf, 1), size);
      dc_diff = UBITS (bit_buf, size); if (!(dc_diff >> (size - 1))) dc_diff = (dc_diff + 1) - (1 << size);
      DOBITS (bit_buf, bits, size);
      return dc_diff;
    }
    else
    {
      DOBITS (bit_buf, bits, 3);
      return 0;
    }
  }
  else
  {
    tab = DC_long + (UBITS (bit_buf, 9) - 0x1e0);
    size = tab->size;
    DOBITS (bit_buf, bits, tab->len);
    //dc_diff = UBITS (bit_buf, size) - UBITS (SBITS (~bit_buf, 1), size);
    dc_diff = UBITS (bit_buf, size); if (!(dc_diff >> (size - 1))) dc_diff = (dc_diff + 1) - (1 << size);
    DOBITS (bit_buf, bits, size);
    return dc_diff;
  }
}

int k9requant::get_chroma_dc_dct_diff ()
{
#define bit_buf (inbitbuf)

  const DCtab * tab;
  int size;
  int dc_diff;

  if (bit_buf < 0xf8000000)
  {
    tab = DC_chrom_5 + UBITS (bit_buf, 5);
    size = tab->size;
    if (size)
    {
      COPYBITS (bit_buf, bits, tab->len);
      //dc_diff = UBITS (bit_buf, size) - UBITS (SBITS (~bit_buf, 1), size);
      dc_diff = UBITS (bit_buf, size); if (!(dc_diff >> (size - 1))) dc_diff = (dc_diff + 1) - (1 << size);
      COPYBITS (bit_buf, bits, size);
      return dc_diff;
    }
    else
    {
      COPYBITS (bit_buf, bits, 2);
      return 0;
    }
  }
  else
  {
    tab = DC_long + (UBITS (bit_buf, 10) - 0x3e0);
    size = tab->size;
    COPYBITS (bit_buf, bits, tab->len + 1);
    //dc_diff = UBITS (bit_buf, size) - UBITS (SBITS (~bit_buf, 1), size);
    dc_diff = UBITS (bit_buf, size); if (!(dc_diff >> (size - 1))) dc_diff = (dc_diff + 1) - (1 << size);
    COPYBITS (bit_buf, bits, size);
    return dc_diff;
  }
}


void k9requant::get_intra_block_B14 ()
{
#define bit_buf (inbitbuf)
  int i, li;
  int val;
  const DCTtab * tab;

  li = i = 0;

  while (1)
  {
    if (bit_buf >= 0x28000000)
    {
      tab = DCT_B14AC_5 + (UBITS (bit_buf, 5) - 5);

      i += tab->run;
      if (i >= 64) break;	/* end of block */

    normal_code:
      DUMPBITS (bit_buf, bits, tab->len);
      val = tab->level;
      val = (val ^ SBITS (bit_buf, 1)) - SBITS (bit_buf, 1);
      UPDATE_VAL
      if (val)
      {
        if (putAC(i - li - 1, val, 0)) break;
        li = i;
      }

      DUMPBITS (bit_buf, bits, 1);

      continue;
    }
    else if (bit_buf >= 0x04000000)
    {
      tab = DCT_B14_8 + (UBITS (bit_buf, 8) - 4);

      i += tab->run;
      if (i < 64) goto normal_code;

      /* escape code */
      i += (UBITS (bit_buf, 12) & 0x3F) - 64;
      if (i >= 64)
      {
        sliceError++;
        break;	/* illegal, check needed to avoid buffer overflow */
      }

      DUMPBITS (bit_buf, bits, 12);
      val = SBITS (bit_buf, 12);
      UPDATE_VAL
      if (val)
      {
        if (putAC(i - li - 1, val, 0)) break;
        li = i;
      }

      DUMPBITS (bit_buf, bits, 12);

      continue;
    }
    else if (bit_buf >= 0x02000000)
    {
      tab = DCT_B14_10 + (UBITS (bit_buf, 10) - 8);
      i += tab->run;
      if (i < 64) goto normal_code;
    }
    else if (bit_buf >= 0x00800000)
    {
      tab = DCT_13 + (UBITS (bit_buf, 13) - 16);
      i += tab->run;
      if (i < 64) goto normal_code;
    }
    else if (bit_buf >= 0x00200000)
    {
      tab = DCT_15 + (UBITS (bit_buf, 15) - 16);
      i += tab->run;
      if (i < 64) goto normal_code;
    }
    else
    {
      tab = DCT_16 + UBITS (bit_buf, 16);
      DUMPBITS (bit_buf, bits, 16);
      i += tab->run;
      if (i < 64) goto normal_code;
    }
    sliceError++;
    break;	/* illegal, check needed to avoid buffer overflow */
  }

  COPYBITS (bit_buf, bits, 2);	/* end of block code */
}

void k9requant::get_intra_block_B15 ()
{
#define bit_buf (inbitbuf)
  int i, li;
  int val;
  const DCTtab * tab;

  li = i = 0;

  while (1)
  {
    if (bit_buf >= 0x04000000)
    {
      tab = DCT_B15_8 + (UBITS (bit_buf, 8) - 4);

      i += tab->run;
      if (i < 64)
      {
      normal_code:
        DUMPBITS (bit_buf, bits, tab->len);

        val = tab->level;
        val = (val ^ SBITS (bit_buf, 1)) - SBITS (bit_buf, 1);
        UPDATE_VAL
        if (val)
        {
          if (putAC(i - li - 1, val, 1)) break;
          li = i;
        }

        DUMPBITS (bit_buf, bits, 1);

        continue;
      }
      else
      {
        if (i >= 128) break; /* end of block */

        i += (UBITS (bit_buf, 12) & 0x3F) - 64;

        if (i >= 64)
        {
          sliceError++;
          break;	/* illegal, check against buffer overflow */
        }

        DUMPBITS (bit_buf, bits, 12);
        val = SBITS (bit_buf, 12);
        UPDATE_VAL
        if (val)
        {
          if (putAC(i - li - 1, val, 1)) break;
          li = i;
        }

        DUMPBITS (bit_buf, bits, 12);

        continue;
      }
    }
    else if (bit_buf >= 0x02000000)
    {
      tab = DCT_B15_10 + (UBITS (bit_buf, 10) - 8);
      i += tab->run;
      if (i < 64) goto normal_code;
    }
    else if (bit_buf >= 0x00800000)
    {
      tab = DCT_13 + (UBITS (bit_buf, 13) - 16);
      i += tab->run;
      if (i < 64) goto normal_code;
    }
    else if (bit_buf >= 0x00200000)
    {
      tab = DCT_15 + (UBITS (bit_buf, 15) - 16);
      i += tab->run;
      if (i < 64) goto normal_code;
    }
    else
    {
      tab = DCT_16 + UBITS (bit_buf, 16);
      DUMPBITS (bit_buf, bits, 16);
      i += tab->run;
      if (i < 64) goto normal_code;
    }
    sliceError++;
    break;	/* illegal, check needed to avoid buffer overflow */
  }

  COPYBITS (bit_buf, bits, 4);	/* end of block code */
}

int k9requant::get_non_intra_block_rq (RunLevel *blk)
{
#define bit_buf (inbitbuf)
  //int q = quantizer_scale;
  //int nq = new_quantizer_scale, tst = (nq / q) + ((nq % q) ? 1 : 0);
  int i, li;
  int val;
  const DCTtab * tab;

  li = i = -1;

  if (bit_buf >= 0x28000000)
  {
    tab = DCT_B14DC_5 + (UBITS (bit_buf, 5) - 5);
    goto entry_1;
  }
  else goto entry_2;

  while (1)
  {
    if (bit_buf >= 0x28000000)
    {
      tab = DCT_B14AC_5 + (UBITS (bit_buf, 5) - 5);

    entry_1:
      i += tab->run;
      if (i >= 64)
        break;	/* end of block */

    normal_code:

      DUMPBITS (bit_buf, bits, tab->len);
      val = tab->level;
      val = (val ^ SBITS (bit_buf, 1)) - SBITS (bit_buf, 1);
      UPDATE_VAL
      if (val)
      {
        blk->level = val;
        blk->run = i - li - 1;
        li = i;
        blk++;
      }

      DUMPBITS (bit_buf, bits, 1);

      continue;
    }

  entry_2:
    if (bit_buf >= 0x04000000)
    {
      tab = DCT_B14_8 + (UBITS (bit_buf, 8) - 4);

      i += tab->run;
      if (i < 64) goto normal_code;

      /* escape code */

      i += (UBITS (bit_buf, 12) & 0x3F) - 64;

      if (i >= 64)
      {
        sliceError++;
        break;	/* illegal, check needed to avoid buffer overflow */
      }

      DUMPBITS (bit_buf, bits, 12);
      val = SBITS (bit_buf, 12);
      UPDATE_VAL
      if (val)
      {
        blk->level = val;
        blk->run = i - li - 1;
        li = i;
        blk++;
      }

      DUMPBITS (bit_buf, bits, 12);

      continue;
    }
    else if (bit_buf >= 0x02000000)
    {
      tab = DCT_B14_10 + (UBITS (bit_buf, 10) - 8);
      i += tab->run;
      if (i < 64) goto normal_code;
    }
    else if (bit_buf >= 0x00800000)
    {
      tab = DCT_13 + (UBITS (bit_buf, 13) - 16);
      i += tab->run;
      if (i < 64) goto normal_code;
    }
    else if (bit_buf >= 0x00200000)
    {
      tab = DCT_15 + (UBITS (bit_buf, 15) - 16);
      i += tab->run;
      if (i < 64) goto normal_code;
    }
    else
    {
      tab = DCT_16 + UBITS (bit_buf, 16);
      DUMPBITS (bit_buf, bits, 16);

      i += tab->run;
      if (i < 64) goto normal_code;
    }

    sliceError++;
    break;	/* illegal, check needed to avoid buffer overflow */
  }
  DUMPBITS (bit_buf, bits, 2);	/* dump end of block code */

  blk->level = 0;

  return i;
}


int k9requant::get_non_intra_block_sav (RunLevel *blk, int cc)
{
#define bit_buf (inbitbuf)
  int i, li;
  int val;
  const DCTtab * tab;

#ifdef P_FRAME_NON_INTRA_DROP
	#if (P_FRAME_NON_INTRA_DROP < 0)
  RunLevel *oblk = blk;
#else
  int oval;
#endif
#endif

  li = i = -1;

  if (bit_buf >= 0x28000000)
  {
    tab = DCT_B14DC_5 + (UBITS (bit_buf, 5) - 5);
    goto entry_1;
  }
  else goto entry_2;

  while (1)
  {
    if (bit_buf >= 0x28000000)
    {
      tab = DCT_B14AC_5 + (UBITS (bit_buf, 5) - 5);

    entry_1:
      i += tab->run;
      if (i >= 64)
        break;	/* end of block */

    normal_code:

      DUMPBITS (bit_buf, bits, tab->len);
      val = tab->level;
      val = (val ^ SBITS (bit_buf, 1)) - SBITS (bit_buf, 1);
      SAVE_VAL
      if (li == -1)
      {
        if (abs(val) < abs(mb_sav_lev))
        {
          mb_sav_c = cc;
          mb_sav_lev = val;
          mb_sav_run = i - li - 1;
        }
      }
      UPDATE_VAL
      WRITE_VAL

      DUMPBITS (bit_buf, bits, 1);

      continue;
    }

  entry_2:
    if (bit_buf >= 0x04000000)
    {
      tab = DCT_B14_8 + (UBITS (bit_buf, 8) - 4);

      i += tab->run;
      if (i < 64) goto normal_code;

      /* escape code */

      i += (UBITS (bit_buf, 12) & 0x3F) - 64;

      if (i >= 64)
      {
        sliceError++;
        break;	/* illegal, check needed to avoid buffer overflow */
      }

      DUMPBITS (bit_buf, bits, 12);
      val = SBITS (bit_buf, 12);
      SAVE_VAL
      if (li == -1)
      {
        if (abs(val) < abs(mb_sav_lev))
        {
          mb_sav_c = cc;
          mb_sav_lev = val;
          mb_sav_run = i - li - 1;
        }
      }
      UPDATE_VAL
      WRITE_VAL

      DUMPBITS (bit_buf, bits, 12);

      continue;
    }
    else if (bit_buf >= 0x02000000)
    {
      tab = DCT_B14_10 + (UBITS (bit_buf, 10) - 8);
      i += tab->run;
      if (i < 64) goto normal_code;
    }
    else if (bit_buf >= 0x00800000)
    {
      tab = DCT_13 + (UBITS (bit_buf, 13) - 16);
      i += tab->run;
      if (i < 64) goto normal_code;
    }
    else if (bit_buf >= 0x00200000)
    {
      tab = DCT_15 + (UBITS (bit_buf, 15) - 16);
      i += tab->run;
      if (i < 64) goto normal_code;
    }
    else
    {
      tab = DCT_16 + UBITS (bit_buf, 16);
      DUMPBITS (bit_buf, bits, 16);

      i += tab->run;
      if (i < 64) goto normal_code;
    }

    sliceError++;
    break;	/* illegal, check needed to avoid buffer overflow */
  }
  DUMPBITS (bit_buf, bits, 2);	/* dump end of block code */

#ifdef P_FRAME_NON_INTRA_DROP
	#if (P_FRAME_NON_INTRA_DROP < 0)
  blk -= (int)((blk - oblk) * (stress_factor / P_FRAME_NON_INTRA_DROP));
#ifdef DEMO
  if ((gopCount & 0x7F) < 10) blk = oblk;
#endif
	#endif
#endif

  blk->level = 0;

  return i;
}

#ifdef P_FRAME_NON_INTRA_DROP
int k9requant::get_non_intra_block_drop (RunLevel *blk, int cc)
{
#define bit_buf (inbitbuf)
  int i, li;
  int val;
  const DCTtab * tab;
#if (P_FRAME_NON_INTRA_DROP < 0)
  RunLevel *oblk = blk;
#else
  int oval;
#endif

  li = i = -1;

  if (bit_buf >= 0x28000000)
  {
    tab = DCT_B14DC_5 + (UBITS (bit_buf, 5) - 5);
    goto entry_1;
  }
  else goto entry_2;

  while (1)
  {
    if (bit_buf >= 0x28000000)
    {
      tab = DCT_B14AC_5 + (UBITS (bit_buf, 5) - 5);

    entry_1:
      i += tab->run;
      if (i >= 64)
        break;	/* end of block */

    normal_code:

      DUMPBITS (bit_buf, bits, tab->len);
      val = tab->level;
      val = (val ^ SBITS (bit_buf, 1)) - SBITS (bit_buf, 1);
      SAVE_VAL
      UPDATE_VAL
      WRITE_VAL

      DUMPBITS (bit_buf, bits, 1);

      continue;
    }

  entry_2:
    if (bit_buf >= 0x04000000)
    {
      tab = DCT_B14_8 + (UBITS (bit_buf, 8) - 4);

      i += tab->run;
      if (i < 64) goto normal_code;

      /* escape code */

      i += (UBITS (bit_buf, 12) & 0x3F) - 64;

      if (i >= 64)
      {
        sliceError++;
        break;	/* illegal, check needed to avoid buffer overflow */
      }

      DUMPBITS (bit_buf, bits, 12);
      val = SBITS (bit_buf, 12);
      SAVE_VAL
      UPDATE_VAL
      WRITE_VAL

      DUMPBITS (bit_buf, bits, 12);

      continue;
    }
    else if (bit_buf >= 0x02000000)
    {
      tab = DCT_B14_10 + (UBITS (bit_buf, 10) - 8);
      i += tab->run;
      if (i < 64) goto normal_code;
    }
    else if (bit_buf >= 0x00800000)
    {
      tab = DCT_13 + (UBITS (bit_buf, 13) - 16);
      i += tab->run;
      if (i < 64) goto normal_code;
    }
    else if (bit_buf >= 0x00200000)
    {
      tab = DCT_15 + (UBITS (bit_buf, 15) - 16);
      i += tab->run;
      if (i < 64) goto normal_code;
    }
    else
    {
      tab = DCT_16 + UBITS (bit_buf, 16);
      DUMPBITS (bit_buf, bits, 16);

      i += tab->run;
      if (i < 64) goto normal_code;
    }

    sliceError++;
    break;	/* illegal, check needed to avoid buffer overflow */
  }
  DUMPBITS (bit_buf, bits, 2);	/* dump end of block code */

#if (P_FRAME_NON_INTRA_DROP < 0)
  blk -= (int)((blk - oblk) * (stress_factor / P_FRAME_NON_INTRA_DROP));
#ifdef DEMO
  if ((gopCount & 0x7F) < 10) blk = oblk;
#endif
	#endif

  blk->level = 0;

  return i;
}
#endif

#ifdef CHANGE_BRIGHTNESS
void k9requant::putDC(const sVLCtable *tab, int val)
{
  int absval, size;
  absval = abs(val);
  size = 0;
  while (absval)
  {
    absval >>= 1;
    size++;
  }
  putbits(tab[size].code,tab[size].len);
  if (size!=0)
  {
    if (val>=0) absval = val;
    else absval = val + (1<<size) - 1; /* val + (2 ^ size) - 1 */
    putbits(absval,size);
  }
}
#endif
void k9requant::slice_intra_DCT (const int cc)
{
#ifdef CHANGE_BRIGHTNESS
  if (cc == 0)
  {
    int val;
    int bri = get_luma_dc_dct_diff();
    if (dc_reset)
    {
      val = bri + (128 << intra_dc_precision);
      old_dc_pred = val;

      val += delta_bright << intra_dc_precision;
      if (val > (255 << intra_dc_precision)) val = 255 << intra_dc_precision;
      else if (val < 0) val = 0;

      bri = val - (128 << intra_dc_precision);
      new_dc_pred = val;

      dc_reset = 0;
    }
    else
    {
      val = bri + old_dc_pred;
      old_dc_pred = val;

      val += delta_bright << intra_dc_precision;
      if (val > (255 << intra_dc_precision)) val = 255 << intra_dc_precision;
      else if (val < 0) val = 0;

      bri = val - new_dc_pred;
      new_dc_pred = val;
    }
    putDC(DClumtab, bri);
  }
#else
  if (cc == 0)	get_luma_dc_dct_diff ();
#endif
  else			get_chroma_dc_dct_diff ();

  if (intra_vlc_format) get_intra_block_B15 ();
  else get_intra_block_B14 ();
}

void k9requant::slice_non_intra_DCT (int cur_block)
{
#ifdef P_FRAME_NON_INTRA_DROP
  if (picture_coding_type == P_TYPE)
  {
    if ((h_offset == 0) || (h_offset == horizontal_size_value - 16))
      get_non_intra_block_sav(block[cur_block], cur_block);
    else
      get_non_intra_block_drop(block[cur_block], cur_block);
  }
  else
    get_non_intra_block_rq(block[cur_block]);
#else
  if ((picture_coding_type == P_TYPE) && ((h_offset == 0) || (h_offset == horizontal_size_value - 16)))
    get_non_intra_block_sav(block[cur_block], cur_block);
  else
    get_non_intra_block_rq(block[cur_block]);
#endif
}

void k9requant::motion_fr_frame ( uint f_code[2] )
{
  get_motion_delta (f_code[0]);
  get_motion_delta (f_code[1]);
}

void k9requant::motion_fr_field ( uint f_code[2] )
{
  COPYBITS (bit_buf, bits, 1);
  get_motion_delta (f_code[0]);
  get_motion_delta (f_code[1]);

  COPYBITS (bit_buf, bits, 1);
  get_motion_delta (f_code[0]);
  get_motion_delta (f_code[1]);
}

void k9requant::motion_fr_dmv ( uint f_code[2] )
{
  get_motion_delta (f_code[0]);
  get_dmv ();

  get_motion_delta (f_code[1]);
  get_dmv ();
}

void k9requant::motion_fr_conceal ( )
{
  get_motion_delta (f_code[0][0]);
  get_motion_delta (f_code[0][1]);

  COPYBITS (bit_buf, bits, 1);
}

void k9requant::motion_fi_field ( uint f_code[2] )
{
  COPYBITS (bit_buf, bits, 1);

  get_motion_delta (f_code[0]);
  get_motion_delta (f_code[1]);
}

void k9requant::motion_fi_16x8 ( uint f_code[2] )
{
  COPYBITS (bit_buf, bits, 1);

  get_motion_delta (f_code[0]);
  get_motion_delta (f_code[1]);

  COPYBITS (bit_buf, bits, 1);

  get_motion_delta (f_code[0]);
  get_motion_delta (f_code[1]);
}

void k9requant::motion_fi_dmv ( uint f_code[2] )
{
  get_motion_delta (f_code[0]);
  get_dmv ();

  get_motion_delta (f_code[1]);
  get_dmv ();
}

void k9requant::motion_fi_conceal ()
{
  COPYBITS (bit_buf, bits, 1);

  get_motion_delta (f_code[0][0]);
  get_motion_delta (f_code[0][1]);

  COPYBITS (bit_buf, bits, 1);
}


void  k9requant::putmbdata(int macroblock_modes)
{
  putmbtype(macroblock_modes & 0x1F);

  /*switch (picture_coding_type)
  {
  	case I_TYPE:
  		if ((! (frame_pred_frame_dct)) && (picture_structure == FRAME_PICTURE))
  			putbits(macroblock_modes & DCT_TYPE_INTERLACED ? 1 : 0, 1);
  		break;

  	case P_TYPE:
  		if (picture_structure != FRAME_PICTURE)
  		{
  			if (macroblock_modes & MACROBLOCK_MOTION_FORWARD)
  				putbits((macroblock_modes & MOTION_TYPE_MASK) / MOTION_TYPE_BASE, 2);
  			break;
  		}
  		else if (frame_pred_frame_dct) break;
  		else
  		{
  			if (macroblock_modes & MACROBLOCK_MOTION_FORWARD)
  				putbits((macroblock_modes & MOTION_TYPE_MASK) / MOTION_TYPE_BASE, 2);
  			if (macroblock_modes & (MACROBLOCK_INTRA | MACROBLOCK_PATTERN))
  				putbits(macroblock_modes & DCT_TYPE_INTERLACED ? 1 : 0, 1);
  			break;
  		}

  	case B_TYPE:
  		if (picture_structure != FRAME_PICTURE)
  		{
  			if (! (macroblock_modes & MACROBLOCK_INTRA))
  				putbits((macroblock_modes & MOTION_TYPE_MASK) / MOTION_TYPE_BASE, 2);
  			break;
  		}
  		else if (frame_pred_frame_dct) break;
  		else
  		{
  			if (macroblock_modes & MACROBLOCK_INTRA) goto intra;
  			putbits((macroblock_modes & MOTION_TYPE_MASK) / MOTION_TYPE_BASE, 2);
  			if (macroblock_modes & (MACROBLOCK_INTRA | MACROBLOCK_PATTERN))
  			{
  				intra:
  				putbits(macroblock_modes & DCT_TYPE_INTERLACED ? 1 : 0, 1);
  			}
  			break;
  		}
  }*/

  if (macroblock_modes & (MACROBLOCK_MOTION_FORWARD | MACROBLOCK_MOTION_BACKWARD))
  {
    if (picture_structure == FRAME_PICTURE)
    {
      if (frame_pred_frame_dct == 0)
      {
        putbits((macroblock_modes & MOTION_TYPE_MASK) / MOTION_TYPE_BASE, 2);
      }
    }
    else
    {
      putbits((macroblock_modes & MOTION_TYPE_MASK) / MOTION_TYPE_BASE, 2);
    }
  }
  if ((picture_structure == FRAME_PICTURE) && (frame_pred_frame_dct == 0) && (macroblock_modes & (MACROBLOCK_INTRA | MACROBLOCK_PATTERN)))
  {
    putbits(macroblock_modes & DCT_TYPE_INTERLACED ? 1 : 0, 1);
  }
}

void  k9requant::put_quantiser(int quantiser)
{
  putbits(q_scale_type ? map_non_linear_mquant[quantiser] : quantiser >> 1, 5);
  last_coded_scale = quantiser;
}

void  k9requant::putaddrinc(int addrinc)
{
  mb_out += addrinc;
  //LOGF("mb_out: %i\n", mb_out);
  if (mb_out > (horizontal_size_value >> 4))
  {
    sliceError++;
    //LOGF("mb_out: %i, hsv: %i, curo: %i\n", mb_out, horizontal_size_value, (int)outbytecnt + (wbuf - owbuf));
  }
  while (addrinc>33)
  {
    putbits(0x08,11); /* macroblock_escape */
    addrinc-= 33;
  }
  assert( addrinc >= 1 && addrinc <= 33 );
  putbits(addrinctab[addrinc-1].code,addrinctab[addrinc-1].len);
}

int  k9requant::slice_init (int code)
{
#define bit_buf (inbitbuf)

  int offset;
  const MBAtab * mba;

  mb_out = 0;
  v_offset = (code - 1) * 16;

  quantizer_scale = get_quantizer_scale ();
  new_quantizer_scale = getNewQuant(quantizer_scale, 0);
  put_quantiser(new_quantizer_scale);


  /* ignore intra_slice and all the extra data */
  while (bit_buf & 0x80000000)
  {
    DUMPBITS (bit_buf, bits, 9);
  }

  /* decode initial macroblock address increment */
  offset = 0;
  while (1)
  {
    if (bit_buf >= 0x08000000)
    {
      mba = MBA_5 + (UBITS (bit_buf, 6) - 2);
      break;
    }
    else if (bit_buf >= 0x01800000)
    {
      mba = MBA_11 + (UBITS (bit_buf, 12) - 24);
      break;
    }
    else switch (UBITS (bit_buf, 12))
      {
      case 8:		/* macroblock_escape */
        offset += 33;
        DUMPBITS (bit_buf, bits, 11);
        continue;
      default:	/* error */
        sliceError++;
        return 1;
      }
  }
  mb_add = offset + mba->mba + 1;
  mb_skip = 0;
  COPYBITS (bit_buf, bits, 1);
  DUMPBITS(bit_buf, bits, mba->len);

  h_offset = (offset + mba->mba) << 4;

  while (h_offset - (int)horizontal_size_value >= 0)
  {
    h_offset -= horizontal_size_value;
    v_offset += 16;
  }

  if (v_offset > (vertical_size_value - 16)) return 1;

  return 0;

}

void  k9requant::mpeg2_slice ( const int code )
{
#define bit_buf (inbitbuf)

#ifdef CHANGE_BRIGHTNESS
  dc_reset = 1;
#endif

  if (slice_init (code)) return;

  while (1)
  {
    int macroblock_modes;
    int mba_inc;
    const MBAtab * mba;

    macroblock_modes = get_macroblock_modes ();
    if (macroblock_modes & MACROBLOCK_QUANT) quantizer_scale = get_quantizer_scale ();

    if (macroblock_modes & MACROBLOCK_INTRA)
    {
#ifdef STAT
      if (picture_coding_type == P_TYPE) cnt_p_i++;
      else if (picture_coding_type == B_TYPE) cnt_b_i++;
#endif

      new_quantizer_scale = getNewQuant(quantizer_scale, 1);
      if (last_coded_scale == new_quantizer_scale) macroblock_modes &= 0xFFFFFFEF; // remove MACROBLOCK_QUANT
      else macroblock_modes |= MACROBLOCK_QUANT; //add MACROBLOCK_QUANT

      putaddrinc(mb_add + mb_skip); mb_skip = 0;
      putmbdata(macroblock_modes);
      if (macroblock_modes & MACROBLOCK_QUANT) put_quantiser(new_quantizer_scale);

      if (concealment_motion_vectors)
      {
        if (picture_structure == FRAME_PICTURE) motion_fr_conceal ();
        else motion_fi_conceal ();
      }

      curTable = quant_tables[quant_equ[quantizer_scale]][quant_equ[new_quantizer_scale]];
      if (!curTable)
      {
        /*DEBF("Inv. curTable: qs: %i nqs: %i qe_qs: %i qe_nqs: %i\n",
        		quantizer_scale, new_quantizer_scale,
        		quant_equ[quantizer_scale], quant_equ[new_quantizer_scale]);*/
        curTable = quant_table_id;
      }

      slice_intra_DCT ( 0);
      slice_intra_DCT ( 0);
      slice_intra_DCT ( 0);
      slice_intra_DCT ( 0);
      slice_intra_DCT ( 1);
      slice_intra_DCT ( 2);
    }
    else
    {
      int new_coded_block_pattern = 0;

      // begin saving data
      int batb;
      uint8	n_owbuf[32], *n_wbuf, *o_owbuf, *o_wbuf;
      uint32	n_outbitcnt, n_outbitbuf, o_outbitcnt, o_outbitbuf;

#ifdef CHANGE_BRIGHTNESS
      dc_reset = 1;
#endif

#define PUSH_BIT_IO \
			o_owbuf = owbuf; o_wbuf = wbuf; \
			o_outbitcnt = outbitcnt; o_outbitbuf = outbitbuf; \
			owbuf = wbuf = n_owbuf; \
			outbitcnt = BITS_IN_BUF; outbitbuf = 0;

#define POP_BIT_IO \
			n_wbuf = wbuf; \
			n_outbitcnt = outbitcnt; n_outbitbuf = outbitbuf; \
			owbuf = o_owbuf; wbuf = o_wbuf; \
			outbitcnt = o_outbitcnt; outbitbuf = o_outbitbuf;

      PUSH_BIT_IO

      if (picture_structure == FRAME_PICTURE)
        switch (macroblock_modes & MOTION_TYPE_MASK)
        {
        case MC_FRAME: MOTION_CALL (motion_fr_frame, macroblock_modes); break;
        case MC_FIELD: MOTION_CALL (motion_fr_field, macroblock_modes); break;
        case MC_DMV: MOTION_CALL (motion_fr_dmv, MACROBLOCK_MOTION_FORWARD); break;
        }
      else
        switch (macroblock_modes & MOTION_TYPE_MASK)
        {
        case MC_FIELD: MOTION_CALL (motion_fi_field, macroblock_modes); break;
        case MC_16X8: MOTION_CALL (motion_fi_16x8, macroblock_modes); break;
        case MC_DMV: MOTION_CALL (motion_fi_dmv, MACROBLOCK_MOTION_FORWARD); break;
        }

      POP_BIT_IO

      // end saving data

#ifdef STAT
      if (picture_coding_type == P_TYPE) cnt_p_ni++;
      else if (picture_coding_type == B_TYPE) cnt_b_ni++;
#endif
      new_quantizer_scale = getNewQuant(quantizer_scale, 0);

      if (macroblock_modes & MACROBLOCK_PATTERN)
      {
        int coded_block_pattern = get_coded_block_pattern ();

        mb_sav_lev = 0xFFFF;
        curTable = quant_tables[quant_equ[quantizer_scale]][quant_equ[new_quantizer_scale]];
        if (!curTable)
        {
          /*DEBF("Inv. curTable: qs: %i nqs: %i qe_qs: %i qe_nqs: %i\n",
          		quantizer_scale, new_quantizer_scale,
          		quant_equ[quantizer_scale], quant_equ[new_quantizer_scale]);*/
          curTable = quant_table_id;
        }

        if (coded_block_pattern & 0x20) { slice_non_intra_DCT(0); if (isNotEmpty(block[0])) new_coded_block_pattern |= 0x20; }
      if (coded_block_pattern & 0x10) { slice_non_intra_DCT(1); if (isNotEmpty(block[1])) new_coded_block_pattern |= 0x10; }
      if (coded_block_pattern & 0x08) { slice_non_intra_DCT(2); if (isNotEmpty(block[2])) new_coded_block_pattern |= 0x08; }
      if (coded_block_pattern & 0x04) { slice_non_intra_DCT(3); if (isNotEmpty(block[3])) new_coded_block_pattern |= 0x04; }
      if (coded_block_pattern & 0x02) { slice_non_intra_DCT(4); if (isNotEmpty(block[4])) new_coded_block_pattern |= 0x02; }
      if (coded_block_pattern & 0x01) { slice_non_intra_DCT(5); if (isNotEmpty(block[5])) new_coded_block_pattern |= 0x01; }
#ifdef P_FRAME_NON_INTRA_DROP
        if (picture_coding_type == P_TYPE) new_quantizer_scale = quantizer_scale;
#endif
        if (!new_coded_block_pattern)
        {
          macroblock_modes &= 0xFFFFFFED; // remove MACROBLOCK_PATTERN and MACROBLOCK_QUANT flag
          if (	(picture_coding_type == P_TYPE)
               && !(macroblock_modes & MACROBLOCK_MOTION_FORWARD))
          {
            assert(n_wbuf == n_owbuf);
            assert(n_outbitcnt == BITS_IN_BUF);

            if ((h_offset == 0) || (h_offset == horizontal_size_value - 16))  // can't skip last mb
            {
              // we can't transmit mv (0,0) since PMV could be different than 0 for last block
              // so we transmit the single smallest coeff. instead unrequantised
              // anyway this is likely to take no more bit than transmiting a null mv....

              assert((mb_sav_lev) && (mb_sav_lev != 0xFFFF));

              new_coded_block_pattern = 1 << (5 - mb_sav_c);
              macroblock_modes |= MACROBLOCK_PATTERN;
              new_quantizer_scale = quantizer_scale;
              block[mb_sav_c][0].run = mb_sav_run; block[mb_sav_c][0].level = mb_sav_lev;
              block[mb_sav_c][1].run = 0; block[mb_sav_c][1].level = 0;
            }
            else
            {
              mb_skip += mb_add;
              goto skip_mb;
            }
          }
        }
      }

      if (last_coded_scale == new_quantizer_scale) macroblock_modes &= 0xFFFFFFEF; // remove MACROBLOCK_QUANT
      else if (macroblock_modes & MACROBLOCK_PATTERN) macroblock_modes |= MACROBLOCK_QUANT; //add MACROBLOCK_QUANT
      assert( (macroblock_modes & MACROBLOCK_PATTERN) || !(macroblock_modes & MACROBLOCK_QUANT) );

      putaddrinc(mb_add + mb_skip); mb_skip = 0;
      putmbdata(macroblock_modes);
      if (macroblock_modes & MACROBLOCK_QUANT) put_quantiser(new_quantizer_scale);

      // put saved motion data...
      for (batb = 0; batb < (n_wbuf - n_owbuf); batb++) putbits(n_owbuf[batb], 8);
      putbits(n_outbitbuf, BITS_IN_BUF - n_outbitcnt);
      // end saved motion data...

      if (macroblock_modes & MACROBLOCK_PATTERN)
      {
        putcbp(new_coded_block_pattern);

        if (new_coded_block_pattern & 0x20) putnonintrablk(block[0]);
        if (new_coded_block_pattern & 0x10) putnonintrablk(block[1]);
        if (new_coded_block_pattern & 0x08) putnonintrablk(block[2]);
        if (new_coded_block_pattern & 0x04) putnonintrablk(block[3]);
        if (new_coded_block_pattern & 0x02) putnonintrablk(block[4]);
        if (new_coded_block_pattern & 0x01) putnonintrablk(block[5]);
      }
    }

  skip_mb:

    NEXT_MACROBLOCK;

    mba_inc = 0;
    while (1)
    {
      if (bit_buf >= 0x10000000)
      {
        mba = MBA_5 + (UBITS (bit_buf, 5) - 2);
        break;
      }
      else if (bit_buf >= 0x03000000)
      {
        mba = MBA_11 + (UBITS (bit_buf, 11) - 24);
        break;
      }
      else
        switch (UBITS (bit_buf, 11))
        {
        case 8:		/* macroblock_escape */
          mba_inc += 33;
          DUMPBITS (bit_buf, bits, 11);
          continue;
        default:	/* end of slice, or error */
          //LOGF("hoffset: %i, hsv: %i, curo: %i\n", h_offset, horizontal_size_value, (int)outbytecnt + (wbuf - owbuf));
          if (h_offset != 0)
            sliceError++;
          return;
        }
    }
    DUMPBITS (bit_buf, bits, mba->len); //PPP

    mba_inc += mba->mba;
    mb_add = mba_inc + 1;

#ifdef CHANGE_BRIGHTNESS
    if (mba_inc) dc_reset = 1;
#endif

  if (mba_inc) do { NEXT_MACROBLOCK; }
      while (--mba_inc);
  }

}

/////---- end ext mpeg code

void  k9requant::run ()
{
  uint8 ID, found;
  int64 greedyFactor, greedyFactor2;
  int i;

#ifdef DEMO
  gopCount = 0;
#endif

#ifdef LOG_RATE_CONTROL
  LOG_FILE = fopen("Logfile.txt", "w");
#endif

#ifdef STAT
  ori_i = ori_p = ori_b = 0;
  new_i = new_p = new_b = 0;
  cnt_i = cnt_p = cnt_b = 0;
  cnt_p_i = cnt_p_ni = 0;
  cnt_b_i = cnt_b_ni = 0;
#endif

#ifdef USE_FD
  if (argc < 3) { USAGE }
  ifd = fopen(argv[argc - 2], "rb");
  ofd = fopen(argv[argc - 1], "wb");
  if (!ifd)
  {
    LOGF("Bad input path! (%s)\n", argv[argc - 2]);
    return 2;
  }
  if (!ofd)
  {
    LOGF("Bad output path! (%s)\n", argv[argc - 1]);
    return 2;
  }
  argc -= 2;
#endif

#ifndef THREAD
  rbuf = cbuf = orbuf = malloc(BUF_SIZE);
  wbuf = owbuf = malloc(BUF_SIZE);
  inbytecnt = outbytecnt = 0;
  eof = 0;
#endif

  validPicHeader = 0;
  validSeqHeader = 0;
  validExtHeader = 0;

#ifndef THREAD
  // argument parsing
#ifdef CHANGE_BRIGHTNESS
  if (argc < 5) { USAGE }
  delta_bright = atoi(argv[4]);
#else
  if (argc < 4) { USAGE }
#endif
  fact_x = atof(argv[1]);
  sscanf(argv[3], "%lld", &orim2vsize);
#endif

#ifdef THREAD
	orim2vsize=rqt_visize;
	  fact_x     = rqt_fact;
#endif

  greedyFactor = orim2vsize / 100;
  greedyFactor2 = orim2vsize / 50;

#ifndef THREAD
  if (fact_x <= 1.0)
  {
    unsigned char buf[4096];

    while(1)
    {
      int i = read(0, buf, 4096);
      if (i > 0) write(1, buf, i);
      else return 0;
    }
  }
  
#endif
   if (fact_x > 10.0) fact_x = 10.0;

  // factor and stresses setting
  initRequant();

 // fill quant table
  // id table
  for (i = -2048; i <= 2047; i++) quant_table_id[i] = i;

  // other tables
  for (i = 0; i < 42; i++)
  {
    int q = quantisers[i];
    int j;

    for (j = i + 1; j < 42; j++)
    {
      int nq = quantisers[j];
      int k;
      short *cTab = quant_tables[quant_equ[q]][quant_equ[nq]];

      for (k = -2048; k <= 2047; k++)
      {
        int ov = k*q;
        int t = ov / nq;

        if (fact_x <= max_alt_table)
        {
          int t2, t3;
          int d, d2, d3;
          int nv, nv2, nv3;

          t2 = t + 1;
          t3 = t - 1;

          nv = t * nq;
          nv2 = t2 * nq;
          nv3 = t3 * nq;

          d = abs(nv - ov);
          d2 = abs(nv2 - ov);
          d3 = abs(nv3 - ov);

          if (d2 < d) { d = d2; t = t2; }
          if (d3 < d) t = t3;
        }

        if (t > 2047) t = 2047;
        else if (t < -2048) t = -2048;

        cTab[k] = t;
      }
    }
  }

#ifndef THREAD
  LOG("M2VRequantiser by Makira.\n");
#ifdef WIN
  fprintf(stderr, "Using %f as factor, %lld as m2v size.\n", fact_x, orim2vsize);
#else
  LOGF("Using %f as factor, %lld as m2v size.\n", fact_x, orim2vsize);
#endif
#endif

  // recoding
  while(1)
  {
    // get next start code prefix
    found = 0;
    while (!found)
    {
#ifndef REMOVE_BYTE_STUFFING
      LOCK(3)
#else
      LOCK(8)
      if		(		(cbuf[7] == 0) && (cbuf[6] == 0) && (cbuf[5] == 0) && (cbuf[4] == 0)
           &&  (cbuf[3] == 0) && (cbuf[2] == 0) && (cbuf[1] == 0) && (cbuf[0] == 0) ) { SEEKR(1) }
      else
#endif
      if ( (cbuf[0] == 0) && (cbuf[1] == 0) && (cbuf[2] == 1) ) found = 1; // start code !
    else { COPY(1) } // continue search
    }
    COPY(3)

    // get start code
    LOCK(1)
    ID = cbuf[0];
    COPY(1)

    if (ID == 0x00) // pic header
    {
      LOCK(4)
      picture_coding_type = (cbuf[1] >> 3) & 0x7;
      if (picture_coding_type < 1 || picture_coding_type > 3)
      {
        DEBF("illegal picture_coding_type: %i\n", picture_coding_type);
        validPicHeader = 0;
      }
      else
      {
        validPicHeader = 1;
        cbuf[1] |= 0x7; cbuf[2] = 0xFF; cbuf[3] |= 0xF8; // vbv_delay is now 0xFFFF
      }

      validExtHeader = 0;

      COPY(4)
    }
    else if (ID == 0xB3) // seq header
    {
      LOCK(8)
      horizontal_size_value = (cbuf[0] << 4) | (cbuf[1] >> 4);
      vertical_size_value = ((cbuf[1] & 0xF) << 8) | cbuf[2];
      if (	horizontal_size_value > 720 || horizontal_size_value < 352
           ||  vertical_size_value > 576 || vertical_size_value < 480
           || (horizontal_size_value & 0xF) || (vertical_size_value & 0xF))
      {
        DEBF("illegal size, hori: %i verti: %i\n", horizontal_size_value, vertical_size_value);
        validSeqHeader = 0;
      }
      else
        validSeqHeader = 1;

      validPicHeader = 0;
      validExtHeader = 0;

      COPY(8)
    }
    else if (ID == 0xB5) // extension
    {
      LOCK(1)
      if ((cbuf[0] >> 4) == 0x8) // pic coding ext
      {
        LOCK(5)

        f_code[0][0] = (cbuf[0] & 0xF) - 1;
        f_code[0][1] = (cbuf[1] >> 4) - 1;
        f_code[1][0] = (cbuf[1] & 0xF) - 1;
        f_code[1][1] = (cbuf[2] >> 4) - 1;

        intra_dc_precision = (cbuf[2] >> 2) & 0x3;
        picture_structure = cbuf[2] & 0x3;
        frame_pred_frame_dct = (cbuf[3] >> 6) & 0x1;
        concealment_motion_vectors = (cbuf[3] >> 5) & 0x1;
        q_scale_type = (cbuf[3] >> 4) & 0x1;
        intra_vlc_format = (cbuf[3] >> 3) & 0x1;
        alternate_scan = (cbuf[3] >> 2) & 0x1;

        if (	(f_code[0][0] > 8 && f_code[0][0] < 14)
             ||  (f_code[0][1] > 8 && f_code[0][1] < 14)
             ||  (f_code[1][0] > 8 && f_code[1][0] < 14)
             ||  (f_code[1][1] > 8 && f_code[1][1] < 14)
             ||  picture_structure == 0)
        {
          DEBF("illegal ext, f_code[0][0]: %i f_code[0][1]: %i f_code[1][0]: %i f_code[1][1]: %i picture_structure:%i\n",
               f_code[0][0], f_code[0][1], f_code[1][0], f_code[1][1], picture_structure);
          validExtHeader = 0;
        }
        else
          validExtHeader = 1;
        COPY(5)
      }
      else
      {
        COPY(1)
      }
    }
    else if (ID == 0xB8) // gop header
    {
      LOCK(4)
      COPY(4)

#ifdef DEMO
      gopCount++;
#endif
    }
    else if ((ID >= 0x01) && (ID <= 0xAF) && validPicHeader && validSeqHeader && validExtHeader) // slice
    {
      uint8 *outTemp = wbuf, *inTemp = cbuf;
      int64 threshold;

      bytediff = (outbytecnt + (wbuf - owbuf)) - ((inbytecnt - (rbuf - cbuf)) / fact_x);


      if (inbytecnt < greedyFactor2) threshold = inbytecnt >> 1;
      else if (orim2vsize - inbytecnt < greedyFactor2) threshold = (orim2vsize - inbytecnt) >> 1;
      else threshold = greedyFactor;

      if (threshold < 1024) threshold = 1024;

      stress_factor = (float)(bytediff + threshold) / (float)(threshold << 1);
      if (stress_factor > 1.0f)		stress_factor = 1.0f;
      else if (stress_factor < 0.0f)	stress_factor = 0.0f;
 

#ifdef LOG_RATE_CONTROL
      /*fprintf(LOG_FILE, "%f%%: Requested: %f  Current: %f  Delta: %lld   Threshold: %f  Stress: %f\n",
      	(float)(100.0f*inbytecnt)/orim2vsize,		//	percent
      	(float)fact_x,								//	requested
      	(float)(inbytecnt - (rbuf - cbuf))/(float)(outbytecnt + (wbuf - owbuf)), //	current
      	(long long)bytediff,						//	delta
      	(float)threshold, 							//	threshold
      	stress_factor								//	Stress
      );*/
      fprintf(LOG_FILE, "inb: %.0f inb_c: %.0f oub: %.0f oub_c: %.0f cur: %.3f dif: %.0f thr: %.0f str: %.03f\n",
              (float)inbytecnt,
              (float)(inbytecnt - (rbuf - cbuf)),
              (float)outbytecnt,
              (float)(outbytecnt + (wbuf - owbuf)),
              (float)(inbytecnt - (rbuf - cbuf))/(float)(outbytecnt + (wbuf - owbuf)),
              (float)bytediff,
              (float)threshold,
              (float)stress_factor );
#endif


#ifndef CHANGE_BRIGHTNESS
      if (	((picture_coding_type == I_TYPE) && ( stress_factor > i_min_stress))
           ||  ((picture_coding_type == P_TYPE) && ( stress_factor > p_min_stress))
           ||  ((picture_coding_type == B_TYPE) && ( stress_factor > b_min_stress))
#ifdef DEMO
           ||	((gopCount & 0x7F) < 10)
#endif
         )
#endif
      {
        // init error
        sliceError = 0;

        // init bit buffer
        inbitbuf = 0; inbitcnt = 0;
        outbitbuf = 0; outbitcnt = BITS_IN_BUF;

        // get 32 bits
        Refill_bits();
        Refill_bits();
        Refill_bits();
        Refill_bits();

        // begin bit level recoding
        mpeg2_slice(ID);
        flush_read_buffer();
        flush_write_buffer();
        // end bit level recoding

#ifndef CHANGE_BRIGHTNESS
        if ((wbuf - outTemp > cbuf - inTemp) || (sliceError > MAX_ERRORS)) // yes that might happen, rarely
        {
#ifndef NDEBUG
          if (sliceError > MAX_ERRORS)
          {
            DEBF("sliceError (%i) > MAX_ERRORS (%i)\n", sliceError, MAX_ERRORS);
          }
#endif

          // in this case, we'll just use the original slice !
          memcpy(outTemp, inTemp, cbuf - inTemp);
          wbuf = outTemp + (cbuf - inTemp);

          // adjust outbytecnt
          outbytecnt -= (wbuf - outTemp) - (cbuf - inTemp);
        }
#endif

#ifdef STAT
	#ifdef LOG_RATE_CONTROL
        if (picture_coding_type == I_TYPE) fprintf(LOG_FILE, "-I-\n");
#endif
        switch(picture_coding_type)
        {
        case I_TYPE:
          ori_i += cbuf - inTemp;
          new_i += (wbuf - outTemp > cbuf - inTemp) ? (cbuf - inTemp) : (wbuf - outTemp);
          cnt_i ++;
          break;

        case P_TYPE:
          ori_p += cbuf - inTemp;
          new_p += (wbuf - outTemp > cbuf - inTemp) ? (cbuf - inTemp) : (wbuf - outTemp);
          cnt_p ++;
          break;

        case B_TYPE:
          ori_b += cbuf - inTemp;
          new_b += (wbuf - outTemp > cbuf - inTemp) ? (cbuf - inTemp) : (wbuf - outTemp);
          cnt_b ++;
          break;

        default:
          assert(0);
          break;
        }
#endif
      }
    }

#ifndef NDEBUG
    if ((ID >= 0x01) && (ID <= 0xAF) && (!validPicHeader || !validSeqHeader || !validExtHeader))
    {
      if (!validPicHeader) DEBF("missing pic header (%02X)\n", ID);
      if (!validSeqHeader) DEBF("missing seq header (%02X)\n", ID);
      if (!validExtHeader) DEBF("missing ext header (%02X)\n", ID);
    }
#endif
if (rbuf - orbuf > MAX_READ) { MOV_READ }
   if (wbuf - owbuf > MIN_WRITE) { WRITE }
  }


#ifdef LOG_RATE_CONTROL
  fclose(LOG_FILE);
#endif
  rqt_run=false;
  // keeps gcc happy
  return ;
}

void k9requant::initvar()
{
	cbuf = NULL;
	rbuf = NULL;
	wbuf = NULL;
	orbuf = NULL;
	owbuf = NULL;
	inbitcnt = outbitcnt = 0;
	inbitbuf =  outbitbuf = 0;
	inbytecnt = outbytecnt = 0;
	fact_x = 0;
	mloka1 = mloka2 = eof = 0;
	orim2vsize = 0;
	bytediff = 0;
	stress_factor = 0;
	i_factor = 0;
	p_factor = 0;
	b_factor = 0;
	i_min_stress = 0;
	p_min_stress = 0;
	b_min_stress = 0;
	quant_table_id = &quant_table_id_data[2048];
	horizontal_size_value = 0;
	vertical_size_value = 0;

	picture_coding_type = 0;

	memset( f_code,0 , sizeof(f_code));
	intra_dc_precision = 0;
	picture_structure = 0;
	frame_pred_frame_dct = 0;
	concealment_motion_vectors = 0;
	q_scale_type = 0;
	intra_vlc_format = 0;
	alternate_scan = 0;

	validPicHeader = 0;
	validSeqHeader = 0;
	validExtHeader = 0;
	sliceError = 0;

	quantizer_scale = 0;
	new_quantizer_scale = 0;
	last_coded_scale = 0;
	h_offset = v_offset = 0;
	mb_skip = mb_add = 0;
	mb_out = 0;

	mb_sav_run = mb_sav_lev = mb_sav_c = 0;
	curTable = NULL;
	memset( block, 0, sizeof(block));
}



void k9requant::initRequant() {
  if (fact_x <= 1.0)
  {
    i_factor = i_factors[0];
    p_factor = p_factors[0];
    b_factor = b_factors[0];
    i_min_stress = i_min_stresses[0];
    p_min_stress = p_min_stresses[0];
    b_min_stress = b_min_stresses[0];
  }
  else if (fact_x >= 10.0)
  {
    i_factor = i_factors[2];
    p_factor = p_factors[2];
    b_factor = b_factors[2];
    i_min_stress = i_min_stresses[2];
    p_min_stress = p_min_stresses[2];
    b_min_stress = b_min_stresses[2];
  }
  else if (fact_x <= 3.0) // 1.0 .. 3.0
  {
    double inter = (fact_x - 1.0)/(3.0 - 1.0);
    i_factor = i_factors[0] + inter * (i_factors[1] - i_factors[0]);
    p_factor = p_factors[0] + inter * (p_factors[1] - p_factors[0]);
    b_factor = b_factors[0] + inter * (b_factors[1] - b_factors[0]);
    i_min_stress = i_min_stresses[0] + inter * (i_min_stresses[1] - i_min_stresses[0]);
    p_min_stress = p_min_stresses[0] + inter * (p_min_stresses[1] - p_min_stresses[0]);
    b_min_stress = b_min_stresses[0] + inter * (b_min_stresses[1] - b_min_stresses[0]);
  }
  else // 3.0 .. 10.0
  {
    double inter = (fact_x - 3.0)/(10.0 - 3.0);
    i_factor = i_factors[1] + inter * (i_factors[2] - i_factors[1]);
    p_factor = p_factors[1] + inter * (p_factors[2] - p_factors[1]);
    b_factor = b_factors[1] + inter * (b_factors[2] - b_factors[1]);
    i_min_stress = i_min_stresses[1] + inter * (i_min_stresses[2] - i_min_stresses[1]);
    p_min_stress = p_min_stresses[1] + inter * (p_min_stresses[2] - p_min_stresses[1]);
    b_min_stress = b_min_stresses[1] + inter * (b_min_stresses[2] - b_min_stresses[1]);
  }

  /*LOGF(   "i_factor: %i p_factor: %i b_factor: %i\n"
  		"i_min_stress: %.02f p_min_stress: %.02f b_min_stress: %.02f\n",
  		i_factor, p_factor, b_factor,
  		i_min_stress, p_min_stress, b_min_stress);*/

 

}

bool k9requant::lock( int64 x) {
  if (unlikely ((x) > (rbuf - cbuf))) 
  { 
    if (likely (wbuf))
    {
      QMutexLocker locker( &mutw );
      //mutw.lock();
      rqt_wcnt = wbuf - owbuf;
      condw.wakeAll();
      //mutw.unlock();
    }
    //mutr.lock();
    QMutexLocker locker( &mutr );
    while (!rqt_rcnt)
    {
      condr.wait( &mutr);
      if (rqt_stop==true) {
	//mutr.unlock();
	return false;
      }
    }
    cbuf       = rqt_rptr;    //src buffer
    rbuf =orbuf  = cbuf;
    rbuf      += rqt_rcnt + 3;  // end of src buffer
    rqt_rcnt   = 0;
    owbuf      = rqt_wptr;   // dest buffer
    inbytecnt  = rqt_inbytes;
    outbytecnt = rqt_outbytes;
    orim2vsize = rqt_visize;
    //mutr.unlock();
    wbuf = owbuf;
    if (    fact_x    <  rqt_fact) {
	fact_x=rqt_fact;
	initRequant();
    }
    fact_x=rqt_fact;
  }

  return true;

}



