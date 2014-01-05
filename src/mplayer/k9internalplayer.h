//
// C++ Interface: k9internalplayer
//
// Description: 
//
//
// Author:  <>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef K9INTERNALPLAYER
#define K9INTERNALPLAYER

class QImage;
class k9InternalPlayer {
public:
  virtual void drawPpm(uchar *_buf,int _width,int _height,int _len){
	Q_UNUSED(_buf);
	Q_UNUSED(_width);
	Q_UNUSED(_height);
	Q_UNUSED(_len);
  };
  virtual void drawImage( QImage *image){
 	Q_UNUSED(image);
  };
};


#endif
