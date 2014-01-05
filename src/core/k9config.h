//
// C++ Interface: k9config
//
// Description:
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9CONFIG_H
#define K9CONFIG_H
#include "k9common.h"
#include <QColor>
#include <QFont>

#include <KConfig>
#include <kstandarddirs.h>
#include <KShell>
/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/
class k9Config {
private:
    KConfig *m_config;
    int  m_InputDev;
    int  m_OutputDev;
    bool m_keepMenus;
    QList <bool> m_prefColumns;
    QString m_prefOutput;
    QString m_prefOutputIso;
    QString m_prefOutputMpeg4;
    QString m_prefOutputMpeg2;
    QString m_prefOutputAudio;
    
    QString m_prefMencoderPath;
    QString m_prefMplayerPath;
    QString m_prefk3bPath;
    QString m_prefDvdauthorPath;
    QString m_prefFFmpegPath;
    QString m_prefWodimPath;
    QString m_prefGenisoimagePath;
    
    QString m_prefVersion;
    bool m_useDvdAuthor;
    bool m_quickScan;
    int  m_prefSize;
    bool m_prefK3b;
    bool m_prefMenu;
    bool m_prefAutoBurn;
    int  m_prefMp4Codec;
    QString m_prefMp4Encoder;
    int  m_prefMp4AudioCodec;
    int  m_prefMp4AudioGain;
    int  m_prefMp4Size;
    int  m_prefMp4NumberCD;
    int  m_prefAudioBitrate;
    int  m_prefButtonWidth;
    int  m_prefButtonHeight;
    int  m_prefReadAhead;
    bool m_prefMp4AspectRatio;
    bool m_prefMp42Passes;
    bool m_prefDelTmpFiles;
    bool m_prefUseCellCache;
    bool m_prefMpegChapters;
    bool m_prefUserOps;
    bool m_prefShowOutput;
    bool m_GtkStyle;
    QString  m_prefMp4Width;
    QString  m_prefMp4Height;
    QString  m_prefMp4AudioBitrate;
    QString  m_prefMp4VideoBitrate;
    QString  m_prefAudioFormat;
    QString m_prefCodecAudio,m_prefCodecVideo,m_prefCodecLabel;
    QStringList m_prefAudioLang,m_prefSubtitleLang;
    QStringList m_devices;
    QStringList m_devicesLabels;
    QStringList m_devicesIO;

    QColor  m_prefButtonHiliteColor;
    QColor  m_prefButtonTextColor;
    QFont   m_prefButtonFont;

    bool m_useMplayer;
    bool m_useGL;
    int  m_MplayerVout;
    int m_MplayerAout;

    bool m_useXine;
    int  m_XineVout;
    int m_XineAout;

    bool m_usePhonon;

public:
    k9Config();
    void read();
    void save();
    ~k9Config();

    void setInputDev ( const int& _value ) {
        m_InputDev = _value;
    }

    int getInputDev() const {
        return m_InputDev;
    }

    void setOutputDev ( const int& _value ) {
        m_OutputDev = _value;
    }


    int getOutputDev() const {
        return m_OutputDev;
    }

    void setKeepMenus ( bool _value ) {
        m_keepMenus = _value;
    }

    bool getMpegChapters() const {
        return m_prefMpegChapters;
    }

    void setMpegChapters ( bool _value ) {
        m_prefMpegChapters=_value;
    }
    bool getGtkStyle() const {
        return m_GtkStyle;
    }

    void setGtkStyle ( bool _value ) {
        m_GtkStyle=_value;
    }

    bool getKeepMenus() const {
        return m_keepMenus;
    }

    void setPrefOutput ( const QString& _value ) {
        m_prefOutput = _value;
    }

    QString getPrefOutput() const {
        return m_prefOutput;
    }

    void setPrefOutputMpeg4 ( const QString& _value ) {
        m_prefOutputMpeg4 = _value;
    }

    QString getPrefOutputMpeg4() const {
        return m_prefOutputMpeg4;
    }

    void setPrefOutputIso ( const QString& _value ) {
        m_prefOutputIso = _value;
    }

    QString getPrefOutputIso() const {
        return m_prefOutputIso;
    }
    void setPrefOutputMpeg2 ( const QString& _value ) {
        m_prefOutputMpeg2 = _value;
    }

    QString getPrefOutputMpeg2() const {
        return m_prefOutputMpeg2;
    }
    
    void setPrefOutputAudio ( const QString& _value ) {
        m_prefOutputAudio = _value;
    }

    QString getPrefOutputAudio() const {
        return m_prefOutputAudio;
    }    
    
    void setPrefAudioFormat ( const QString &_value ) {
        m_prefAudioFormat=_value;
    }

    QString getPrefAudioFormat() const {
        return m_prefAudioFormat;
    }

    void setPrefMencoderPath( const QString& _value ) {
        m_prefMencoderPath = _value;
    }

    QString getPrefMencoderPath(bool quoted) const {
      if (quoted)
        return KShell::quoteArg(m_prefMencoderPath);
      else
	return m_prefMencoderPath;
    }    

    
    void setPrefMplayerPath( const QString& _value ) {
        m_prefMplayerPath = _value;
    }

    QString getPrefMplayerPath(bool quoted) const {
      if (quoted)
        return KShell::quoteArg(m_prefMplayerPath);
      else
	return m_prefMplayerPath;
    }    
        
    void setPrefk3bPath( const QString& _value ) {
        m_prefk3bPath = _value;
    }

    QString getPrefk3bPath(bool quoted) const {
        if (quoted)
	  return KShell::quoteArg(m_prefk3bPath);
	else
	  return m_prefk3bPath;
    }    

    
    void setPrefDvdauthorPath( const QString& _value ) {
        m_prefDvdauthorPath = _value;
    }

    QString getPrefDvdauthorPath(bool quoted) const {
        if (quoted)
	  return KShell::quoteArg(m_prefDvdauthorPath);
	else
	  return m_prefDvdauthorPath;
    }    

    void setPrefFFmpegPath( const QString& _value ) {
        m_prefFFmpegPath = _value;
    }

    QString getPrefFFmpegPath(bool quoted) const {
        if (quoted)
	  return KShell::quoteArg(m_prefFFmpegPath);
	else
	  return m_prefFFmpegPath;
    }    

    void setPrefWodimPath( const QString& _value ) {
        m_prefWodimPath = _value;
    }

    QString getPrefWodimPath(bool quoted) const {
        if (quoted)
	  return KShell::quoteArg(m_prefWodimPath);
	else
	  return m_prefWodimPath;
    }    

    void setPrefGenisoimagePath( const QString& _value ) {
        m_prefGenisoimagePath = _value;
    }

    QString getPrefGenisoimagePath(bool quoted) const {
        if (quoted)
	  return KShell::quoteArg(m_prefGenisoimagePath);
	else
	  return m_prefGenisoimagePath;
    }    

    void setPrefVersion ( const QString& _value ) {
        m_prefVersion = _value;
    }

    QString getPrefVersion() const {
        return m_prefVersion;
    }

    void setUseDvdAuthor ( bool _value ) {
        m_useDvdAuthor = _value;
    }


    bool getUseDvdAuthor() const {
        return m_useDvdAuthor;
    }

    void setPrefDelTmpFiles ( bool _value ) {
        m_prefDelTmpFiles=_value;
    }

    bool getPrefDelTmpFiles() const {
        return m_prefDelTmpFiles;
    }

    void setPrefUseCellCache ( bool _value ) {
        m_prefUseCellCache=_value;
    }

    bool getPrefUseCellCache() const {
        return m_prefUseCellCache;
    }
    void setQuickScan ( bool _value ) {
        m_quickScan = _value;
    }


    bool getQuickScan() const {
        return m_quickScan;
    }

    void setPrefSize ( const int& _value ) {
        m_prefSize = _value;
    }


    int getPrefSize() const {
        return m_prefSize;
    }

    void setPrefK3b ( bool _value ) {
        m_prefK3b = _value;
    }


    bool getPrefK3b() const {
        return m_prefK3b;
    }

    void setPrefMenu ( bool _value ) {
        m_prefMenu = _value;
    }


    bool getPrefMenu() const {
        return m_prefMenu;
    }

    void setPrefAutoBurn ( bool _value ) {
        m_prefAutoBurn = _value;
    }


    bool getPrefAutoBurn() const {
        return m_prefAutoBurn;
    }

    void setPrefMp4Codec ( const int& _value ) {
        m_prefMp4Codec = _value;
    }

    int getPrefMp4Codec() const {
        return m_prefMp4Codec;
    }

    void setPrefMp4Encoder ( const QString& _value ) {
        m_prefMp4Encoder = _value;
    }

    QString getPrefMp4Encoder() const {
        return m_prefMp4Encoder;
    }

    void setPrefMp4AudioCodec ( const int& _value ) {
        m_prefMp4AudioCodec = _value;
    }

    int getPrefMp4AudioCodec() const {
        return m_prefMp4AudioCodec;
    }


    void setPrefMp4Size ( const int& _value ) {
        m_prefMp4Size = _value;
    }


    int getPrefMp4Size() const {
        return m_prefMp4Size;
    }

    void setPrefMp4AudioGain ( const int& _value ) {
        m_prefMp4AudioGain = _value;
    }


    int getPrefMp4AudioGain() const {
        return m_prefMp4AudioGain;
    }

    void setPrefMp4NumberCD ( const int& _value ) {
        m_prefMp4NumberCD = _value;
    }


    int getPrefMp4NumberCD() const {
        return m_prefMp4NumberCD;
    }

    void setPrefMp4Width ( const QString& _value ) {
        m_prefMp4Width = _value;
    }


    QString getPrefMp4Width() const {
        return m_prefMp4Width;
    }

    void setPrefMp4Height ( const QString& _value ) {
        m_prefMp4Height = _value;
    }


    QString getPrefMp4Height() const {
        return m_prefMp4Height;
    }

    void setPrefMp4AudioBitrate ( const QString& _value ) {
        m_prefMp4AudioBitrate = _value;
    }


    QString getPrefMp4AudioBitrate() const {
        return m_prefMp4AudioBitrate;
    }

    void setPrefMp4VideoBitrate ( const QString& _value ) {
        m_prefMp4VideoBitrate = _value;
    }


    QString getPrefMp4VideoBitrate() const {
        return m_prefMp4VideoBitrate;
    }


    void setDevices ( const QStringList& _value ) {
        m_devices = _value;
    }


    QStringList getDevices() const {
        return m_devices;
    }

    void setDevicesLabels ( const QStringList& _value ) {
        m_devicesLabels = _value;
    }


    QStringList getDevicesLabels() const {
        return m_devicesLabels;
    }

    void setDevicesIO ( const QStringList& _value ) {
        m_devicesIO = _value;
    }


    QStringList getDevicesIO() const {
        return m_devicesIO;
    }

    void setUseMplayer ( bool _value ) {
        m_useMplayer = _value;
    }


    bool getUseMplayer() const {
        return m_useMplayer;
    }

    void setUseXine ( bool _value ) {
        m_useXine = _value;
    }


    bool getUseXine() const {
        return m_useXine;
    }

    void setUsePhonon ( bool _value ) {
        m_usePhonon=_value;
    }

    bool getUsePhonon() const {
        return m_usePhonon;
    }

    void setUseGL ( bool _value ) {
        m_useGL = _value;
    }


    bool getUseGL() const {
        return m_useGL;
    }

    void setPrefMp4AspectRatio ( bool _value ) {
        m_prefMp4AspectRatio = _value;
    }


    bool getPrefMp4AspectRatio() const {
        return m_prefMp4AspectRatio;
    }

    void setPrefMp42Passes ( bool _value ) {
        m_prefMp42Passes = _value;
    }


    bool getPrefMp42Passes() const {
        return m_prefMp42Passes;
    }

    void setMplayerVout ( const int& _value ) {
        m_MplayerVout = _value;
    }


    int getMplayerVout() const {
        return m_MplayerVout;
    }

    void setMplayerAout ( const int& _value ) {
        m_MplayerAout = _value;
    }


    int getMplayerAout() const {
        return m_MplayerAout;
    }

    void setXineVout ( const int& _value ) {
        m_XineVout = _value;
    }


    int getXineVout() const {
        return m_XineVout;
    }

    void setXineAout ( const int& _value ) {
        m_XineAout = _value;
    }


    int getXineAout() const {
        return m_XineAout;
    }

    bool getPrefShowOutput() const{
        return m_prefShowOutput;
    }

    void setPrefShowOutput(bool _value) {
        m_prefShowOutput=_value;
    }


    void setPrefAudioBitrate ( int _value ) {
        m_prefAudioBitrate = _value;
    }


    int getPrefAudioBitrate() const {
        return m_prefAudioBitrate;
    }

    void setPrefButtonWidth ( int _value ) {
        m_prefButtonWidth = _value;
    }


    int getPrefButtonWidth() const {
        return m_prefButtonWidth;
    }

    void setPrefButtonHeight ( int _value ) {
        m_prefButtonHeight = _value;
    }


    int getPrefButtonHeight() const {
        return m_prefButtonHeight;
    }

    void setPrefButtonHiliteColor ( const QColor& _value ) {
        m_prefButtonHiliteColor = _value;
    }


    QColor getPrefButtonHiliteColor() const {
        return m_prefButtonHiliteColor;
    }

    void setPrefButtonTextColor ( const QColor& _value ) {
        m_prefButtonTextColor = _value;
    }


    QColor getPrefButtonTextColor() const {
        return m_prefButtonTextColor;
    }


    void setPrefButtonFont ( const QFont& _value ) {
        m_prefButtonFont = _value;
    }


    QFont getPrefButtonFont() const {
        return m_prefButtonFont;
    }

    void setPrefReadAhead ( int _value ) {
        m_prefReadAhead = _value;
    }


    int getPrefReadAhead() const {
        return m_prefReadAhead;
    }

    KConfig* getConfig() ;

    void setPrefUserOps ( bool theValue ) {
        m_prefUserOps = theValue;
    }


    bool getPrefUserOps() const {
        return m_prefUserOps;
    }

    void setPrefAudioLang ( const QStringList& theValue ) {
        m_prefAudioLang = theValue;
    }


    QStringList getPrefAudioLang() const {
        return m_prefAudioLang;
    }

    void setPrefSubtitleLang ( const QStringList& theValue ) {
        m_prefSubtitleLang = theValue;
    }


    QStringList getPrefSubtitleLang() const {
        return m_prefSubtitleLang;
    }

    QList <bool> getPrefColumns() const {
        return m_prefColumns;
    }

    void setPrefColumns(const QList <bool> & value) {
        m_prefColumns=value;
    }
    static void checkCodecs();
};

#endif
