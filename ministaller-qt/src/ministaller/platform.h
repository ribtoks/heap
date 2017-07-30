/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

#ifndef PLATFORM_H
#define PLATFORM_H

#include <QtGlobal>

// platform-specific type aliases
#ifdef Q_OS_UNIX
#include <unistd.h>
#define PLATFORM_PID pid_t
#else
#define PLATFORM_PID DWORD
#endif

#endif // PLATFORM_H
