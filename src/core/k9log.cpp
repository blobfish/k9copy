#include "k9log.h"
#include <QDebug>
#include <QMutex>

k9log m_instance;


QMutex m_mutex;
k9log::k9log()
{
}

k9log::~k9log() {
   qDeleteAll(m_list);
}

void k9log::write(QString s) {
    QTextStream out (stdout);
    out << s;
}
void k9log::writer(QString s) {
    QTextStream out (stdout);
    out << s << "\r";
}

void k9log::writeln(QString s) {
    QTextStream out (stdout);
    out << s <<"\n";
}

void k9log::add(QString s,k9logLevel::type level) {
   k9logEntry *entry=new k9logEntry();
   entry->level=level;
   entry->text=s;
   m_mutex.lock();
   m_instance.list() << entry;
   m_mutex.unlock();
}

k9logEntry* k9log::dequeue () {
    m_mutex.lock();
    k9logEntry *entry=m_instance.list().dequeue();
    m_mutex.unlock();
    return entry;

}

int k9log::count() {
    int c;
    m_mutex.lock();
    c=m_instance.list().count();
    m_mutex.unlock();
    return c;
}

QQueue<k9logEntry*> &k9log::list() {return m_list;}
