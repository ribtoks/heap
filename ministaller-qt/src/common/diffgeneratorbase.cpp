/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

#include "diffgeneratorbase.h"
#include <QDirIterator>
#include <QCryptographicHash>
#include <QFile>
#include <QHash>

#ifdef OWN_LOGGING
#include "logger.h"
#else
#include <QDebug>
#define LOG qDebug()
#endif

#define BLOCK_SIZE 8192

QHash<QString, QString> Sha1Cache;

QString getFileSha1(const QString &path) {
    if (Sha1Cache.contains(path)) {
        return Sha1Cache[path];
    }

    QCryptographicHash crypto(QCryptographicHash::Sha1);
    QFile file(path);
    file.open(QFile::ReadOnly);

    while (!file.atEnd()) {
        crypto.addData(file.read(BLOCK_SIZE));
    }

    QByteArray hash = crypto.result();
    QString hex = QString::fromLatin1(hash.toHex());
    Sha1Cache.insert(path, hex);
    return hex;
}

bool areFilesEqual(const QString &path1, const QString &path2) {
    return getFileSha1(path1) == getFileSha1(path2);
}

void appendToList(const QString &relativePath, const QString &fullPath, QVector<FileEntry> &list) {
    QString sha1 = getFileSha1(fullPath);    

#ifdef QT_DEBUG
    foreach (const FileEntry &entry, list) {
        Q_ASSERT(entry.m_Filepath != relativePath);
    }
#endif

    list.append({relativePath, sha1});
}

void addToListRecursively(const QDir &relativeDir, const QString &path, QVector<FileEntry> &list) {
    QDirIterator it(path, QDir::NoDotAndDotDot | QDir::Files, QDirIterator::Subdirectories);

    while (it.hasNext()) {
        QString fullPath = it.next();
        QString path = relativeDir.relativeFilePath(fullPath);
        appendToList(path, fullPath, list);
    }
}

DiffGeneratorBase::DiffGeneratorBase(const QString &baseDirPath, const QString &newDirPath, bool forceUpdate, bool keepMissing):
    m_BaseDir(baseDirPath),
    m_NewDir(newDirPath),
    m_ForceUpdate(forceUpdate),
    m_KeepMissing(keepMissing)
{
    m_BaseDirPath = m_BaseDir.absolutePath();
    m_NewDirPath = m_NewDir.absolutePath();
}

void DiffGeneratorBase::generateDiffs() {
    generateDirsDiff(m_BaseDirPath, m_NewDirPath);
}

void DiffGeneratorBase::generateDirsDiff(const QString &baseDirPath, const QString &newDirPath) {
    LOG << "BaseDir:" << baseDirPath << "NewDir:" << newDirPath;

    findFilesToRemoveOrUpdate(baseDirPath, newDirPath);
    findFilesToAdd(baseDirPath, newDirPath);
}

void DiffGeneratorBase::findFilesToRemoveOrUpdate(const QString &baseDirPath, const QString &newDirPath) {
    LOG << "Examining" << baseDirPath;
    QDirIterator baseIt(baseDirPath, QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files, QDirIterator::NoIteratorFlags);
    QDir newDir(newDirPath);

    const bool keepMissing = m_KeepMissing;

    while (baseIt.hasNext()) {
        QString baseFilepath = baseIt.next();
        LOG << "Checking path:" << baseFilepath;
        QString baseFilename = baseIt.fileName();
        QString newFilepath = newDir.absoluteFilePath(baseFilename);

        QFileInfo fi(newFilepath);
        if (!keepMissing && !fi.exists()) {
            QFileInfo baseFi(baseFilepath);
            if (baseFi.isFile()) {
                LOG << "Removing file:" << baseFilepath;
                QString path = m_BaseDir.relativeFilePath(baseFilepath);
                appendToList(path, baseFilepath, m_ItemsToRemove);
            } else if (baseFi.isDir()) {
                LOG << "Removing dir:" << baseFilepath;
                addToListRecursively(m_BaseDir, baseFilepath, m_ItemsToRemove);
            }
        } else {
            if (fi.isFile()) {
                QString path = m_BaseDir.relativeFilePath(baseFilepath);
                if (m_ForceUpdate || !areFilesEqual(baseFilepath, newFilepath)) {
                    LOG << "Updating the file:" << path;
                    appendToList(path, baseFilepath, m_ItemsToUpdate);
                } else {
                    LOG << "Skipping same file:" << path;
                }
            } else if (fi.isDir()) {
                Q_ASSERT(QFileInfo(baseFilepath).isDir());
                generateDirsDiff(baseFilepath, newFilepath);
            }
        }
    }
}

void DiffGeneratorBase::findFilesToAdd(const QString &baseDirPath, const QString &newDirPath) {
    LOG << "Examining" << newDirPath;
    QDirIterator newIt(newDirPath, QDir::NoDotAndDotDot | QDir::Dirs | QDir::Files, QDirIterator::NoIteratorFlags);
    QDir baseDir(baseDirPath);

    while (newIt.hasNext()) {
        QString newFilepath = newIt.next();
        QString newFilename = newIt.fileName();
        QFileInfo newFI = newIt.fileInfo();
        QString baseFilepath = baseDir.absoluteFilePath(newFilename);

        QFileInfo fi(baseFilepath);
        if (!fi.exists()) {
            if (newFI.isFile()) {
                LOG << "Adding file:" << newFilepath;
                QString relativePath = m_NewDir.relativeFilePath(newFilepath);
                appendToList(relativePath, newFilepath, m_ItemsToAdd);
            } else if (newFI.isDir()) {
                LOG << "Adding dir:" << newFilepath;
                addToListRecursively(m_NewDir, newFilepath, m_ItemsToAdd);
            }
        }
    }
}
