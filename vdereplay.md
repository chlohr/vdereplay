% VDETCPCAPTURE(1)

# NAME

vdereplay -- Inject packets from a pcap file to a VDE plug


# SYNOPSIS

`vdereplay` [*options*] -i *<input>* [-s *<speed>*] [-d *<delay>*] [-l *<loop>*] [-h] *VNL*


# DESCRIPTION

`vdereplay` aims to replay a network activity.  It reads a pcap file with
previously captured Ethernet packets and reinject them to a VDE plug, while
respecting the recorded interpacket delay.  The rate can be adjusted, or
scheduled at a fixed value.  This can be repeated several times in loop.


# OPTIONS

  `-i` *input*
: Pcap file with Ethernet packets

  `-s` *speed*
: Speed factor for interpacket delay (default 1.0)

  `-d` *delay*
: Fixed interpacket delay in microseconds

  `-l` *loop*
: Number of time to replay the pcap file (default 1; 0 for non-stop)

  `-h`
: Display a short help message and exit

  `VNL`
: Virtual Network Locator (e.g. vde:///tmp/vde.ctl)


# SEE ALSO
vde_plug(1), vdecapture(1), vdetcpcapture(1), dumpcap(1), randpkt(1), tcpreplay(1)


# AUTHOR
C.Lohr
