//
// C++ Interface: k9avifile
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9AVIFILE_H
#define K9AVIFILE_H

#include <qobject.h>
#include <qdatetime.h>
#include <qimage.h>

/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/
class k9Title;
class k9MenuButton;
class k9AviFile : public QObject
{
Q_OBJECT
public:
    k9AviFile(k9Title *parent = 0, const char *name = 0);

    ~k9AviFile();

	void setFileName ( const QString& _value );
	QString getFileName() const;

	void setStart ( const QTime& _value );
	QTime getStart() const;

	void setEnd ( const QTime& _value );
	QTime getEnd() const;

	void setLabel ( const QString& _value );
	QString getLabel() const;

	k9Title* getTitle() const;

	int getNum() const;

	void setPrevious ( k9AviFile* _value );
	k9AviFile* getPrevious() const;

	void setNext ( k9AviFile* _value );
	k9AviFile* getNext() const;

        void setBreakPrevious(bool _state);
        bool getBreakPrevious();

        void setBreakNext(bool _state);
        bool getBreakNext();

	void setImage(const QImage& _value);
	QImage getImage() const;

	void setButton(k9MenuButton* theValue);
	k9MenuButton* getButton() const;

	void setReencode(bool theValue);
	

	bool getReencode() const;
	
	
	

private:
    QString m_fileName;
    QTime m_start;
    QString m_label;
    QTime m_end;
    k9Title *m_title;
    QImage m_image;
    int m_num;
    bool m_breakNext,m_breakPrevious;
    bool m_reencode;
    k9AviFile *m_previous;
    k9AviFile *m_next;
    void setTitle ( k9Title* _value );
    k9MenuButton *m_button;
signals:
    void aviFileUpdated(k9AviFile *_aviFile);
    void imageChanged(const QImage &);
};

#endif
