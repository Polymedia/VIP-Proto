#ifndef RCONSOLE_H
#define RCONSOLE_H

#include "rconfig.h"
#include "robject.h"

#include <QObject>

class RINSIDE_EXPORT RConsole : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool verbose READ verbose WRITE setVerbose)

public:
    RConsole(bool verbose, QObject *parent = 0);
    ~RConsole();

    bool verbose() const { return m_verbose; }
    void setVerbose(bool v) { m_verbose = v; }

    RObject get(const QString &name) const;
    void set(const QString &name, const RObject& var);

public slots:
    bool execute(const QString& code, RObject& value = RObject());

signals:
    void write(QString);
    void callback();
    void busy(bool);
    void error(QString);

private:
    friend int ReadConsole(const char *, char *, int, int);
    friend void WriteConsole(const char *, int);
    friend void CallBack(void);
    friend void Busy(int);

private:
    bool m_verbose;

};

#endif // RCONSOLE_H
