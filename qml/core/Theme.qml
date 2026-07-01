pragma Singleton
import QtQuick

// Design tokens for the whole YAS suite (see each repo's DESIGN.md).
// `accent` and `tag` are set at startup by the store app via YasAppWindow.
QtObject {
    property color base: "#1E1E2E"
    property color surface: "#252537"
    property color surfaceAlt: "#2C2C42"
    property color border: "#3B3B54"
    property color accent: "#FFC107"
    property color accentSubtle: Qt.rgba(accent.r, accent.g, accent.b, 0.10)
    property color textPrimary: "#F8F8F2"
    property color textSecondary: "#A9B1D6"
    property color danger: "#F7768E"
    property color success: "#9ECE6A"
    property string tag: "YAS"

    property int radius: 8
    property int spacing: 12
    property int sidebarWidth: 200

    property string headingFont: "Outfit"
    property string uiFont: "Inter"
    property string monoFont: "JetBrains Mono"
}
