import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import GroundControl

Page {
    id: commandPage

    background: Rectangle {
        color: "black"
    }

    readonly property int titleFontSize: Math.round(height * .06)
    readonly property int pageMargin: Math.round(height * .03)
    readonly property int buttonHeight: Math.round(height * 0.1)
    readonly property int buttonFontSize: Math.round(height * 0.05)
    readonly property int commandCount: Math.max(1, commandRepeater.count)
    readonly property int rowHeight: Math.round((height - titleFontSize - buttonHeight - pageMargin * (commandCount + 3)) / commandCount)
    readonly property int rowFontSize: Math.round(rowHeight * 0.4)

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: commandPage.pageMargin
        spacing: commandPage.pageMargin

        Label {
            text: "Commands"
            color: "green"
            font.pixelSize: commandPage.titleFontSize
            Layout.alignment: Qt.AlignHCenter
        }

        Repeater {
            id: commandRepeater
            model: groundControl.commandsModel

            delegate: RowLayout {
                id: commandRow
                Layout.fillWidth: true
                Layout.fillHeight: true
                spacing: commandPage.pageMargin * 2

                required property int index
                required property string label
                required property string value
                required property int status

                Button {
                    text: commandRow.label
                    font.pixelSize: commandPage.rowFontSize
                    //fontSizeMode: Text.Fit
                    //minimumPixelSize: 8
                    //elide: Text.ElideRight
                    //verticalAlignment: Text.AlignVCenter
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    onClicked: groundControl.SendCommand(commandRow.index)
                }

                Label {
                    text: commandRow.value
                    color: root.statusColor(commandRow.status)
                    font.pixelSize: commandPage.rowFontSize
                    fontSizeMode: Text.Fit
                    minimumPixelSize: 8
                    elide: Text.ElideRight
                    verticalAlignment: Text.AlignVCenter
                    Layout.preferredWidth: commandPage.width * 0.35
                    Layout.fillHeight: true
                }
            }
        }
        Button {
            text: "Back"
            Layout.preferredWidth: commandPage.buttonHeight * 2
            Layout.preferredHeight: commandPage.buttonHeight
            font.pixelSize: commandPage.buttonFontSize
            onClicked: commandPage.StackView.view.pop()
        }
    }
}
