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
#ifndef K9CELLCOPYLIST_H
#define K9CELLCOPYLIST_H

#include "k9common.h"
#include "k9dvd.h"
#include "k9cell.h"
#include "k9dvdread.h"

class k9CellCopyVTS {
private:
    uint num;
    uint64_t size;
public:
    k9CellCopyVTS (int _num) {
        num=_num;
        size=0;
    };
    uint getnum() ;
    void addsize(uint32_t _size) ;
    uint64_t getsize() ;
};


/**
@author Jean-Michel PETIT
*/

class k9CellVTSList : public QList<k9CellCopyVTS*> {
public:
    static bool compareItems ( k9CellCopyVTS *item1,k9CellCopyVTS *item2 );
};


class k9CellCopyList : public QList<k9Cell*> {
public:
    k9CellCopyList(k9DVDRead  * _dvdHandle,k9DVD *_DVD);
    double getfactor(bool _withMenus,bool _streams);
    double gettotalSize();
    double getforcedSize(bool _withFactor);
    double getMinFactor(bool _withMenus);
    k9CellVTSList VTSList;
    ~k9CellCopyList();

    void addInbytes(const uint64_t& _value) {
        m_inbytes += _value;
    }


    void addOutbytes(const uint64_t& _value) {
        m_outbytes += _value;
    }

    void addFrcinbytes(const uint64_t& _value) {
        m_frcinbytes += _value;
    }

    void addFrcoutbytes(const uint64_t& _value) {
        m_frcoutbytes += _value;
    }

private:
    k9DVD *DVD;
    k9DVDRead *dvdHandle;
    uint64_t m_inbytes,m_outbytes;
    uint64_t m_frcinbytes,m_frcoutbytes;

    void fill();
    k9Cell *addCell(int _VTS,int _pgc,int _id,uint32_t startSector,uint32_t lastSector,uchar _angleBlock);
    bool checkSelected(k9Cell *_cell);
    void addStreams(k9DVDTitle *_title,k9Cell *_cell);
    void setVTS(uint _numVTS,uint32_t _size);

    //QPtrList <k9CellCopyVTS> VTSList;
    void sortVTSList();
    void clearList();

};

#endif
