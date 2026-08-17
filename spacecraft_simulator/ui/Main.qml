import QtQuick
import QtQuick.Window
import QtQuick.Controls
import SpacecraftSimulator

ApplicationWindow {
    id: root

    /* Timer {
        interval: 100
        running: true
        repeat: false
        onTriggered: {
            root.x = (Screen.width - width) / 2;
            root.y = (Screen.height - height) / 2;
        }
    } */

    Spacecraft {
        id: simulator
        Component.onCompleted: Start()
    }

    function statusColor(status) {
        switch (status) {
        case Spacecraft.Status.good:
            return "green"; // good
        case Spacecraft.Status.warning:
            return "yellow"; // warning
        case Spacecraft.Status.critical:
            return "red"; // critical
        default:
            return "white"; // none / unknown
        }
    }

    visible: true
    visibility: Window.FullScreen

    title: "Spacecraft Simulator"

    color: "black"

    readonly property int readoutRowCount: simulator.readouts.length
    readonly property int readoutColumnCount: readoutRowCount > 0 ? simulator.readouts[0].length - 1 : 0
    readonly property int rowHeight: Math.round(height * 0.1)
    readonly property int baseFontSize: Math.round(rowHeight * 0.45)
    readonly property int cellPadding: Math.round(baseFontSize * 0.5)

    // Esc Key exits full screen mode and returns to windowed mode, or vice versa
    Shortcut {
        sequence: "Esc"
        onActivated: root.visibility = root.visibility === Window.FullScreen ? Window.Windowed : Window.FullScreen
    }

    Shortcut {
        sequence: "Right"
        onActivated: simulator.IncreaseTimeScale()
    }

    Shortcut {
        sequence: "Left"
        onActivated: simulator.DecreaseTimeScale()
    }

    Shortcut {
        sequence: "Up"
        onActivated: simulator.ModeTestUp()
    }

    Shortcut {
        sequence: "Down"
        onActivated: simulator.ModeTestDown()
    }

    Shortcut {
        sequence: "="
        onActivated: simulator.BatteryTestUp()
    }

    Shortcut {
        sequence: "-"
        onActivated: simulator.BatteryTestDown()
    }

    Shortcut{
        sequence: "s,0"
        onActivated: simulator.ToggleSensorFault(0)
    }

    Shortcut{
        sequence: "s,1"
        onActivated: simulator.ToggleSensorFault(1)
    }

    Shortcut{
        sequence: "s,2"
        onActivated: simulator.ToggleSensorFault(2)
    }

    Shortcut{
        sequence: "f"
        onActivated: simulator.FailRandomSensor( Math.floor( Math.random()*3 ))
    }

    ScrollView {
        id: readoutsScrollView
        anchors.fill: parent
        anchors.margins: 20

        Column {
            id: readoutColumn
            width: readoutsScrollView.availableWidth
            spacing: 0

            Repeater {
                model: simulator.readouts

                delegate: Row {
                    id: readoutRow
                    width: readoutColumn.width
                    required property var modelData

                    Rectangle {
                        width: readoutRow.width / readoutColumnCount
                        height: rowHeight
                        color: "black"
                        border.color: "white"
                        border.width: 1

                        Label {
                            anchors.fill: parent
                            anchors.margins: cellPadding
                            verticalAlignment: Label.AlignVCenter
                            horizontalAlignment: Label.AlignHCenter
                            elide: Text.ElideRight

                            text: modelData[0]

                            font.pixelSize: baseFontSize
                            color: "white"
                        }
                    }

                    Rectangle {
                        width: readoutRow.width / readoutColumnCount
                        height: rowHeight
                        color: "black"
                        border.color: "white"
                        border.width: 1

                        Label {

                            anchors.fill: parent
                            anchors.margins: cellPadding
                            verticalAlignment: Label.AlignVCenter
                            horizontalAlignment: Label.AlignHCenter
                            elide: Text.ElideRight

                            text: modelData[1]

                            font.pixelSize: baseFontSize
                            color: statusColor(modelData[2])
                        }
                    }
                }
            }
        }
    }
}
