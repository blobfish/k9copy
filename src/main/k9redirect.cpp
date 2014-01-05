//
// C++ Implementation: k9redirect
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9redirect.h"
#include <kprocess.h>
k9Redirect::k9Redirect()
{
}


k9Redirect::~k9Redirect()
{
}

void k9Redirect::execute() {
    KProcess *p=new KProcess();
    *p << "mencoder";
    *p << "-oac" << "lavc" << "-ovc" << "lavc" << "-of" << "mpeg";
    *p << "-mpegopts" << "format=dvd" << "-vf" << "scale=720:576,harddup" << "-srate" << "48000" << "-af"  << "lavcresample=48000" << "-lavcopts" << "vcodec=mpeg2video:vrc_buf_size=1835:vrc_maxrate=9800:vbitrate=5000:keyint=15:acodec=ac3:abitrate=192:aspect=16/9" << "-ofps" << "25" << "-o" << "/tmp/kde-jmp/k9copy/tmp.mpeg" << "-ss" << "00:00:00.000" << "-endpos" << "00:00:57.360" << "-really-quiet" << "/home/jmp/t.avi";
    p->start(KProcess::NotifyOnExit);
    p->wait();


    QFile f("/tmp/kde-jmp/k9copy/tmp.mpeg");
    QFile fout;
    f.open(IO_ReadOnly );
    fout.open(IO_WriteOnly,stdout);
    while ( ! f.atEnd()) {
        char *buf=new char [2048]; 
        f.readBlock(buf,2048);
        fout.writeBlock(buf,2048);
        delete buf;
    }
    f.close();
    fout.close();
}
