//
// C++ Interface: k9tools
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9TOOLS_H
#define K9TOOLS_H

#include <QString>
#include <QList>
#include <QComboBox>
/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/


class k9Tools{
private:
    k9Tools() {}
    ~k9Tools(){}
public: 
   static bool checkProgram(QString _progName,bool _msg=false);
   static void clearOutput(QString name);
   static void clearOutput();
   static void setComboText(QComboBox *_cb,const QString &_text);
   static long getFreeSpace(const QString &_path);
   static bool runAsRoot(const QString &_command,const QString &_description);
   static void addTemporaryDir(const QString &dir);
   static const QString getTempPath() ;
   static const QString randomFileName();
   static QString findExe(const QString _program);
   static void processEvents();
   static void setBatchCopy(bool);
   static bool getBatchCopy();
   static void setMainThread();
};

#ifdef BACKLITE
QIcon SmallIcon(const QString &name);
QIcon getStdIcon(const QString &name);
class KShell {
public:
    static QString quoteArg( const QString &arg );
};
#endif
#endif
