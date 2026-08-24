import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import FlightComputer

ApplicationWindow {
    id: root

    FlightComputerUI {
        id: flightComputer
    }

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

    visible: true
    visibility: Window.Windowed
    //width: Screen.width / 2
    //height: Screen.height - 20

    //x: 0
    //y: 0

    title: "Flight Computer"

    color: "black"

    readonly property int readoutRowCount: Math.max(1, Math.ceil(readoutRepeater.count / 2))
    readonly property int rowHeight: Math.floor(readoutGrid.height / readoutRowCount)
    readonly property int baseFontSize: Math.round(rowHeight * 0.45)
    readonly property int cellPadding: Math.round(baseFontSize * 0.5)

    // Esc Key exits full screen mode and returns to windowed mode, or vice versa
    Shortcut {
        sequence: "Esc"
        onActivated: root.visibility = root.visibility === Window.FullScreen ? Window.Windowed : Window.FullScreen
    }

    //Shortcut {
    //    sequence: "Right"
    //    onActivated: simulator.IncreaseTimeScale()
    //}
//
    //Shortcut {
    //    sequence: "Left"
    //    onActivated: simulator.DecreaseTimeScale()
    //}
//
    //Shortcut {
    //    sequence: "Up"
    //    onActivated: simulator.ModeTestUp()
    //}
//
    //Shortcut {
    //    sequence: "Down"
    //    onActivated: simulator.ModeTestDown()
    //}
//
    //Shortcut {
    //    sequence: "="
    //    onActivated: simulator.BatteryTestUp()
    //}
//
    //Shortcut {
    //    sequence: "-"
    //    onActivated: simulator.BatteryTestDown()
    //}
//
    //Shortcut {
    //    sequence: "s,0"
    //    onActivated: simulator.ToggleSensorFault(0)
    //}
//
    //Shortcut {
    //    sequence: "s,1"
    //    onActivated: simulator.ToggleSensorFault(1)
    //}
//
    //Shortcut {
    //    sequence: "s,2"
    //    onActivated: simulator.ToggleSensorFault(2)
    //}
//
    //Shortcut {
    //    sequence: "f"
    //    onActivated: simulator.FailRandomSensor(Math.floor(Math.random() * 3))
    //}
//
    //Shortcut {
    //    sequence: "c"
    //    onActivated: simulator.ToggleChaosMode()
    //}

    //ScrollView {
    //    id: readoutsScrollView
    //    anchors.fill: parent
    //    anchors.margins: 20

        Grid {
            id: readoutGrid
            //width: readoutsScrollView.availableWidth
            anchors.fill: parent
            anchors.margins: 10
            columns: 2
            rowSpacing: 0
            columnSpacing: 10

            Repeater {
                id: readoutRepeater
                model: flightComputer.readoutsModel

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
    //}
}
