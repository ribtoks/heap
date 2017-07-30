/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

#ifndef FILEENTRY_H
#define FILEENTRY_H

#include <QString>

struct FileEntry {
    QString m_Filepath;
    QString m_Sha1;
};

#endif // FILEENTRY_H
