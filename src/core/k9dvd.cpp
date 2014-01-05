/**************************************************************************
*   Copyright (C) 2005 by Jean-Michel Petit                               *
*   k9copy@free.fr                                                            *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/

#include "k9dvd.h"
#include "k9dvdtitle.h"
#include "k9dvdtitleset.h"
#include "k9cellcopylist.h"
#include "k9dvdprogress.h"
#include "k9ifo2.h"
#include <QApplication>
#include <sys/stat.h>


k9DVDTitle* k9DVD::gettitle(int num) {
    return ((k9DVDTitle*)m_titles.at(num));
}

k9DVDTitle* k9DVD::gettitleByNum(int VTS,int numPgc) {
    for (int i=0; i < m_titles.count();i++) {
        k9DVDTitle *track=(k9DVDTitle*)m_titles.at(i);
        if (track->getIndexed()) {          
            if ((track->getpgc()==(numPgc-1)) && (track->getVTS()==VTS))
                return track;

            //if (track->getnumTitle()==num)
                //return track;
        }
    }
// if no title found
    for (int i=0; i < m_titles.count();i++) {
        k9DVDTitle *track=(k9DVDTitle*)m_titles.at(i);
            if ((track->getpgc()==(numPgc-1)) && (track->getVTS()==VTS))
                return track;
    }


    return (NULL);
}


/** Read property of int longestTitle. */
k9DVDTitle *k9DVD::getlongestTitle() {
    return m_longestTitle;
}

/** Read property of QString title. */
const QString& k9DVD::getDVDTitle() {
    return m_title;
}

void k9DVD::setDVDTitle(const QString &_newVal) {
    m_title=_newVal;
}

/** Read property of int titleCount. */
const int& k9DVD::gettitleCount() {
    return m_titleCount;
}

/** Read property of int titlesetCount. */
const int& k9DVD::gettitlesetCount() {
    return m_titlesetCount;
}

k9DVDTitleset *k9DVD::gettitleset(int num) {
    if (num >=m_titlesets.count())
        return NULL;
    else
        return m_titlesets[num];
}


int k9DVD::getmenuSize() {
    return m_menuSize;
}

k9DVD::k9DVD(QObject  *parent) :QObject(parent)  {
    m_error=false;
    m_opened=false;
    m_arrLng=new lng[200];
    k9DVD::LangList(m_arrLng);

//   m_titles.setAutoDelete(true);
//   m_titlesets.setAutoDelete(true);
    m_lvideoFormat.append("NTSC");
    m_lvideoFormat.append("PAL");

    m_laspectRatio.append("4:3");
    m_laspectRatio.append("16:9");
    m_laspectRatio.append("?:?");
    m_laspectRatio.append("16:9");

    m_lquantization.append("16bps");
    m_lquantization.append("20bps");
    m_lquantization.append("24bps");
    m_lquantization.append("drc");

    m_lmpegVersion.append("mpeg1");
    m_lmpegVersion.append("mpeg2");

    m_lvideoHeight.append("480");
    m_lvideoHeight.append("576");
    m_lvideoHeight.append("???");
    m_lvideoHeight.append("576");

    m_lvideoWidth.append("720");
    m_lvideoWidth.append("704");
    m_lvideoWidth.append("352");
    m_lvideoWidth.append("352");

    m_lpermittedDf.append("");
    m_lpermittedDf.append("noletterbox");
    m_lpermittedDf.append("nopanscan");
    m_lpermittedDf.append("noletterbox & nopanscan");

    m_laudioFormat.append("ac3");
    m_laudioFormat.append("?");
    m_laudioFormat.append("mp2");
    m_laudioFormat.append("mp2");
    m_laudioFormat.append("pcm");
    m_laudioFormat.append("sdds");
    m_laudioFormat.append("dts");

    m_lsampleFreq.append("48kHz");
    m_lsampleFreq.append("48kHz");
    m_lsampleFreq.append("Error");

    m_laudioType.append("");
    m_laudioType.append("Normal");
    m_laudioType.append(i18n("for visually impaired"));
    m_laudioType.append(i18n("director's comments"));
    m_laudioType.append(i18n("alternate director's comments"));

    m_lsubpType.append("");
    m_lsubpType.append("Normal");
    m_lsubpType.append(i18n("Large"));
    m_lsubpType.append(i18n("Children"));
    m_lsubpType.append(i18n("reserved"));
    m_lsubpType.append(i18n("Normal captions"));
    m_lsubpType.append(i18n("Large captions"));
    m_lsubpType.append(i18n("Children captions"));
    m_lsubpType.append("");
    m_lsubpType.append(i18n("Forced"));
    m_lsubpType.append(i18n("reserved"));
    m_lsubpType.append(i18n("reserved"));
    m_lsubpType.append(i18n("reserved"));
    m_lsubpType.append(i18n("Director's comments"));
    m_lsubpType.append(i18n("Large director's comments"));
    m_lsubpType.append(i18n("Director's comments for children"));


    m_frames_per_s[0]=-1.0;
    m_frames_per_s[1]=25.00;
    m_frames_per_s[2]=-1.0;
    m_frames_per_s[3]=29.97;

    m_start=NULL;
}
k9DVD::~k9DVD() {
    if (m_dvd.opened())
        m_dvd.close();
    deleteTitles();
    deleteTitlesets();
    delete[] m_arrLng;
}

void k9DVD::deleteTitles() {
    //while (!m_titles.isEmpty()) delete m_titles.takeFirst();
    qDeleteAll(m_titles);
    m_titles.clear();
}
void k9DVD::deleteTitlesets() {
// while (!m_titlesets.isEmpty())delete m_titlesets.takeFirst();
    qDeleteAll(m_titlesets);
    m_titlesets.clear();
}

int k9DVD::dvdtime2msec(dvd_time_t *dt) {
    double fps = m_frames_per_s[(dt->frame_u & 0xc0) >> 6];
    long   ms;
    ms  = (((dt->hour &   0xf0) >> 3) * 5 + (dt->hour   & 0x0f)) * 3600000;
    ms += (((dt->minute & 0xf0) >> 3) * 5 + (dt->minute & 0x0f)) * 60000;
    ms += (((dt->second & 0xf0) >> 3) * 5 + (dt->second & 0x0f)) * 1000;

    if (fps > 0)
        ms +=(long)( ((dt->frame_u & 0x30) >> 3) * 5 + (dt->frame_u & 0x0f) * 1000.0 / fps);

    return ms;
}

/*
*  The following method is based on code from vobcopy, by Robos, with thanks.
*/

int k9DVD::get_title_name(const char* dvd_device, char* title) {
    FILE *filehandle = 0;
    int  i;
    QString c;
    if (! (filehandle = fopen(dvd_device, "r"))) {
        c=i18n("Couldn't open %1 for title\n").arg(dvd_device);
        // setError(c );
        strcpy(title, i18n("unknown").toUtf8());
        return -1;
    }

    if ( fseek(filehandle, 32808, SEEK_SET )) {
        fclose(filehandle);
        c=i18n("Couldn't seek in %1 for title\n").arg(dvd_device);
        setError(c);
        strcpy(title, i18n("unknown").toUtf8());
        return -1;
    }

    if ( 32 != (i = fread(title, 1, 32, filehandle)) ) {
        fclose(filehandle);
        //        c=i18n("Couldn't read enough bytes for title.\n");
        //        setError(c);
        strcpy(title, i18n("unknown").toUtf8());
        return 0;
    }

    fclose (filehandle);

    title[32] = '\0';
    while (i-- > 2)
        if (title[i] == ' ')
            title[i] = '\0';
    return 0;
}


void k9DVD::LangList(lng *_arrLng) {
        lng arrLng[] = {
            { "  ", i18n("Not Specified") }, { "aa", i18n("Afar") },	{ "ab", i18n("Abkhazian") }, { "af", i18n("Afrikaans") },	{ "am", i18n("Amharic")},
            { "ar", i18n("Arabic") }, { "as", i18n("Assamese") },	{ "ay", i18n("Aymara") }, { "az", i18n("Azerbaijani") }, { "ba", i18n("Bashkir") },
            { "be", i18n("Byelorussian") }, { "bg", i18n("Bulgarian") }, { "bh", i18n("Bihari") }, { "bi", i18n("Bislama") }, { "bn", i18n("Bengali; Bangla") },
            { "bo", i18n("Tibetan") }, { "br", i18n("Breton") }, { "ca", i18n("Catalan") }, { "co", i18n("Corsican") }, { "cs", i18n("Czech") },
            { "cy", i18n("Welsh") }, { "da", i18n("Dansk") }, { "de", i18n("Deutsch") }, { "dz", i18n("Bhutani") }, { "el", i18n("Greek") }, { "en", i18n("English") },
            { "eo", i18n("Esperanto") }, { "es", i18n("Espanol") }, { "et", i18n("Estonian") }, { "eu", i18n("Basque") }, { "fa", i18n("Persian") },
            { "fi", i18n("Suomi") }, { "fj", i18n("Fiji") }, { "fo", i18n("Faroese") }, { "fr", i18n("Francais") }, { "fy", i18n("Frisian") }, { "ga", i18n("Gaelic") },
            { "gd", i18n("Scots Gaelic") }, { "gl", i18n("Galician") }, { "gn", i18n("Guarani") }, { "gu", i18n("Gujarati") }, { "ha", i18n("Hausa") },
            { "he", i18n("Hebrew") }, { "hi", i18n("Hindi") }, { "hr", i18n("Hrvatski") }, { "hu", i18n("Magyar") }, { "hy", i18n("Armenian") },
            { "ia", i18n("Interlingua") }, { "id", i18n("Indonesian") }, { "ie", i18n("Interlingue") }, { "ik", i18n("Inupiak") }, { "in", i18n("Indonesian") },
            { "is", i18n("Islenska") }, { "it", i18n("Italiano") }, { "iu", i18n("Inuktitut") }, { "iw", i18n("Hebrew") }, { "ja", i18n("Japanese") },
            { "ji", i18n("Yiddish") }, { "jw", i18n("Javanese") }, { "ka", i18n("Georgian") }, { "kk", i18n("Kazakh") }, { "kl", i18n("Greenlandic") },
            { "km", i18n("Cambodian") }, { "kn", i18n("Kannada") }, { "ko", i18n("Korean") }, { "ks", i18n("Kashmiri") }, { "ku", i18n("Kurdish") },
            { "ky", i18n("Kirghiz") }, { "la", i18n("Latin") }, { "ln", i18n("Lingala") }, { "lo", i18n("Laothian") }, { "lt", i18n("Lithuanian") },
            { "lv", i18n("Latvian, Lettish") }, { "mg", i18n("Malagasy") }, { "mi", i18n("Maori") }, { "mk", i18n("Macedonian") }, { "ml", i18n("Malayalam") },
            { "mn", i18n("Mongolian") }, { "mo", i18n("Moldavian") }, { "mr", i18n("Marathi") }, { "ms", i18n("Malay") }, { "mt", i18n("Maltese") },
            { "my", i18n("Burmese") }, { "na", i18n("Nauru") }, { "ne", i18n("Nepali") }, { "nl", i18n("Nederlands") }, { "no", i18n("Norsk") }, { "oc", i18n("Occitan") },
            { "om", i18n("Oromo") }, { "or", i18n("Oriya") }, { "pa", i18n("Punjabi") }, { "pl", i18n("Polish") }, { "ps", i18n("Pashto, Pushto") },
            { "pt", i18n("Portugues") }, { "qu", i18n("Quechua") }, { "rm", i18n("Rhaeto-Romance") }, { "rn", i18n("Kirundi") }, { "ro", i18n("Romanian")  },
            { "ru", i18n("Russian") }, { "rw", i18n("Kinyarwanda") }, { "sa", i18n("Sanskrit") }, { "sd", i18n("Sindhi") }, { "sg", i18n("Sangho") },
            { "sh", i18n("Serbo-Croatian") }, { "si", i18n("Sinhalese") }, { "sk", i18n("Slovak") }, { "sl", i18n("Slovenian") }, { "sm",i18n( "Samoan") },
            { "sn", i18n("Shona")  }, { "so", i18n("Somali") }, { "sq", i18n("Albanian") }, { "sr", i18n("Serbian") }, { "ss", i18n("Siswati") },
            { "st", i18n("Sesotho") }, { "su", i18n("Sundanese") }, { "sv", i18n("Svenska") }, { "sw", i18n("Swahili") }, { "ta", i18n("Tamil") },
            { "te", i18n("Telugu") }, { "tg", i18n("Tajik") }, { "th", i18n("Thai") }, { "ti", i18n("Tigrinya") }, { "tk", i18n("Turkmen") }, { "tl", i18n("Tagalog") },
            { "tn", i18n("Setswana") }, { "to", i18n("Tonga") }, { "tr", i18n("Turkish") }, { "ts", i18n("Tsonga") }, { "tt", i18n("Tatar") }, { "tw", i18n("Twi") },
            { "ug", i18n("Uighur") }, { "uk", i18n("Ukrainian") }, { "ur", i18n("Urdu") }, { "uz", i18n("Uzbek") }, { "vi", i18n("Vietnamese") },
            { "vo", i18n("Volapuk") }, { "wo", i18n("Wolof") }, { "xh", i18n("Xhosa") }, { "yi", i18n("Yiddish") }, { "yo", i18n("Yoruba") }, { "za", i18n("Zhuang") },
            { "zh", i18n("Chinese") }, { "zu", i18n("Zulu") }, { "xx", i18n("Unknown") }, { "\0", i18n("Unknown") }
        };
        for (uint i=0; i <(sizeof(arrLng)/sizeof(lng));i++) {
            _arrLng[i]=arrLng[i];
        }
}


QString k9DVD::lang_name(lng *arrLng,const QString& code,const QString & name) {
    QString c;
    int i=0;
    //lng* arrLng=m_arrLng;
    c=i18n("Unknown");
    for (i=0 ; arrLng[i].code[0]!=0;i++) {
        lng l =arrLng[i];
        if (name=="") {
            if (code==l.code) {
                c = l.name;
            }
        } else {
            if (name==l.name) {
                c = l.code;
            }

        }
    }
    return c;
}


int k9DVD::calcNumTitle(ifo_handle_t *ifo,int _vts,int _ttn) {
    for (int i=0; i< ifo->tt_srpt->nr_of_srpts;i++) {
        if (ifo->tt_srpt->title[i].title_set_nr==_vts && ifo->tt_srpt->title[i].vts_ttn == _ttn)
            return (i+1);
    }
    return 0;

}

int k9DVD::scandvd (const QString & device,bool _quickScan) {
    char ctitle[255];
    k9DVDChapter::setcptChapter(0);
    ifo_handle_t *ifo_zero, *ifo;
    pgcit_t *vts_pgcit;
    vtsi_mat_t *vtsi_mat;
    vmgi_mat_t *vmgi_mat;
    audio_attr_t *audio_attr;
    video_attr_t *video_attr;
    subp_attr_t *subp_attr;
    pgc_t *pgc;
    int i, j,  ltitles, cell, vts_ttn, title_set_nr;
    QString langCode;
    int has_title = 0;
    int max_length = 0;
    bool ok;
    tt_srpt_t   *tt_srpt;
    QString txt,c;

    m_start=NULL;
    int menuSizes[100];
    for ( j=0; j<100;j++)
        menuSizes[j]=0;

    deleteTitles();
    m_Device=device;

    m_error=false;
    m_errMsg="";

    if (!_quickScan) {
        m_progressDlg= new k9DVDProgress(0);
        m_progressDlg->setpbTitleStep(0);
        m_progressDlg->setpbTotalStep(0);

        connect(this, SIGNAL(sigVobProgress(unsigned int,unsigned int)), this, SLOT(slotVobProgress(unsigned int,unsigned int)));
        connect(this, SIGNAL(sigTitleProgress(unsigned int,unsigned int)), this, SLOT(slotTitleProgress(unsigned int,unsigned int)));
        connect(this, SIGNAL(sigTitleText(QString&)), this, SLOT(slotTitleText(QString&)));
        connect(this, SIGNAL(sigTotalText(QString&)), this, SLOT(slotTotalText(QString&)));
        m_progressDlg->show();
    } else
        m_progressDlg=NULL;

    k9Tools::processEvents();

    k9DVDTitle *l_track;
    k9DVDAudioStream *l_auds;
    k9DVDChapter *l_chap;
    k9DVDSubtitle *l_sub;
    QString sh,sm,ss;
    /*
        ret = stat(device.latin1(), &dvd_stat);
        if ( ret < 0 ) {
            c=i18n("Can't find device %1\n").arg( device);
            setError(c);
            return 1;
        }
    */

    if (m_dvd.opened())
        m_dvd.close();
    m_dvd.openDevice(device);
    if ( !m_dvd.opened() ) {
        c=i18n("Can't open disc %1!\n").arg(device);
        setError(c);
        return 2;
    }

//    k9Ifo2 kifo_zero(&m_dvd),kifo(&m_dvd);
//    kifo_zero.openIFO( 0);
    k9Ifo2 *kifo_zero= m_dvd.getIfo(0);
    k9Ifo2 *kifo;

    ifo_zero = kifo_zero->getIFO();
    if ( !ifo_zero ) {
        c=i18n("Can't open main ifo!\n");
        setError(c);
        return 3;
    }

    ltitles = ifo_zero->tt_srpt->nr_of_srpts;
    m_titleCount = 0;
    has_title = get_title_name(device.toUtf8(), ctitle);

    vmgi_mat = ifo_zero->vmgi_mat;
    m_titlesetCount = vmgi_mat->vmg_nr_of_title_sets;

    menuSizes[0]=vmgi_mat->vmg_last_sector;

    m_title=( has_title ? i18n("unknown") : ctitle);

    for (int ts=1;ts <=m_titlesetCount;ts++) {
        tt_srpt = ifo_zero->tt_srpt;
        kifo=m_dvd.getIfo(ts);
        ifo = kifo->getIFO();
        if (ifo==NULL) {
            //ifo is null when trying to open a protected dvd
            setError(i18n("Can't open disc %1!\n").arg(device));
            return 2;
            //continue;
        }
        //add the titleset in the titleset list
        k9DVDTitleset *titleset = new k9DVDTitleset(ts,ifo->vtsi_mat->vts_last_sector -ifo->vtsi_mat->vtstt_vobs-1);
        m_titlesets.append(titleset);

        for (j=0; j < ifo->vts_pgcit->nr_of_pgci_srp; j++) {
            // tt_srpt->title[j].title_set_nr);
            // GENERAL
            vtsi_mat   = ifo->vtsi_mat;
            vts_pgcit  = ifo->vts_pgcit;
            pgc = vts_pgcit->pgci_srp[j].pgc;
            //retrieve the ttn
            int ttn=(vts_pgcit->pgci_srp[j].entry_id) & 0x7F;
            bool entryPgc= (vts_pgcit->pgci_srp[j].entry_id & 0x80)==0x80;
            int numTitle=calcNumTitle(ifo_zero,ts,ttn);
            //JMP : vérifier la numérotation des titres ......
            if (vtsi_mat && (pgc->nr_of_cells >0)) {
                m_titleCount++;
                vts_ttn =  ttn;//ifo->vts_ptt_srpt->title[j].ptt[0].pgcn; //ifo_zero->tt_srpt->title[j].vts_ttn;

                //JMPtxt=i18n("Title %1").arg(indexedCount);
                txt=i18n("Title %1").arg(numTitle);
                emit sigTotalText (txt);
                emit sigTitleProgress(numTitle,ltitles);
                video_attr = &vtsi_mat->vts_video_attr;

                vmgi_mat = ifo_zero->vmgi_mat;
                title_set_nr = ts; //ifo_zero->tt_srpt->title[j].title_set_nr;
                menuSizes[title_set_nr]=vtsi_mat->vtstt_vobs + vtsi_mat->vtsi_last_sector +1;

                //vts_pgcit->pgci_srp[ifo->vts_ptt_srpt->title[vts_ttn - 1].ptt[0].pgcn - 1].pgc;

                int titleStartSector=pgc->cell_playback[0].first_sector;
                //l_track=addTitle(j+1,title_set_nr,ifo->vts_ptt_srpt->title[vts_ttn - 1].ptt[0].pgcn - 1,titleStartSector,isTitleIndex(ifo_zero,ts,vts_ttn));
                l_track=addTitle(titleset,m_titleCount, numTitle,title_set_nr,j,titleStartSector, entryPgc);
                titleset->add
                (l_track);

                sh.sprintf("%02x",pgc->playback_time.hour);
                sm.sprintf("%02x",pgc->playback_time.minute);
                ss.sprintf("%02x",pgc->playback_time.second);
                l_track->length.setHMS(sh.toInt(&ok,10),sm.toInt(&ok,10),ss.toInt(&ok,10));

                //printf(i18n("Title: %02d, Length: %02x:%02x:%02x "), j+1, pgc->playback_time.hour, pgc->playback_time.minute, pgc->playback_time.second);

                if (dvdtime2msec(&pgc->playback_time) > max_length && entryPgc) {
                    max_length = dvdtime2msec(&pgc->playback_time);
                    m_longestTitle = l_track;
                }

                l_track->chapterCount =  pgc->nr_of_programs;//  ifo_zero->tt_srpt->title[j].nr_of_ptts;
                l_track->audioStreamCount = vtsi_mat->nr_of_vts_audio_streams;
                l_track->subPictureCount = vtsi_mat->nr_of_vts_subp_streams;
                l_track->VTS = ts;//  ifo_zero->tt_srpt->title[j].title_set_nr;
                l_track->TTN = ttn; // ifo_zero->tt_srpt->title[j].vts_ttn;
                l_track->FPS = m_frames_per_s[(pgc->playback_time.frame_u & 0xc0) >> 6];
                l_track->format= m_lvideoFormat[video_attr->video_format];
                m_format = l_track->format;
                /*      QStringList::Iterator it;
                it= videoFormat.at(video_attr->video_format);
                c= (*it).latin1();
                */

                l_track->aspectRatio =m_laspectRatio[video_attr->display_aspect_ratio];
                l_track->width = m_lvideoWidth[video_attr->picture_size];
                ;
                l_track->height = m_lvideoHeight[video_attr->video_format];
                l_track->DF = m_lpermittedDf[video_attr->permitted_df];

                for (i=0; i<16; i++) {
                    QString pal;
                    pal.sprintf("%08x",pgc->palette[i]);
                    l_track->palette.append(pal);
                }

                // ANGLES
                l_track->angleCount =  ifo_zero->tt_srpt->title[j].nr_of_angles;

                // AUDIO
                //       		for (i=0; i<vtsi_mat->nr_of_vts_audio_streams; i++) {
                l_track->audioStreamCount=0;
                for (i=0; i<vtsi_mat->nr_of_vts_audio_streams; i++) {
                    if (pgc->audio_control[i]>>8 !=0) {
                        l_track->audioStreamCount++;
                        l_auds=l_track->addAudioStream();
                        l_auds->id = 1+ ((pgc->audio_control[i]>>8) & 0x7) ;
                        //this only true for AC3 streams
                        l_auds->m_streamId = (pgc->audio_control[i]>>8);
                        audio_attr = &vtsi_mat->vts_audio_attr[i];
                        //dts stream starts at 0x88
                        if (audio_attr->audio_format==6)
                            l_auds->m_streamId +=8;
                        //JMPaudio_attr = &vtsi_mat->vts_audio_attr[l_auds->id-1];
                        langCode.sprintf("%c%c", audio_attr->lang_code>>8, audio_attr->lang_code & 0xff);
                        if (! (audio_attr->lang_code>>8)) {
                            langCode = "xx";
                        }
                        l_auds->langCod=langCode;
                        l_auds->language=lang_name(m_arrLng,l_auds->langCod,"");
                        if (l_auds->language==i18n("Unknown"))
                            l_auds->langCod="xx";

                        l_auds->format= m_laudioFormat[audio_attr->audio_format];
                        l_auds->frequency = m_lsampleFreq[audio_attr->sample_frequency];
                        l_auds->quantization = m_lquantization[audio_attr->quantization];
                        l_auds->channels = audio_attr->channels+1;
                        l_auds->appMode = audio_attr->application_mode;
                        l_auds->content = m_laudioType[audio_attr->code_extension];
                        //if (((pgc->audio_control[i]>>8) & 0x80) ==0x80) {
                        //} else {
                        //    l_auds->id=1;
                        //}
                    }
                }
                l_track->m_defAudio=NULL;
                l_track->m_defSubtitle=NULL;
                // CHAPTERS
                cell = 0;
                unsigned long total_sectors = 0;

                l_track->chapterCount= pgc->nr_of_programs;
                for (i=0;i < l_track->chapterCount;i++)
                    l_track->addChapter(i+1);

                k9DVDChapter *l_pchap=NULL;
                for (i=0; i<pgc->nr_of_programs; i++) {
                    int second=0, minute=0, hour=0, tmp;
                    char hex[3];
                    int next = pgc->program_map[i+1];
                    unsigned long sectors = 0;
                    l_chap = l_track->getChapter(i);
                    //first sector
                    l_chap->startSector=pgc->cell_playback[cell].first_sector;
                    ;
                    l_chap->endSector=0;
                    if (i == pgc->nr_of_programs - 1)
                        next = pgc->nr_of_cells + 1;

                    int iangleStart=-1,iangleEnd=-1;
                    while (cell < next - 1) {
                        //using c2 otherwise the value of cell were lost
                        //int c2=cell;
                        //l_chap->cells.append(c2);
                        int startSect=pgc->cell_playback[cell].first_sector;
                        l_chap->startSectors.append(startSect);

                        snprintf(hex,3, "%02x", pgc->cell_playback[cell].playback_time.second);
                        tmp = second + atoi(hex);
                        minute = minute + (tmp / 60);
                        second = tmp % 60;
                        snprintf(hex,3, "%02x", pgc->cell_playback[cell].playback_time.minute);
                        tmp = minute + atoi(hex);
                        hour = hour + (tmp / 60);
                        minute = tmp % 60;

                        l_chap->endSector= pgc->cell_playback[cell].last_sector;
                        //last sector

                        /* Check if we're entering an angle block. (vamp_play-title) */
                        int cell2=cell;
                        if ( pgc->cell_playback[ cell ].block_type
                                == BLOCK_TYPE_ANGLE_BLOCK  && cell>iangleEnd ) {
                            iangleStart=cell;
                            for ( int idc = 0;; ++idc ) {
                                k9ChapterCell * chapterCell=new k9ChapterCell(cell2+idc,idc+1);
                                l_chap->cells.append(chapterCell);
                                chapterCell->setstartSector(pgc->cell_playback[cell2+idc].first_sector);
                                chapterCell->setlastSector(pgc->cell_playback[cell2+idc].last_sector);
                                if (idc==0)
                                    chapterCell->setangleBlock(angleStart);
                                else
                                    chapterCell->setangleBlock(angleInside);
                                if ( pgc->cell_playback[ cell2 + idc ].block_mode
                                        == BLOCK_MODE_LAST_CELL ) {
                                    iangleEnd=cell2+idc;
                                    chapterCell->setangleBlock(angleEnd);
                                    sectors += pgc->cell_playback[iangleEnd].last_sector - pgc->cell_playback[iangleStart].first_sector + 1;

                                    break;
                                }
                            }
                        } else {
                            if (!(cell>=iangleStart  && cell <=iangleEnd)) {
                                k9ChapterCell * chapterCell=new k9ChapterCell(cell,1);
                                l_chap->cells.append(chapterCell);
                                chapterCell->setstartSector(pgc->cell_playback[cell].first_sector);
                                chapterCell->setlastSector(pgc->cell_playback[cell].last_sector);

                                sectors += pgc->cell_playback[cell].last_sector - pgc->cell_playback[cell].first_sector + 1;

                            }
                        }
                        cell++;

                    }

                    l_track->vobusize_mb += calcVobuSize(ifo,l_chap);
                    total_sectors += sectors;
                    l_chap->setLength(QTime(hour,minute,second));
                    l_chap->sectors=sectors;
                    if (l_pchap!=NULL) {
                        QTime t(0,0,0);
                        int position;
                        position=t.secsTo(l_pchap->getLength());
                        t=l_pchap->getTime().addSecs(position);
                        l_chap->setTime(t);
                    }
                    l_pchap=l_chap;
                }
                l_track->size_mb =((float)total_sectors *2048)/(1024*1024);
                l_track->m_sectors= total_sectors;
                // SUBTITLES
                l_track->subPictureCount=0;
                for (i=0; i<vtsi_mat->nr_of_vts_subp_streams; i++) {
                    if (pgc->subp_control[i]>>24 !=0) {
                        l_sub=l_track->addSubtitle(i+1);
                        unsigned char subpc;
                        subpc=pgc->subp_control[i]>>24;
                        if ((subpc & 0x80)==0x80) {
                            if (l_track->aspectRatio=="4:3") {
                                l_sub->id.append(1+ (subpc & 0x1F));
                            } else {
                                subpc=(pgc->subp_control[i]>>16) & 0x1F;
                                l_sub->id.append(1+ subpc);
                                subpc=(pgc->subp_control[i]>>8) & 0x1F;
                                if (! l_sub->id.contains(1+ subpc) && ! l_track->DF.contains("noletterbox"))
                                    l_sub->id.append(1+ subpc);
                                subpc=pgc->subp_control[i] & 0x1F;
                                if (! l_sub->id.contains(1+ subpc) && ! l_track->DF.contains("nopanscan"))
                                    l_sub->id.append(1+ subpc);
                            }
                        }
                        l_track->subPictureCount++;
                        subp_attr = &vtsi_mat->vts_subp_attr[i];
                        langCode.sprintf("%c%c", subp_attr->lang_code>>8, subp_attr->lang_code & 0xff);
                        if (!(subp_attr->lang_code>>8)) {
                            langCode ="xx";
                        }
                        //JMP : l_sub->setselected(!titleIndexed);
                        l_sub->langCod=langCode;
                        l_sub->language=lang_name(m_arrLng,langCode,"");
                        l_sub->content= m_lsubpType[subp_attr->code_extension];
                    }
                }
                if (entryPgc && !_quickScan)
                    calcStreamSize(*l_track);
            }

        }
        //kifo.closeIFO();
    }
    m_menuSize=0;
    for (j=0;j<100;j++)
        m_menuSize+=menuSizes[j];
    //kifo_zero.closeIFO();

    if (!_quickScan)
        delete m_progressDlg;

    m_progressDlg=0;
    m_opened=true;
    //m_dvd.close();
    return 0;
}

float k9DVD::calcVobuSize(ifo_handle_t *_ifo,k9DVDChapter *_chapter) {
    vobu_admap_t * vobu_admap = _ifo->vts_vobu_admap;
    int length = vobu_admap->last_byte + 1 - VOBU_ADMAP_SIZE;
    float total=0;
    for (uint32_t i = 0; i < length/sizeof(uint32_t); i++) {
        if (vobu_admap->vobu_start_sectors[i] >= _chapter->startSector
                &&vobu_admap->vobu_start_sectors[i] <= _chapter->endSector)
            total++;
    }

    total=total*2048;
    total=total/(1024*1024);
    return total;

}

/** Adds a new track in the titles list the list is sorted by VTS and pgc*/
k9DVDTitle* k9DVD::addTitle(k9DVDTitleset *_titleset,int id,int num,uint _VTS,int _pgc,uint32_t _startSector,bool _indexed) {
    k9DVDTitle *track,*tmp;
    track = new k9DVDTitle(this);
    track->numTitle=num;
    track->indexed=_indexed;
    track->id=id;
    track->m_titleset=_titleset;
    //JMP : modification
    track->forceSelection = false; // !_indexed;
    track->ts_nr=_VTS;
    track->pgc=_pgc;
    bool bappend=true;
    for (int i=0;i<m_titles.count();i++) {
        tmp=(k9DVDTitle*)m_titles.at(i);
        k9DVDChapter *chap =tmp->getChapter(0);

        if (tmp->ts_nr >_VTS) {
            bappend=false;
        } else if ( (tmp->ts_nr==_VTS) && ((tmp->pgc >_pgc) || (chap->getstartSector() > _startSector)) ) {
            bappend=false;
        }
        if (!bappend) {
            m_titles.insert(i,track);
            break;
        }

    }
    if (bappend)
        m_titles.append(track);
    track->name=i18n("Title %1").arg(num);

    if (!_indexed) {
        for (int i=0;i<m_titles.count();i++) {
            tmp=(k9DVDTitle*)m_titles.at(i);
            if (tmp->numTitle==num && tmp->indexed)   {
                tmp->m_titles.append(track);
                track->m_mainTitle=tmp;
            }
        }
    }

    return(track);

}

void k9DVD::calcStreamSize(k9DVDTitle & track) {
    struct stream_counter streams[64];
    int stream_count;
    int start = 0;
    int stop = 0;
    k9DVDChapter *c_start,*c_stop;
    struct streamSize streamList[64];
    int c, x;
    QString pg;

    for ( x=0 ; x<64 ; x++ ) {                                                  // init stream usage list
        streams[x].id = 0;
        streams[x].count = 0;
        streams[x].size_mb = 0;
        streams[x].percent = 0;
        streamList[x].id=0;
        streamList[x].percent=0;
        streamList[x].size_mb=0;
    }

    x = 0;
    c = 0;

    if ( track.chapterCount == 2 ) {                                                  // select which chapters to probe for stream sizes
        start=0;
        stop=1;
    }
    if (track.chapterCount == 1) {
        start=0;
        stop=0;
    }
    if (track.chapterCount >2) {
        start=0;
        stop=(int)(track.chapterCount-1);
    }
    c_start=(k9DVDChapter*)track.chapters.at(start);
    c_stop=(k9DVDChapter*)track.chapters.at(stop);
    pg=i18n("reading title");
    emit sigTitleText(pg);

    //probe stream usage
    c = stream_vob( track.ts_nr,c_start->startSector, c_stop->endSector, streams);

    for ( x=0 ; x<64 ; x++ ) {
        if ( streams[x].id == 0 )
            break;
        streams[x].percent = (float)streams[x].count / (float)(c / 100.);     // calc usage in percent
    }
    // create streamlist with all streams
    stream_count=0;
    for ( c=0 ; c<64 ; c++ ) {
        if ( streams[c].id == 0 )
            break;
        streamList[stream_count].id = streams[c].id;
        if ( stream_count >= 64 )
            break;
        stream_count++;
        x++;
    }

    //   other_streams_mb = 0;                                                       // calc size of all selected streams
    for ( x=0 ; x<stream_count ; x++ ) {
        for ( c=0 ; c<64 ; c++ ) {
            if ( streamList[x].id == streams[c].id ) {
                streamList[x].percent=streams[c].percent;
                streamList[x].size_mb= (float) ((track.size_mb / 100.) * streams[c].percent);
            }
        }
    }

    k9DVDAudioStream *l_auds;
    k9DVDSubtitle *l_sub;
    k9DVDTitle *l_track;
    int i;
    l_track=&track;

    for (x=0;x<64;x++) {
        if (streamList[x].id==0)
            break;
        if (streamList[x].id== 0xe0)
            l_track->videosize_mb = streamList[x].size_mb;
    }

    for (i=0;i<l_track->audioStreamCount;i++) {
        l_auds=(k9DVDAudioStream*)l_track->audioStreams.at(i);
        for (x=0;x<64;x++) {
            if (streamList[x].id==0)
                break;
            int id=streamList[x].id;
            if (( (id >=0x80) && (id <=0x8f)) || ((id >=0xa0) && (id <=0xa7)) || ((id >=0xc0) && (id <=0xdf))) {
                if ( getVampsID(id) == l_auds->id) {
                    l_auds->size_mb = streamList[x].size_mb;
                    //stop the loop, go to next audio stream
                    break;
                }
            }

        }
    }
    for (i=0;i<l_track->subPictureCount;i++) {
        l_sub=(k9DVDSubtitle*)l_track->subtitles.at(i);
        for (x=0;x<64;x++) {
            if (streamList[x].id==0)
                break;
            int id=streamList[x].id;
            if ( (id >=0x20) && (id <=0x3f)) {
                if (  l_sub->id.contains(id - 0x20 +1) ) {
                    l_sub->size_mb = streamList[x].size_mb;
                    break;
                }
            }
        }
    }


}

int k9DVD::identify_stream( unsigned char *buffer ) {
    uint8_t packet_type = buffer[17];

    if ( (packet_type >= 0xE0) && (packet_type <= 0xEF) ) {                // video streams
        ;
    } else if ( packet_type == 0xBB ) {                                      // system header
        ;
    } else if ( packet_type == 0xBE ) {                                      // padding
        ;
    } else if ( packet_type == 0xBF ) {                                      // nav pack
        ;
    } else if ( (packet_type >= 0xC0) && (packet_type <= 0xDF) ) {           // mpeg audio
        ;
    } else if ( packet_type == 0xBD ) {                                      // private stream, check content
        packet_type = buffer[23+buffer[22]];
    }

    return packet_type;
}

long k9DVD::stream_vob( int title, unsigned long startblock, unsigned long lastblock, struct stream_counter *sc) {

    k9DVDFile *dvdfile;
    //JMP unsigned char buffer[DVD_VIDEO_LB_LEN];
    unsigned char buffer[DVD_VIDEO_LB_LEN*800];
    unsigned char *ptrbuff;
    int blocksize=800,y;
    int step=(int)((lastblock-startblock)/800) / 10;// * 0.05;
    if (step==0)
        step=1;
    unsigned char stream_id;
    long total=0;
    unsigned long i, x ,nbread=0;
    ssize_t size;
    QString c;
    dvdfile =m_dvd.openTitle( title);
    if ( !dvdfile ) {
        c=i18n("Error opening vobs for title %1\n").arg(title);
        setError(c);
        return 0;
    }

    i = startblock;
    while ( (i >= startblock) && (i <= lastblock) && (blocksize>0)) {                        // read blocks
        size= dvdfile->readBlocks( i,blocksize , buffer);
        emit sigVobProgress(i-startblock,lastblock-startblock);
        total+=size;
        if ( !size ) {
            c=i18n("ERROR reading block %1\n").arg(i);
            setError(c);
            break;
        }
        ptrbuff=buffer;
        for (y=0;y<blocksize;y++) {
            stream_id = identify_stream(ptrbuff);  // get stream id
            nbread++;
            for ( x=0 ; x<64 ; x++ ) {            // scan for already present counter
                if ( sc[x].id != 0 ) {
                    if ( stream_id != sc[x].id ) {
                        continue;
                    }
                }

                sc[x].id = stream_id;             // increase stream packet counter
                sc[x].count++;
                break;
            }
            ptrbuff+=DVD_VIDEO_LB_LEN;
        }
        i+=blocksize*step;//   6;
        if (i+blocksize> lastblock)
            blocksize=lastblock-i;

    }

    dvdfile->close();
    return total;
}



uint64_t k9DVD::getsizeSelected(bool ) {
    uint64_t selstreams=0,vidstreams=0;
    int i;
    k9DVDTitle *l_track;
    bool withvideo;

    for (i=0;i<m_titleCount;i++) {
        l_track=gettitle(i);

        withvideo=l_track->isSelected() && l_track->getIndexed();

        if ( withvideo) {
	  vidstreams +=l_track->getChaptersSize(true);
	   /*
            vidstreams +=l_track->getsectors();
            if (_streams) {
                for (x=0;x<l_track->audioStreamCount;x++) {
                    l_auds=l_track->getaudioStream(x);
                    if (!l_auds->selected)
                        selstreams += l_auds->size_mb*512;
                }
                for (x=0;x<l_track->subPictureCount;x++) {
                    l_sub=l_track->getsubtitle(x);
                    if (!l_sub->selected)
                        selstreams += l_sub->size_mb*512;
                }
            }*/
        }
    }
    vidstreams -=selstreams;

    return (vidstreams );
}


float k9DVD::getfactor(bool _withMenus,bool _streams,bool _useDvdAuthor) {
    if (!_useDvdAuthor || _withMenus) {
        //m_dvd.openDevice(Device);
        k9CellCopyList *cellCopyList =new k9CellCopyList(&m_dvd,this);
        double factor=cellCopyList->getfactor(_withMenus,_streams);
        //m_dvd.close();
        return (factor);
    } else {
        float selstreams=0,vidstreams=0,l_factor;
        int i,x;
        k9DVDTitle *l_track;
        k9DVDAudioStream *l_auds;
        k9DVDSubtitle *l_sub;
        bool withvideo;
        double forced=0,forcedsh=0;
        for (i=0;i<m_titleCount;i++) {
            l_track=gettitle(i);

            withvideo=l_track->isSelected() && l_track->getIndexed();

            if ( withvideo) {
                //size_mb = size of titles - size of unselected chapters
                float size_mb=l_track->gettotalsize_mb()-l_track->getChaptersSize_mb( false) ; //gettotalsize_mb does include parts of titles
                vidstreams +=size_mb ;
                if (l_track->getforceFactor()) {
                    forced+=size_mb;
                    forcedsh+=(size_mb/l_track->getfactor());
                }
                if (_streams) {
                    for (x=0;x<l_track->audioStreamCount;x++) {
                        l_auds=l_track->getaudioStream(x);
                        if (!l_auds->selected)
                            selstreams += l_auds->size_mb;
                    }
                    for (x=0;x<l_track->subPictureCount;x++) {
                        l_sub=l_track->getsubtitle(x);
                        if (!l_sub->selected)
                            selstreams += l_sub->size_mb;
                    }
                }
            }
        }
        vidstreams -=selstreams;

        l_factor = ((float) vidstreams - forced) / (k9DVDSize::getMaxSize() - forcedsh);
        l_factor = (int)((l_factor+0.01)*100);
        l_factor /=100;
        if (l_factor <1)
            l_factor=1;

        return(l_factor);
    }
}
void k9DVD::slotVobProgress(unsigned int position,unsigned int total) {
    m_progressDlg->setpbTitleStep(position);
    m_progressDlg->setpbTitleTotalSteps(total);
    k9Tools::processEvents();
}

void k9DVD::slotTitleProgress(unsigned int position,unsigned int total) {
    m_progressDlg->setpbTotalStep(position);
    m_progressDlg->setpbTotalTotalSteps(total);
    k9Tools::processEvents();
}

void k9DVD::slotTitleText(QString& text) {
    m_progressDlg->setlblTitle(text);
    k9Tools::processEvents();
}

void k9DVD::slotTotalText(QString& text) {
    m_progressDlg->setlblTotal(text);
    k9Tools::processEvents();
}

const QString& k9DVD::getDevice() {
    return m_Device;
}

const bool& k9DVD::geterror() {
    return m_error;
}

const QString& k9DVD::geterrMsg() {
    return m_errMsg;
}

void k9DVD::setError(const QString & err) {
    m_error=true;
    m_errMsg=err;
    if (m_progressDlg !=NULL)
        m_progressDlg->hide();
}

const bool& k9DVD::getopened() {
    return m_opened;
}


QString &k9DVD::getFormat() {
    return (m_format);
}

int k9DVD::getVampsID(int type) {
    int abase=0;
    if (type >= 0x80 && type <= 0x87) {
        // AC3 audio
        abase = 0x80;
    } else if (type >= 0x88 && type <= 0x8f) {
        // DTS audio
        abase = 0x88;
    } else if (type >= 0xa0 && type <= 0xbf) {
        // LPCM audio
        abase = 0xa0;
    } else if (type >= 0xc0 && type <= 0xdf) {
        // MPEG audio
        abase = 0xc0;
    }

    return (type-abase +1);

}


k9DVDTitle* k9DVD::getstart() {
    return m_start;
}

void k9DVD::setstart(k9DVDTitle* title) {
    m_start=title;
}

// returns the title number in the reauthored DVD
int k9DVD::getnewTitleNum(k9DVDTitle *title) {
    int num=0;
    k9DVDTitle *tr;
    for (int i=0 ;i < m_titleCount;i++) {
        tr=gettitle(i);
        if (tr->isSelected() && tr->getIndexed()) {
            ++num;
            if (tr->getnumTitle() == title->getnumTitle())
                return(num);
        }
    }

    return(-1);
}


void k9DVD::close() {
    m_opened=false;
    if (m_dvd.opened())
        m_dvd.close();
    deleteTitles();
    deleteTitlesets();
}
