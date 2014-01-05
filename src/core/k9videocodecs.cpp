//
// C++ Implementation: k9videocodecs
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9videocodecs.h"
#include <QStringList>
#ifndef BACKLITE
#include <KConfigGroup>
#endif
class _k9VideoCodec
{
public:
    _k9VideoCodec():name(""),fourcc(""),optPass1(""),optPass2(""),optOnePass(""),encoder(""){}
   _k9VideoCodec(QString _name,QString _fourcc,QString _optOnePass,QString _optPass1,QString _optPass2,QString _encoder) {
      name=_name;
      fourcc=_fourcc;
      optOnePass=_optOnePass;
      optPass1=_optPass1;
      optPass2=_optPass2; 
      encoder=_encoder;
   }
   QString name;
   QString fourcc;
   QString optPass1;
   QString optPass2;
   QString optOnePass;
   QString encoder;
}; 


k9VideoCodecs::k9VideoCodecs(QObject *parent)
 : QObject(parent)
{
   m_config=new k9Config();
#ifdef BACKLITE
    QSettings *group=m_config->getConfig();
    group->beginGroup("codecs");
    int nbVideo=group->value("VideoCount",0).toInt();
    group->endGroup();
#else
   KConfig *config= m_config->getConfig();
   KConfigGroup group=config->group("codecs");
   int nbVideo=group.readEntry("VideoCount",0);
#endif



   bool bReset;
   bReset=(nbVideo==0);
   //adds default codecs
   if (bReset) {
      reset();
      m_config=new k9Config();
   }  
   for (int cpt=0;cpt <nbVideo;cpt++) {
#ifdef BACKLITE
        group->beginGroup(QString("videocodec%1").arg(cpt));
        m_codecs[cpt] = _k9VideoCodec(group->value("name").toString(), group->value("fourcc","").toString(),group->value("opt1","").toString(),group->value("opt2","").toString(),group->value("opt3","").toString(),group->value("encoder","mencoder").toString());
        group->endGroup();
#else
        group=config->group(QString("videocodec%1").arg(cpt));
        m_codecs[cpt] = _k9VideoCodec(group.readEntry("name"), group.readEntry("fourcc",""),group.readEntry("opt1",""),group.readEntry("opt2",""),group.readEntry("opt3",""),group.readEntry("encoder","mencoder"));
#endif
   }


   delete m_config;

}

void k9VideoCodecs::reset() {
      m_codecs[0]=_k9VideoCodec("copy","","-ovc copy","-ovc copy","-ovc copy","mencoder");
      m_codecs[1]=_k9VideoCodec("XviD","","-ovc xvid -xvidencopts bitrate=$VIDBR:aspect=$ASPECT -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc xvid -xvidencopts bitrate=$VIDBR:turbo:pass=$PASS:aspect=$ASPECT -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc xvid -xvidencopts bitrate=$VIDBR:turbo:pass=$PASS:aspect=$ASPECT -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","mencoder");
      m_codecs[2]=_k9VideoCodec("x264","","-ovc x264 -x264encopts bitrate=$VIDBR:threads=0 -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc x264 -x264encopts bitrate=$VIDBR:turbo=1:pass=$PASS:threads=0 -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc x264 -x264encopts bitrate=$VIDBR:turbo=1:pass=$PASS:threads=0 -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","mencoder");     
      m_codecs[3]=_k9VideoCodec("MJPEG","","-ovc lavc -lavcopts vcodec=mjpeg:vhq:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc lavc -lavcopts vcodec=mjpeg:vhq:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT:turbo:vpass=$PASS -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc lavc -lavcopts vcodec=mjpeg:vhq:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT:turbo:vpass=$PASS -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","mencoder");
      m_codecs[4]=_k9VideoCodec("LJPEG","","-ovc lavc -lavcopts vcodec=ljpeg:vhq:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc lavc -lavcopts vcodec=ljpeg:vhq:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT:turbo:vpass=$PASS -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc lavc -lavcopts vcodec=ljpeg:vhq:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT:turbo:vpass=$PASS -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","mencoder");
      m_codecs[5]=_k9VideoCodec("H261","","-ovc lavc -lavcopts vcodec=h261:vhq:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc lavc -lavcopts vcodec=h261:vhq:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT:turbo:vpass=$PASS -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc lavc -lavcopts vcodec=h261:vhq:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT:turbo:vpass=$PASS -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","mencoder");
      m_codecs[6]=_k9VideoCodec("H263","","-ovc lavc -lavcopts vcodec=h263:vhq:v4mv:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc lavc -lavcopts vcodec=h263:vhq:v4mv:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT:turbo:vpass=$PASS -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc lavc -lavcopts vcodec=h263:vhq:v4mv:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT:turbo:vpass=$PASS -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","mencoder");
      m_codecs[7]=_k9VideoCodec("H263+","","-ovc lavc -lavcopts vcodec=h263p:vhq:v4mv:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc lavc -lavcopts vcodec=h263p:vhq:v4mv:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT:turbo:vpass=$PASS -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc lavc -lavcopts vcodec=h263p:vhq:v4mv:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT:turbo:vpass=$PASS -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","mencoder");
      m_codecs[8]=_k9VideoCodec("MPEG-4 (DivX 4/5)","DIVX","-ovc lavc -lavcopts vcodec=mpeg4:vhq:v4mv:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT  -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc lavc -lavcopts vcodec=mpeg4:vhq:v4mv:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT:turbo:vpass=$PASS -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc lavc -lavcopts vcodec=mpeg4:vhq:v4mv:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT:turbo:vpass=$PASS -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","mencoder");
      m_codecs[9]=_k9VideoCodec("MS MPEG-4 (DivX 3)","DIVX","-ovc lavc -lavcopts vcodec=msmpeg4:vhq:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc lavc -lavcopts vcodec=msmpeg4:vhq:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT:turbo:vpass=$PASS -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc lavc -lavcopts vcodec=msmpeg4:vhq:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT:turbo:vpass=$PASS -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","mencoder");
      m_codecs[10]=_k9VideoCodec("MS MPEG-4 v2","","-ovc lavc -lavcopts vcodec=msmpeg4v2:vhq:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc lavc -lavcopts vcodec=msmpeg4v2:vhq:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT:turbo:vpass=$PASS -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc lavc -lavcopts vcodec=msmpeg4v2:vhq:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT:turbo:vpass=$PASS -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","mencoder");
      m_codecs[11]=_k9VideoCodec("WMV7","","-ovc lavc -lavcopts vcodec=wmv1:vhq:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc lavc -lavcopts vcodec=wmv1:vhq:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT:turbo:vpass=$PASS -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc lavc -lavcopts vcodec=wmv1:vhq:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT:turbo:vpass=$PASS -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","mencoder");
      m_codecs[12]=_k9VideoCodec("WMV8","","-ovc lavc -lavcopts vcodec=wmv2:vhq:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc lavc -lavcopts vcodec=wmv2:vhq:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT:turbo:vpass=$PASS -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc lavc -lavcopts vcodec=wmv2:vhq:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT:turbo:vpass=$PASS -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","mencoder");

      m_codecs[13]=_k9VideoCodec("RealVideo","","-ovc lavc -lavcopts vcodec=rv10:vhq:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc lavc -lavcopts vcodec=rv10:vhq:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT:turbo:vpass=$PASS -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc lavc -lavcopts vcodec=rv10:vhq:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT:turbo:vpass=$PASS -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","mencoder");
      m_codecs[14]=_k9VideoCodec("MPEG-1 Video","","-ovc lavc -lavcopts vcodec=mpeg1video:vhq:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc lavc -lavcopts vcodec=mpeg1video:vhq:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT:turbo:vpass=$PASS -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc lavc -lavcopts vcodec=mpeg1video:vhq:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT:turbo:vpass=$PASS -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","mencoder");
      m_codecs[15]=_k9VideoCodec("MPEG-2 Video","","-ovc lavc -lavcopts vcodec=mpeg2video:vhq:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc lavc -lavcopts vcodec=mpeg2video:vhq:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT:turbo:vpass=$PASS -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc lavc -lavcopts vcodec=mpeg2video:vhq:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT:turbo:vpass=$PASS -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","mencoder");
      m_codecs[16]=_k9VideoCodec("Huffmann yuv","","-ovc lavc -lavcopts vcodec=huffyuv:vhq:v4mv:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT:format=422p -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc lavc -lavcopts vcodec=huffyuv:vhq:v4mv:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT::format=422p:turbo:vpass=$PASS -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc lavc -lavcopts vcodec=huffyuv:vhq:v4mv:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT::format=422p:turbo:vpass=$PASS -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","mencoder");
      m_codecs[17]=_k9VideoCodec("ffv Huffmann","","-ovc lavc -lavcopts vcodec=ffvhuff:vhq:v4mv:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc lavc -lavcopts vcodec=ffvhuff:vhq:v4mv:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT:turbo:vpass=$PASS -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc lavc -lavcopts vcodec=ffvhuff:vhq:v4mv:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT:turbo:vpass=$PASS -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","mencoder");
      m_codecs[18]=_k9VideoCodec("ASUS v1","","-ovc lavc -lavcopts vcodec=asv1:vhq:v4mv:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc lavc -lavcopts vcodec=asv1:vhq:v4mv:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT:turbo:vpass=$PASS -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc lavc -lavcopts vcodec=asv1:vhq:v4mv:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT:turbo:vpass=$PASS -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","mencoder");
      m_codecs[19]=_k9VideoCodec("ASUS v2","","-ovc lavc -lavcopts vcodec=asv2:vhq:v4mv:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc lavc -lavcopts vcodec=asv2:vhq:v4mv:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT:turbo:vpass=$PASS -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc lavc -lavcopts vcodec=asv2:vhq:v4mv:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT:turbo:vpass=$PASS -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","mencoder");
      m_codecs[20]=_k9VideoCodec("flv","","-ovc lavc -lavcopts vcodec=flv:vhq:v4mv:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc lavc -lavcopts vcodec=flv:vhq:v4mv:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT:turbo:vpass=$PASS -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","-ovc lavc -lavcopts vcodec=flv:vhq:v4mv:vqmin=2:vbitrate=$VIDBR:aspect=$ASPECT:turbo:vpass=$PASS -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP,scale=$WIDTH:$HEIGHT,dsize=$ASPECT","mencoder");
      m_codecs[21]=_k9VideoCodec("copy","","-vcodec copy","-vcodec copy","-vcodec copy","ffmpeg");
      m_codecs[22]=_k9VideoCodec("x264","","-b $VIDBRk -s $WIDTHx$HEIGHT -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP -aspect $ASPECT -vcodec libx264 -level 30","-b $VIDBRk -s $WIDTHx$HEIGHT -croptop $CROPTOP -cropbottom $CROPBOTTOM -cropleft $CROPLEFT -cropright $CROPRIGHT -aspect $ASPECT -vcodec libx264 -pass $PASS -passlogfile $PASSLOGFILE -level 30","-b $VIDBRk -s $WIDTHx$HEIGHT -croptop $CROPTOP -cropbottom $CROPBOTTOM -cropleft $CROPLEFT -cropright $CROPRIGHT -aspect $ASPECT -vcodec libx264 -pass $PASS -passlogfile $PASSLOGFILE -level 30","ffmpeg");
      m_codecs[23]=_k9VideoCodec("x264 (high)","","-b $VIDBRk -s $WIDTHx$HEIGHT -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP -aspect $ASPECT -vcodec libx264 -level 30 -flags +loop+mv4 -cmp 256 -partitions +parti4x4+parti8x8+partp4x4+partp8x8+partb8x8 -me_method hex -subq 7 -trellis 1 -refs 5 -bf 3 -flags2 +bpyramid+wpred+mixed_refs+dct8x8 -coder 1 -me_range 16 -g 250 -keyint_min 25 -sc_threshold 40 -i_qfactor 0.71 -qmin 10 -qmax 51 -qdiff 4","-b $VIDBRk -s $WIDTHx$HEIGHT -croptop $CROPTOP -cropbottom $CROPBOTTOM -cropleft $CROPLEFT -cropright $CROPRIGHT -aspect $ASPECT -vcodec libx264 -level 30 -flags +loop+mv4 -cmp 256 -partitions +parti4x4+parti8x8+partp4x4+partp8x8+partb8x8 -me_method hex -subq 7 -trellis 1 -refs 5 -bf 3 -flags2 +bpyramid+wpred+mixed_refs+dct8x8 -coder 1 -me_range 16 -g 250 -keyint_min 25 -sc_threshold 40 -i_qfactor 0.71 -qmin 10 -qmax 51 -qdiff 4 -pass $PASS -passlogfile $PASSLOGFILE","-b $VIDBRk -s $WIDTHx$HEIGHT -croptop $CROPTOP -cropbottom $CROPBOTTOM -cropleft $CROPLEFT -cropright $CROPRIGHT -aspect $ASPECT -vcodec libx264 -level 30 -flags +loop+mv4 -cmp 256 -partitions +parti4x4+parti8x8+partp4x4+partp8x8+partb8x8 -me_method hex -subq 7 -trellis 1 -refs 5 -bf 3 -flags2 +bpyramid+wpred+mixed_refs+dct8x8 -coder 1 -me_range 16 -g 250 -keyint_min 25 -sc_threshold 40 -i_qfactor 0.71 -qmin 10 -qmax 51 -qdiff 4 -pass $PASS -passlogfile $PASSLOGFILE","ffmpeg");
      m_codecs[24]=_k9VideoCodec("MPEG-4 ","","-b $VIDBRk -s $WIDTHx$HEIGHT -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP -aspect $ASPECT -vcodec mpeg4 ","-b $VIDBRk -s $WIDTHx$HEIGHT -croptop $CROPTOP -cropbottom $CROPBOTTOM -cropleft $CROPLEFT -cropright $CROPRIGHT -aspect $ASPECT -vcodec mpeg4  -pass $PASS -passlogfile $PASSLOGFILE","-b $VIDBRk -s $WIDTHx$HEIGHT -croptop $CROPTOP -cropbottom $CROPBOTTOM -cropleft $CROPLEFT -cropright $CROPRIGHT -aspect $ASPECT -vcodec mpeg4 -pass $PASS -passlogfile $PASSLOGFILE","ffmpeg");
      m_codecs[25]=_k9VideoCodec("Flash Video","","-b $VIDBRk -s $WIDTHx$HEIGHT -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP -aspect $ASPECT -vcodec flv ","-b $VIDBRk -s $WIDTHx$HEIGHT -croptop $CROPTOP -cropbottom $CROPBOTTOM -cropleft $CROPLEFT -cropright $CROPRIGHT -aspect $ASPECT -vcodec flv -pass $PASS -passlogfile $PASSLOGFILE","-b $VIDBRk -s $WIDTHx$HEIGHT -croptop $CROPTOP -cropbottom $CROPBOTTOM -cropleft $CROPLEFT -cropright $CROPRIGHT -aspect $ASPECT -vcodec flv -pass $PASS -passlogfile $PASSLOGFILE","ffmpeg");
      m_codecs[26]=_k9VideoCodec("Windows Media 8","","-b $VIDBRk -s $WIDTHx$HEIGHT -vf crop=$CROPWIDTH:$CROPHEIGHT:$CROPLEFT:$CROPTOP -aspect $ASPECT -vcodec wmv2","-b $VIDBRk -s $WIDTHx$HEIGHT -croptop $CROPTOP -cropbottom $CROPBOTTOM -cropleft $CROPLEFT -cropright $CROPRIGHT -aspect $ASPECT -vcodec wmv2 -pass $PASS -passlogfile $PASSLOGFILE","-b $VIDBRk -s $WIDTHx$HEIGHT -croptop $CROPTOP -cropbottom $CROPBOTTOM -cropleft $CROPLEFT -cropright $CROPRIGHT -aspect $ASPECT -vcodec wmv2 -pass $PASS -passlogfile $PASSLOGFILE","ffmpeg");

      save();

}

void k9VideoCodecs::save() {
   m_config=new k9Config();

#ifdef BACKLITE
    QSettings *group=m_config->getConfig();
#else
    KConfig *config= m_config->getConfig();
   KConfigGroup group=config->group("codecs");
#endif
   

    int cpt=0;
    for (QMap<int,_k9VideoCodec>::iterator i=m_codecs.begin();i!=m_codecs.end();++i) {
#ifdef BACKLITE
        group->beginGroup(QString("videocodec%1").arg(cpt));
        group->setValue("name", i.value().name);
        group->setValue("fourcc", i.value().fourcc);
        group->setValue("opt1",i.value().optOnePass);
        group->setValue("opt2", i.value().optPass1);
        group->setValue("opt3",i.value().optPass2);
        group->setValue("encoder",i.value().encoder);
        group->endGroup();
#else
	group=config->group(QString("videocodec%1").arg(cpt));
        group.writeEntry("name", i.value().name);
        group.writeEntry("fourcc", i.value().fourcc);
        group.writeEntry("opt1",i.value().optOnePass);
        group.writeEntry("opt2", i.value().optPass1);
        group.writeEntry("opt3",i.value().optPass2);
	group.writeEntry("encoder",i.value().encoder);
#endif
        cpt++;
    }

#ifdef BACKLITE
    group->beginGroup("codecs");
    group->setValue("VideoCount",cpt);
    group->endGroup();
    group->sync();
#else
    group=config->group("codecs");
    group.writeEntry("VideoCount",cpt);

    m_config->save();
#endif
    delete m_config;
}




int k9VideoCodecs::count() {
   return m_codecs.count();
}

void k9VideoCodecs::setFourcc(int _num,QString _value) {
    m_codecs[_num].fourcc=_value;
}
void k9VideoCodecs::setOptions0(int _num,QString _value) {
    m_codecs[_num].optOnePass=_value;
}
void k9VideoCodecs::setOptions1(int _num,QString _value) {
    m_codecs[_num].optPass1=_value;
}
void k9VideoCodecs::setOptions2(int _num,QString _value) {
    m_codecs[_num].optPass2=_value;
}

void k9VideoCodecs::setCodecName(int _num,QString _value) {
    m_codecs[_num].name=_value;
}
void k9VideoCodecs::setEncoder(int _num,QString _value) {
   m_codecs[_num].encoder=_value;
}
QString k9VideoCodecs::getFourcc(int _num) {
   return m_codecs[_num].fourcc;
}

QString k9VideoCodecs::getOptions0(int _num) {
   return m_codecs[_num].optOnePass;
}

QString k9VideoCodecs::getOptions1(int _num) {
   return m_codecs[_num].optPass1;
}

QString k9VideoCodecs::getOptions2(int _num) {
   return m_codecs[_num].optPass2;
}
QString k9VideoCodecs::getCodecName(int _num) {
   return m_codecs[_num].name;
}

int k9VideoCodecs::getAbsCodecNum(int _num,const QString &_encoder) {
   k9VideoCodecs v(0);
   int nb=v.count();
   int num=0;
   for (int i=0;i<nb;i++) {
      if (v.getEncoder(i) == _encoder) {
	  if (_num==num)
	     return i;
	  else num++;

      }
   }
   return 0;
}


QString k9VideoCodecs::getEncoder(int _num) {
    return m_codecs[_num].encoder=="" ? QString("mencoder"): m_codecs[_num].encoder;

}

void k9VideoCodecs::remove(int _num) {
    int nb=count();
    if (nb>0) {
        for(int i=_num;i<nb-1;i++) {
            m_codecs[i]=m_codecs[i+1];
        }
        m_codecs.remove(nb-1);
    }
}

k9VideoCodecs::~k9VideoCodecs()
{

}

