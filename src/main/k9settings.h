//
// C++ Interface: k9settings
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9SETTINGS_H
#define K9SETTINGS_H

#include "k9common.h"
#include "k9prefdvd.h"
#include "k9prefmpeg4.h"
#include "k9prefmencoder.h"
#include "k9prefpreview.h"
#ifndef BACKLITE
#include "kconfigdlg.h"
#endif
#include "k9prefauthor.h"
#include "k9prefdirs.h"
#include <kpagedialog.h>

/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/
class k9Settings:public KPageDialog{
Q_OBJECT
public:
    k9Settings(QWidget *parent,const QString &caption);

    virtual ~k9Settings();

private:
    k9prefDVD *m_prefDVD;
    k9prefMPEG4 *m_prefMPEG4;
    k9prefMencoder *m_prefMencoder;
    k9prefPreview *m_prefPreview;
#ifndef BACKLITE
    kConfigDlg *m_configDlg;
#endif
    k9prefAuthor *m_prefAuthor;
    k9prefDirs *m_prefDirs;
private slots:
    void slotOkClicked();
    void slotApplyClicked();
};
#endif
