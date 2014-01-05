//
// C++ Implementation: k9menu
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9menu.h"
#include "k9menubutton.h"
#include <qpainter.h>
#include <qbitmap.h>
#include <qfile.h>
#include <qdom.h>
#include "k9tools.h"


k9Menu::k9Menu(QObject *parent, const char *)
        : QObject(parent),m_format(PAL) {
 //   m_buttons.setAutoDelete(false);
    m_scene=new k9MenuScene(this);
    QPixmap pix(720,576);
    pix.fill(Qt::black);
    //m_canvas->setBackgroundPixmap(pix);
    m_startScript=m_startScript2=m_endScript="";
}


k9Menu::~k9Menu() {
  // qDeleteAll(m_buttons);

}

k9MenuButton *k9Menu::addButton() {
    k9MenuButton *button=new k9MenuButton(m_scene,"");
    m_buttons.append(button);
    return button;
}


void k9Menu::removeButton(k9MenuButton *_button) {
   m_buttons.removeAll(_button);
}


bool k9Menu::createMenus(QDomElement *_rootNode) {
    m_error=false;
    m_config=new k9Config();
    int height=m_format==PAL?576:480;

    QImage img;

    m_scene->setSceneRect(0,0,720,height);
    foreach (QGraphicsItem *item, m_scene->selectedItems())
        item->setSelected(false);
    //create menu image

    QPixmap pixbg(720,height);
    QPainter pbg(&pixbg);
    m_scene->render(&pbg,QRectF(0,0,720,height),QRectF(0,0,720,height));


    //create the hilite picture
    int lineWidth=2;
    img=QImage(720,height,QImage::Format_RGB32);
    QPixmap pixhi;
    pixhi=QPixmap::fromImage(img,0);
    pixhi.fill(Qt::transparent);
    QPainter phi(&pixhi);
    QPen penhi;
    penhi.setColor(m_config->getPrefButtonHiliteColor());
    penhi.setWidth(lineWidth*2);
    phi.setPen(penhi);
    phi.setBrush(Qt::NoBrush);

    //draw buttons
    foreach (k9MenuButton *button,m_buttons) {
        //draw hilight
        phi.drawRect(button->getLeft()+lineWidth,button->getTop()+lineWidth,button->getWidth()-lineWidth,button->getHeight()-lineWidth);
            
    }
    pbg.end();
    phi.end();
    //save menu image

    QString bgFileName=m_workDir+k9Tools::randomFileName()+".jpg";
    QString hiFileName=m_workDir+k9Tools::randomFileName()+".png";
    QString m2vFileName= m_workDir+k9Tools::randomFileName()+".m2v";
    QString mpaFileName= m_workDir+k9Tools::randomFileName()+".m2v";
    QString mpgFileName= m_workDir+k9Tools::randomFileName()+".mpg";

    pixbg.save(bgFileName,"JPEG",-1);
    pixhi.save(hiFileName,"PNG",-1);

    convertJpegToMpeg(bgFileName,m2vFileName);
    createAudio(mpaFileName);
    multiplex(mpaFileName,m2vFileName,mpgFileName);
    spumux(hiFileName,mpgFileName);
    appendMenu(_rootNode);

    QFile::remove(bgFileName);
    QFile::remove(m2vFileName);
    QFile::remove(mpaFileName);
    QFile::remove(mpgFileName);
    QFile::remove(hiFileName);
    delete m_config;

    return !m_error;
}

void k9Menu::convertJpegToMpeg(const QString &_imageJpg,const QString &_imageMpg) {
    if (! (k9Tools::checkProgram("ppmtoy4m",true) && k9Tools::checkProgram("jpegtopnm",true) && k9Tools::checkProgram("mpeg2enc",true))) {
	m_error=true;
	return;
    }
    k9Process *process=new k9Process(0,0);
    process->setWorkingDirectory(m_workDir);
    //process->setUseShell(true);
    QStringList proc;
    if (m_format==NTSC) {
    //    proc << "jpeg2yuv" << "-n" << "50" << "-I" << "p" << "-f" << "29.97" << "-j" << _imageJpg << "|";
        proc << "jpegtopnm" << _imageJpg << "|" <<"ppmtoy4m" << "-n" << "50"<<"-F29.97:1"<<"-I" <<"t" << "-A"<<"59:54"<< "-L" <<"-S" << "420mpeg2"<<"|";
        proc << "mpeg2enc" << "-F" << "4" << "-n" << "n"  <<"-f" << "8" << "-o" << _imageMpg;
    } else {
    //    proc << "jpeg2yuv" << "-n" << "50" << "-I" << "p" << "-f" << "25" << "-j" << _imageJpg << "|";
        proc << "jpegtopnm" << _imageJpg << "|" <<"ppmtoy4m" << "-n" << "50"<<"-F25:1"<<"-I" <<"t" << "-A"<<"59:54"<< "-L" << "-S" << "420mpeg2"<< "|";
        proc << "mpeg2enc" << "-n" << "p"  <<"-f" << "8" << "-o" << _imageMpg;
    }



    process->setShellCommand(proc.join(" "));
    process->start();
    process->sync();
    qDebug() << process->debug();

    if (!process->normalExit()) {
        qDebug("error converting jpeg to mpeg");
    }
}

void k9Menu::createAudio(const QString & _audioFile) {
    if (! (k9Tools::checkProgram("twolame",false) || k9Tools::checkProgram("toolame",false))) {
	m_error=true;
        k9Dialogs::error(i18n("Error starting program %1").arg("twolame | toolame"),i18n("Running program"));
	return;
    }

    k9Process *process=new k9Process(0,0);
    process->setWorkingDirectory(m_workDir);
//    process->setUseShell(true);
    QStringList proc;
    if (k9Tools::checkProgram("toolame"))
	    proc << "dd" << "if=/dev/zero" << "bs=4" << "count=1920" << "|" << "toolame"  << "-b" << "128"  << "-s" << "48" << "/dev/stdin" << _audioFile;
    else
	    proc << "dd" << "if=/dev/zero" << "bs=4" << "count=1920" << "|" << "twolame" <<"-r" << "-b" << "128"  << "-s" << "48000" << "/dev/stdin" << _audioFile;

    process->setShellCommand(proc.join(" "));
    process->start();
    process->sync();
    if (!process->normalExit()) {
        qDebug("error creating audio stream");
    }
}
void k9Menu::multiplex(const QString &_audio, const QString &_video, const QString _result) {

    if (! k9Tools::checkProgram("mplex",true) ) {
	m_error=true;
	return;
    }

    k9Process *process=new k9Process(0,0);
    process->setWorkingDirectory(m_workDir);
   // process->setUseShell(true);
    QStringList cmd;
    cmd << "mplex" <<  "-f" << "8" << "-o" << _result << _video << _audio;
    process->setShellCommand(cmd.join(" "));
    process->start();
    process->sync();
    if (!process->normalExit()) {
        qDebug("error multiplexing audio and video");
    }
}

void k9Menu::spumux(const QString &_hiFileName,const QString &_mpgFileName) {
    if (! k9Tools::checkProgram("spumux",true)) {
	m_error=true;
	return;
    }
    QDomElement stream,spu;

    QDomDocument xml;

    QDomElement root = xml.createElement( "subpictures" );
    xml.appendChild( root );

    stream=xml.createElement("stream");
    root.appendChild(stream);

    spu=xml.createElement("spu");
    stream.appendChild(spu);

    spu.setAttribute("start","00:00:00.0");
    spu.setAttribute("end","00:00:00.0");
    spu.setAttribute("highlight",_hiFileName);
    spu.setAttribute("select",_hiFileName);

    //spu.setAttribute("autooutline","infer");
    spu.setAttribute("force","yes");
 //   spu.setAttribute("autoorder","rows");
    spu.setAttribute("xoffset","0");
    spu.setAttribute("yoffset","0");

    int i=1;
    
    foreach (k9MenuButton *button,m_buttons) {
        QDomElement eButton = xml.createElement("button");
        spu.appendChild(eButton);
        eButton.setAttribute("name",QString("button%1").arg(i));
        eButton.setAttribute("x0", QString::number(button->getLeft()));
        eButton.setAttribute("y0",QString::number(button->getTop()));
        eButton.setAttribute("x1",QString::number(button->getLeft()+button->getWidth()));
        eButton.setAttribute("y1",QString::number(button->getTop()+button->getHeight()));
        i++;
    }

    QString spuFileName(m_workDir + k9Tools::randomFileName()+ ".xml");
    QFile file( spuFileName );
    if ( file.open( QIODevice::WriteOnly ) ) {
        QTextStream stream( &file );
        xml.save(stream,1);
        file.close();
    }


    k9Process *process=new k9Process(0,0);
    process->setWorkingDirectory(m_workDir);
//    process->setUseShell(true);
//    *process << "mplex" <<  "-f" << "8" << "-o" << _result << _video << _audio;
    QStringList cmd;
    cmd << "spumux" << spuFileName << "<" << _mpgFileName << ">" << m_menuFileName;
    process->setShellCommand(cmd.join(" "));
    process->start();
    process->sync();
    if (!process->normalExit()) {
        qDebug("error multiplexing audio and video");
    }

    file.remove();
}

void k9Menu::appendMenu(QDomElement *_rootNode) {
    QDomDocument doc=_rootNode->ownerDocument();
    QDomNodeList l=_rootNode->elementsByTagName("menus");
    QDomNode menus;
    if (l.count()==0) {
        menus=doc.createElement("menus");
        _rootNode->appendChild(menus);
    } else {
        menus=l.item(0);
    }
   
    QDomElement pgc=doc.createElement("pgc");
    menus.appendChild(pgc);
    QDomElement pre=doc.createElement("pre");
    pgc.appendChild(pre);
    QDomText txt=doc.createTextNode(m_startScript +m_startScript2);
    pre.appendChild(txt);

    QDomElement vob=doc.createElement("vob");
    pgc.appendChild(vob);
    vob.setAttribute("file",m_menuFileName);
    vob.setAttribute("pause","inf");
    int i=1;
    foreach (k9MenuButton *button,m_buttons) {
        QDomElement eButton = doc.createElement("button");
        pgc.appendChild(eButton); 
        QDomText eButtonTxt=doc.createTextNode(button->getScript());
        eButton.appendChild(eButtonTxt);
        eButton.setAttribute("name",QString("button%1").arg(i));
        i++;
    }
    

}

QString k9Menu::getWorkDir() const {
    return m_workDir;
}


void k9Menu::setWorkDir(const QString& _value) {
    m_workDir = _value;
}


void k9Menu::setFormat(const eFormat& _value) {
    m_format = _value;
}


QString k9Menu::getMenuFileName() const {
    return m_menuFileName;
}


void k9Menu::setMenuFileName(const QString& _value) {
    m_menuFileName = _value;
}



k9MenuScene *k9Menu::getScene() const  {
    return m_scene;
}
void k9Menu::setScene(k9MenuScene *_scene) {
    m_scene=_scene;
}


void k9Menu::setStartScript(const QString &_value) {
    m_startScript=_value;
}


QString k9Menu::getStartScript() const {
    return m_startScript;
}

void k9Menu::setStartScript2(const QString &_value) {
    m_startScript2=_value;
}


QString k9Menu::getStartScript2() const {
    return m_startScript2;
}


QString k9Menu::getEndScript() const {
    return m_endScript;
}


void k9Menu::setEndScript(const QString& _value) {
    m_endScript = _value;
}
