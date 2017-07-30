/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

#include "jsondiffgenerator.h"
#include <QJsonObject>
#include "../common/logger.h"
#include "../common/defines.h"

JsonDiffGenerator::JsonDiffGenerator(const ParsedOptions &options):
    DiffGeneratorBase(options.m_BaseDir, options.m_NewDir, options.m_ForceUpdate, options.m_KeepMissing),
    m_Options(options)
{
    Q_UNUSED(m_Options);
}

QJsonDocument JsonDiffGenerator::generateJson() {
    QJsonObject rootObject;

    QJsonArray itemsToRemove;
    listToJsonArray(getItemsToRemove(), itemsToRemove);
    rootObject.insert(REMOVE_ITEMS_KEY, itemsToRemove);

    QJsonArray itemsToUpdate;
    listToJsonArray(getItemsToUpdate(), itemsToUpdate);
    rootObject.insert(UPDATE_ITEMS_KEY, itemsToUpdate);

    QJsonArray itemsToAdd;
    listToJsonArray(getItemsToAdd(), itemsToAdd);
    rootObject.insert(ADD_ITEMS_KEY, itemsToAdd);

    QJsonDocument document(rootObject);
    return document;
}

void JsonDiffGenerator::listToJsonArray(const QVector<FileEntry> &list, QJsonArray &array) {
    for (auto &entry: list) {
        QJsonObject fileEntryObj;
        fileEntryObj.insert(PATH_KEY, QJsonValue(entry.m_Filepath));
        fileEntryObj.insert(SHA1_KEY, QJsonValue(entry.m_Sha1));

        array.append(fileEntryObj);
    }
}
