//
// C++ Interface: k9dvdread
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9DVDREAD_H
#define K9DVDREAD_H

#include "k9common.h"
#include <QList>

/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/

class k9DVDRead;
class k9Ifo2;

class k9DVDFile {
friend class k9DVDRead;
private:
    dvd_file_t *m_file;
    k9DVDRead *m_dvd;
    void openIfo(uint _vts);
    void openMenu(uint _vts);
    void openTitle(uint _vts);

public:
    k9DVDFile(k9DVDRead *_dvd);
    ~k9DVDFile();
    void close();
    int readBytes(uchar *_buffer,uint32_t _size);
    int readBlocks(uint32_t _sector,uint32_t _size,uchar*_buffer);
};


class k9DVDRead{
friend class k9DVDTitle;
private:
  dvd_reader_t *m_dvd;
  QList <k9DVDFile*> m_files;
  QList <k9Ifo2*> m_ifos;
  void deleteIfos();
  void deleteFiles();
public:
    k9DVDRead();
    ~k9DVDRead();
    void openDevice(const QString & _device);
    k9DVDFile *openIfo(uint _vts);
    k9DVDFile *openMenu(uint _vts);
    k9DVDFile *openTitle(uint _vts);
    k9Ifo2 *getIfo(int _num);
    QString getDiscId();
    void close();
    bool opened();

	dvd_reader_t* getDvd() const {
	    return m_dvd;
	}
	

};


#endif
