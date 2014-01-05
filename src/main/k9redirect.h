//
// C++ Interface: k9redirect
//
// Description: 
//
//
// Author: Jean-Michel PETIT <k9copy@free.fr>, (C) 2007
//
// Copyright: See COPYING file that comes with this distribution
//
//
#ifndef K9REDIRECT_H
#define K9REDIRECT_H

#include "k9common.h"
#include <qfile.h>
/**
	@author Jean-Michel PETIT <k9copy@free.fr>
*/
class k9Redirect{
public:
    k9Redirect();

    ~k9Redirect();
    void execute();
};

#endif
