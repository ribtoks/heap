/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

import QtQuick 2.2
import "Colors.js" as Colors

Text {
    property bool isActive: true
    color: (isActive && enabled) ? Colors.labelActiveForeground : Colors.labelInactiveForeground
    renderType: Text.NativeRendering
    font.family: Qt.platform.os === "windows" ? "Arial" : "Helvetica"
    font.pixelSize: 14
    focus: false
}
