# Universal, Multi-protocol Digital Voice Reflector

The URF Multiprotocol Gateway Reflector Server, ***urfd***, is part of the software system for a Digital Voice Network. The sources are published under GPL Licenses.

# Information about this fork
This fork of URFD supports *all* modes currently used in ham radio: D-Star, DMR, YSF, P25, NXDN, M17, and USRP (for connections to AllStar nodes, etc).  All transcoding is centralized so there is no double transcoding to/from any mode. This fork of urfd, along with the swambe2 branch of my tcd repo, contains many changes from the original:

Integraded P25 Reflector with software imbe vocoder.

Integrated NXDN Reflector

Inegrated USRP Reflector with a single client currently defined in Main.h  This is currently used by me to connect to an AllStar node to provide direct AllStar transcoding in and out of all modes.

Software vocoding of AMBE+2(DMR/YSF/NXDN) is done using md380_vocoder library.  This means that only 1 USB dv dongle is required per module.  This also makes an ARM platform (like Rpi) a reqirement.

Numerous fixes like late entry recognition from modes like YSF that are otherwise ignored by the original reflector when no header has been received.

The rest of this README is unchanged from the original.

## Introduction

This will build a new kind of digital voice reflector. Based on N7TAE's [new-xlxd](https://github.com/n7tae/new-xlxd), which, in turn, is based on the first multi-protocol reflector, [xlxd](https://github.com/LX3JL/xlxd), **urfd** supports all protocols of it's predecessors, as well as both M17 protocols, **voice-only** and **voice+data**! A key part of this is the hybrid transcoder, [tcd](https://github.com/n7tae/tcd), which is in a seperate repository. URFd is not compatible with either new-xlxd or xlxd. You can't interlink urfd with xlxd. This reflector can be built without a transcoder, but clients will only hear other clients using the same protocol. Please note that currently, urfd only supports the tcd transcoder when run locally. For best performance, urfd and tcd uses UNIX DGRAM sockets for interprocess communications. These kernal-base sockets are signifantly faster than conventional UDP/IP sockets. It should be noted that tcd supports DVSI-3003 nad DVSI-3000 devices, which it uses for AMBE vocoding.

This build support *dual-stack* operation, so the server on which it's running, must have both an IPv4 and IPv6 routable address if you are going to configure a dual-stack reflector. URF can support out-going DExtra links, by adding a new DExtra Peer type *and* it has many changes designed to increase reliability and stability.

There are many improvements of urfd over xlxd, some of which were inherited from new-xlxd:

- Nearly all std::vector containers have been replaced with more appropriate containers.
- No classes are derived from any standard containers.
- For concurancy, *i.e.*, thread management, the standard thread (std::thread) library calls have been replaced with std::future.
- Managed memory, std::unique_ptr and std::shared_ptr, is used replacing the need for calls to *new* and *delete*.
- Your reflector can be configured with up to 26 modules, *A* through *Z* and as few as one module. For other choices, the configure modules don't have to be contigious. For example, you could configure modules A, B, C and E.

Only systemd-based operating systems are supported. Debian or Ubuntu is recommended. If you want to install this on a non-systemd based OS, you are on your own. Also, by default, tcd and urfd are built without gdb support. You can add debugging support in the configuration script, `./rconfig`. Finally, this repository is designed so that you don't have to modify any file in the repository when you build your system. Any file you need to modify to properly configure your reflector will be a file you copy from you locally cloned repo. This makes it easier to update the source code when this repository is updated. Follow the instructions below to build your transcoding URF reflector.

## Usage

The packages which are described in this document are designed to install server software which is used for the D-Star network infrastructure. It requires a 24/7 internet connection which can support up to three transcoded modules and up to 23 more untranscoded modules to connect repeaters and hot-spot dongles!

- The server can build a reflector that support IPv4, IPv6 or both (dual stack).
- The public IP addresses should have a DNS record which must be published in the common host files.

## Installation

Below are instructions to build a URF reflector. If you are planning on an URF reflector without a transcoder, you can help your users by naming modules with names that suggest which protocol is welcome. You name modules in the config.inc.php file mentioned below.

The transcoder is in a seperate repository, but you will build, install and monitor the transcoder and reflector from two different scripts, *rconfig* and *radmin* in this repository. You *should* look over the README.md file in the tcd repository to understand the transcoder.

### After a clean installation of Debian make sure to run update and upgrade

```bash
sudo apt update
sudo apt upgrade
```

### Required packages (some of these will probably already be installed)

```bash
sudo apt install git
sudo apt install apache2 php5
sudo apt install build-essential
sudo apt install libmariadb-dev-compat
```

### YSF direct connection support

The following is needed if you plan on supporting local YSF frequency registration database for those YSF-clients that want to directly connect to URF. You will also need to install the client frequency registration pages on your web server. This is because the WiresX protocol supplies the operational frequency to connecting clients.

```bash
sudo apt install php-mysql mariadb-server mariadb-client
```

### Download the repository(s)

```bash
git clone https://github.com/n7tae/urfd.git
```

And, if needed, the hybrid transcoder:

```bash
git clone https://github.com/n7tae/tcd.git
```

### Create and edit your blacklist, whitelist and linking files

First, move to the reflector build directory:

```bash
cd urfd/reflector
```

The blacklist file defines callsigns that are blocked from linking or transmitting. The whitelist file defines callsigns that are allowed to link and transmit. Both of these files support the astrisk as a wild-card. The supplied blacklist file is empty and the supplied whitelist file contains a single definition, \*, which will allow any callsign to link and transmit, blocking no one. The interlink file defines possible Brandmeister, XRF and URF linking. The terminal file defines operations for Icom's Terminal and Access Point mode, sometimes called *G3*. This protocol requires significantly higher connection resources than any other mode, so it is possible to build a URF reflector without G3 support.

```bash
cp ../config/urfd.blacklist .
cp ../config/urfd.whitelist .
cp ../config/urfd.interlink .
cp ../config/urfd.terminal .
```

If you are not going to support G3 linking, you don't need to copy the .terminal file. Use your favorite editor to modify each of these files. If you want a totally open network, the blacklist and whitelist files are ready to go. The blacklist determine which callsigns can't use the reflector. The whitelist determines which callsigns can use the reflector. The interlink file sets up the URF<--->URF inter-linking and/or out-going XRF peer linking.

When you are done with the configuration files and ready to start the installation process, you can return to the main repository directory:

```bash
cd ..
```

### Configuring your reflector

Configuring, compiling and maintaining your reflector build is easy! Start the configuration script in the base directory of you cloned repo:

```bash
./rconfig
```

There are only a few things that need to be specified. Most important are, the reflector callsign and the IP addresses for the IPv4 and IPv6 listen ports and a transcoder port, if there is a transcoder. Dual-stack operation is enabled by specifying both an IPv4 and IPv6 address. IPv4-only single stack can be specified by leaving the IPv6 address set to `none`. It's even possible to operate in an IPv6-only configuration by leaving the IPv4 address to the default `none`. For most users, you can define the IP addresses as "any", but you can specify specific IPv4 and IPv6 addresses, if this is required for you installation site.

You can configure any modules, from **A** to **Z**. They don't have to be contigious. If your reflector is configured with a transcoder, you can specify which configured modules will be transcoded. Up to three modules can be transcoded. There are also true/false flags to prevent G3 support and so that you can build executables that will support gdb debugging.

You can support your own YSF frequency database. This is very useful for hot-spots that use YSF linking. These linked hot-spots can then use the *WiresX* command on their radios to be able to connect to any configured URF module. Users can register their TX and RX frequency (typically the same for most hot-spot configurations) on http:<*urf url*>/wiresx/login.php. Once their hot-spot is registered, URF will return the correct frequency for their hot-spot when a *WiresX* command is sent to the reflector. You'll need to enable YSF auto-linking, specify a default module and define a database name, user and user password. When you write you URF configuration, a database **configure.sql** script will be built to not only create the database and database user, but also the table for the hot-spot frequency data.

Be sure to write out the configuration files and look over the up to seven different configration files that are created. The first file, reflector.cfg is the memory file for rconfig so that if you start that script again, it will remember how you left things. There are one or two `.h` files for the reflector and tcd and there are one or two `.mk` files for the reflector and tcd makefiles. You should **not** modify these files by hand unless you really know exactly how they work. The rconfig script will not start if it detects that an URF server is already running. You can override this behavior in expert mode: `./rconfig expert`. If you do change the configuration after you have already compiled the code, it is safest if you clean the repo and then recompile.

### Compling and installing your system

After you have written your configutation files, you can build and install your system:

```bash
./radmin
```

Use this command to compile and install your system. It can also be used to uninstall your system. It will use the information in reflector.cfg to perform each task. This radmin menu can also perform other tasks like restarting the reflector or transcoder process. It can also be used to update the software, if the system is uninstalled.

### Stoping and starting the services manually

```bash
sudo systemctl stop urfd # (or xrfd)
sudo systemctl stop tcd
```

You can start each component by replacing `stop` with `start`, or you can restart each by using `restart`.

### Copy dashboard to /var/www

Since URF is a superset of XLX, we can still take advantage of the existing XLX infrastructure. In fact, the xml file generated by urfd reports itself as an XLX reflector. This will change at some point in time.

```bash
sudo cp -r ~/urfd/dashboard /var/www/urf     # or whatever your html server uses
```

Please note that your www root directory might be some place else. There is one file that needs configuration. Edit the copied files, not the ones from the repository:

- **pgs/config.inc.php** - At a minimum set your email address, country and comment.

**DO NOT** enable the "calling home" feature unless you are sure that you will not be infringing on an existing XLX or XRF reflector with the same callsign suffix. If you don't understand what this means, don't set `$CallingHome['Active']` to true!

If you have configured support of hot-spot frequency registation, recursively copy the **wiresx** directory where the index.php file is for your dashboard. Also from the build directory, create the database and database user and hot-spot frequency table:

```bash
sudo mysql < configure.sql
```

The configure.sql file will be generated automatically by the rconfig script **if** you have enabled the **YSF Local Database**.

## Updating urfd and tcd

Updating can be performed entirely in the radmin script, but just in case there is a new version of the radmin script, you can start first with a simple `git pull`. If any .h or .cpp fiiles have updates, you can then start radmin and do a clean and compile and then uninstall and install: `cl, co, us, is`. Follow that with a `rl` to watch the reflector log, or an `rt` to watch the transcoder while it comes up.

If rconfig was updated with the `git pull`, it might be wise to run it first to see if there have been any new options added to the code base. If so, be sure to write out the new configuration files before exiting rconfig. THen you can rebuild and reinstall your reflector.

If you change any configuration after your reflector has been compiled, be sure to do a clean/compile/uninstall/reinstall to sync your system to the new configuration.

## Firewall settings

URF Server requires the following ports to be open and forwarded properly for in- and outgoing network traffic:

```text
TCP port    80         (http) optional TCP port 443 (https)
UDP port 10002         (BM connection)
UDP port 10017         (URF interlinking)
UDP port 42000         (YSF protocol)
UDP port 17000         (M17 protocol)
UDP port 30001         (DExtra protocol)
UPD port 20001         (DPlus protocol)
UDP port 30051         (DCS protocol)
UDP port  8880         (DMR+ DMO mode)
UDP port 62030         (MMDVM protocol)
UDP port 12345 - 12346 (Icom Terminal presence and request port)
UDP port 40000         (Icom Terminal dv port)
```

## YSF Master Server

Pay attention, the URF Server acts as an YSF Master, which provides 26 wires-x rooms.
It has nothing to do with the regular YSFReflector network, hence you don’t need to register your URF at ysfreflector.de !

## To-dos

I will eventually support a remote transcoder option, so that you can, for example, run urfd in a data center, and then run the transcoder somewhere you have physical access to it so you can plug in your AMBE vocoders. I don't recommend this as it will add unnessary and variable latency to your reflector.

The M17 team will be working on big changes for the dashboard. I can't wait to see what they come up with!

## Copyright

- Copyright © 2016 Jean-Luc Deltombe LX3JL and Luc Engelmann LX1IQ
- Copyright © 2021 Thomas A. Early N7TAE
