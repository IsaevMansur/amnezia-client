import QtQuick
import QtQuick.Layouts

import ThemeMode 1.0

import "../Controls2"

SheetType {
    id: root

    readonly property var themeOptions: [
        { mode: ThemeMode.Light, label: qsTr("Light") },
        { mode: ThemeMode.Dark, label: qsTr("Dark") },
        { mode: ThemeMode.System, label: qsTr("Automatic (system)") }
    ]
    readonly property string currentThemeName: themeOptions.find(function(option) {
        return option.mode === ThemeController.themeMode
    }).label

    expandedHeight: content.implicitHeight + 24 + PageController.safeAreaBottomMargin

    ColumnLayout {
        id: content

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        spacing: 0

        SectionHeaderType {
            Layout.fillWidth: true
            Layout.topMargin: 18
            Layout.rightMargin: 16
            Layout.leftMargin: 16
            Layout.bottomMargin: 32

            headerText: qsTr("Theme")
        }

        Repeater {
            model: root.themeOptions

            delegate: RadioItemType {
                required property var modelData

                Layout.fillWidth: true
                Layout.leftMargin: 16
                Layout.rightMargin: 16

                text: modelData.label
                checked: ThemeController.themeMode === modelData.mode
                showSeparator: modelData.mode !== ThemeMode.System

                onClicked: {
                    ThemeController.themeMode = modelData.mode
                    root.close()
                }
            }
        }
    }
}
