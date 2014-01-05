/***************************************************************************
 *   Copyright (C) 2005 by Jean-Michel Petit                               *
 *   k9copy@free.fr                                                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef K9IFO2_H
#define K9IFO2_H
#include "k9common.h"
#include "k9dvdread.h"
#include <QFile>

/**
@author Jean-Michel Petit
*/
class k9Ifo2  {
public:
    k9Ifo2(k9DVDRead *_dvdread);

    ~k9Ifo2();

    void setDevice(QString &_device);
    void setOutput(QString &_output);
    ifo_handle_t * openIFO(int _num);
    void closeIFO();
    void saveIFO();
    ifo_handle_t * getIFO();
    static  void navRead_PCI(pci_t *pci, uchar *buffer);
    void setVTSVobs(uint32_t _sector);
    uint32_t getSize();
    int test;


protected:
    QString device;
    QString output;
    ifo_handle_t *_ifo;
    int numIfo;
private:
    QFile* outputFile;
    long cellSize;
    uchar * bufCopy,*m_buffer;
    int bufCopySize;
    k9DVDRead *m_dvd;
    uint m_position;
    uint32_t m_VTSVobs,m_size;
private:
    int round(int);
    void updateVMG(uchar * _buffer);
    void updateFP_PGC(uchar * _buffer);
    void updatePGC_COMMAND_TBL(uchar  *_buffer,pgc_command_tbl_t *_cmd_tbl,int offset);
    void updatePGC_PROGRAM_MAP(uchar *_buffer,pgc_program_map_t *_program_map, int _nr,int _offset);
    void updatePGC_CELL_PLAYBACK_TBL(uchar *_buffer, cell_playback_t *_cell_playback,int _nr, int _offset);
    void updatePGC_CELL_POSITION_TBL(uchar *_buffer,cell_position_t *_cell_position,int _nr, int _offset);
    void updatePGC(uchar *_buffer,pgc_t *_pgc,int _offset);
    void updateTT_SRPT(uchar *_buffer);
    void updatePGCI_UT(uchar *_buffer);
    void updatePTL_MAIT(uchar *_buffer);
    void updateVTS_ATRT(uchar *_buffer);
    void updateTXTDT_MGI(uchar * _buffer);
    void updateC_ADT(uchar * _buffer, bool _isMenu);
    void updateVOBU_ADMAP(uchar * _buffer, bool _isMenu);
    void updatePGCIT(uchar *_buffer);
    void updatePGCIT_internal(uchar *_buffer, pgcit_t *_pgcit, int offset);
    void updateVTS(uchar *_buffer);
    void updateVTS_PTT_SRPT(uchar *_buffer);
    void updateVTS_TMAPT(uchar *_buffer) ;
    void updateC_ADT_Internal(uchar *_buffer,c_adt_t *_c_adt,int _sector);
    void updateVOBU_ADMAP_Internal(uchar *_buffer,vobu_admap_t *_vobu_admap,int _sector);
    void checkBuffer(QString lib,uchar *_buffer);
};


#endif
