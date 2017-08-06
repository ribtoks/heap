#include "exiv2inithelper.h"

#include "exiv2inithelper.h"
#include <QtGlobal>
#ifdef Q_OS_WIN
#include <Windows.h>
#endif
#include <exiv2/xmp_exiv2.hpp>

Exiv2InitHelper::Exiv2InitHelper() {
    Exiv2::XmpParser::initialize();
}

Exiv2InitHelper::~Exiv2InitHelper() {
    Exiv2::XmpParser::terminate();
}
