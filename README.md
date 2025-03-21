# vdereplay

`vdereplay` aims to replay a network activity.  It reads a pcap file with
previously captured Ethernet packets and reinject them to a VDE plug, while
respecting the recorded interpacket delay.  The rate can be adjusted, or
scheduled at a fixed value.  This can be repeated several times in loop.


## Install

get the source code, from the root of the source tree run:
```
$ make
$ sudo make install
```


## Usage

`vdereplay` [*options*] -i *<input>* [-s *<speed>*] [-d *<delay>*] [-l *<loop>*] [-h] *VNL*


### Options

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


## See Also

[vde_plug](https://wiki.virtualsquare.org/#/tutorials/vdebasics),
[vdecapture](https://github.com/virtualsquare/vdecapture),
[vdetcpcapture](https://github.com/chlohr/vdetcpcapture),
[dumpcap](https://www.wireshark.org/docs/man-pages/dumpcap.html),
[randpkt](https://www.wireshark.org/docs/man-pages/randpkt.html),
[tcpreplay](http://tcpreplay.appneta.com/)
