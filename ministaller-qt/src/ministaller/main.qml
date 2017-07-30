/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

import QtQuick 2.5
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.4
import "Colors.js" as Colors

Window {
    id: applicationWindow
    visible: true
    height: 350
    width: 500

    function openDetails() {
        var options = {}
        var directParent = applicationWindow

        var component = Qt.createComponent("DetailsWindow.qml");
        if (component.status !== Component.Ready) {
            console.debug("Component Error: " + component.errorString());
        } else {
            var instance = component.createObject(directParent, options);
        }
    }

    Timer {
        id: closingTimer
        interval: 500
        running: false
        repeat: false
        onTriggered: Qt.quit()
    }

    Connections {
        target: uiHelper
        onBeforeQuit: {
            applicationWindow.visibility = "Minimized"
            closingTimer.start()
        }
    }

    Rectangle {
        anchors.fill: parent
        color: Colors.defaultDarkColor

        StyledBusyIndicator {
            id: spinner
            anchors.top: parent.top
            anchors.topMargin: 50
            width: 150
            height: 150
            anchors.horizontalCenter: parent.horizontalCenter
            running: true
        }

        StyledText {
            id: summaryText
            anchors.top: spinner.bottom
            anchors.topMargin: 20
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 20
            text: qsTr("Installing updates...")
            color: Colors.inputForegroundColor
        }

        Item {
            id: currentOperationItem
            anchors.top: summaryText.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            width: childrenRect.width
            height: childrenRect.height
            clip: true

            StyledText {
                id: currentOperationText
                property string nextMessage: ''
                text: qsTr("Preparing installation...")
                color: Colors.labelInactiveForeground
                font.pixelSize: 12
                x: 0

                SequentialAnimation {
                    id: opacityAnimation
                    loops: Animation.Infinite

                    NumberAnimation {
                        target: currentOperationText
                        properties: "opacity"
                        from: 1
                        to: 0.5
                        duration: 1000
                        easing {type: Easing.InCurve;}
                    }

                    NumberAnimation {
                        target: currentOperationText
                        properties: "opacity"
                        from: 0.5
                        to: 1
                        duration: 1000
                        easing {type: Easing.InCurve;}
                    }
                }

                SequentialAnimation {
                    id: slidingAnimation

                    NumberAnimation {
                        target: currentOperationText
                        properties: "x"
                        from: 0
                        to: -200
                        duration: 500
                        easing { type: Easing.InQuad; }
                    }

                    PropertyAnimation {
                        target: currentOperationText
                        properties: "text"
                        to: currentOperationText.nextMessage
                    }

                    NumberAnimation {
                        target: currentOperationText
                        properties: "x"
                        from: 200
                        to: 0
                        duration: 500
                        easing { type: Easing.OutQuad; }
                    }
                }

                Connections {
                    target: uiHelper
                    onSystemMessageArrived: {
                        currentOperationText.nextMessage = message
                        slidingAnimation.start()
                    }
                }

                Component.onCompleted: opacityAnimation.start()
            }
        }

        RowLayout {
            anchors.top: currentOperationItem.bottom
            anchors.topMargin: 20
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 20

            StyledButton {
                width: 70
                text: qsTr("Details")
                onClicked: {
                    openDetails()
                }
            }

            StyledButton {
                width: 70
                text: qsTr("Cancel")
                onClicked: {
                    uiHelper.cancelInstall()
                }
            }
        }
    }
}
