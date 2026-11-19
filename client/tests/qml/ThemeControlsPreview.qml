import QtQuick
import QtQuick.Layouts

import Style 1.0

import "../../ui/qml/Controls2"
import "../../ui/qml/Controls2/TextTypes"

Rectangle {
    id: root

    property alias hoverTarget: hoverButton
    property alias focusTarget: focusButton

    implicitWidth: 620
    implicitHeight: content.implicitHeight + 40
    width: implicitWidth
    height: implicitHeight
    color: AmneziaStyle.color.bgBase

    function preparePreview() {
        errorField.errorText = qsTr("Check this value")
        selectedField.textField.selectAll()
        selectedSearch.selectAll()
        selectedArea.textArea.selectAll()
        focusButton.forceActiveFocus(Qt.TabFocusReason)
    }

    ColumnLayout {
        id: content

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 20
        spacing: 16

        H2TextType {
            Layout.fillWidth: true
            text: qsTr("Theme controls")
        }

        SmallTextType {
            Layout.fillWidth: true
            text: qsTr("Enabled, pressed and disabled buttons")
        }

        GridLayout {
            Layout.fillWidth: true
            columns: 3
            columnSpacing: 12
            rowSpacing: 12

            ButtonType {
                Layout.fillWidth: true
                text: qsTr("Primary")
            }

            ButtonType {
                Layout.fillWidth: true
                variant: ButtonType.Secondary
                text: qsTr("Secondary")
            }

            ButtonType {
                Layout.fillWidth: true
                variant: ButtonType.Destructive
                text: qsTr("Destructive")
            }

            ButtonType {
                Layout.fillWidth: true
                text: qsTr("Pressed")
                down: true
            }

            ButtonType {
                Layout.fillWidth: true
                variant: ButtonType.Secondary
                text: qsTr("Pressed")
                down: true
            }

            ButtonType {
                Layout.fillWidth: true
                variant: ButtonType.Destructive
                text: qsTr("Pressed")
                down: true
            }

            ButtonType {
                Layout.fillWidth: true
                text: qsTr("Disabled")
                enabled: false
            }

            ButtonType {
                Layout.fillWidth: true
                variant: ButtonType.Secondary
                text: qsTr("Disabled")
                enabled: false
            }

            ButtonType {
                Layout.fillWidth: true
                variant: ButtonType.Destructive
                text: qsTr("Disabled")
                enabled: false
            }
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            ButtonType {
                id: hoverButton
                objectName: "hoverTarget"
                Layout.fillWidth: true
                variant: ButtonType.Secondary
                text: qsTr("Hover")
            }

            ButtonType {
                id: focusButton
                objectName: "focusTarget"
                Layout.fillWidth: true
                variant: ButtonType.Secondary
                text: qsTr("Keyboard focus")
            }
        }

        SmallTextType {
            Layout.fillWidth: true
            text: qsTr("Selection controls: on, off and disabled")
        }

        GridLayout {
            Layout.fillWidth: true
            columns: 3
            columnSpacing: 12
            rowSpacing: 8

            SwitchType {
                Layout.fillWidth: true
                text: qsTr("On")
                checked: true
            }

            SwitchType {
                Layout.fillWidth: true
                text: qsTr("Off")
            }

            SwitchType {
                Layout.fillWidth: true
                text: qsTr("Disabled")
                checked: true
                enabled: false
            }

            CheckBoxType {
                Layout.fillWidth: true
                text: qsTr("Checked")
                checked: true
            }

            CheckBoxType {
                Layout.fillWidth: true
                text: qsTr("Unchecked")
            }

            CheckBoxType {
                Layout.fillWidth: true
                text: qsTr("Disabled")
                checked: true
                enabled: false
            }

            RadioItemType {
                Layout.fillWidth: true
                text: qsTr("Selected")
                checked: true
                showSeparator: false
            }

            RadioItemType {
                Layout.fillWidth: true
                text: qsTr("Unselected")
                showSeparator: false
            }

            RadioItemType {
                Layout.fillWidth: true
                text: qsTr("Disabled")
                checked: true
                enabled: false
                showSeparator: false
            }
        }

        TextFieldType {
            id: selectedField
            Layout.fillWidth: true
            headerText: qsTr("Selected field text")
            helperText: qsTr("Selection remains visible without focus")
            textField.text: qsTr("Selected text in a field")
            textField.persistentSelection: true
        }

        SearchFieldType {
            id: selectedSearch
            Layout.fillWidth: true
            text: qsTr("Selected search text")
            persistentSelection: true
        }

        TextAreaType {
            id: selectedArea
            Layout.fillWidth: true
            Layout.preferredHeight: 110
            textAreaText: qsTr("Selected multiline text\nSecond line of the selection")
            textArea.persistentSelection: true
            clearable: false
        }

        TextFieldType {
            id: errorField
            Layout.fillWidth: true
            headerText: qsTr("Error state")
            textField.text: qsTr("Invalid value")
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 12

            StatusBadgeType {
                text: qsTr("Success")
                variant: StatusBadgeType.Success
                dot: true
            }

            StatusBadgeType {
                text: qsTr("Warning")
                variant: StatusBadgeType.Warning
                dot: true
            }

            StatusBadgeType {
                text: qsTr("Error")
                variant: StatusBadgeType.Destructive
                dot: true
            }

            StatusBadgeType {
                text: qsTr("Recommended")
                variant: StatusBadgeType.Recommendation
            }
        }
    }
}
