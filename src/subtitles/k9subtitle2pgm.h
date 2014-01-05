//
// C++ Interface: k9subtitle2pgm
//
// Description:
//
//
// Author:  <>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9SUBTITLE2PGM_H
#define K9SUBTITLE2PGM_H

#include "k9common.h"
#include "k9fifo.h"
#include <QWaitCondition>
#include <QMutex>
#include <QThread>
#include <QFile>

typedef struct {
    FILE *fsub;
    FILE *fidx;
    unsigned int aid;
} vobsub_out_t;


class k9Subtitle2Pgm : public QThread {
    Q_OBJECT
public:
    k9Subtitle2Pgm ( QObject *parent);
    ~k9Subtitle2Pgm();
    void addData(uchar *buffer,uint size);
    void setNoData() ;

    void setBaseName(const QString& theValue);

    void end();

    void setPalette(uint _index,uint value);

    void setLangCod(const QString& theValue);

    void setTrackCod(int theValue);



private:
    double last_pts;
    QString  m_langCod;
    int m_trackCod;
    int last_pts_set;
    QString m_baseName;
    k9MemoryFifo *m_fifo;
    uint m_palette[16];
    double start_rpts,end_rpts,m_totalLength;

    double m_offset_rpts,m_last_rpts;
    int m_discont;

    uint stream_read_int16(uchar *s);
    uint read_tc_time_stamp(char *s);
    uint stream_read_int32(uchar *s);
    int get_pts_dts(char *buffer, ulong *pts, ulong *dts) ;
    void pes_ac3_loop();
    void *vobsub_out_open(const char *basename, const unsigned int *palette,
                          unsigned int orig_width, unsigned int orig_height,
                          const char *id, unsigned int index);
    void create_idx(vobsub_out_t *me, const unsigned int *palette, unsigned int orig_width, unsigned int orig_height);
    void vobsub_out_close(void *me);
    void vobsub_out_output(void *me, const unsigned char *packet, int len, double pts);
    QTime pts2Time(double _pts);

protected:
    virtual void run();
};



#endif
