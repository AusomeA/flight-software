import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import GroundControl

Page {
    id: statsPage

    function statusColor(status) {
        switch (status) {
        case SharedTypes.Status.good:
            return "green";
        case SharedTypes.Status.warning:
            return "yellow";
        case SharedTypes.Status.critical:
            return "red";
        case SharedTypes.Status.stale:
            return "steelblue";
        default:
            return "white"; // none / unknown
        }
    }

    ListModel {
        id: placeholderListModel
        ListElement {
            label: "Link"
            value: "No Link"
            status: 2
        }
        ListElement {
            label: "Mode"
            value: "N/A"
            status: 0
        }
        ListElement {
            label: "Battery"
            value: "100%"
            status: 1
        }
        ListElement {
            label: "Temp"
            value: "-----"
            status: 3
        }
        ListElement {
            label: "Something"
            value: "Else"
            status: 4
        }
    }

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
            model: placeholderListModel

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
                        color: statusColor(readoutRow.status)
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
