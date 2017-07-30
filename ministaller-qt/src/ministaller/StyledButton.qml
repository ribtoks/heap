/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

import QtQuick 2.2
import QtQuick.Controls 1.1
import QtQuick.Controls.Styles 1.1
import "Colors.js" as Colors

Button {
    property bool isDefault: false
    clip: true

    style: ButtonStyle {
        background: Rectangle {
            implicitWidth: control.width
            implicitHeight: 24
            color: {
                var result = Colors.defaultControlColor;

                if (control.enabled &&
                        (control.isDefault || control.hovered)) {
                    result = Colors.buttonHoverBackground;
                }

                return result
            }
        }

        label: StyledText {
            font.bold: true
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            text: control.text
            color: {
                var result = Colors.defaultControlColor;

                if (control.enabled) {
                    result = control.pressed ? Colors.buttonPressedForeground : Colors.inputForegroundColor;
                } else {
                    result = Colors.buttonDisabledForeground;
                }

                return result
            }
        }
    }
}
