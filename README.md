# Virtual Satellite (VSAT)

**Austin Anderson**

VSAT is a three-application satellite testbed: a spacecraft simulator that plays the
vehicle, a flight computer that makes the onboard decisions, and a ground control client
that gives the operator a seat. It is a portfolio project, built to show modern C++,
Linux networking, fault management, automated testing, and multi-device deployment on
real hardware.

The simulator and flight computer each run on their own Raspberry Pi with a touchscreen.
Ground control runs on a PC or an Android device. All three talk over UDP on a local
network and find each other without configuration.

## Highlights

- **A flight computer brain with no UI, networking, or event-loop dependency.** The
  decision logic is a plain C++ class that uses only Qt's core value types. Sockets,
  timers, and the screen live in a separate shell, so the brain can be driven directly
  by a test.
- **GoogleTest on every decision and every parser.** The full mode table including the
  safe-mode latch and recovery hysteresis, payload and communications scheduling, heater
  hysteresis, the exit, reboot, and inhibit commands, the JSON envelope, and telemetry
  and command serialization all run under CTest with no sockets and no UI.
- **A layered protection stack in the simulator.** Physics at the bottom, then power
  protection (brownout load shedding and blackout), then autonomous fallback reflexes
  when the flight computer link drops, then flight computer modes on top. Each layer
  survives the failure of the one above it.
- **Acknowledged commands with retries, plus zero-config peer discovery.** Ground
  commands and fault injection are sequence-numbered, acknowledged or rejected, and
  retried on timeout. Applications announce themselves by UDP broadcast, so moving
  between the home network and an off-grid hotspot needs no reconfiguration.
- **Deployed across three devices.** Two Raspberry Pis and a PC over Wi-Fi, including a
  mode where one Pi hosts its own hotspot for demonstrations with no internet.

## The three applications

**Spacecraft simulator.** Models a small Earth-observation satellite in low Earth orbit:
a sunlight and eclipse cycle, solar generation, a battery, switched loads, a first-order
thermal model with a heater and passive radiator louvers, and sensor health. It obeys
commands from the flight computer, publishes telemetry several times a second, and
accepts fault injection from ground control on a separate "god mode" channel the
flight computer never sees.

**Flight computer.** Receives telemetry, runs the mode state machine (Nominal, Degraded,
Safe), decides payload, communications, and heater state, and commands the simulator.
Safe mode is a hard latch: the vehicle stays safe until an operator commands the exit,
and the exit is refused while any safe-mode reason is still true. The flight computer
also accepts a reboot command that wipes its state and takes it dark for a few seconds,
and per-sensor inhibit commands that let the operator fly through a known-bad sensor.
Its local screen is read-only.

**Ground control.** A Qt Quick client with live telemetry, a fault injection page with a
matching inhibit column, and a commands page. Every switch and button shows the
lifecycle of its message: sending, accepted, rejected, or no response.

## Architecture

```
spacecraft_simulator  --telemetry-->  flight_computer  --telemetry-->  ground_control
spacecraft_simulator  <--commands---  flight_computer  <--commands---  ground_control
spacecraft_simulator  <------------- fault injection (god mode) ------  ground_control
```

- Transport is UDP. Telemetry is fire-and-forget because a newer sample always follows.
  Ground commands and fault injection travel in a JSON envelope with a message type and
  sequence number, and are acknowledged or rejected by the receiver with retry on
  timeout.
- Flight computer to simulator commands are repeated on every telemetry tick with
  explicit set semantics, so a lost packet is simply replaced by the next one.
- Peer discovery is a UDP broadcast announcement carrying the application name, a
  per-boot instance ID, and a vehicle name. Peers are tracked by instance and expire
  when they go quiet. Multiple ground control clients can watch one vehicle at once.
- Shared constants, message vocabulary, and the telemetry and command types live in
  one header used by all three applications, so they cannot drift apart on field names.

Layout:

```
apps/spacecraft_simulator/   physics, actuators, fault injection, touchscreen UI
apps/flight_computer/        FlightComputer (the brain) + FlightComputerShell (Qt glue, sockets, UI)
apps/ground_control/         GroundControl backend + QML pages
libs/core/                   SharedTypes, Discovery, Helpers
libs/protocol/               JSON envelope and telemetry serialization
libs/networking/             UDP receiver, acknowledged UDP sender
libs/ui/                     readouts model and shared telemetry rows
tests/                       GoogleTest suites
```

## Operating modes

- **Nominal.** Payload runs in its scheduled window, communications transmit during
  ground station passes, heater holds the comfort band.
- **Degraded.** Entered on low battery during eclipse. Payload off, communications
  window shortened, heater continues. Recovers to Nominal with hysteresis once the
  battery is comfortably charged.
- **Safe.** Entered on critically low battery, any failed sensor, or prolonged loss of
  ground contact. Payload off, communications reduced to a periodic beacon. Latched
  until ground commands the exit.

The simulator has its own reflexes underneath all of this. If the flight computer stops
commanding, the simulator turns the payload off and runs a survival thermostat on its
own. If the battery falls far enough, it sheds every switched load, and at the very
bottom it goes dark entirely and reboots when the sun comes back.

## Building and running (Ubuntu / WSL)

Prerequisites:

```bash
sudo apt install build-essential cmake git qt6-base-dev qt6-declarative-dev \
    qml6-module-qtquick-controls qml6-module-qtquick-layouts qml6-module-qtquick-window
```

Tested on Ubuntu with Qt 6.10 and CMake 3.20 or newer. The first configure downloads
GoogleTest, so it needs network access once.

Build:

```bash
git clone https://github.com/AusomeA/vsat.git
cd vsat
cmake -S . -B build
cmake --build build -j 8
```

Run, each in its own terminal, in any order. They find each other by discovery:

```bash
./build/spacecraft_simulator
./build/flight_computer
./build/ground_control
```

Each application also accepts a peer IP address as its first argument as a manual
override when broadcast discovery is unavailable.

## Tests

```bash
ctest --test-dir build
```

The suite covers the full mode table including the safe-mode latch and recovery
hysteresis, payload and communications window edges, heater hysteresis in both
directions, the exit, reboot, and inhibit commands, and JSON round-trips including
rejection of malformed and out-of-range packets.

## Raspberry Pi deployment

Each Pi has the repository cloned to `~/projects/vsat` and a launcher script,
`~/run-vsat.sh`, wired to a desktop shortcut. The script pulls the latest commit, builds,
prints a banner showing exactly which commit is running and whether the pull succeeded,
and launches its application. The banner exists because "the Pi is running old code" is
the single most common cause of confusing behavior in a multi-device system.

```bash
#!/bin/bash
set -e
export QT_LOGGING_RULES="qt6ct.debug=false"
cd ~/projects/vsat

commitBeforePull=$(git rev-parse --short HEAD)
pullFailed=0
git pull || pullFailed=1
commitAfterPull=$(git rev-parse --short HEAD)

cmake -S . -B build
cmake --build build -j 4

echo
echo "================================================================"
if [ "$pullFailed" -eq 1 ]; then
    echo "  !!! GIT PULL FAILED - RUNNING OLD CODE !!!"
fi
if [ "$commitBeforePull" = "$commitAfterPull" ]; then
    echo "  No new commits pulled"
else
    echo "  Updated $commitBeforePull -> $commitAfterPull"
fi
echo "  Running: $(git log --oneline -1)"
echo "================================================================"
echo

./build/flight_computer      # ./build/spacecraft_simulator on the simulator Pi
```

Both applications start full screen. Press and hold the top-left corner to toggle full
screen and the top-right corner to quit.

**Off-grid mode.** For demonstrations without a network, one Pi hosts a NetworkManager
hotspot and the other Pi and the ground control device join it. Discovery handles the
address change; nothing is reconfigured.

## Design document

A system design document defines the architecture, protocol, mode table, and fault set.
It is being revised to match what was actually built and will be added to the repository
when that revision is complete.

## What's next

- **Multiple vehicles.** One ground control client flying a small constellation. The
  discovery protocol and vehicle ID field already point this way.
- **Scenario engine.** The simulator already has accelerated time and live fault
  injection. The next step is scripting them into repeatable timelines, with pause,
  single-step, and record and replay of a whole session.
- **Hardware in the loop.** A real sensor board feeding the simulator, or real actuators
  driven by it.
- **Unreal Engine visualization.** The vehicle rendered in orbit, driven by live
  telemetry.
- **Attitude and navigation.** An attitude model with redundant sensors and
  disagreement detection.

## License

Copyright (c) 2026 Austin Anderson. All rights reserved. The source is published for
portfolio review. For licensing or commercial use, contact the author. See `LICENSE`.