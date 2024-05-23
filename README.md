# Universal, Multi-protocol Digital Voice Reflector

The URF Multi-protocol Gateway Reflector Server, ***urfd***, is part of the software system for a Digital Voice Network. The sources are published under GPL Licenses.

## Introduction

This will build a new kind of digital voice reflector. A *urfd* supports DStar protocols (DPlus, DCS, DExtra and G3) DMR protocols (MMDVMHost, DMR+ and NXDN), M17, YSF, P25 (using IMBE) and USRP (Allstar). A key part of this is the hybrid transcoder, [tcd](https://github.com/n7tae/tcd), which is in a separate repository. You can't interlink urfd with xlxd. This reflector can be built without a transcoder, but clients will only hear other clients using the same codec. This version of urfd/tcd now supports ***remote transcoding***. Communication between urfd and tcd is now handled *via* TCP, with *urfd* acting as the server and *tcd* is the client. In this way, it shouldn't be necessary to open any ports on the tcd system. A two-way TCP channel will be opened for each transcoded module. It should be noted that tcd supports DVSI-3003 nad DVSI-3000 devices, which it uses for AMBE vocoding.

This build support *dual-stack* operation, so the server on which it's running, must have both an IPv4 and IPv6 routable address if you are going to configure a dual-stack reflector.

There are many improvements previous multi-mode reflectors:


- Nearly all std::vector containers have been replaced with more appropriate containers.
- No classes are derived from any standard containers.
- For concurrency, *i.e.*, thread management, the standard thread (std::thread) library calls have been replaced with std::future.
- Managed memory, std::unique_ptr and std::shared_ptr, is used replacing the need for calls to *new* and *delete*.
- Your reflector can be configured with up to 26 modules, *A* through *Z* and as few as one module. For other choices, the configure modules don't have to be contiguous. For example, you could configure modules A, B, C and E.
- An integrated P25 Reflector with software imbe vocoder.
- An integrated NXDN Reflector
- An integrated USRP Reflector

Only systemd-based operating systems are supported. Debian or Ubuntu is recommended. If you want to install this on a non-systemd based OS, you are on your own. Finally, this repository is designed so that you don't have to modify any file in the repository when you build your system. Any file you need to modify to properly configure your reflector will be a file you copy from you locally cloned repo. This makes it easier to update the source code when this repository is updated. Follow the instructions below to build your transcoding URF reflector.

## Usage

The packages which are described in this document are designed to install server software which is used for the D-Star network infrastructure. It requires a 24/7 internet connection which can support up to three transcoded modules and up to 23 more untranscoded modules to connect repeaters and hot-spot dongles.

- The server can build a reflector that support IPv4, IPv6 or both (dual stack).
- The public IP addresses should have a DNS record which must be published in the common host files.

## Installation

Below are instructions to build a URF reflector. If you are planning on an URF reflector without a transcoder, you can help your users by naming modules with names that suggest which protocol is welcome. You name modules in the config.inc.php file mentioned below.

For local transcoding reflectors only: The transcoder is in a separate repository, but you can install and monitor the transcoder and reflector from a script, *radmin* in this repository. You *should* look over the README.md file in the tcd repository to understand the transcoder.

### After a clean installation of Debian make sure to run update and upgrade

```bash
sudo apt update
sudo apt upgrade
```

### Required packages (some of these may already be installed)

```bash
sudo apt install git apache2 php build-essential nlohmann-json3-dev libcurl4-gnutls-dev
```

On the latest systems. *e.g.*, Ubuntu 24, Debian 12, you can install the OpenDHT support without having to build it.

```bash
sudo apt install libopendht-dev
```

If the above command reports that this package is using something earlier that C++17, don't install it. You'll need to build it instead (described in the next section).

### Ham-DHT support (optional, but highly recommended)

**Ham-DHT**, a DHT network for hams, is implemented using a distributed hash table provided by OpenDHT.

OpenDHT is available [here](https://github./com/savoirfairelinux/opendht.git). Building and installing instructions are in the [OpenDHT Wiki](https://github.com/savoirfairelinux/opendht/wiki/Build-the-library). Pascal support and proxy-server support (RESTinio) is not required for urfd and so can be considered optional. With this in mind, this should work on Debian/Ubuntu-based systems:

```bash
# Install OpenDHT dependencies
sudo apt install libncurses5-dev libreadline-dev nettle-dev libgnutls28-dev libargon2-0-dev libmsgpack-dev  libssl-dev libfmt-dev libjsoncpp-dev libhttp-parser-dev libasio-dev cmake pkg-config libcppunit-dev

# clone the repo
git clone https://github.com/savoirfairelinux/opendht.git

# build and install
cd opendht
mkdir build && cd build
cmake -DOPENDHT_PYTHON=OFF -DCMAKE_INSTALL_PREFIX=/usr ..
make
sudo make install
```

### Download and build the repository and

```bash
git clone https://github.com/n7tae/urfd.git
cd urfd/reflector
```

### Create and edit your configuration files

First, move to the reflector build directory and create your configuration file:

```bash
cp ../config/* .
```

This will create seven files:
1. The `urfd.mk` file contains compile-time options for *urfd*. If you change the `BINDIR`, you'll need to update how `urfd.service` starts *urfd*.
2. The `urfd.ini` file contains the run-time options for *urfd* and will be discussed below.
3. The `urfd.blacklist` file defines callsigns that are blocked from linking or transmitting.
4. The `urfd.whitelist` file defines callsigns that are allowed to link and transmit. Both of these files support the asterisk as a wild-card. The supplied blacklist and whitelist file are empty, which will allow any callsign to link and transmit, blocking no one. Both files support a limited wildcard feature.
5. The `urfd.interlink` file defines possible Brandmeister and URF linking.
6. The `urfd.terminal` file defines operations for Icom's Terminal and Access Point mode, sometimes called *G3*. This protocol requires significantly higher connection resources than any other mode, so it is possible to build a URF reflector without G3 support.
7. The `urfd.service` file is a systemd file that will start and stop *urfd*. Importantly, it contains the only reference to where the *urfd* ini file is located. Be sure to set a fully qualified path to your urfd.ini file on the `ExecStart` line.

You can actually put the blacklist, whitelist, interlink, terminal and ini file anyplace and even rename them. Just make sure your ini file and service file have the proper, fully-qualified paths. The service file and the mk file need to remain in your `urfd/reflector` directory.


When you are done with the configuration files and ready to start the installation process, you can return to the main repository directory:

```bash
cd ..
```

### Build *urfd*

After possibly editing `urfd.mk`, you can build your reflector: `make` . Besides building *urfd*, this will also build two helper applications that will be discussed below.

### Configuring your reflector

Use your favorite text editor to set your run-time configuration in your copy of `urfd.ini`.

There are only a few things that need to be specified. Most important are, the reflector callsign and the IP addresses for the IPv4 and IPv6 listen ports and a transcoder port, if there is a transcoder. If you don't have a transcoder, be sure to set the transcoding port to zero. If you have a local transcoder, set the binding address to `127.0.0.1` or `::1`. If your transcoder is remote, you will usually set the binding address to `0.0.0.0` or `::`. Dual-stack operation is enabled by specifying both an IPv4 and IPv6 address. IPv4-only single stack can be specified by leaving the IPv6 address undefined.

You can configure any modules, from **A** to **Z**. They don't have to be contiguous. If your reflector is configured with a transcoder, you can specify which configured modules will be transcoded. Up to three modules can be transcoded if you have the necessary hardware.

Three protocols, BrandMeister, G3 and USRP should be disabled if you aren't going to use them.

There are three databases needed by *urfd*:
1. The *DMR ID* database maps a DMR ID to a callsign and *vis versa*.
2. The *NXDN ID* database maps an NXDN ID to a callsign and *vis versa*.
3. The *YSF Tx/Rx* database maps a callsign to a transmit/receive RF frequencies.
These databases can come from a URL or a file, or both. If you specify "both", then the file will be read after the URL.

#### Special *USRP* configuration

If configured, a *USRP* client is very unique. A *USRP* client (an AllStar node) doesn't support any connect or disconnect protocol. The USRP client simply sends and receives *USRP* voice packets. That means, if *USRP* is enabled, the client is created during initialization using the configured callsign, IP address and Tx/Rx ports.

If `FilePath` is defined, this should point to a text file listing special, listen-only client(s), one per line. Each line defining a read-only client contains an IP address, a port number, and a callsign. Here is an example:

```bash
1.2.3.1;34001;ALLSTR1;
1.2.3.4;34004;ALLSTR4;
```

If you want to create listen-only clients, but you don't need a configured read/write client, then set its `Callsign` to `NONE`.

### Helper apps

There are two, very useful helper applications, *inicheck* and *dbutil*. Both apps will show you a usage message if you execute them without any arguments.

The *inicheck* app will use the exact same code that urfd uses to validate your `urfd.ini` file. Do `./inicheck -q mrefd.ini` to check your infile for errors. If you see any messages containing `ERROR`, that means that *urfd* won't start. You'll have to fix the errors described in the message(s). If you only see messages containing `WARNING`, *urfd* will start, but it may not perform as expected. You will have to decide if the warning should be fixed. If you don't see any messages, it means that your ini file is syntactically correct.

The *dbutil* app can be used for several tasks relating to the three databases that *urfd* uses. The usage is: `./dbutil DATABASE SOURCE ACTION INIFILE`, where:
- DATABASE is "dmr", "nxdn" or "ysf"
- SOURCE is "html" or "file"
- ACTION is "parse" or "errors"
- INIFILE is the path to the infile that defines the location of the http and file sources for these three databases.
One at a time, *dbutil* can work with any of the three DATABASEs. It can read either the http or the file SOURCE. It can either show you the data entries that are syntactically correct or incorrect (ACTION).

### Installing your system with a local transcoder

After you have written your configuration files, you can install your system:

```bash
./radmin
```

You can use this interactive shell script to install and uninstall your system. This can also perform other tasks like restarting the reflector or transcoder process, or be used to view the reflector or transcoder log in real time.

### Installing your system with a remote transcoder

In the `reflector` subdirectory, use:
- `make` to compile *urfd*.
- `sudo install` to install and start *urfd*.
- `systemctl` and `journalctl` can be used to manage *urfd* and view the log.
- `sudo make uninstall` will uninstall *urfd*.

### Copy dashboard to /var/www

Since URF is a superset of XLX, we can still take advantage of the existing XLX infrastructure. In fact, the xml file generated by urfd reports itself as an XLX reflector. This will change at some point in time.

```bash
sudo cp -r ~/urfd/dashboard /var/www/urf     # or whatever your html server uses
```

Please note that your www root directory might be some place else. There is one file that needs configuration. Edit the copied files, not the ones from the repository:

- **pgs/config.inc.php** - At a minimum set your email address, country and comment.

**DO NOT** enable the "calling home" feature unless you are sure that you will not be infringing on an existing XLX or XRF reflector with the same callsign suffix. If you don't understand what this means, don't set `$CallingHome['Active']` to true!

## Firewall settings

URF Server requires the following ports to be open and forwarded properly for in- and outgoing network traffic. Obviously you don't need to open ports for G3, USRP and BrandMeister if they are not enabled:

```text
TCP port    80         (http) optional TCP port 443 (https)
UDP port  8880         (DMR+ DMO mode)
UDP port 10002         (BM connection)
UDP port 10017         (URF interlinking)
TCP port 10100         (Transcoder listening port)
UDP port 12345 - 12346 (G3 Icom Terminal presence and request port)
UDP port 17000         (M17 protocol)
UPD port 20001         (DPlus protocol)
UDP port 30001         (DExtra protocol)
UDP port 30051         (DCS protocol)
UDP port 32000         (USRP protocol)
UDP port 40000         (G3 Icom Terminal port)
UDP port 41000         (P25 port)
UDP port 41400         (NXDN port)
UDP port 42000         (YSF protocol)
UDP port 62030         (MMDVM protocol)
```

## YSF Master Server

Pay attention, the URF Server acts as an YSF Master, which provides 26 wires-x rooms.
It has nothing to do with the regular YSFReflector network, hence you don’t need to register your URF at ysfreflector.de!

## Copyright

- Copyright © 2016 Jean-Luc Deltombe LX3JL and Luc Engelmann LX1IQ
- Copyright © 2022 Doug McLain AD8DP and Thomas A. Early N7TAE
- Copyright © 2024 Thomas A. Early N7TAE
