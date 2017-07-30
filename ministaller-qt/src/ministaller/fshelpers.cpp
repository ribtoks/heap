/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

#include "fshelpers.h"
#include <QTemporaryDir>
#include <QStringList>
#include <QFile>
#include <quazip/JlCompress.h>

#define TEST_FILEPATH "install-test-file"

bool extractPackage(const QString &packagePath, const QString &resultPath) {
    bool success = false;

    auto fileList = JlCompress::extractDir(packagePath, resultPath);
    success = !fileList.empty();

    return success;
}

bool needToElevatePermissions(const QString &destinationPath) {
    QDir directory(destinationPath);
    Q_ASSERT(directory.exists());

    QString filePath = directory.absoluteFilePath(TEST_FILEPATH);
    QFile testFile(filePath);

    bool result = false;

    if (testFile.open(QIODevice::ReadWrite | QIODevice::Truncate)) {
        result = true;
        testFile.close();
        testFile.remove();
    }

    return result;
}
