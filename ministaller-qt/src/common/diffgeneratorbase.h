/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

#ifndef DIFFGENERATORBASE_H
#define DIFFGENERATORBASE_H

#include <QString>
#include <QVector>
#include <QDir>
#include <QString>
#include <QStringList>
#include <QDebug>
#include "fileentry.h"
#include "ifilesprovider.h"

class DiffGeneratorBase: public IFilesProvider {
public:
    DiffGeneratorBase(const QString &baseDirPath, const QString &newDirPath, bool forceUpdate, bool keepMissing);
    virtual ~DiffGeneratorBase() {}

public:
    void generateDiffs();

public:
    const QVector<FileEntry> &getItemsToAdd() const override { return m_ItemsToAdd; }
    const QVector<FileEntry> &getItemsToUpdate() const override { return m_ItemsToUpdate; }
    const QVector<FileEntry> &getItemsToRemove() const override { return m_ItemsToRemove; }

private:
    void generateDirsDiff(const QString &baseDirPath, const QString &newDirPath);
    void findFilesToRemoveOrUpdate(const QString &baseDirPath, const QString &newDirPath);
    void findFilesToAdd(const QString &baseDirPath, const QString &newDirPath);

private:
    QVector<FileEntry> m_ItemsToAdd;
    QVector<FileEntry> m_ItemsToUpdate;
    QVector<FileEntry> m_ItemsToRemove;
    QDir m_BaseDir;
    QDir m_NewDir;
    QString m_BaseDirPath;
    QString m_NewDirPath;
    bool m_ForceUpdate;
    bool m_KeepMissing;
};

#endif // DIFFGENERATORBASE_H
