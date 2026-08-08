import QtQuick
import QtQuick.Controls

ApplicationWindow {
    function statusColor(status) {
        switch (status) {
        case 0:
            return "green"; // good
        case 1:
            return "yellow"; // warning
        case 2:
            return "red"; // critical
        default:
            return "white"; // unknown
        }
    }

    visible: true

    width: 900
    height: 1080

    title: "Spacecraft Simulator"

    font.pixelSize: 32
    color: "black"

    readonly property int labelWidth: 260
    readonly property int valueWidth: 200
    readonly property int rowHeight: 40

    ScrollView {
        anchors.fill: parent
        anchors.margins: 20

        Column {
            spacing: 0

            Repeater {
                model: readouts
                //spacing: 0

                delegate: Row {
                    required property var modelData

                    Rectangle {
                        width: labelWidth
                        height: rowHeight
                        color: "black"
                        border.color: "white"
                        border.width: 1

                        Label {
                            text: modelData[0]
                            width: 260
                            font.pixelSize: 20
                            color: "white"
                        }
                    }

                    Rectangle {
                        width: labelWidth
                        height: rowHeight
                        color: "black"
                        border.color: "white"
                        border.width: 1

                        Label {
                            text: modelData[1]
                            font.pixelSize: 20
                            color: statusColor(modelData[2])
                        }
                    }
                }
            }
        }
    }
}
