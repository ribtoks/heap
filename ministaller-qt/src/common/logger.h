/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <vector>
#include <mutex>
#include <QString>

#define LOG Logger()

struct Logger {
    Logger() {}

    virtual ~Logger();

    Logger &operator<<(const std::string &s) {
        m_LogLines.push_back(s);
        return *this;
    }

    Logger &operator<<(const QString &s) {
        m_LogLines.push_back(s.toStdString());
        return *this;
    }

    Logger &operator<<(const char *c) {
        m_LogLines.emplace_back(c);
        return *this;
    }

    static std::mutex m_Mutex;
    static bool m_Verbose;

private:
    std::vector<std::string> m_LogLines;
};

#endif // LOGGER_H
