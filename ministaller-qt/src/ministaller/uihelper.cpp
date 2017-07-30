/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

#include "uihelper.h"
#include <QCoreApplication>
#include <QThread>

UIHelper::UIHelper(QObject *parent):
    QObject(parent),
    m_Cancel(false)
{
}

void UIHelper::systemMessage(const QString &message) {
    emit systemMessageArrived(message);
    QCoreApplication::processEvents();
    QThread::sleep(1);
}

void UIHelper::cancelInstall() {
    m_Cancel = true;
}
