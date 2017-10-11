# Commands  {#commands}
The various tools implement the following commands.  Some of the commands are only available in the connected mode (that is, after the node has joined the RF network).

## Commands available in all modes
The following commands are available in all modes.

### help
This command gives a brief summary of the commands available.

### lbr
Puts the node into LBR mode.  The response is the same as from a `state` command.  There may only be one LBR node on the RF network.  All other nodes must be NLBR nodes.

### nlbr
Puts the node into NLBR mode.  The response is the same as from a `state` command.

### buildid
Returns the buildid string for the radio firmware.  It returns a JSON string.
> { "buildid":"Jan 13 2017 17:08:51 1v01D04" }
### state
> { "mode":"LBR", "neighbors":1, "discoveryState":4 }
### mac
> { "mac":"00:19:59:ff:fe:0f:ff:01" }

### setmac macaddr
Sets the MAC address for this node.  Note that the command requires a sequence of eight hexadecimal numbers *without punctuation* (that is, without colons).  Does not return a response.

### restart
Restarts the node and puts it back into its initial state.

## Commands only available in connected mode
These commands are only available when the node is (or was previously) connected to the RF network.

### diag 01
> { "addrinfo": { "time":1615708, "acceptedframes":5, "rejectedaddresses":0, "recvdmhr":58113, "lastrcvddstpanid":"0000", "lastrcvdsrcpanid":"feca", "lastrejectaddr":"00:00:00:00:00:00:00:00", "lastrcvdaddr":"00:19:59:ff:fe:0f:ff:02" } }
### diag 02
> { "iecounters": { "fcie":0, "uttie":5, "rslie":0, "btie":2, "usie":5, "bsie":2, "panie":0, "netnameie":3, "panverie":2, "gtkhashie":2, "mpie":0, "mhdsie":0, "vhie":0, "vpie":0 } }
### diag 03
> { "ieunknown": { "count":0, "rejected": [ { "desc":0, "subdesc":0}, { "desc":0, "subdesc":0}, { "desc":0, "subdesc":0}, { "desc":0, "subdesc":0}, { "desc":0, "subdesc":0}, { "desc":0, "subdesc":0}, { "desc":0, "subdesc":0}, { "desc":0, "subdesc":0}, { "desc":0, "subdesc":0}, { "desc":0, "subdesc":0} ] } }
### diag 04
> { "mpxie": { "timestamp":0, "mpxie_count":0, "mpx_id":0, "mpx_subid":0, "msdulength":0, "srcaddr":"00:00:00:00:00:00:00:00", "destaddr":"00:00:00:00:00:00:00:00" } }
### diag 05
> { "fhieinfo": { "index":0, "timestamp":1675525, "clock_drift":5, "timestamp_accuracy":10, "unicast_dwell":250, "broadcast_dwell":0, "broadcast_interval":0, "broadcast_schedule_id":0, "channel_plan":0, "channel_function":2, "reg_domain":1, "operating_class":2, "ch0":902400, "channelspacing":400, "number_channels":64, "fixed_channel":0, "excludedchanmask":"0000000000000000000000000000000000" } }
### diag 06
> { "mysequence": [ { "slot":6721, "chan":31}, { "slot":6722, "chan":22}, { "slot":6723, "chan":58}, { "slot":6724, "chan":10}, { "slot":6725, "chan":45}, { "slot":6715, "chan":1}, { "slot":6716, "chan":29}, { "slot":6717, "chan":55}, { "slot":6718, "chan":37}, { "slot":6719, "chan":4} ] }
### diag 07
> { "targetseq": [ { "slot":0, "chan":0}, { "slot":0, "chan":0}, { "slot":0, "chan":0}, { "slot":0, "chan":0}, { "slot":0, "chan":0}, { "slot":0, "chan":0}, { "slot":0, "chan":0}, { "slot":0, "chan":0}, { "slot":0, "chan":0}, { "slot":0, "chan":0} ] }
### diag 08
> { "fhnbinfo": { "index":0, "timestamp":1675525, "lastrcvdufsi":1446862, "neighborlastms":1412951, "lastrslrssi":0, "rawmeasrssi":58, "lastrcvdbfi":0, "lastrcvdbsn":0, "panid":"feca", "panversion":1 } }
### diag 09
> { "radiostats": { "rxcount":6, "fifoerrors":0, "crcerrors":0, "rxinterrupts":6, "lastrxlen":54, "rssi":58, "txinterrupts":640, "spuriousints":0, "txerrors":0, "txpackets":640, "txfifoerr":0, "txchipstat":15 } }
### neighbors
> { "neighbors": [ { "index":0, "validated":1, "timestamp":1736418, "mac":"00:19:59:ff:fe:0f:ff:02"} ] }
## maccap 01|00
Enables capture if set to 01, or disables capture if set to 00.  When enabled, sends received packets to capture file.
## pansize xx
Needs explanatory text.
## routecost xx
Needs explanatory text.
## useparbs xx
Needs explanatory text.
## rank xx
Needs explanatory text.
## macsec xx
Needs explanatory text.
### quit
