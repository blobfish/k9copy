/* 
 * Copyright (C) 2000-2001 the xine project
 * 
 * This file is part of xine, a unix video player.
 * 
 * xine is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * xine is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA
 *
 * $Id: compat.h,v 1.3 2003/04/20 21:13:26 guenter Exp $
 *
 */

#ifndef XINE_COMPAT_H
#define XINE_COMPAT_H

#include <limits.h>

#ifdef __cplusplus
extern "C" {
#endif

#if   defined _MSC_VER
#define __XINE_FUNCTION__    __FILE__
#elif defined __GNUC__
#define __XINE_FUNCTION__    __FUNCTION__
#else
#define __XINE_FUNCTION__    __func__
#endif

#ifndef NAME_MAX
#define XINE_NAME_MAX   256
#else
#define XINE_NAME_MAX   NAME_MAX
#endif

#ifndef PATH_MAX
#define XINE_PATH_MAX   768
#else
#define XINE_PATH_MAX   PATH_MAX
#endif


#ifdef __cplusplus
}
#endif

#endif
