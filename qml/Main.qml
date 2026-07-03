import QtQuick
import Yas.Core

YasAppWindow {
    appName: qsTr("Yet Another Store for Brew")
    accent: "#FFC107" // DESIGN.md — signature Yellow
    tag: "BREW"
    defaultKind: "cask"
    extraViews: [
        { label: qsTr("Taps"), icon: "tap", source: Qt.resolvedUrl("TapsView.qml") }
    ]
}
