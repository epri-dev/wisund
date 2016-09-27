## Tool design
This tool consists of two main parts.  One is a simple parser that is constructed with flex and bison.  Its purpose is to interact with either a human user or a script and to translate and convey the commands to the Wi-SUN board in binary form.  The other part connects the serial port to a tun device.  Essentially, anything that comes in via the IPv6 tun device is passed to the serial port and vice versa.  The program can differentiate message types because the first byte of each message designates the message type.  See [message types](#message-type).

## Original prototype
The original version was written in C and used the POSIX `select` to determine with of the three interfaces had data ready.  This worked well enough for a prototype, but the interface was not elegant and the code was even less so.  

## Interfaces

There are essentially four interfaces that the program must handle:

  1. **`tun` device**: The `tun` device provides both read and write access to what is essentially a serial stream of IPv6 traffic for the purposes of this program.  On the Raspberry Pi, *other* software will interact with this device just as with any other IPv6 network interface.  The only difference is that instead of a device name such as `/dev/eth0` for a typical Ethernet device, this device is generally named `/dev/tun0`.  
  2. **serial port**: The serial port is physically connected to the radio board. Sending data to the serial port is simple, but messages coming in need to be classified according to message type.  Messages that are IPv6 frames are sent to the `tun` device.  All others are handled internally.
  3. **`stdin`**: Unlike the two above, interaction with the user is divided into input and output.  The program gets commands from `stdin` which are simple text commands.  These are translated into packets according to the [message types](#message-types).
  4. **`stdout`**: As with `stdin`, `stdout` is half of the interaction with the user.  The output that goes to the user is text messages that are neither IPv6 messages nor command responses.  They are simply printed to the screen as ASCII.

## Class design
The [original prototype](#original-prototype) was written in C and largely procedural.  The new version is written in C++ and incorporates the C++ `asio` library for both the serial port and for socket access.  This aids with both portability and structure.

### MessageQueue
Actually a deque, handles the sequential processing and routing of messages.  Must be accessible by multiple threads.  Each interface has its own inbound queue; routing is done by the outbound message handler associated with the device.

### Device
Each device has two interfaces; one is the external facing interface that defines it (e.g. serial port, console or tun) and the internal interface which looks the same for all devices.  The internal device interface is a receive message queue.  

### Router
This object is at the heart of the application and has at least three bidirectional ports for six total I/O ports.  Messages that come in via one port are classified and sent to one (or more) of the other ports based on the arrival port and the contents of the message.

### SerialPort
Needs to receive serial data, unwrap it (SLIP) and send raw message to Router. For transmit, each received message is wrapped via SLIP and sent.

### Parser
Translates text commands recieved via console into messages that are sent to Router.  Received messages are presumed to be reactions (answers) to sent messages via a 1-to-1 pairing.  Received messages are parsed and printed to the console in human-readable JSON format.

### TunHandler
Anything received via tun is sent directly to Router; anything received on internal port is assumed to an outbound message and is sent.

## Message types
The message types are summarized below:

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

