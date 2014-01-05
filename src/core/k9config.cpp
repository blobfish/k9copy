//
// C++ Implementation: k9config
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "k9config.h"
#include <KConfigGroup>
#include <KStandardDirs>
#include <QDir>
#include <KLocale>
#include "k9audiocodecs.h"
#include "k9videocodecs.h"

k9Config::k9Config() {
    m_config=NULL;
    read();
}

void k9Config::read() {
    // if (! m_config->hasGroup( "options"))
    // 	readOldConfig();
    if (m_config!=NULL)
        delete m_config;
    m_config=new KConfig("K9Copy");
  
    KConfigGroup group=m_config->group("dir");

// m_config->setGroup( "dir");
    m_prefOutput=group.readEntry("output",KStandardDirs::locateLocal("tmp","k9copy/",true));
    m_prefOutputMpeg4=group.readEntry("outputMpeg4",QDir::homePath() );
    m_prefOutputMpeg2=group.readEntry("outputMpeg2",QDir::homePath());
    m_prefOutputIso=group.readEntry("outputIso",QDir::homePath());
    m_prefOutputAudio=group.readEntry("outputAudio",QDir::homePath());
    
    group=m_config->group("programs");
    m_prefMencoderPath=group.readEntry("Mencoder","");
    if (m_prefMencoderPath.isEmpty()) m_prefMencoderPath=KStandardDirs::findExe("mencoder");
    
    m_prefMplayerPath=group.readEntry("Mplayer","");
    if (m_prefMplayerPath.isEmpty()) m_prefMplayerPath=KStandardDirs::findExe("mplayer");
    
    m_prefk3bPath=group.readEntry("k3b","");
    if (m_prefk3bPath.isEmpty()) m_prefk3bPath=KStandardDirs::findExe("k3b");
    
    m_prefDvdauthorPath=group.readEntry("Dvdauthor","");
    if (m_prefDvdauthorPath.isEmpty()) m_prefDvdauthorPath=KStandardDirs::findExe("dvdauthor");
    
    m_prefFFmpegPath=group.readEntry("FFMpeg","");
    if (m_prefFFmpegPath.isEmpty()) m_prefFFmpegPath=KStandardDirs::findExe("ffmpeg");
    
    m_prefWodimPath=group.readEntry("Wodim","");
    if (m_prefWodimPath.isEmpty()) m_prefWodimPath=KStandardDirs::findExe("wodim");
    
    m_prefGenisoimagePath=group.readEntry("Genisoimage","");
    if (m_prefGenisoimagePath.isEmpty()) m_prefGenisoimagePath=KStandardDirs::findExe("genisoimage");
    
    group=m_config->group("devices");
    m_InputDev=group.readEntry("input",0);
    m_OutputDev=group.readEntry("output",0);

    group=m_config->group("options");
    m_GtkStyle=group.readEntry("GtkStyle",false);
    m_prefK3b=group.readEntry("usek3b",false);
    m_prefAutoBurn=group.readEntry("autoburn",false);
    m_quickScan=group.readEntry("quickscan",true);
    m_prefSize=group.readEntry("dvdsize",4400);
    m_useDvdAuthor=group.readEntry("useDvdAuthor",true);
    m_keepMenus=group.readEntry("keepMenus",true);
    m_prefDelTmpFiles=group.readEntry("deltmpfiles",false);
    m_prefReadAhead=group.readEntry("readahead",-1);
    m_prefUserOps=group.readEntry("userops",false);
    QString lang(qgetenv("LANGUAGE"));
    QStringList llang;
    llang << lang.left(2);

    m_prefAudioLang=group.readEntry("audiolang",llang);
    m_prefSubtitleLang=group.readEntry("subtitlelang",llang);
    m_prefVersion=group.readEntry("version","0.0.0");

    group=m_config->group("mp4");
    m_prefMp4Codec=group.readEntry("codec",0);
    m_prefMp4Encoder=group.readEntry("encoder","mencoder");
    m_prefMp4Size=group.readEntry("size",700);
    m_prefMp4NumberCD=group.readEntry("numberCD",1);
    m_prefMp4Width=group.readEntry("width","720");
    m_prefMp4Height=group.readEntry("height","");
    m_prefMp4AspectRatio=group.readEntry("aspectRatio",true);
    m_prefMp42Passes=group.readEntry("2passes",false);
    m_prefMp4AudioBitrate=group.readEntry("audiobitrate","128");
    m_prefMp4VideoBitrate=group.readEntry("videobitrate" "");
    m_prefMp4AudioCodec=group.readEntry("audiocodec",0);
    m_prefMp4AudioGain=group.readEntry("audiogain",7);
    m_prefUseCellCache=group.readEntry("usecellcache",true);
    m_prefMpegChapters=group.readEntry("mpegchapters",true);


    group=m_config->group("devices");
    m_devices=group.readEntry("dev",QStringList());
    m_devicesLabels=group.readEntry("labels",QStringList());
    m_devicesIO=group.readEntry("IO",QStringList());

    group=m_config->group("preview");
    m_useMplayer=group.readEntry("useMplayer",false);
    m_useGL=group.readEntry("useGL",false);
    m_MplayerVout=group.readEntry("MplayerVout",0);
    m_MplayerAout=group.readEntry("MplayerAout",0);
    m_useXine=group.readEntry("useXine",false);
    m_XineVout=group.readEntry("XineVout",0);
    m_XineAout=group.readEntry("XineAout",0);
    m_usePhonon=group.readEntry("usePhonon",false);

    group=m_config->group("authoring");
    m_prefAudioFormat=group.readEntry("audioFormat","AC3");
    m_prefAudioBitrate=group.readEntry("audioBitrate",192);
    m_prefButtonWidth=group.readEntry("buttonWidth",50);
    m_prefButtonHeight=group.readEntry("buttonHeight",40);
    m_prefButtonHiliteColor=group.readEntry("buttonHiliteColor",QColor(Qt::yellow));
    m_prefButtonTextColor=group.readEntry("buttonTextColor",QColor(Qt::yellow));

    m_prefButtonFont=group.readEntry("buttonFont");

    group = m_config->group("gui");
    QList<bool> defCol;
    defCol << true << true <<true << true;
    m_prefColumns=group.readEntry<bool>("columns",defCol);

    m_prefShowOutput=group.readEntry("showOutput",true);
}

void k9Config::save() {
    KConfigGroup group=m_config->group("dir");
    group.writeEntry("output",m_prefOutput);
    group.writeEntry("outputMpeg4",m_prefOutputMpeg4);
    group.writeEntry("outputMpeg2",m_prefOutputMpeg2);
    group.writeEntry("outputAudio",m_prefOutputAudio);
    group.writeEntry("outputIso",m_prefOutputIso);
    
    group=m_config->group("programs");
    group.writeEntry("Mencoder",m_prefMencoderPath);
    group.writeEntry("Mplayer",m_prefMplayerPath);
    group.writeEntry("k3b",m_prefk3bPath);
    group.writeEntry("Dvdauthor",m_prefDvdauthorPath);
    group.writeEntry("FFmpeg",m_prefFFmpegPath);
    group.writeEntry("Wodim",m_prefWodimPath);
    group.writeEntry("Genisoimage",m_prefGenisoimagePath);
    
    group=m_config->group("devices");
    group.writeEntry("input", m_InputDev);
    group.writeEntry("output",m_OutputDev);

    group=m_config->group("options");
    group.writeEntry("GtkStyle",m_GtkStyle);
    group.writeEntry("usek3b",m_prefK3b);
    group.writeEntry("autoburn",m_prefAutoBurn);
    group.writeEntry("quickscan",m_quickScan);
    group.writeEntry("dvdsize",m_prefSize);
    group.writeEntry("useDvdAuthor",m_useDvdAuthor);
    group.writeEntry("keepMenus",m_keepMenus);
    group.writeEntry("deltmpfiles",m_prefDelTmpFiles);
    group.writeEntry("readahead",m_prefReadAhead);
    group.writeEntry("userops",m_prefUserOps);
    group.writeEntry("audiolang",m_prefAudioLang);
    group.writeEntry("subtitlelang",m_prefSubtitleLang);
    group.writeEntry("version",m_prefVersion);

    group=m_config->group("mp4");
    group.writeEntry("codec",m_prefMp4Codec);
    group.writeEntry("encoder",m_prefMp4Encoder);
    group.writeEntry("size",m_prefMp4Size);
    group.writeEntry("numberCD",m_prefMp4NumberCD);
    group.writeEntry("width",m_prefMp4Width);
    group.writeEntry("height",m_prefMp4Height);
    group.writeEntry("audiobitrate",m_prefMp4AudioBitrate);
    group.writeEntry("videobitrate",m_prefMp4VideoBitrate);
    group.writeEntry("aspectRatio",m_prefMp4AspectRatio);
    group.writeEntry("2passes",m_prefMp42Passes);
    group.writeEntry("audiogain",m_prefMp4AudioGain);
    group.writeEntry("audiocodec", m_prefMp4AudioCodec);
    group.writeEntry("usecellcache",m_prefUseCellCache);
    group.writeEntry("mpegchapters",m_prefMpegChapters);


    group=m_config->group("devices");
    group.writeEntry("dev",m_devices);
    group.writeEntry("labels",m_devicesLabels);
    group.writeEntry("IO",m_devicesIO);

    group=m_config->group("preview");
    group.writeEntry("useMplayer",m_useMplayer);
    group.writeEntry("useGL",m_useGL);
    group.writeEntry("MplayerVout",m_MplayerVout);
    group.writeEntry("MplayerAout",m_MplayerAout);
    group.writeEntry("useXine",m_useXine);
    group.writeEntry("XineVout",m_XineVout);
    group.writeEntry("XineAout",m_XineAout);
    group.writeEntry("usePhonon",m_usePhonon);

    group=m_config->group("authoring");
    group.writeEntry("audioFormat",m_prefAudioFormat);
    group.writeEntry("audioBitrate",m_prefAudioBitrate);
    group.writeEntry("buttonWidth",m_prefButtonWidth);
    group.writeEntry("buttonHeight",m_prefButtonHeight);
    group.writeEntry("buttonHiliteColor",m_prefButtonHiliteColor);
    group.writeEntry("buttonTextColor",m_prefButtonTextColor);
    group.writeEntry("buttonFont",m_prefButtonFont);

    group=m_config->group("gui");
    group.writeEntry("columns",m_prefColumns);
    group.writeEntry("showOutput",m_prefShowOutput);

}


k9Config::~k9Config() {
    delete m_config;

}




KConfig* k9Config::getConfig()  {
    if (m_config ==NULL)
	read();
    return m_config;
}


void k9Config::checkCodecs() {
    k9Config *config=new k9Config();
    if (config->getPrefVersion() != VERSION) {
        k9AudioCodecs acodec;
        acodec.reset();
        k9VideoCodecs vcodec;
        vcodec.reset();
        delete config;
        config=new k9Config();
        config->setPrefVersion(VERSION);
        config->save();
    }
    delete config;
}
