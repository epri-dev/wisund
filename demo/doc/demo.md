## Outage message demo
To better convey what EPRI has done so far, a demonstration has been created to illustrate the use of the protocol stack.  The demonstration involves two Raspberry Pi + radio boxes, a laptop and some additional props.  

## Equipment and connections
First, everything should be powered up and communicating.  To make it easier to refer to each of the machines, we'll arbitrarily call them "pi1", "pi2" and "x3" which is a laptop or some other full-functioned computer.  

`pi1` is connected to a speaker and is powered via USB.  No other connections are required.

`pi2` is connected to an LED and to `x3` via Ethernet.  It's also powered via USB.

`x3` is a laptop, for example, running Linux.  Its only connection is to the `pi2` box via Ethernet.  No other connection is required.  The `x3` machine must either be configured as a DHCP host or there must be some other machine acting as a DHCP server that's usable from either `pi2` or `x3`.  

## Demonstration
The demonstration starts by opening a browser on `x3` and pointing it to a web page on `pi2` which should be configured to automatically run a server.  If we assume that the local domain is `.local`, then the URL is [`http:://pi2.local:bunnyhouse.html`](http:://pi2.local:bunnyhouse.html).

The radios for `pi1` and `pi2` should already be synchronized and operating.  It does not matter if one or neither is configured as a Local Border Router (LBR).

The web page depicts a house in the dark during a rainstorm.  A single window has a light shining in it and simultaneously, the LED is lit.  Clicking once anywhere on the page causes an "outage" which results in three things: first, the window light on the picture go out; second, the server is sent a message which causes it to turn off the LED and third, the server sends a UDP message over the radio indicating that an outage has occurred and giving the time of day.  (Note that for the time of day to be accurate, the `pi2` device needs to have access to a Network Time Protocol (NTP) server to set its time or the time must be set manually before the demo).

When `pi1` receives the UDP message, it parses the message into a speech output saying something like "Outage at Mrs. Bunny's house at 18:25".  There is no response message to the outage message.

After this is done, the outage is "repaired" by clicking once again anywhere on the web page.  This also has multiple results, first, the window lights up again, the server on `pi2` is sent a restore message and so turns on the LED, and finally it sends a restoration message which includes the time of day.

When `pi1` receives the UDP restoration message, it parses the message into speech output saying something like "Power restored at Mrs. Bunny's house at 18:28." 

## Additional packages
In order to use the speech synthesizer, additional packages need to be installed on the Pi.  These are:

    sudo apt-get festival-dev speech-tools libestools2.1-dev

That will also install other dependencies; around 20 is typical.

## Building the software
The software is only optionally built.  Building it is enabled using `cmake`.  The usual build procedure would be to create a build directory, e.g. `build` from the root.  Navigate to that directory and execute `cmake` as follows.

    cmake -DWITH_DEMO=ON ..

This will create the usual array of software, but with the special hooks and additional software that's needed for the demonstration described above.

## Tasks
  1. ~~Create HTML page with rain effect and house picture~~
  1. ~~Wire LED to GPIO4 on `pi2`~~
  1. ~~Modify server to receive web request to turn off LED~~
  1. ~~Modify server to send outage UDP message~~
  1. ~~Modify server to remember outage time~~
  1. ~~Modify server to send restoration UDP message containing time and duration~~
  1. ~~Write software to receive UDP outage message~~
  1. ~~Add speech output to outage software~~
  1. ~~Add restoration message handling to outage software~~
  1. ~~Modify `pi1` startup script to automatially run outage software~~
  1. ~~Install decent sounding voice on Pi1~~

## Questions
  1. How does `pi2` determine the IPv6 address of `pi1`?
      * for now, we can just use a hardcoded global address of `2016:bd8:0:f101::102` = `pi1` and `2016:bd8:0:f101::103` = `pi2`.
  1. should outage software run as true daemon?
      * would be nice but not really needed for now
  1. if so, what's required for that?
      * N/A

