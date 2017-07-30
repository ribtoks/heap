/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

#ifndef UIHELPER_H
#define UIHELPER_H

#include <QObject>

class UIHelper : public QObject
{
    Q_OBJECT
public:
    explicit UIHelper(QObject *parent = 0);

public:
    void systemMessage(const QString &message);
    bool getShouldCancel() const { return m_Cancel; }
    Q_INVOKABLE void cancelInstall();

signals:
    void systemMessageArrived(QString message);
    void beforeQuit();

private:
    volatile bool m_Cancel;
};

#endif // UIHELPER_H
