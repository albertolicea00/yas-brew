import QtQuick
import QtQuick.Controls.Basic
import Yas.Core

// Brew-specific extra view: manage third-party taps.
Item {
    id: root
    property var taps: []

    function reload() { App.fetchActionOutput("tap", "") }
    Component.onCompleted: reload()

    Connections {
        target: App
        function onActionOutputReady(actionId, packageId, stdOut, ok) {
            if (actionId === "tap" && ok)
                root.taps = stdOut.trim().length > 0 ? stdOut.trim().split("\n") : []
        }
        function onCommandFinished(exitCode) {
            // refresh after tap/untap operations complete
        }
    }

    Column {
        anchors.fill: parent
        anchors.margins: 16
        spacing: 12

        Row {
            width: parent.width
            spacing: 6

            Text {
                width: parent.width - reloadBtn.width - 6
                text: qsTr("Taps")
                color: Theme.textPrimary
                font.family: Theme.headingFont
                font.pixelSize: 24
                font.weight: Font.Bold
            }
            IconButton {
                id: reloadBtn
                icon: "↻"
                tooltip: qsTr("Reload tap list")
                anchors.verticalCenter: parent.verticalCenter
                onClicked: root.reload()
            }
        }

        Row {
            width: parent.width
            spacing: 6

            Rectangle {
                width: parent.width - addBtn.width - 6
                height: 36
                radius: Theme.radius
                color: Theme.surface
                border.color: tapField.activeFocus ? Theme.accent : Theme.border

                TextField {
                    id: tapField
                    anchors.fill: parent
                    anchors.leftMargin: 10
                    placeholderText: qsTr("user/repo (e.g. homebrew/cask-fonts)")
                    placeholderTextColor: Theme.textSecondary
                    color: Theme.textPrimary
                    font.family: Theme.monoFont
                    font.pixelSize: 13
                    background: null
                    onAccepted: addBtn.clicked()
                }
            }
            AccentButton {
                id: addBtn
                anchors.verticalCenter: parent.verticalCenter
                text: qsTr("Add tap")
                enabled: tapField.text.trim().length > 0
                onClicked: {
                    App.runAction("tap-add", tapField.text.trim())
                    tapField.text = ""
                    reloadTimer.restart()
                }
            }
        }

        ListView {
            width: parent.width
            height: parent.height - y
            clip: true
            spacing: 4
            model: root.taps
            ScrollBar.vertical: ScrollBar {}

            delegate: Rectangle {
                required property string modelData
                width: ListView.view.width
                height: 46
                radius: Theme.radius
                color: Theme.surface

                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: 14
                    anchors.verticalCenter: parent.verticalCenter
                    text: modelData
                    color: Theme.textPrimary
                    font.family: Theme.monoFont
                    font.pixelSize: 13
                }
                IconButton {
                    anchors.right: parent.right
                    anchors.rightMargin: 8
                    anchors.verticalCenter: parent.verticalCenter
                    icon: "−"
                    tint: Theme.danger
                    tooltip: qsTr("Remove tap %1").arg(modelData)
                    onClicked: {
                        App.runAction("untap", modelData)
                        reloadTimer.restart()
                    }
                }
            }
        }
    }

    Timer {
        id: reloadTimer
        interval: 4000 // give brew time to finish, then refresh the list
        onTriggered: root.reload()
    }
}
