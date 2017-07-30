/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

#ifndef OPTIONS_H
#define OPTIONS_H

#include <QString>

struct ParsedOptions {
    QString m_BaseDir;
    QString m_NewDir;
    QString m_JsonPath;
    bool m_ForceUpdate;
    bool m_KeepMissing;
};

#endif // OPTIONS_H
