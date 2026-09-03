import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import GroundControl

Page {
    id: statsPage

    background: Rectangle {
        color: "black"
    }

    readonly property int readoutRowCount: Math.max(1, Math.ceil(readoutRepeater.count / 2))
    readonly property int rowHeight: Math.floor(readoutGrid.height / readoutRowCount)
    readonly property int baseFontSize: Math.round(rowHeight * 0.45)
    readonly property int cellPadding: Math.round(baseFontSize * 0.5)

    Grid {
        id: readoutGrid
        anchors.fill: parent
        anchors.margins: 10
        anchors.bottomMargin: 100
        columns: 2
        rowSpacing: 0
        columnSpacing: 10

        Repeater {
            id: readoutRepeater
            model: groundControl.readoutsModel

            delegate: RowLayout {
                id: readoutRow
                width: (readoutGrid.width - readoutGrid.columnSpacing) / readoutGrid.columns
                spacing: 0
                required property string label
                required property string value
                required property int status

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: rowHeight
                    color: "black"
                    border.color: "white"
                    border.width: 1

                    Label {
                        anchors.fill: parent
                        anchors.margins: cellPadding
                        verticalAlignment: Label.AlignVCenter
                        horizontalAlignment: Label.AlignHCenter
                        elide: Text.ElideRight
                        fontSizeMode: Text.Fit
                        minimumPixelSize: 8

                        text: readoutRow.label

                        font.pixelSize: baseFontSize
                        color: "white"
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: rowHeight
                    color: "black"
                    border.color: "white"
                    border.width: 1

                    Label {

                        anchors.fill: parent
                        anchors.margins: cellPadding
                        verticalAlignment: Label.AlignVCenter
                        horizontalAlignment: Label.AlignHCenter
                        elide: Text.ElideRight
                        fontSizeMode: Text.Fit
                        minimumPixelSize: 8

                        text: readoutRow.value

                        font.pixelSize: baseFontSize
                        color: root.statusColor(readoutRow.status)
                    }
                }
            }
        }
    }

    Button {
        text: "Back"
        width: 120
        height: 60
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.margins: 20
        onClicked: statsPage.StackView.view.pop()
    }
}
