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
#include "k9ifo2.h"
#include "bswap.h"
#include "dvdread.h"
#include <QDebug>

k9Ifo2::k9Ifo2(k9DVDRead *_dvdread) {
    _ifo=NULL;
    m_dvd=_dvdread;
    m_VTSVobs=0;
}


k9Ifo2::~k9Ifo2() {
    closeIFO();


}

/*! 
    \fn k9Ifo2::setDevice(QString &_device)
 */
void k9Ifo2::setDevice(QString &_device) {
    device=_device;
}


/*!
    \fn k9Ifo2::setOutput(QString &_output)
 */
void k9Ifo2::setOutput(QString &_output) {
    output=_output;
}

void k9Ifo2::setVTSVobs(uint32_t _sector) {
    m_VTSVobs=_sector;
}

uint32_t k9Ifo2::getSize() {
    return m_size;
}

/*!
    \fn k9Ifo2::openIFO(int _num)
 */
ifo_handle_t * k9Ifo2::openIFO(int _num) {
    _ifo = ifoOpen(m_dvd->getDvd(), _num);
    if (_ifo) {
    numIfo=_num;
       if( _ifo->vtsi_mat)
           m_size=_ifo->vtsi_mat->vtsi_last_sector +1;
       else
           m_size=_ifo->vmgi_mat->vmgi_last_sector+1;
    }
    return _ifo;

}


/*!
    \fn k9Ifo2::IFOClose( ifo_handle_t* _ifo)
 */
void k9Ifo2::closeIFO() {
    if(_ifo!=NULL) {
        ifoClose(_ifo);
        _ifo=NULL;
    }
}

ifo_handle_t * k9Ifo2::getIFO() {
    return _ifo;
}

/*!
    \fn k9Ifo2::saveIFO(ifo_handle_t* _ifo)
 */
void k9Ifo2::saveIFO() {
    uint32_t size;
    /* DVD handler */
    k9DVDFile *dvdfile;
    bool mainIfo = (numIfo==0);

    QString filename,filename2;
    if (mainIfo) {
        filename="/VIDEO_TS/VIDEO_TS.IFO";
    } else {
        filename.sprintf("/VIDEO_TS/VTS_%02d_0.IFO",numIfo);
    }
    if (numIfo==0) 
	size=_ifo->vmgi_mat->vmgi_last_sector +1;
    else
	size=_ifo->vtsi_mat->vtsi_last_sector +1;

    size*=DVD_BLOCK_LEN;
 //   if (k9UDFFindFile(dvd,(char*) filename.latin1(), &size)) {
    if (size > 0) {
        uchar *buffer ;
        buffer=new uchar[size];
        bufCopy=new uchar[size];
        m_buffer=new uchar[size*2];
        memset(buffer,0,size);
        memset(bufCopy,0,size);
	memset(m_buffer,0,size*2);
        //Lecture du fichier IFO original et sauvegarde dans buffer
        if ((dvdfile = m_dvd->openIfo(numIfo))== 0) {
            fprintf(stderr, "Failed opening IFO for titleset %d\n", 0);
            free(buffer);
            return;
        }

        /*	 if(DVDFileSeek(dvd_file, 0)!=0) {
            	return;
          	}
        */

        if (dvdfile->readBytes(buffer,size) != (int)size) {
            fprintf(stderr, "Error reading IFO for titleset %d\n", 0);
            free(buffer);
            dvdfile->close();
            return;
        }

        memcpy(bufCopy,buffer,size);
        bufCopySize=size;
        dvdfile->close();
        if (mainIfo) {
            m_position=0;
            updateVMG(buffer);
	    m_position=1024; //sizeof(vmgi_mat_t);    
            updateFP_PGC(buffer);
            _ifo->vmgi_mat->vmgi_last_byte=m_position -1;

            updateTT_SRPT(buffer);
            updatePGCI_UT(buffer);

            updateVTS_ATRT(buffer);
            updateTXTDT_MGI(buffer);
            updateC_ADT(buffer,true);
            updateVOBU_ADMAP(buffer,true);
            
            updateVMG(buffer);

        } else {
            m_position=0;
            updateVTS(buffer);
            m_position=sizeof(vtsi_mat_t);
	    //_ifo->vtsi_mat->vtsi_last_byte=m_position -1;
            updateVTS_PTT_SRPT(buffer);
            updatePGCIT(buffer);
            updatePGCI_UT(buffer);
            updateVTS_TMAPT(buffer);
            updateC_ADT(buffer,true);
            updateVOBU_ADMAP(buffer,true);
            updateC_ADT(buffer, false);
            updateVOBU_ADMAP(buffer,false);
            updateVTS(buffer);
        }
	m_position=round(m_position);

        //sauvegarder buffer dans fichier
        if (mainIfo) {
            filename=output + "/VIDEO_TS.IFO";
            filename2=output + "/VIDEO_TS.BUP";
        } else {
            filename.sprintf("/VTS_%02d_0.IFO",numIfo);
            filename=output + filename;

            filename2.sprintf("/VTS_%02d_0.BUP",numIfo);
            filename2=output + filename2;
        }

        QFile ifofile (filename);
        ifofile.remove();
        ifofile.open(QIODevice::WriteOnly);
        if (ifofile.write((char*)m_buffer,m_position) == -1) {
            QString sError("erreur");
        }
        ifofile.close();

        QFile ifofile2 (filename2);
        ifofile2.remove();
        ifofile2.open(QIODevice::WriteOnly);
        if (ifofile2.write((char*)m_buffer,m_position) == -1) {
            QString sError("erreur");
        }
        ifofile2.close();


        delete buffer;
        delete bufCopy;
        delete m_buffer;
    }
}


/*!
    \fn k9Ifo2::updateVMG(uchar * _buffer)
 */
void k9Ifo2::updateVMG(uchar * ) {
    vmgi_mat_t vmgi_mat;
    memcpy(&vmgi_mat,_ifo->vmgi_mat,sizeof(vmgi_mat_t));
    uint32_t lastSector=vmgi_mat.vmgi_last_sector;
    //JMP : à vérifier
    if (m_position !=0) {
	lastSector=(round(m_position) - DVD_BLOCK_LEN) / DVD_BLOCK_LEN;
	//lastByte=vmgi_mat.vmgi_last_sector * DVD_BLOCK_LEN +DVD_BLOCK_LEN -1;
	vmgi_mat.vmg_last_sector+=2*(lastSector-vmgi_mat.vmgi_last_sector);
    }
    
    vmgi_mat.vmgi_last_sector=lastSector;
    if (vmgi_mat.vmgm_vobs !=0)
    	vmgi_mat.vmgm_vobs=vmgi_mat.vmgi_last_sector +1;
    
    B2N_32(vmgi_mat.vmg_last_sector);
    B2N_32(vmgi_mat.vmgi_last_sector);
    B2N_32(vmgi_mat.vmg_category);
    B2N_16(vmgi_mat.vmg_nr_of_volumes);
    B2N_16(vmgi_mat.vmg_this_volume_nr);
    B2N_16(vmgi_mat.vmg_nr_of_title_sets);
    B2N_64(vmgi_mat.vmg_pos_code);
    B2N_32(vmgi_mat.vmgi_last_byte);
    B2N_32(vmgi_mat.first_play_pgc);
    B2N_32(vmgi_mat.vmgm_vobs);
    B2N_32(vmgi_mat.tt_srpt);
    B2N_32(vmgi_mat.vmgm_pgci_ut);
    B2N_32(vmgi_mat.ptl_mait);
    B2N_32(vmgi_mat.vts_atrt);
    B2N_32(vmgi_mat.txtdt_mgi);
    B2N_32(vmgi_mat.vmgm_c_adt);
    B2N_32(vmgi_mat.vmgm_vobu_admap);
    B2N_16(vmgi_mat.vmgm_audio_attr.lang_code);
    B2N_16(vmgi_mat.vmgm_subp_attr.lang_code);

    memcpy(m_buffer,&vmgi_mat,sizeof(vmgi_mat_t));
}


/*!
    \fn k9Ifo2::updateFP_PGC(uchar * _buffer)
 */
void k9Ifo2::updateFP_PGC(uchar * _buffer) {
    //int offset=_ifo->vmgi_mat->first_play_pgc;
    _ifo->vmgi_mat->first_play_pgc=m_position;
    return updatePGC(_buffer,_ifo->first_play_pgc,m_position);


}

/*!
    \fn k9Ifo2::updatePGC(uchar *_buffer,pgc_t *_pgc,int _offset)
 */
void k9Ifo2::updatePGC(uchar *_buffer,pgc_t *_pgc,int _offset) {
    pgc_t pgc;
    memcpy(&pgc,_pgc,sizeof(pgc_t));
    uint start=m_position;
    m_position+=PGC_SIZE;
    
    if(pgc.command_tbl_offset != 0) {
        pgc.command_tbl_offset=PGC_SIZE;
        updatePGC_COMMAND_TBL(_buffer ,pgc.command_tbl  ,m_position);
    }
    if(pgc.program_map_offset != 0) {
        pgc.program_map_offset=m_position-start;
        updatePGC_PROGRAM_MAP(_buffer, pgc.program_map,pgc.nr_of_programs ,m_position);
    }
    if(pgc.cell_playback_offset != 0) {
        pgc.cell_playback_offset=m_position-start;
        updatePGC_CELL_PLAYBACK_TBL(_buffer ,pgc.cell_playback,pgc.nr_of_cells,m_position);
    }

    if(pgc.cell_position_offset != 0) {
        pgc.cell_position_offset=m_position-start;
        updatePGC_CELL_POSITION_TBL(_buffer,pgc.cell_position,pgc.nr_of_cells,m_position);
    }
    
    B2N_16(pgc.command_tbl_offset);
    B2N_16(pgc.next_pgc_nr);
    B2N_16(pgc.prev_pgc_nr);
    B2N_16(pgc.goup_pgc_nr);
    B2N_16(pgc.program_map_offset);
    B2N_16(pgc.cell_playback_offset);
    B2N_16(pgc.cell_position_offset);

    int i;
    for(i = 0; i < 8; i++)
        B2N_16(pgc.audio_control[i]);
    for(i = 0; i < 32; i++)
        B2N_32(pgc.subp_control[i]);
    for(i = 0; i < 16; i++)
        B2N_32(pgc.palette[i]);

    memcpy(m_buffer+_offset,&pgc,PGC_SIZE);
}



/*!
    \fn k9Ifo2::updatePGC_COMMAND_TBL(uchar  *_buffer,pgc_command_tbl_t *_cmd_tbl,int offset)
 */
void k9Ifo2::updatePGC_COMMAND_TBL(uchar  *,pgc_command_tbl_t *_cmd_tbl,int _offset) {
    struct {
        uint16_t nr_of_pre;
        uint16_t nr_of_post;
        uint16_t nr_of_cell;
        uint16_t last_byte;
    }
    ATTRIBUTE_PACKED cmd_tbl;

    memcpy(&cmd_tbl,_cmd_tbl,sizeof(cmd_tbl));

    //moves the offset to save vm_cmd
    m_position+=sizeof(cmd_tbl);

    if(_cmd_tbl->nr_of_pre != 0) {
        unsigned int pre_cmds_size  = _cmd_tbl->nr_of_pre * COMMAND_DATA_SIZE;
        memcpy(m_buffer + m_position,_cmd_tbl->pre_cmds,pre_cmds_size);
        m_position+=pre_cmds_size;
    }
    if(_cmd_tbl->nr_of_post != 0) {
        unsigned int post_cmds_size = _cmd_tbl->nr_of_post * COMMAND_DATA_SIZE;
        memcpy(m_buffer + m_position,_cmd_tbl->post_cmds,post_cmds_size);
        m_position+=post_cmds_size;
    }
    if(_cmd_tbl->nr_of_cell != 0) {
        unsigned int cell_cmds_size = _cmd_tbl->nr_of_cell * COMMAND_DATA_SIZE;
        memcpy(m_buffer +m_position,_cmd_tbl->cell_cmds,cell_cmds_size);
        m_position+=cell_cmds_size;
    }
    B2N_16(cmd_tbl.nr_of_pre);
    B2N_16(cmd_tbl.nr_of_post);
    B2N_16(cmd_tbl.nr_of_cell);
    cmd_tbl.last_byte=m_position-_offset-1;
    B2N_16(cmd_tbl.last_byte);
    memcpy(m_buffer+_offset,&cmd_tbl,sizeof(cmd_tbl));


}


/*!
    \fn k9Ifo2::updatePGC_PROGRAM_MAP(uchar *_buffer, pgc_program_map_t *_program_map, int _nr, int_offset
 */
void k9Ifo2::updatePGC_PROGRAM_MAP(uchar *, pgc_program_map_t *_program_map, int _nr, int _offset) {
    int size = _nr * sizeof(pgc_program_map_t);
    memcpy(m_buffer+_offset, _program_map, size);
    // pad to word boundary
    size += size % 2;
    m_position += size;
}


/*!
    \fn k9Ifo2::updatePGC_CELL_PLAYBACK_TBL(uchar *_buffer, cell_playback_t *_cell_playback,int _nr, int _offset)
 */
void k9Ifo2::updatePGC_CELL_PLAYBACK_TBL(uchar *, cell_playback_t *_cell_playback,int _nr, int _offset) {
    cell_playback_t *cell_playback;
    int size = _nr * sizeof(cell_playback_t);
    //cell_playback=(cell_playback_t*) malloc(size);
    cell_playback=new cell_playback_t[_nr];
    memcpy(cell_playback,_cell_playback,size);

    for( int i = 0; i < _nr; i++) {
        B2N_32(cell_playback[i].first_sector);
        B2N_32(cell_playback[i].first_ilvu_end_sector);
        B2N_32(cell_playback[i].last_vobu_start_sector);
        B2N_32(cell_playback[i].last_sector);
    }

    memcpy(m_buffer + _offset,cell_playback,size);
    m_position+=size;

    //free(cell_playback);
    delete[] cell_playback;
}


/*!
    \fn k9Ifo2::updatePGC_CELL_POSITION_TBL(uchar *_buffer,cell_position_t *_cell_position,int _nr, int _offset)
 */
void k9Ifo2::updatePGC_CELL_POSITION_TBL(uchar *,cell_position_t *_cell_position,int _nr, int _offset) {
    cell_position_t * cell_position;
    int size = _nr * sizeof(cell_position_t);
    //cell_position=(cell_position_t*)malloc(size);
    cell_position=new cell_position_t[_nr];
    memcpy(cell_position,_cell_position,size);
    for( int i = 0; i < _nr; i++) {
        B2N_16(cell_position[i].vob_id_nr);
    }
    memcpy(m_buffer + _offset,cell_position,size);
    m_position+=size;
    //free(cell_position);
    delete[] cell_position;
}

int k9Ifo2::round(int _value) {
   return ((_value-1)|(DVD_BLOCK_LEN-1))+1; // DVD_BLOCK_LEN-1 = 0x7FF
}

/*!
    \fn k9Ifo2::updateTT_SRPT(uchar *_buffer)
 */
void k9Ifo2::updateTT_SRPT(uchar *) {
    if(_ifo->vmgi_mat->tt_srpt != 0) {
        tt_srpt_t * tt_srpt;
        //tt_srpt=(tt_srpt_t*) malloc(sizeof(tt_srpt_t));
        tt_srpt=new tt_srpt_t;
      //  int offset= _ifo->vmgi_mat->tt_srpt * DVD_BLOCK_LEN;
         
         m_position=round(m_position);
        int offset=m_position;
        _ifo->vmgi_mat->tt_srpt=m_position/DVD_BLOCK_LEN;
        
        memcpy (tt_srpt,_ifo->tt_srpt,sizeof(tt_srpt_t));


        int  info_length = tt_srpt->last_byte + 1 - TT_SRPT_SIZE;
        title_info_t * title_info;
        //title_info =(title_info_t*) malloc(info_length);
        title_info = new title_info_t[info_length / sizeof(title_info_t)];
        memcpy(title_info, tt_srpt->title,info_length);
	
        for(int i =  0; i < tt_srpt->nr_of_srpts; i++) {
            B2N_16(title_info[i].nr_of_ptts);
            B2N_16(title_info[i].parental_id);
            B2N_32(title_info[i].title_set_sector);
        }
        memcpy(m_buffer+offset+TT_SRPT_SIZE,title_info,info_length);

        delete[] title_info;
	m_position +=info_length;

        B2N_16(tt_srpt->nr_of_srpts);
        B2N_32(tt_srpt->last_byte);
        memcpy(m_buffer+offset,tt_srpt,TT_SRPT_SIZE);
        delete tt_srpt;
    }

}


void k9Ifo2::updatePGCI_UT(uchar *_buffer) {
    int sector,sector2;
    if(_ifo->vmgi_mat) {
        if(_ifo->vmgi_mat->vmgm_pgci_ut == 0)
            return;
        m_position=round(m_position);
        _ifo->vmgi_mat->vmgm_pgci_ut=m_position/DVD_BLOCK_LEN;
    } else if(_ifo->vtsi_mat) {
        if(_ifo->vtsi_mat->vtsm_pgci_ut == 0)
            return;
        m_position=round(m_position);
        _ifo->vtsi_mat->vtsm_pgci_ut=m_position/DVD_BLOCK_LEN;
    } else {
        return;
    }
    
    
    sector2=sector=m_position;
    if (_ifo->pgci_ut !=NULL) {
	pgci_ut_t * pgci_ut;
	//pgci_ut = (pgci_ut_t*) malloc( sizeof(pgci_ut_t));
        pgci_ut = new pgci_ut_t;
	memcpy (pgci_ut,_ifo->pgci_ut,sizeof(pgci_ut_t));
	m_position+=PGCI_UT_SIZE;
	sector2=m_position;
	
	pgci_lu_t pgci_lu[_ifo->pgci_ut->nr_of_lus];
	m_position+=_ifo->pgci_ut->nr_of_lus*PGCI_LU_SIZE;
	memcpy(pgci_lu,_ifo->pgci_ut->lu,_ifo->pgci_ut->nr_of_lus*sizeof(pgci_lu_t));
	
	for(int i = 0; i < _ifo->pgci_ut->nr_of_lus; i++) {
		B2N_16(pgci_lu[i].lang_code);
		pgci_lu[i].lang_start_byte=m_position - sector;
		B2N_32(pgci_lu[i].lang_start_byte);
		updatePGCIT_internal(_buffer,_ifo->pgci_ut->lu[i].pgcit,m_position);	
	}
	for (int i=0;i <_ifo->pgci_ut->nr_of_lus;i++)
		memcpy(m_buffer+sector2+i*PGCI_LU_SIZE ,&(pgci_lu[i]),PGCI_LU_SIZE);
		
    	B2N_16(pgci_ut->nr_of_lus);
    	pgci_ut->last_byte=m_position-sector-1;
	B2N_32(pgci_ut->last_byte);
	memcpy(m_buffer+sector,pgci_ut,PGCI_UT_SIZE);
	delete pgci_ut;
	

    }

}

void k9Ifo2::updatePGCIT(uchar *_buffer) {
    if(!_ifo->vtsi_mat)
        return ;

    if(_ifo->vtsi_mat->vts_pgcit == 0) /* mandatory */
        return ;
    m_position=round(m_position);
    _ifo->vtsi_mat->vts_pgcit=m_position / DVD_BLOCK_LEN;
    updatePGCIT_internal(_buffer,_ifo->vts_pgcit,_ifo->vtsi_mat->vts_pgcit * DVD_BLOCK_LEN);
}


void k9Ifo2::updatePGCIT_internal(uchar *_buffer, pgcit_t *_pgcit, int _offset) {
    pgcit_t * pgcit;
    //pgcit=(pgcit_t*)malloc(sizeof(pgcit_t));
    pgcit=new pgcit_t;
    memcpy(pgcit,_pgcit,sizeof(pgcit_t));

    int offset=m_position+PGCIT_SIZE;
    m_position+=PGCIT_SIZE;

    pgci_srp_t pgci_srp[_pgcit->nr_of_pgci_srp];
    memcpy(pgci_srp,_pgcit->pgci_srp,sizeof(pgci_srp_t)*_pgcit->nr_of_pgci_srp);
    
    m_position+=_pgcit->nr_of_pgci_srp*PGCI_SRP_SIZE;

    for(int i = 0; i < _pgcit->nr_of_pgci_srp; i++) {
        B2N_16(pgci_srp[i].ptl_id_mask);
        pgci_srp[i].pgc_start_byte=m_position-_offset;
        B2N_32(pgci_srp[i].pgc_start_byte); //JMP:faux
        updatePGC(_buffer,_pgcit->pgci_srp[i].pgc,m_position);
    }

    for(int i = 0; i < _pgcit->nr_of_pgci_srp; i++) 
        memcpy(m_buffer+offset+i*PGCI_SRP_SIZE,&(pgci_srp[i]),PGCI_SRP_SIZE);
    
    B2N_16(pgcit->nr_of_pgci_srp);
    pgcit->last_byte=m_position-_offset-1;
    B2N_32(pgcit->last_byte);
    memcpy(m_buffer+_offset ,pgcit,PGCIT_SIZE);
    delete pgcit;

}


void k9Ifo2::updatePTL_MAIT(uchar *) {
    if(!_ifo->vmgi_mat)
        return;

    _ifo->vmgi_mat->ptl_mait = 0;
     return;
}

void k9Ifo2::updateVTS_ATRT(uchar *_buffer) {
    if(_ifo->vmgi_mat->vts_atrt == 0)
        return;
    uint32_t orig=_ifo->vmgi_mat->vts_atrt * DVD_BLOCK_LEN;
    m_position=round(m_position);
   _ifo->vmgi_mat->vts_atrt=m_position/DVD_BLOCK_LEN;
   
    memcpy(m_buffer+m_position,_buffer+orig,_ifo->vts_atrt->last_byte+1);
   
    m_position+=_ifo->vts_atrt->last_byte+1;
    /*
    int sector = _ifo->vmgi_mat->vts_atrt * DVD_BLOCK_LEN;
    vts_atrt_t *vts_atrt;
    vts_atrt = (vts_atrt_t*)malloc(sizeof(vts_atrt_t));
    memcpy(vts_atrt,_ifo->vts_atrt,VTS_ATRT_SIZE);
    B2N_16(vts_atrt->nr_of_vtss);
    B2N_32(vts_atrt->last_byte);
    memcpy(m_buffer+sector,vts_atrt,VTS_ATRT_SIZE);
    free(vts_atrt);
    m_position+=VTS_ATRT_SIZE;

    sector += VTS_ATRT_SIZE;
        
    memcpy(m_buffer+sector,_buffer +orig+VTS_ATRT_SIZE ,VTS_ATTRIBUTES_SIZE*_ifo->vts_atrt->nr_of_vtss);
    
    m_position+=VTS_ATTRIBUTES_SIZE*_ifo->vts_atrt->nr_of_vtss;
    */
}

void k9Ifo2::updateTXTDT_MGI(uchar * _buffer) {
    if(_ifo->vmgi_mat->txtdt_mgi == 0)
        return;
    
   struct  {
     char disc_name[15];
     char nr_of_language_units; 
     uint32_t last_byte;
     txtdt_lu_t *lu;
  } ATTRIBUTE_PACKED txtdtmgi;    
    
    
    m_position=round(m_position);
    int orig=_ifo->vmgi_mat->txtdt_mgi*DVD_BLOCK_LEN;
    int offset=m_position;
    _ifo->vmgi_mat->txtdt_mgi =m_position/ DVD_BLOCK_LEN;
    memcpy(&txtdtmgi,_buffer+orig,sizeof(txtdtmgi));
    
    B2N_32(txtdtmgi.last_byte);
    memcpy(m_buffer+offset,_buffer+orig ,txtdtmgi.last_byte+1);
    m_position+=txtdtmgi.last_byte+1;
}

void k9Ifo2::updateC_ADT(uchar * _buffer, bool _isMenu) {
    if(_ifo->vmgi_mat) {
        if(_ifo->vmgi_mat->vmgm_c_adt != 0) {
            m_position =round(m_position); //
            _ifo->vmgi_mat->vmgm_c_adt=m_position/ DVD_BLOCK_LEN;
            updateC_ADT_Internal(_buffer,_ifo->menu_c_adt,m_position);
        }
    } else if(_ifo->vtsi_mat) {
        if(_ifo->vtsi_mat->vtsm_c_adt != 0 && _isMenu) {
            m_position=round(m_position); //sector =
            _ifo->vtsi_mat->vtsm_c_adt=m_position / DVD_BLOCK_LEN;
            updateC_ADT_Internal(_buffer,_ifo->menu_c_adt,m_position);
        }
        if (_ifo->vtsi_mat->vts_c_adt !=0 && !_isMenu) {
            m_position=round(m_position); //sector = 
            _ifo->vtsi_mat->vts_c_adt=m_position / DVD_BLOCK_LEN;
            updateC_ADT_Internal(_buffer,_ifo->vts_c_adt,m_position);
        }
    } else {
        return ;
    }

}

void k9Ifo2::updateC_ADT_Internal(uchar *,c_adt_t *_c_adt,int _sector) {
    c_adt_t * c_adt;
    //c_adt =(c_adt_t*) malloc (sizeof(c_adt_t));
    c_adt =new c_adt_t;
    memcpy(c_adt,_c_adt,sizeof(c_adt_t));


    int offset =_sector + C_ADT_SIZE;
    m_position+=C_ADT_SIZE;
    
    int info_length = _c_adt->last_byte + 1 - C_ADT_SIZE;

    cell_adr_t *cell_adr,*ptr;
    //cell_adr=(cell_adr_t*) malloc(sizeof(cell_adr_t));
    cell_adr=new cell_adr_t;

    ptr= _c_adt->cell_adr_table;
    for(uint i = 0; i < info_length/sizeof(cell_adr_t); i++) {
        memcpy(cell_adr,&(ptr[i]),sizeof(cell_adr_t));
        B2N_16(cell_adr->vob_id);
        B2N_32(cell_adr->start_sector);
        B2N_32(cell_adr->last_sector);
        memcpy(m_buffer+offset,cell_adr,sizeof(cell_adr_t));
        offset+=sizeof(cell_adr_t);
        //ptr+=sizeof(cell_adr_t);
    }
    
    m_position+=info_length;
    delete cell_adr;
    
    B2N_16(c_adt->nr_of_vobs);
    c_adt->last_byte=m_position-_sector-1;
    B2N_32(c_adt->last_byte);
    memcpy(m_buffer+_sector,c_adt,C_ADT_SIZE);
    delete c_adt;
}

void k9Ifo2::updateVOBU_ADMAP(uchar * _buffer, bool _isMenu) {
    int sector;
    if(_ifo->vmgi_mat) {

        if(_ifo->vmgi_mat->vmgm_vobu_admap == 0)
            return ;
        sector = m_position=round(m_position);//_ifo->vmgi_mat->vmgm_vobu_admap * DVD_BLOCK_LEN;
        _ifo->vmgi_mat->vmgm_vobu_admap=m_position/DVD_BLOCK_LEN;
        updateVOBU_ADMAP_Internal(_buffer,_ifo->menu_vobu_admap,sector);

    } else if(_ifo->vtsi_mat) {

        if(_ifo->vtsi_mat->vtsm_vobu_admap != 0 && _isMenu) {
            sector = m_position=round(m_position);//sector = _ifo->vtsi_mat->vtsm_vobu_admap * DVD_BLOCK_LEN;
            _ifo->vtsi_mat->vtsm_vobu_admap=m_position/DVD_BLOCK_LEN;
            updateVOBU_ADMAP_Internal(_buffer,_ifo->menu_vobu_admap,sector);
        }
        if (_ifo->vtsi_mat->vts_vobu_admap !=0 && !_isMenu) {
            sector = m_position=round(m_position);//sector = _ifo->vtsi_mat->vts_vobu_admap * DVD_BLOCK_LEN;
            _ifo->vtsi_mat->vts_vobu_admap=m_position/DVD_BLOCK_LEN;
            updateVOBU_ADMAP_Internal(_buffer,_ifo->vts_vobu_admap,sector);
        }
    } else {
        return ;
    }
}

void k9Ifo2::updateVOBU_ADMAP_Internal(uchar *,vobu_admap_t *_vobu_admap,int _sector) {
    vobu_admap_t *vobu_admap;
    //vobu_admap=(vobu_admap_t*)malloc(sizeof(vobu_admap_t));
    vobu_admap=new vobu_admap_t;
    memcpy(vobu_admap,_vobu_admap,sizeof(vobu_admap_t));

    int offset = _sector + VOBU_ADMAP_SIZE;
    m_position+=VOBU_ADMAP_SIZE;
    
    int info_length = _vobu_admap->last_byte + 1 - VOBU_ADMAP_SIZE;
    uint32_t *vobu_start_sectors;
    //vobu_start_sectors=(uint32_t*)malloc(info_length);
    vobu_start_sectors=new uint32_t[info_length/sizeof(uint32_t)];
    memcpy(vobu_start_sectors,_vobu_admap->vobu_start_sectors,info_length);

    for(uint i = 0; i < info_length/sizeof(uint32_t); i++)
        B2N_32(vobu_start_sectors[i]);

    memcpy(m_buffer+offset,vobu_start_sectors,info_length);
    
    m_position+=info_length;
    delete[] vobu_start_sectors;
    vobu_admap->last_byte=m_position-_sector-1;
    B2N_32(vobu_admap->last_byte);
    memcpy(m_buffer+_sector,vobu_admap,VOBU_ADMAP_SIZE);
    delete vobu_admap;

}

void k9Ifo2::updateVTS(uchar *) {
    vtsi_mat_t *vtsi_mat;
    //vtsi_mat = (vtsi_mat_t *)malloc(sizeof(vtsi_mat_t));
    vtsi_mat = new vtsi_mat_t;
    memcpy(vtsi_mat,_ifo->vtsi_mat,sizeof(vtsi_mat_t));

    uint32_t lastSector=vtsi_mat->vtsi_last_sector;
    //uint32_t lastByte=vtsi_mat->vtsi_last_byte;
    //JMP : à vérifier
    if (m_position >0) {
	lastSector=(round(m_position)-DVD_BLOCK_LEN) /DVD_BLOCK_LEN;
	//lastByte=vtsi_mat->vtsi_last_sector*DVD_BLOCK_LEN +DVD_BLOCK_LEN-1;
	vtsi_mat->vts_last_sector+=2*(lastSector-vtsi_mat->vtsi_last_sector);
        if (m_VTSVobs !=0)
           vtsi_mat->vtstt_vobs+=m_VTSVobs;
        else
           vtsi_mat->vtstt_vobs+=lastSector-vtsi_mat->vtsi_last_sector;
    }      
    m_size=lastSector+1;
    vtsi_mat->vtsi_last_sector=lastSector;
    //vtsi_mat->vtsi_last_byte=lastByte;
    if (vtsi_mat->vtsm_vobs !=0)
    	vtsi_mat->vtsm_vobs= vtsi_mat->vtsi_last_sector +1 ;
      
    B2N_32(vtsi_mat->vts_last_sector);
    B2N_32(vtsi_mat->vtsi_last_sector);
    B2N_32(vtsi_mat->vts_category);
  
    B2N_32(vtsi_mat->vtsi_last_byte);
    B2N_32(vtsi_mat->vtsm_vobs);
    B2N_32(vtsi_mat->vtstt_vobs);
    B2N_32(vtsi_mat->vts_ptt_srpt);
    B2N_32(vtsi_mat->vts_pgcit);
    B2N_32(vtsi_mat->vtsm_pgci_ut);
    B2N_32(vtsi_mat->vts_tmapt);
    B2N_32(vtsi_mat->vtsm_c_adt);
    B2N_32(vtsi_mat->vtsm_vobu_admap);
    B2N_32(vtsi_mat->vts_c_adt);
    B2N_32(vtsi_mat->vts_vobu_admap);
    B2N_16(vtsi_mat->vtsm_audio_attr.lang_code);
    B2N_16(vtsi_mat->vtsm_subp_attr.lang_code);
    for(int i = 0; i < 8; i++)
        B2N_16(vtsi_mat->vts_audio_attr[i].lang_code);
    for(int i = 0; i < 32; i++)
        B2N_16(vtsi_mat->vts_subp_attr[i].lang_code);

    memcpy(m_buffer,vtsi_mat,sizeof(vtsi_mat_t));
    
    delete vtsi_mat;
}

void k9Ifo2::updateVTS_PTT_SRPT(uchar *_buffer) {
    if(!_ifo->vtsi_mat)
        return ;

    if(_ifo->vtsi_mat->vts_ptt_srpt == 0) /* mandatory */
        return ;

    vts_ptt_srpt_t * vts_ptt_srpt;
    //vts_ptt_srpt = (vts_ptt_srpt_t *)malloc(sizeof(vts_ptt_srpt_t));
    vts_ptt_srpt = new vts_ptt_srpt_t;
    memcpy(vts_ptt_srpt,_ifo->vts_ptt_srpt,sizeof(vts_ptt_srpt_t));
    int orig=_ifo->vtsi_mat->vts_ptt_srpt * DVD_BLOCK_LEN;
    int offset = m_position=round(m_position);//_ifo->vtsi_mat->vts_ptt_srpt * DVD_BLOCK_LEN;
    _ifo->vtsi_mat->vts_ptt_srpt=m_position/DVD_BLOCK_LEN;

    B2N_16(vts_ptt_srpt->nr_of_srpts);
    B2N_32(vts_ptt_srpt->last_byte);

    memcpy(m_buffer+offset,vts_ptt_srpt,VTS_PTT_SRPT_SIZE);
    delete vts_ptt_srpt;
    m_position+=VTS_PTT_SRPT_SIZE;
    
    int info_length = _ifo->vts_ptt_srpt->last_byte + 1 - VTS_PTT_SRPT_SIZE;

    memcpy(m_buffer+m_position,_buffer+orig+VTS_PTT_SRPT_SIZE, info_length);
    m_position+=info_length;
}


/*!
    \fn k9Ifo2::checkBuffer()
 */


void k9Ifo2::updateVTS_TMAPT(uchar *) {
    if(!_ifo->vtsi_mat)
        return ;

    if(_ifo->vtsi_mat->vts_tmapt == 0) { /* optional(?) */
        return ;
    }

    vts_tmapt_t *vts_tmapt1,*vts_tmapt;
    vts_tmapt=_ifo->vts_tmapt;
    int info_length = vts_tmapt->nr_of_tmaps * 4;
  
    uint32_t  offsets[vts_tmapt->nr_of_tmaps];
    
    //vts_tmapt1 = (vts_tmapt_t *)malloc(VTS_TMAPT_SIZE);
    vts_tmapt1 = new vts_tmapt_t;
    memcpy(vts_tmapt1,_ifo->vts_tmapt,VTS_TMAPT_SIZE);
    uint32_t offset = m_position=round(m_position);
    
    _ifo->vtsi_mat->vts_tmapt =m_position / DVD_BLOCK_LEN;
    
    int offset0=offset;
    offset+=VTS_TMAPT_SIZE;
    m_position +=VTS_TMAPT_SIZE;

    
    offset += info_length;
    m_position+=info_length;

    info_length=0;
    vts_tmap_t *tmap=vts_tmapt->tmap;
    //loop on tmaps to compute total size
    for (int i=0;i <vts_tmapt->nr_of_tmaps;i++) {
         int tmapSize=VTS_TMAP_SIZE+tmap[i].nr_of_entries*sizeof(map_ent_t);
         info_length+=tmapSize+4;
    }

    //tmap = (vts_tmap_t *)malloc(sizeof(vts_tmap_t)* vts_tmapt->nr_of_tmaps);
    tmap = new vts_tmap_t[vts_tmapt->nr_of_tmaps];
    memcpy(tmap,vts_tmapt->tmap,sizeof(vts_tmap_t)* vts_tmapt->nr_of_tmaps);
    
    vts_tmapt1->last_byte=VTS_TMAPT_SIZE+info_length-1;
    for(int i = 0; i < vts_tmapt->nr_of_tmaps; i++) {
        int tmapSize=VTS_TMAP_SIZE+tmap[i].nr_of_entries*sizeof(map_ent_t);
        int nr_map_ent=tmap[i].nr_of_entries;
        B2N_16(tmap[i].nr_of_entries);
        offsets[i]=m_position-offset0;
        B2N_32(offsets[i]);
        memcpy(m_buffer+m_position,&(tmap[i]),VTS_TMAP_SIZE);
	m_position+=VTS_TMAP_SIZE;
        offset+=VTS_TMAP_SIZE;
        
        if(nr_map_ent == 0) { // Early out if zero entries
            continue;
        }
        int info_length2 = tmapSize-VTS_TMAP_SIZE;
        //map_ent_t *map_ent = (map_ent_t *)malloc(info_length2);
	map_ent_t *map_ent = new map_ent_t[info_length2/sizeof(map_ent_t)];
        memcpy(map_ent,tmap[i].map_ent,info_length2);
        for(int j = 0; j < nr_map_ent; j++) {
            B2N_32(map_ent[j]);
            memcpy(m_buffer+m_position,&(map_ent[j]),sizeof(map_ent_t));
            offset+=sizeof(map_ent_t);
            m_position+=sizeof(map_ent_t);
        }
        delete map_ent;
    }

    delete[] tmap;
    B2N_16(vts_tmapt1->nr_of_tmaps);
    B2N_32(vts_tmapt1->last_byte);
    memcpy(m_buffer+offset0,vts_tmapt1,VTS_TMAPT_SIZE);
    memcpy(m_buffer+offset0+VTS_TMAPT_SIZE,offsets,vts_tmapt->nr_of_tmaps*4);
    delete vts_tmapt1;


}
void k9Ifo2::checkBuffer(QString lib,uchar* _buffer) {
    /*     for (int j=0;j<bufCopySize;j++) {
           uchar *c,*c2;
    c=(uchar*) (_buffer+j);
    c2=(uchar*)(bufCopy+j);
    if (*c!=*c2) {
      return;
    }
       }
    */
    return;
    if (memcmp(bufCopy,_buffer,bufCopySize)!=0) {
        QString c;
        c="Buffer de sortie altéré: " +lib;
        qDebug() << c;
    }
}

void k9Ifo2::navRead_PCI(pci_t *pci, uchar *buffer) {
  int i, j;

  memcpy(pci, buffer, sizeof(pci_t));

  /* Endian conversions  */

  /* pci pci_gi */
  B2N_32(pci->pci_gi.nv_pck_lbn);
  B2N_16(pci->pci_gi.vobu_cat);
  B2N_32(pci->pci_gi.vobu_s_ptm);
  B2N_32(pci->pci_gi.vobu_e_ptm);
  B2N_32(pci->pci_gi.vobu_se_e_ptm);

  /* pci nsml_agli */
  for(i = 0; i < 9; i++)
    B2N_32(pci->nsml_agli.nsml_agl_dsta[i]);

  /* pci hli hli_gi */
  B2N_16(pci->hli.hl_gi.hli_ss);
  B2N_32(pci->hli.hl_gi.hli_s_ptm);
  B2N_32(pci->hli.hl_gi.hli_e_ptm);
  B2N_32(pci->hli.hl_gi.btn_se_e_ptm);

  /* pci hli btn_colit */
  for(i = 0; i < 3; i++)
    for(j = 0; j < 2; j++)
      B2N_32(pci->hli.btn_colit.btn_coli[i][j]);

  /* NOTE: I've had to change the structure from the disk layout to get
   * the packing to work with Sun's Forte C compiler. */
  
  /* pci hli btni */
  for(i = 0; i < 36; i++) {
    char tmp[sizeof(pci->hli.btnit[i])], swap;
    memcpy(tmp, &(pci->hli.btnit[i]), sizeof(pci->hli.btnit[i]));
    /* Byte 4 to 7 are 'rotated' was: ABCD EFGH IJ is: ABCG DEFH IJ */
    swap   = tmp[6]; 
    tmp[6] = tmp[5];
    tmp[5] = tmp[4];
    tmp[4] = tmp[3];
    tmp[3] = swap;
    
    /* Then there are the two B2N_24(..) calls */
#ifndef WORDS_BIGENDIAN
    swap = tmp[0];
    tmp[0] = tmp[2];
    tmp[2] = swap;
    
    swap = tmp[4];
    tmp[4] = tmp[6];
    tmp[6] = swap;
#endif
    memcpy(&(pci->hli.btnit[i]), tmp, sizeof(pci->hli.btnit[i]));
  }

}

