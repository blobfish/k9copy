//
// C++ Implementation: k9ffmpegcmdgen
//
// Description:
//
//
// Author:  <>, (C) 2009
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "k9ffmpegcmdgen.h"
#include <QLineEdit>
#include <kdialog.h>

k9ffmpegCmdGen::k9ffmpegCmdGen(QWidget* parent)
        : QWidget(parent) {
    Ui_ffmpegCmdGen.setupUi(this);
    readCodecs();
}

k9ffmpegCmdGen::~k9ffmpegCmdGen() {

}

void k9ffmpegCmdGen::getStdout(char *_data, int _size) {
  QString tmp(QByteArray(_data,_size));
  m_codecs=m_codecs.append(tmp);
}

void k9ffmpegCmdGen::readCodecs() {
  k9Config config;
  k9Process process(this,0);
  connect (&process,SIGNAL(receivedStdout(char*, int)),this,SLOT(getStdout(char*, int)));
  process << config.getPrefFFmpegPath(true) <<"-formats";
  process.start();
  process.sync();
  QStringList c=m_codecs.split("\n");

  bool skip=true;
  foreach (QString s,c)	{
     if (!skip && s!="") {
        bool encoder=s.mid(2,1)=="E";
        bool video =s.mid(3,1)=="V";
        bool audio =s.mid(3,1)=="A";
	QString codecName=s.mid(8,16).trimmed();
	if (audio && encoder)
	    Ui_ffmpegCmdGen.acodec->addItem(codecName);
	else if(video && encoder)
	    Ui_ffmpegCmdGen.vcodec->addItem(codecName);
     }
     if (s=="Codecs:")
	skip=false;
     if (s=="" && !skip)
	break;	
  }
}


void k9ffmpegCmdGen::show (FFMpegCmdType _type) {
    m_type=_type;
    QStringList slCmd=m_cmd.split(' ');
    for ( int i=0; i< slCmd.count();i++) {
        QString opt=slCmd.at(i);
        if (opt.startsWith("-")) {
            QString optName=opt.mid(1);
            QWidget *w=Ui_ffmpegCmdGen.stkOptions->findChild<QWidget*>(optName);

            QLineEdit *le= dynamic_cast<QLineEdit*>(w);
            QComboBox *cb= dynamic_cast<QComboBox*>(w);
            QSpinBox *ni = dynamic_cast<QSpinBox*>(w);
            QCheckBox *ck=dynamic_cast<QCheckBox*>(w);
            if (le) {
                if (i<slCmd.count())
                    le->setText(slCmd.at(++i));
            } else if (cb) {
                if (i<slCmd.count()) {
                    if (cb->isEditable())
                        cb->setEditText(slCmd.at(++i));
                    else
                        cb->setCurrentIndex(cb->findText(slCmd.at(++i)));

                }
            } else if (ni) {
                QString text;
                if (i<slCmd.count())
                    text=slCmd.at(++i);
                bool flg;
		QString txt2=text;
                int val=txt2.replace("k","").toInt(&flg);
                if (!flg)
                    ni->setSpecialValueText(text);
                else
                    ni->setValue(val);
            } else if (ck) {
                ck->setCheckState(Qt::Checked);
            }

            // looking for the corresponding checkbox
            if (w) {
                QList<QCheckBox*> wl =Ui_ffmpegCmdGen.stkOptions->findChildren<QCheckBox*>();
                foreach	(QCheckBox *ck,wl) {
                    if (ck->text() == optName) {
                        ck->setCheckState(Qt::Checked);
                    }

                }
            }
        }
    }
    switch ( _type) {
    case AUDIO:
        Ui_ffmpegCmdGen.stkOptions->setCurrentIndex(1);
        break;
    case VIDEO:
        Ui_ffmpegCmdGen.stkOptions->setCurrentIndex(0);
        break;

    }
}

bool k9ffmpegCmdGen::getFFMpegCmd(FFMpegCmdType _type,QString &_cmd) {
    KDialog *dlg= new KDialog();
    k9ffmpegCmdGen *cmdGen=new k9ffmpegCmdGen(dlg);
    dlg->resize(cmdGen->width(),cmdGen->height());
    cmdGen->setCmd(_cmd);
    cmdGen->show(_type);
    dlg->setMainWidget(cmdGen);
    dlg->setWindowTitle(i18n("ffmpeg command generator"));

    if (dlg->exec())
        _cmd=cmdGen->getCmd();
    delete dlg;
    return true;
}

QString k9ffmpegCmdGen::getCmd()  {
    m_cmd.clear();
    QStringList cmd;

    QList<QCheckBox*> wl =Ui_ffmpegCmdGen.stkOptions->findChildren<QCheckBox*>();
    foreach(QCheckBox *ck,wl) {
        if (ck->checkState()==Qt::Checked) {
            QString name=ck->text().replace("&","");
            QWidget *w=Ui_ffmpegCmdGen.stkOptions->findChild<QWidget*>(name);
            cmd << "-"+name;
            if (w) {
                QLineEdit *le= dynamic_cast<QLineEdit*>(w);
                QComboBox *cb= dynamic_cast<QComboBox*>(w);
                QSpinBox *ni = dynamic_cast<QSpinBox*>(w);
                if (le)
                    cmd << le->text();
                else if (cb)
                    cmd << cb->currentText();
                else if (ni)
                    cmd << ni->text();
            }

        }
    }

    m_cmd=cmd.join(" ");

    return m_cmd;
}


void k9ffmpegCmdGen::setCmd(const QString& theValue) {
    m_cmd = theValue;
}
