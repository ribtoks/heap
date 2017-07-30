/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

#ifndef LIVELOG_H
#define LIVELOG_H

#include <QObject>
#include <QAbstractListModel>
#include <QCoreApplication>

class LiveLog : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString contents READ getLiveLogContents NOTIFY logContentsChanged)
private:
    explicit LiveLog(QObject *parent = 0):
        QObject(parent)
    {
    }

    LiveLog(LiveLog const&);
    void operator=(LiveLog const&);

public:
    static LiveLog& getInstance()
    {
        static LiveLog instance; // Guaranteed to be destroyed.
        // Instantiated on first use.
        return instance;
    }

    void log(const QString &message);
    void saveLog();

    const QString &getLiveLogContents() const { return m_JoinedLog; }

signals:
    void logContentsChanged();

private:
    QStringList m_LogLines;
    QString m_JoinedLog;
};

#endif // LIVELOG_H
