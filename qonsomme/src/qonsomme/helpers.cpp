#include "helpers.h"

// copy-paste code from libkexiv2
QString detectEncodingAndDecode(const std::string &value) {
    if (value.empty()) {
        return QString();
    }

    if (isUtf8(value.c_str())) {
        return QString::fromUtf8(value.c_str());
    }

    // Utf8 has a pretty unique byte pattern.
    // Thats not true for ASCII, it is not possible
    // to reliably autodetect different ISO-8859 charsets.
    // So we can use either local encoding, or latin1.

    //TODO: KDE4PORT: check for regression of #134999 (very probably no regression!)
    return QString::fromLocal8Bit(value.c_str());
}

// helper from libkexiv2
bool isUtf8(const char * const buffer) {
    int i, n;
    unsigned char c;
    bool gotone = false;

    if (!buffer) {
        return true;
    }

    // character never appears in text
#define F 0
    // character appears in plain ASCII text
#define T 1
    // character appears in ISO-8859 text
#define I 2
    // character appears in non-ISO extended ASCII (Mac, IBM PC)
#define X 3

    static const unsigned char text_chars[256] =
    {
        //                  BEL BS HT LF    FF CR
        F, F, F, F, F, F, F, T, T, T, T, F, T, T, F, F,  // 0x0X
        //                              ESC
        F, F, F, F, F, F, F, F, F, F, F, T, F, F, F, F,  // 0x1X
        T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  // 0x2X
        T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  // 0x3X
        T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  // 0x4X
        T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  // 0x5X
        T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, T,  // 0x6X
        T, T, T, T, T, T, T, T, T, T, T, T, T, T, T, F,  // 0x7X
        //            NEL
        X, X, X, X, X, T, X, X, X, X, X, X, X, X, X, X,  // 0x8X
        X, X, X, X, X, X, X, X, X, X, X, X, X, X, X, X,  // 0x9X
        I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  // 0xaX
        I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  // 0xbX
        I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  // 0xcX
        I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  // 0xdX
        I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I,  // 0xeX
        I, I, I, I, I, I, I, I, I, I, I, I, I, I, I, I   // 0xfX
    };

    for (i = 0; (c = buffer[i]); ++i) {
        if ((c & 0x80) == 0) {
            // 0xxxxxxx is plain ASCII

            // Even if the whole file is valid UTF-8 sequences,
            // still reject it if it uses weird control characters.

            if (text_chars[c] != T) {
                return false;
            }

        }
        else if ((c & 0x40) == 0) {
            // 10xxxxxx never 1st byte
            return false;
        }
        else {
            // 11xxxxxx begins UTF-8
            int following = 0;

            if ((c & 0x20) == 0) {
                // 110xxxxx
                following = 1;
            }
            else if ((c & 0x10) == 0) {
                // 1110xxxx
                following = 2;
            }
            else if ((c & 0x08) == 0) {
                // 11110xxx
                following = 3;
            }
            else if ((c & 0x04) == 0) {
                // 111110xx
                following = 4;
            }
            else if ((c & 0x02) == 0) {
                // 1111110x
                following = 5;
            }
            else {
                return false;
            }

            for (n = 0; n < following; ++n) {
                i++;

                if (!(c = buffer[i])) { goto done; }

                if ((c & 0x80) == 0 || (c & 0x40)) {
                    return false;
                }
            }

            gotone = true;
        }
    }

done:
    return gotone;   // don't claim it's UTF-8 if it's all 7-bit.
}

#undef F
#undef T
#undef I
#undef X
