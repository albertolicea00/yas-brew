import QtQuick
import Yas.Core

YasAppWindow {
    appName: qsTr("Yet Another Store for Brew")
    accent: "#FFC107" // DESIGN.md — signature Yellow
    tag: "BREW"
    extraViews: [
        { label: qsTr("Taps"), icon: "⑂", source: Qt.resolvedUrl("TapsView.qml") }
    ]
}
