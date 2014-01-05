#ifndef K9LOG_H
#define K9LOG_H
#include <QString>
#include <QQueue>

namespace k9logLevel {
    enum type {WARNING,INFO,ERROR};
}

class k9logEntry {
public:
    QString text;
    k9logLevel::type level;
};


class k9log {
private:
    QQueue<k9logEntry*> m_list;
public:
    k9log();
    ~k9log();
    QQueue<k9logEntry*> &list();
    static void add(QString s,k9logLevel::type level);
    static k9logEntry* dequeue ();
    static int count();
    static void write(QString s);
    static void writer(QString s);
    static void writeln(QString s);
};

#endif // K9LOG_H
