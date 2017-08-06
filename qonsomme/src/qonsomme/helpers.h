#ifndef HELPERS_H
#define HELPERS_H

#include <QString>
#include <string>

QString detectEncodingAndDecode(const std::string &value);
bool isUtf8(const char* const buffer);

#endif // HELPERS_H
