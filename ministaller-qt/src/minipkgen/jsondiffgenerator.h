/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

#ifndef DIFFGENERATOR_H
#define DIFFGENERATOR_H

#include <QJsonDocument>
#include <QJsonArray>
#include "options.h"
#include "../common/fileentry.h"
#include "../common/diffgeneratorbase.h"

class JsonDiffGenerator: public DiffGeneratorBase
{
public:
    JsonDiffGenerator(const ParsedOptions &options);

public:
    QJsonDocument generateJson();

private:
    void listToJsonArray(const QVector<FileEntry> &list, QJsonArray &array);

private:
    const ParsedOptions &m_Options;
};

#endif // DIFFGENERATOR_H
