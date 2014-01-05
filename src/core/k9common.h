//
// C++ Interface: k9common
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "config.h"
#ifndef __STDC_LIMIT_MACROS
#endif
#ifndef __STDC_CONSTANT_MACROS
#define __STDC_CONSTANT_MACROS
#endif

#if defined(HAVE_INTTYPES_H)
    #include <inttypes.h>
#elif defined( HAVE_STDINT_H)
     #include <stdint.h>
#endif


#ifdef __FreeBSD__
    #include <osreldate.h>
#endif
#include "ifo_types.h"
#include "dvd_reader.h"
#include "ifo_read.h"
#include "nav_read.h"
#include "nav_types.h"

#include <QObject>

#ifndef DVD_BLOCK_LEN
#define DVD_BLOCK_LEN 2048
#endif

#include "k9dvdsize.h"

#include <QtDebug>
#include "k9dialogs.h"

#include "k9log.h"
#include "k9tools.h"
#ifndef BACKLITE
#include <KLocale>
#include "k9config.h"
#include "k9process.h"

#else
#include <QCoreApplication>
#include "k9process2.h"
#include "k9config2.h"
#include "i18n.h"

#endif


