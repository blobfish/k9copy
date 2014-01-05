//
// C++ Interface: k9menu
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9MENU_H
#define K9MENU_H

#include "k9common.h"
#include <qobject.h>
#include <qimage.h>
#include <qfont.h>
#include <qdom.h>
#include "k9menuscene.h"
/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/
class k9MenuButton;
class k9Menu : public QObject {
    Q_OBJECT
public slots:
    void setStartScript(const QString &);
    void setStartScript2(const QString &);
    void setEndScript(const QString& _value);

public:
    enum eFormat {
        PAL=1,NTSC=2
    };
    k9Menu(QObject *parent = 0, const char *name = 0);
    ~k9Menu();
    k9MenuButton *addButton();
    void removeButton(k9MenuButton *_button);
    bool createMenus(QDomElement *_rootNode);
    void setWorkDir(const QString& _value);
    QString getWorkDir() const;
    void setFormat(const eFormat& _value);
    void setMenuFileName(const QString& _value);
    QString getMenuFileName() const;

    void setScene(k9MenuScene* _value);
    k9MenuScene* getScene() const;

    QString getStartScript() const;
    QString getEndScript() const;
    QString getStartScript2() const;
private:
    QList <k9MenuButton*> m_buttons;
    QString m_workDir;
    QString m_menuFileName;
    eFormat m_format;
    k9MenuScene *m_scene;
    QString m_startScript;
    QString m_startScript2;
    QString m_endScript;
    k9Config *m_config;
    bool m_error;
    void convertJpegToMpeg(const QString &_imageJpg,const QString &_imageMpg) ;
    void createAudio(const QString & _audioFile);
    void multiplex(const QString &_audio, const QString &_video, const QString _result);
    void spumux(const QString &_hiFileName,const QString &_mpgFileName);
    void appendMenu(QDomElement *_rootNode);
};

#endif
