/*
 * Copyright (C) 2016 Taras Kushnir
 * All rights reserved.
 *
 * This software may be modified and distributed under the terms
 * of the BSD license. See the LICENSE file for details.
*/

import QtQuick 2.2
import QtQuick.Dialogs 1.1
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.1
import "Colors.js" as Colors;

ApplicationWindow {
    id: detailsWindow
    modality: "ApplicationModal"
    title: qsTr("Details")
    width: 400
    height: 600
    visible: true

    function scrollToBottom() {
        var flickable = scrollView.flickableItem
        if (flickable.contentHeight > flickable.height) {
            flickable.contentY = flickable.contentHeight - flickable.height
        } else {
            flickable.contentY = 0
        }
    }

    Rectangle {
        anchors.fill: parent
        color: Colors.defaultDarkColor

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 20
            spacing: 20

            ScrollView {
                id: scrollView
                anchors.left: parent.left
                anchors.right: parent.right
                Layout.fillHeight: true

                style: ScrollViewStyle {
                    //transientScrollBars: true
                    minimumHandleLength: 20

                    handle: Item {
                        implicitHeight: 10
                        implicitWidth: 10

                        Rectangle {
                            anchors.fill: parent
                            radius: 5
                            color: Colors.activeColor
                            opacity: styleData.pressed ? 1 : (styleData.hovered ? 0.9 : 0.8)
                        }
                    }

                    scrollBarBackground: Item {
                        property bool sticky: false
                        property bool hovered: styleData.hovered
                        implicitWidth: 10
                        implicitHeight: 10
                        clip: true

                        onHoveredChanged: if (hovered) sticky = true
                    }

                    corner: Item {}
                    decrementControl: Item { width: 1; height: 1 }
                    incrementControl: Item { width: 1; height: 1 }
                }

                TextEdit {
                    readOnly: true
                    text: liveLog.contents
                    color: Colors.labelActiveForeground
                    onTextChanged: scrollToBottom()
                }
            }

            RowLayout {
                anchors.left: parent.left
                anchors.right: parent.right

                Item {
                    Layout.fillWidth: true
                }

                StyledButton {
                    width: 100
                    text: qsTr("Close")
                    onClicked: detailsWindow.close()
                }
            }
        }
    }
}
