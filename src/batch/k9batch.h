#ifndef k9batch_H
#define k9batch_H

#include "k9common.h"

class k9DVD;
class k9DVDTitle;

class k9batch {
private:
  QMap <QString,QString> args;
  void showHelp();
  void doSelect(k9DVD &dvd);  
  void doSelectLang(k9DVDTitle *title);
public:
  bool exec(int argc, char **argv);
  
};

#endif