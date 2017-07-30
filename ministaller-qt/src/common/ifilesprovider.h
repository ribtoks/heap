/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

#ifndef IFILESPROVIDER_H
#define IFILESPROVIDER_H

#include "fileentry.h"
#include <QVector>

class IFilesProvider {
public:
    virtual ~IFilesProvider() {}

    virtual const QVector<FileEntry> &getItemsToAdd() const = 0;
    virtual const QVector<FileEntry> &getItemsToUpdate() const = 0;
    virtual const QVector<FileEntry> &getItemsToRemove() const = 0;
};

#endif // IFILESPROVIDER_H
