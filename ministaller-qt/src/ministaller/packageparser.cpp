/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

#include "packageparser.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QFile>
#include <QDebug>
#include "../common/defines.h"

PackageParser::PackageParser(const QString &pathToConfig):
    m_ConfigPath(pathToConfig)
{
}

bool PackageParser::parsePackage() {
    QFile file(m_ConfigPath);

    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Can't open file" << m_ConfigPath;
        return false;
    }

    QJsonDocument document = QJsonDocument::fromJson(file.readAll());
    QJsonObject rootObject = document.object();

    bool anyFault = false;

    do {
        if (rootObject.contains(ADD_ITEMS_KEY)) {
            QJsonValue addList = rootObject[ADD_ITEMS_KEY];
            if (addList.isArray()) {
                parseJsonArray(addList.toArray(), m_ItemsToAdd);
                qInfo() << m_ItemsToAdd.size() << "items to add";
            } else {
                qWarning() << "'add' element is not an array";
                anyFault = true;
                break;
            }
        }

        if (rootObject.contains(UPDATE_ITEMS_KEY)) {
            QJsonValue updateList = rootObject[UPDATE_ITEMS_KEY];
            if (updateList.isArray()) {
                parseJsonArray(updateList.toArray(), m_ItemsToUpdate);
                qInfo() << m_ItemsToUpdate.size() << "items to update";
            } else {
                qWarning() << "'update' element is not an array";
                anyFault = true;
                break;
            }
        }

        if (rootObject.contains(REMOVE_ITEMS_KEY)) {
            QJsonValue removeList = rootObject[REMOVE_ITEMS_KEY];
            if (removeList.isArray()) {
                parseJsonArray(removeList.toArray(), m_ItemsToRemove);
                qInfo() << m_ItemsToRemove.size() << "items to remove";
            } else {
                qWarning() << "'remove' element is not an array";
                anyFault = true;
                break;
            }
        }
    } while (false);

    return !anyFault;
}

void PackageParser::parseJsonArray(const QJsonArray &array, QVector<FileEntry> &entryList) {
    qDebug() << "#";
    int size = array.size();
    entryList.reserve(size);

    for (int i = 0; i < size; ++i) {
        const auto &item = array.at(i);

        if (!item.isObject()) {
            qWarning() << "Element at" << i << "is not an object";
            continue;
        }

        auto object = item.toObject();
        if (!object.contains(PATH_KEY) ||
                !object.contains(SHA1_KEY)) {
            qWarning() << "Element at" << i << "does not have all keys";
            continue;
        }

        QString relativePath = object.value(PATH_KEY).toString();
        QString hashSum = object.value(SHA1_KEY).toString();
        entryList.append({relativePath, hashSum});
    }
}
