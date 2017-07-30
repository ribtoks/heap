/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

#ifndef FSHELPERS_H
#define FSHELPERS_H

#include <QString>

bool extractPackage(const QString &packagePath, const QString &resultPath);
bool needToElevatePermissions(const QString &destinationPath);

#endif // FSHELPERS_H
