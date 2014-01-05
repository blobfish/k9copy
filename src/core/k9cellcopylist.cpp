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
#include "k9cellcopylist.h"
#include "k9dvdtitle.h"
#include "k9ifo2.h"

bool k9CellVTSList::compareItems ( k9CellCopyVTS *item1, k9CellCopyVTS *item2 ) {
    k9CellCopyVTS * it1,*it2;
    it1=item1;
    it2=item2;
    if (it1->getsize() >it2->getsize())
        return false;
    else if (it1->getsize() <it2->getsize())
        return true;
    else {
        if (it1->getnum()>it2->getnum())
            return false;
        else
            return true;
    }
}

uint k9CellCopyVTS::getnum() {
    return num;
}
void k9CellCopyVTS::addsize(uint32_t _size) {
    size+=_size;
}
uint64_t k9CellCopyVTS::getsize() {
    return size;
}

void k9CellCopyList::clearList() {
    while (!isEmpty()) delete takeFirst();
}

k9CellCopyList::k9CellCopyList(k9DVDRead * _dvdHandle,k9DVD *_DVD) {
    //setAutoDelete(true);
    DVD=_DVD;
    dvdHandle=_dvdHandle;
    fill();
}

k9CellCopyList::~k9CellCopyList() {
    clearList();
}

/*!
    \fn k9CellCopyList::fill()
 */
void k9CellCopyList::fill() {
//    k9Ifo2 kifo(dvdHandle) ,kifoZero(dvdHandle);
    ifo_handle_t *hifo,*hifoZero;
    k9Ifo2 *kifo,*kifoZero;
    kifoZero=dvdHandle->getIfo(0);
    hifoZero=kifoZero->getIFO();

//    kifoZero.openIFO( 0);

    int nrTS= hifoZero->vmgi_mat->vmg_nr_of_title_sets;

    for (int iTS=1 ; iTS<=nrTS;iTS++) {
        //kifo.openIFO( iTS);
        kifo=dvdHandle->getIfo(iTS);
        hifo=kifo->getIFO();
        c_adt_t *c_adt = hifo->vts_c_adt;
        uint32_t length = c_adt->last_byte + 1 - C_ADT_SIZE;
        cell_adr_t *ptr;
        ptr= c_adt->cell_adr_table;
        uint32_t imax=length/sizeof(cell_adr_t);

        for (uint32_t i = 0; i <imax; i++) {
            uchar angleBlock=angleNone;
            k9Cell *cell=NULL;
            cell=addCell(iTS,0,i+1,ptr[i].start_sector,ptr[i].last_sector,angleBlock);
        }
        //kifo.closeIFO();
    }
    //kifoZero.closeIFO();
    sortVTSList();
    m_frcinbytes=m_frcoutbytes=m_inbytes=m_outbytes=0;
}


k9Cell *k9CellCopyList::addCell(int _VTS,int _pgc,int _id,uint32_t startSector,uint32_t lastSector,uchar _angleBlock) {
    bool bInsert=false;
    bool bFound=false;
    uint position=0;
    k9Cell *cell =NULL;

    for (int i=0;i< count() && !bFound && !bInsert;i++) {
        cell=(k9Cell*) at(i);
        if (cell->startSector ==startSector && cell->vts==_VTS) {
            bFound=true;
            if (cell->lastSector!=lastSector  )
                qDebug("last sector doesn't match");

        }
        if (!bFound && (_VTS <cell->vts  || (startSector<cell->startSector  && cell->vts ==_VTS ) )) {
            bInsert=true;
            position=i;
        }

    }

    if (!bFound) {
        cell = new k9Cell();
        cell->vts=_VTS;
        cell->startSector=startSector;
        cell->lastSector=lastSector;
        cell->pgc=_pgc;
        cell->id=_id;
        cell->angleBlock=_angleBlock;
        cell->selected=checkSelected(cell);

        if (bInsert)
            insert(position,cell);
        else
            append(cell);

        uint32_t isize;
        isize= cell->selected ? ((lastSector-startSector)*2048):2;
        setVTS(_VTS,isize);
    }
    return(cell);
}
void k9CellCopyList::setVTS(uint _numVTS,uint32_t _size) {
    bool bfound=false;
    for (int iVTS=0; iVTS<VTSList.count();iVTS++) {
        k9CellCopyVTS *VTS=VTSList.at(iVTS);
        if (VTS->getnum()==_numVTS) {
            VTS->addsize(_size);
            bfound=true;
        }
    }
    if (!bfound) {
        k9CellCopyVTS * VTS= new k9CellCopyVTS(_numVTS);
        VTS->addsize(_size);
        VTSList.append(VTS);
    }
}

void k9CellCopyList::sortVTSList() {
    qSort(VTSList.begin(),VTSList.end(),k9CellVTSList::compareItems);
}


void k9CellCopyList::addStreams(k9DVDTitle *_title,k9Cell *_cell) {
    k9DVDSubtitle *l_sub;
    k9DVDAudioStream *l_auds;
    for (int i=0;i<_title->getaudioStreamCount();i++) {
        l_auds=_title->getaudioStream(i);
        if (l_auds->getselected()) {
            bool found=false;
            foreach (int audio,_cell->audio) {
                if (audio == l_auds->getID())
                    found=true;
            }
            if (!found)
                _cell->audio.append(l_auds->getID());
        }
    }

    for (int i=0;i<_title->getsubPictureCount();i++) {
        QString c;
        l_sub=_title->getsubtitle(i);
        if (l_sub->getselected()) {
            bool found=false;
            foreach ( int sub,_cell->subpicture) {
                if (l_sub->getID().contains(sub))
                    found=true;
            }
            if (!found)
                _cell->subpicture+=l_sub->getID();
        }
    }
}

/*!
    \fn k9CellCopyList::checkSelected(k9Cell *_cell)
 */
bool k9CellCopyList::checkSelected(k9Cell *_cell) {
    bool selected=false;
    for (int i=0; i< DVD->gettitleCount();i++) {
        k9DVDTitle *title=DVD->gettitle(i);
//FACTOR        if ( title->getVTS()==_cell->vts  && title ->isSelected()) {
        if ( title->getVTS()==_cell->vts) { // {  && title ->isSelected()) {
            for (int j=0; j <title->getchapterCount();j++) {
                k9DVDChapter * chapter= title->getChapter(j);
                if (_cell->startSector >= chapter->getstartSector() && _cell->startSector<=chapter->getendSector()) {
                    //add a reference to the title in the titles list from the cell
                    _cell->addTitle( title);	//FACTOR
//		    if (title->isSelected()) {
                    if (chapter->getSelected()) {
                        addStreams (title,_cell);
                        selected=true;
                    }
                }
            }
        }
    }
    return selected;
}

double  k9CellCopyList::gettotalSize() {
    double  totalSize=0;
    for (int iCell=0;iCell<count();iCell++) {
        k9Cell *cell=(k9Cell*)at(iCell);
//        if (!cell->copied) {
        if (cell->selected) {
            if (cell->angleBlock==angleNone)
                totalSize += cell->lastSector-cell->startSector;
            else if (cell->angleBlock==angleStart) {
                uint32_t start=0,end=0;
                start=cell->startSector;
                // loop inside the angle block to find the last sector.
                while (((k9Cell*)at(iCell))->angleBlock !=angleNone) {
                    end=((k9Cell*)at(iCell))->lastSector;
                    iCell++;
                }
                iCell--;
                totalSize += end-start;
            }
        } else
            totalSize += 1;
//       }
    }
    return  (totalSize*2048);
    ;

}

//gives the final size of cells with a forced shrink factor
double k9CellCopyList::getforcedSize(bool _withFactor) {
    double  totalSize=0;
    for (int iCell=0;iCell<count();iCell++) {
        k9Cell *cell=(k9Cell*)at(iCell);
        double factor = _withFactor ? cell->getFactor():1;
        double size=0;
//        if (!cell->copied) {
        if (cell->selected && cell->getforceFactor() ) {
            if (cell->angleBlock==angleNone)
                size = cell->lastSector-cell->startSector;
            else if (cell->angleBlock==angleStart) {
                uint32_t start=0,end=0;
                start=cell->startSector;
                // loop inside the angle block to find the last sector.
                while (((k9Cell*)at(iCell))->angleBlock !=angleNone) {
                    end=((k9Cell*)at(iCell))->lastSector;
                    iCell++;
                }
                iCell--;
                size += end-start;
            }
            totalSize += (size/factor);
        }
//       }
    }
    return  (totalSize*2048);

}

double k9CellCopyList::getMinFactor(bool _withMenus) {

    double fforced=getforcedSize( false);
    double MaxSize=k9DVDSize::getMaxSize();
    MaxSize*=1024*1024;
    double menuSize=0;

    if (_withMenus)
        menuSize=DVD->getmenuSize();

    menuSize= menuSize*2048;
    //totalSize=part of dvd with auto shrink factor/2.5
    //double totalSize=gettotalSize()+menuSize - (fforced-m_frcinbytes) -m_inbytes;
    double totalSize=gettotalSize()+menuSize - (fforced) -m_inbytes;
    totalSize/=2.50;

    qDebug() << QString("totalSize(%1)=gettotalSize()(%2)+menuSize(%3) -(fforced(%4))-m_inbytes(%6)").arg(totalSize).arg(gettotalSize()).arg(menuSize).arg(fforced).arg(m_frcinbytes).arg(m_inbytes);

    double minFactor=(fforced-m_frcinbytes) /(MaxSize- (totalSize +m_outbytes) - m_frcoutbytes) ;
    qDebug() << QString("minfactor(%1)=(fforced(%2) -m_frinbytes(%3))/(MacSize(%4)-totalSize(%5)-m_outbytes(%6) - m_frcoutbytes(%7))").arg(minFactor).arg(fforced).arg(m_frcinbytes).arg(MaxSize).arg(totalSize).arg(m_outbytes).arg(m_frcoutbytes);

    if (minFactor<1)
        minFactor=1;
    return minFactor;
}


double k9CellCopyList::getfactor(bool _withMenus,bool _streams) {

    double totalSize=gettotalSize();

    if (_streams) {
        double unsel=0;
        for (int i=0;i<DVD->gettitleCount();i++) {
            k9DVDTitle *track=DVD->gettitle(i);
            for (int j=0; j<track->getaudioStreamCount();j++) {
                k9DVDAudioStream *audio=track->getaudioStream(j);
                if (! audio->getselected())
                    unsel+=audio->getsize_mb();
            }
            for (int j=0; j<track->getsubPictureCount();j++) {
                k9DVDSubtitle *sub=track->getsubtitle(j);
                if (! sub->getselected())
                    unsel+=sub->getsize_mb();
            }
        }
        unsel*=1024*1024;
        totalSize-=unsel;
    }

    double menuSize=0;

    if (_withMenus)
        menuSize=DVD->getmenuSize();

    menuSize= menuSize*2048;

    double dvdSize=k9DVDSize::getMaxSize() ;
    dvdSize*=1024*1024;

    double factor;

    double fforced=getforcedSize(false)-m_frcinbytes;
    double fforcedsh=getforcedSize(true)-m_frcoutbytes;
    double dvdSize2=dvdSize-m_outbytes -fforcedsh;
    //     dvdSize2 -=menuSize;

    if (dvdSize2 <0) {
        factor=2.5;
        //qDebug(QString("dvdSize (%1)- _outbytes(%2) - fforcedsh(%3)=%4").arg(dvdSize).arg(m_outbytes).arg(fforcedsh).arg(dvdSize2));
    } else {
        factor=(totalSize +menuSize -  fforced -m_inbytes)/ dvdSize2 ;

        factor = (int)(factor*100);
        factor /=100;
        factor+=0.01;
        //    }

//        uint64_t o=m_outbytes;
//        uint64_t i=m_inbytes;
        //if (o==0)
        //	o=1;
        //qDebug(QString("factor : %1   realise : %2").arg(factor).arg((double)i/(double)o));

        if (factor<=1)
            factor=1;
        else if (factor>3)
            factor=3;
    }

    return (factor);
}
