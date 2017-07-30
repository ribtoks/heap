/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

#include "logger.h"
#include <iostream>
#include <ctime>

std::mutex Logger::m_Mutex;
bool Logger::m_Verbose = false;

Logger::~Logger() {
    if (!m_Verbose) { return; }

    std::lock_guard<std::mutex> guard(m_Mutex);

    std::time_t now = std::time(NULL);
    std::string timeStr(std::ctime(&now));
    timeStr.erase(std::remove(timeStr.begin(), timeStr.end(), '\n'), timeStr.end());
    std::cout << timeStr << " - ";

    for (auto &s: m_LogLines) {
        std::cout << s << ' ';
    }

    std::cout << std::endl;
}
