//
// C++ Implementation: k9dvdread
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9dvdread.h"
#include "dvdread.h"
#include "k9ifo2.h"

k9DVDRead::k9DVDRead(){
	m_dvd=NULL;
}

void k9DVDRead::deleteIfos() {
//	while (!m_ifos.isEmpty()) delete m_ifos.takeFirst();
	qDeleteAll(m_ifos);
	m_ifos.clear();
}
void k9DVDRead::deleteFiles() {
//	while (!m_files.isEmpty()) delete m_files.takeFirst();
	qDeleteAll(m_files);
	m_files.clear();
}

k9DVDRead::~k9DVDRead() {
	if (m_dvd !=NULL)
		close();
	deleteIfos();
	deleteFiles();
}

k9DVDFile::k9DVDFile(k9DVDRead *_dvd) {
	m_file=NULL;
	m_dvd=_dvd;
}

k9DVDFile::~k9DVDFile() {
	if (m_file != NULL)
		close();
}

/*!
    \fn k9DVDRead::openDevice(const QString & _device)
 */
void k9DVDRead::openDevice(const QString & _device) {
	m_dvd=DVDOpen(_device.toUtf8());
	//turn UDF cache off
	//DVDUDFCacheLevel(m_dvd, 0 );
        if (m_dvd) {
            k9Ifo2 *ifoz=new k9Ifo2(this);
            ifoz->openIFO(0);
            m_ifos.append(ifoz);
            ifo_handle_t *hifoz=ifoz->getIFO();
	    if(!hifoz) {
		m_dvd=NULL;
		return;
	    }
            int nrTS= hifoz->vmgi_mat->vmg_nr_of_title_sets;
    
            for (int iTS=1 ; iTS<=nrTS;iTS++) {
                k9Ifo2 *ifo=new k9Ifo2(this);
                if (!ifo->openIFO(iTS)) {
                    close();
                    return;
                }

                m_ifos.append(ifo);
            }
        }
}

k9Ifo2 *k9DVDRead::getIfo(int _num) {
   k9Ifo2 *ifo=m_ifos[_num];

    return ifo;
}

QString k9DVDRead::getDiscId() {
uchar ID[17];
QString id="";
if (DVDDiscID(m_dvd,ID) !=-1) {
	ID[16]=0;
	id=QString::fromLatin1((const char*)ID);
}
return id;
}
/*!
    \fn k9DVDRead::close()
 */
void k9DVDRead::close()
{
	DVDClose(m_dvd);
	deleteIfos();
	deleteFiles();
	m_dvd=NULL;
}

bool k9DVDRead::opened() {
	return (m_dvd !=NULL);
}


k9DVDFile * k9DVDRead::openIfo(uint _vts) {
	k9DVDFile *file =new k9DVDFile(this);
	file->openIfo(_vts);
	m_files.append(file);
	return file;
}

k9DVDFile *k9DVDRead::openMenu(uint _vts) {
	k9DVDFile *file =new k9DVDFile(this);
	file->openMenu(_vts);
	m_files.append(file);
	return file;
}

k9DVDFile *k9DVDRead::openTitle(uint _vts) {
	k9DVDFile *file =new k9DVDFile(this);
	file->openTitle(_vts);
	m_files.append(file);
	return file;
}

/*!
    \fn k9DVDFile::openIfo(uint _vts)
 */
void k9DVDFile::openIfo(uint _vts) {
	m_file=DVDOpenFile(m_dvd->getDvd(),_vts,DVD_READ_INFO_FILE);
}


/*!
    \fn k9DVDFile::openMenu(uint _vts)
 */
void k9DVDFile::openMenu(uint _vts)
{
	m_file=DVDOpenFile(m_dvd->getDvd() ,_vts,DVD_READ_MENU_VOBS);
}


/*!
    \fn k9DVDFile::openTitle(uint _vts)
 */
void k9DVDFile::openTitle(uint _vts)
{
    	m_file=DVDOpenFile(m_dvd->getDvd(),_vts,DVD_READ_TITLE_VOBS);
}


/*!
    \fn k9DVDFile::close()
 */
void k9DVDFile::close()
{
    if (m_file !=NULL) {
	DVDCloseFile(m_file);
	m_file=NULL;
    }
}


/*!
    \fn k9DVDFile::read(uchar *_buffer,uint32_t _size)
 */
int k9DVDFile::readBytes(uchar *_buffer,uint32_t _size)
{
	if (m_file !=NULL)
		return  DVDReadBytes(m_file,_buffer,_size);
	else 
		return -1;
}

int k9DVDFile::readBlocks(uint32_t _sector,uint32_t _size,uchar*_buffer) {
	if (m_file !=NULL)  {
	   return DVDReadBlocks(m_file,_sector,_size,_buffer);
	}
	else 
		return -1;
}




