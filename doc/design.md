Overview
========
The purpose of this tool is to provide a reference implementation of the IEEE 802.15.4g radio protocol stack that conforms to the Wi-SUN Alliance specification. It also provides a means by which other such radios can be tested and evaluated.

## Essential pieces
### Hardware
The first iteration of this reference implementation is built using the Texas Instruments CC1200 evaluation board and a Raspberry Pi model B.  The evaluation board provides the radio parts and a minimal interface by which they can be used to build higher layer protocols.  The higher layers are built on a Raspberry Pi and the two boards (Raspberry Pi and CC1200 Evaluation board) are connected together via a serial connection running at 115200 bps.  A summary of the connections is shown below:

![Hardware Photo](../hardware.jpg)

The connections between the two are shown in the table below:

Pi pin #|Pi pin name|CC1200 pin name
--------|-----------|---------------
1       | 3.3V      | Vext
6       | GND       | GND
8       | UART\_TXD  | Port 5 pin 7
10      | UART\_RXD  | Port 5 pin 6

### Software
There are several distinct software pieces within this package.  The pieces are named `wisun-cli`, `wisund`, `wiunsimd`, and `web_server`.  Each tool provides a means of interpreting and responding to the defined [commands](@ref commands).

## @ref wisun-cli.cpp
This software provides a command-line text-based interface for interacting with the EPRI Wi-SUN stack.  In addition to conveying commands and displaying the results, this software also takes care of routing the IPv6 packets across the RF link.

## @ref wisund.cpp
This software is mostly identical to the `wisun-cli` code except that instead of interacting via text on the command line, this software interacts via text served on TCP/IPv4 port 5555.  All of the same commands are available.  It is intended that this code would normally run on the Raspberry Pi to run the radio software and that either the web interface (via `web_server`) or a direct link, as, for example by using `telnet` on port 5555. 

## @ref wisunsimd.cpp
This software is mostly identical to the `wisund` software except for two significant differences.  First, it uses a simulator rather than actually communicating with a radio over the serial port.  Second, since the RF link is simulated, the IPv6 routing portion of the code is omitted from `wisunsimd`

## @ref server.cpp
As the name suggests, this software is intended to run on the Raspberry Pi and implements a complete web server running on port 8000 with facilities for communicating with `wisund` or `wisunsimd`.  One of those programs must also be running for the web server to operate properly.  All of the [commands](@ref commands) are accessible via the relative path `\tool?`.  That is, if the server is running on `localhost`, one can request `diag 03` by using the url <http://localhost:8000/tool?diag 03>.


## Building the software and firmware
There are [instructions for building the software on the Raspberry Pi](@ref pibuild) and also instructions for building the firmware for the CC1200 evaluation board. (TODO: add CC1200 instructions).

## Tool design
This tool consists of two main parts.  One is a simple parser that is constructed with flex and bison.  Its purpose is to interact with either a human user or a script and to translate and convey the commands to the Wi-SUN board in binary form.  The other part connects the serial port to a tun device.  Essentially, anything that comes in via the IPv6 tun device is passed to the serial port and vice versa.  The program can differentiate message types because the first byte of each message designates the message type.  See the Message types section below.

## Original prototype
The original version was written in C and used the POSIX `select` to determine with of the three interfaces had data ready.  This worked well enough for a prototype, but the interface was not elegant and the code was even less so.  

## Interfaces

There are essentially four interfaces that the program must handle:

  1. **`tun` device**: The `tun` device provides both read and write access to what is essentially a serial stream of IPv6 traffic for the purposes of this program.  On the Raspberry Pi, *other* software will interact with this device just as with any other IPv6 network interface.  The only difference is that instead of a device name such as `/dev/eth0` for a typical Ethernet device, this device is generally named `/dev/tun0`.  
  2. **serial port**: The serial port is physically connected to the radio board. Sending data to the serial port is simple, but messages coming in need to be classified according to message type.  Messages that are IPv6 frames are sent to the `tun` device.  All others are handled internally.
  3. **`stdin`**: Unlike the two above, interaction with the user is divided into input and output.  The program gets commands from `stdin` which are simple text commands.  These are translated into packets according to the [message types](@ref MsgTypes).
  4. **`stdout`**: As with `stdin`, `stdout` is half of the interaction with the user.  The output that goes to the user is text messages that are neither IPv6 messages nor command responses.  They are simply printed to the screen as ASCII.

## Class design
The original prototype was written in C and largely procedural.  The new version is written in C++ and incorporates the C++ `asio` library for both the serial port and for socket access.  This aids with both portability and structure.

### SafeQueue
Functioning as a message queue, this class handles the sequential processing and routing of messages.  Must be accessible by multiple threads.  Each interface has its own inbound queue; routing is done by the outbound message handler associated with the device.

### Device
Each device has two interfaces; one is the external facing interface that defines it (e.g. serial port, console or tun) and the internal interface which looks the same for all devices.  The internal device interface is a receive message queue.  

### Router
This object is at the heart of the application and has at least three bidirectional ports for six total I/O ports.  Messages that come in via one port are classified and sent to one (or more) of the other ports based on the arrival port and the contents of the message.

### SerialDevice
Needs to receive serial data, unwrap it (SLIP) and send raw message to Router. For transmit, each received message is wrapped via SLIP and sent.

### Console
Translates text commands recieved via console into messages that are sent to Router.  Received messages are presumed to be reactions (answers) to sent messages via a 1-to-1 pairing.  Received messages are parsed and printed to the console in human-readable JSON format.

### TunDevice
Anything received via tun is sent directly to Router; anything received on internal port is assumed to an outbound message and is sent.

### Simulator
As the name suggests, this device is intended to provide a simulated version of the radio hardware.  The primary purpose for this module is to allow for a simulated test to run on any Linux machine without the need for any additional hardware. This can be useful for performing development on the web pages or server.

## Threads
The program is multithreaded and includes three threads.  One is the main thread that has both the user interface (`std::cin` and `std::cout`) as well as the `SafeQueue` message queue.  The second thread handles the serial port, and the third thread handles the `tun` device.

Message types  {#MsgTypes} 
=============

The Message types are summarized below:

00
: IPv6 packet with Ethertype

01
: FCHAN HEXBYTE

02
: TR51CF

04
: PHY HEXBYTE

10
: LBR

11
: NLBR

20
: STATE

21
: DIAG

2F
: GETZZ

30
: PING

FF
: reset

