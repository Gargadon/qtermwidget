/*
 * This file is a part of QTerminal - http://gitorious.org/qterminal
 *
 * This file was un-linked from KDE and modified
 * by Maxim Bourmistrov <maxim@unixconn.com>
 *
 * This file is part of Konsole, KDE's terminal emulator.
 * (GPL-2.0-or-later; see the original copyright header.)
 *
 * Forked for BanchoXterm: on Windows the Pty is a pure signal router used
 * with ConPTY (external I/O mode), instead of a KPtyProcess.
 */

#ifndef PTY_H
#define PTY_H

// Qt
#include <QStringList>
#include <QVector>
#include <QList>
#include <QSize>
#include <QProcess>
#include <QObject>

#ifndef Q_OS_WIN
// KDE
#include "kptyprocess.h"
#endif

namespace Konsole {

#ifdef Q_OS_WIN

/**
 * Windows: no KPty. The Pty is a stub that routes I/O between the emulator
 * and an external backend (ConPTY) driven by QTermWidget::feedData() and the
 * QTermWidget::sendData signal.
 */
class Pty : public QObject {
    Q_OBJECT
public:
    explicit Pty(QObject* parent = nullptr);
    explicit Pty(int ptyMasterFd, QObject* parent = nullptr);
    ~Pty() override;

    int start(const QString&, const QStringList&, const QStringList&, ulong, bool) {
        return 0;
    }
    void setEmptyPTYProperties() {}
    void setWriteable(bool) {}
    void setFlowControlEnabled(bool) {}
    bool flowControlEnabled() const {
        return false;
    }
    void setWindowSize(int lines, int cols);
    QSize windowSize() const;
    void setErase(char erase);
    char erase() const;
    void setInitialWorkingDirectory(const QString&) {}
    int foregroundProcessGroup() const {
        return 0;
    }
    void closePty() {}
    QProcess::ProcessState state() const {
        return QProcess::NotRunning;
    }
    qint64 processId() const {
        return 0;
    }
    bool waitForFinished(int) {
        return true;
    }
    QProcess::ExitStatus exitStatus() const {
        return QProcess::NormalExit;
    }

public slots:
    void setUtf8Mode(bool) {}
    void lockPty(bool) {}
    void sendData(const char*, int) {}

signals:
    void receivedData(const char* buffer, int length);
    void finished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    int _windowColumns;
    int _windowLines;
    char _eraseChar;
};

#else // Q_OS_WIN

/**
 * The Pty class is used to start the terminal process,
 * send data to it, receive data from it and manipulate
 * various properties of the pseudo-teletype interface
 * used to communicate with the process.
 */
class Pty : public KPtyProcess {
    Q_OBJECT

public:
    explicit Pty(QObject* parent = nullptr);
    explicit Pty(int ptyMasterFd, QObject* parent = nullptr);
    ~Pty() override;

    int start(const QString& program, const QStringList& arguments, const QStringList& environment, ulong winid,
              bool addToUtmp);
    void setEmptyPTYProperties();
    void setWriteable(bool writeable);
    void setFlowControlEnabled(bool on);
    bool flowControlEnabled() const;
    void setWindowSize(int lines, int cols);
    QSize windowSize() const;
    void setErase(char erase);
    char erase() const;
    void setInitialWorkingDirectory(const QString& dir);
    int foregroundProcessGroup() const;
    void closePty();

public slots:
    void setUtf8Mode(bool on);
    void lockPty(bool lock);
    void sendData(const char* buffer, int length);

signals:
    void receivedData(const char* buffer, int length);

private slots:
    void dataReceived();

private:
    void init();
    void addEnvironmentVariables(const QStringList& environment);

    int _windowColumns;
    int _windowLines;
    char _eraseChar;
    bool _xonXoff;
    bool _utf8;
};

#endif // Q_OS_WIN

} // namespace Konsole

#endif // PTY_H
