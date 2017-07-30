/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

#ifndef DIFFGENERATOR_H
#define DIFFGENERATOR_H

#include <QVector>
#include <QDir>
#include <QString>
#include <QStringList>
#include <QJsonDocument>
#include <QJsonArray>

struct FileEntry {
    QString m_Filepath;
    QString m_Sha1;
};

class DiffGenerator
{
public:
    DiffGenerator(const QString &baseDir, const QString &newDir);

public:
    void generateDiffs();
    QJsonDocument generateJson();

private:
    void generateDirsDiff(const QString &baseDirPath, const QString &newDirPath);
    void listToJsonArray(const QVector<FileEntry> &list, QJsonArray &array);

private:
    QVector<FileEntry> m_ItemsToAdd;
    QVector<FileEntry> m_ItemsToUpdate;
    QVector<FileEntry> m_ItemsToRemove;
    QDir m_BaseDir;
    QDir m_NewDir;
    const QString &m_BaseDirPath;
    const QString &m_NewDirPath;
};

#endif // DIFFGENERATOR_H
