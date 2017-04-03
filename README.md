# HermitCore - A lightweight unikernel for a scalable and predictable runtime behavior

The project [HermitCore]( http://www.hermitcore.org ) is a new
[unikernel](http://unikernel.org) targeting a scalable and predictable runtime
for high-performance and cloud computing. HermitCore extends the multi-kernel
approach (like
[McKernel](http://www-sys-aics.riken.jp/ResearchTopics/os/mckernel.html)) with
unikernel features for a better programmability and scalability for hierarchical
systems.


On the startup of HermitCore applications, cores are isolated from the Linux
system enabling bare-metal execution of on these cores. This approach achieves
lower OS jitter and a better scalability compared to full-weight kernels.
Inter-kernel communication between HermitCore applications and the Linux system
is realized by means of an IP interface.

In addition to the multi-kernel approach described above, HermitCore can be used
as a classical standalone unikernel as well. In this case, HermitCore runs a
single-kernel exclusively on the hardware or within a virtual machine. This
reduces the resource demand and loweres the boot time which is critical for
cloud computing applications. It is the result of a research project at RWTH
Aachen University and is currently an experimental approach, i.e., not
production ready. Please use it with caution.


## Requirements

The build process works currently only on **x86-based Linux** systems. To build
the HermitCore kernel and applications you need:

 * CMake
 * Netwide Assember (NASM)
 * recent host compiler such as GCC
 * HermitCore cross-toolchain, i.e. Binutils, GCC, newlib, pthreads-embedded


### HermitCore cross-toolchain

We provide prebuilt packages (currently Debian-based only) of the HermitCore
toolchain, which can be installed as follows:

```bash
$ echo "deb [trusted=yes] https://dl.bintray.com/rwth-os/hermitcore vivid main" | sudo tee -a /etc/apt/sources.list
$ sudo apt-get -qq update
$ sudo apt-get install binutils-hermit newlib-hermit pthread-embedded-hermit gcc-hermit libhermit
```

If you want to build the toolchain yourself, have a look at the following
repositories, especially at `debian/rules` in each repository:

 * [GCC](https://github.com/RWTH-OS/gcc)
 * [Binutils](https://github.com/RWTH-OS/binutils)
 * [Newlib](https://github.com/RWTH-OS/newlib)
 * [Pthread-embedded](https://github.com/RWTH-OS/pthread-embedded)

Depending on how you want to use HermitCore, you might need additional packages
such as:

 * QEMU (`apt-get install qemu-system-x86`)


## CMake requirements

We require a fairly recent version of CMake (`3.7`) which is not yet present in
most Linux distributions. We therefore provide a helper script that fetches the
required CMake binaries from the upstream project and stores them locally, so
you only need to download it once.

```bash
$ . cmake/local-cmake.sh
-- Downloading CMake
--2017-03-28 16:13:37--  https://cmake.org/files/v3.7/cmake-3.7.2-Linux-x86_64.tar.gz
Loaded CA certificate '/etc/ssl/certs/ca-certificates.crt'
Resolving cmake.org... 66.194.253.19
Connecting to cmake.org|66.194.253.19|:443... connected.
HTTP request sent, awaiting response... 200 OK
Length: 30681434 (29M) [application/x-gzip]
Saving to: ‘cmake-3.7.2-Linux-x86_64.tar.gz’

cmake-3.7.2-Linux-x86_64.tar.gz         100%[===================>]  29,26M  3,74MB/s    in 12s     

2017-03-28 16:13:50 (2,48 MB/s) - ‘cmake-3.7.2-Linux-x86_64.tar.gz’ saved [30681434/30681434]

-- Unpacking CMake
-- Local CMake v3.7.2 installed to cmake/cmake-3.7.2-Linux-x86_64
-- Next time you source this script, no download will be neccessary
```

So before you build HermitCore you have to source the `local-cmake.sh` script
everytime you open a new terminal.

	
## Building HermitCore

```bash
$ mkdir build
$ cd build
$ cmake ..
$ make
```

If your toolchain is not located in `/opt/hermit/bin` then you have to supply
its location to the `cmake` command above like so:

```bash
$ cmake -DTOOLCHAIN_BIN_DIR=/home/user/hermit/bin
```

assuming that binaries like `x86_64-hermit-gcc` and friends are located in that
directory.


## Testing

### As multi-kernel within a virtual machine

```bash
$ cd build
$ make qemu
$ # or 'make qemu-dep' to build HermitCore dependencies before
```

Within the QEMU session you can start HermitCore application just the same as
traditional Linux programs:

```bash
(QEMU) $ /hermit/extra/tests/hello
smpboot: CPU 1 is now offline
Hello World!!!
argv[0] = /hermit/extra/tests/hello
Receive signal with number 30
Hostname: hermit.localdomain
x86: Booting SMP configuration:
smpboot: Booting Node 0 Processor 1 APIC 0x1
```

Per default, the virtual machine has 10 cores, 2 NUMA nodes, and 8 GiB RAM.
Inside the VM runs a small Linux system, which already includes the patches for
HermitCore. Per NUMA node (= HermitCore isle) there is a directory called
`isleX` under `/sys/hermit` , where `X` represents the NUMA node ID.

The demo applications are located in the directories
`/hermit/extra/{tests,benchmarks}`. A HermitCore loader is already registered.
By starting a HermitCore application, a proxy will be executed on the Linux
system, while the HermitCore binary will be started on isle 0 with cpu 1. To
change the default behavior, the environment variable `HERMIT_ISLE` is used to
specify the (memory) location of the isle, while the environment variable
`HERMIT_CPUS` is used to specify the cores.

For instance, `HERMIT_ISLE=1 HERMIT_CPUS="3-5" /hermit/extra/tests/hello` starts
a HelloWorld demo on the HermitCore isle 1, which uses the cores 3 to 5. The
output messages are forwarded to the Linux proxy and printed on the Linux
system.

HermitCore's kernel messages of `isleX` are available via `cat
/sys/hermit/isleX/log`.

There is a virtual IP device for the communication between the HermitCore isles
and the Linux system (see output of `ifconfig`). Per default, the Linux system
has the IP address `192.168.28.1`. The HermitCore isles starts with the IP
address `192.168.28.2` for isle 0 and is increased by one for every isle.

More HermitCore applications are available at `/hermit/usr/{tests,benchmarks}`
which is a shared directory between the host and QEMU.


### As classical standalone unikernel within a virtual machine

HermitCore applications can be directly started as standalone kernel within a
virtual machine. In this case,
[iRCCE](http://www.lfbs.rwth-aachen.de/publications/files/iRCCE.pdf ) is not
supported.

```bash
$ cd build
$ make install DESTDIR=~/hermit-build
$ cd ~/hermit-build/opt/hermit
$ # using QEMU
$ HERMIT_ISLE=qemu bin/proxy extra/tests/hello
$ # using uHyve
$ HERMIT_ISLE=uhyve bin/proxy extra/tests/hello
```

With `HERMIT_ISLE=qemu`, the application will be started within a QEMU VM.
Please note that the loader requires QEMU and uses per default *KVM*.
Furthermore, it expects that the executable is called `qemu-system-x86_64`.

With `HERMIT_ISLE=hyve`, the application will be started within a thin
hypervisor powered by Linux's KVM API and therefore requires *KVM* support.
uHyve has a considerably smaller startup time than QEMU, but lacks some features
such as GDB debugging.

In this context, the environment variable `HERMIT_CPUS` specifies the number of
cpus (and no longer a range of core ids). Furthermore, the variable `HERMIT_MEM`
defines the memory size of the virtual machine. The suffix of *M* or *G* can be
used to specify a value in megabytes or gigabytes respectively. Per default, the
loader initializes a system with one core and 2 GiB RAM.

The virtual machine opens two TCP/IP ports. One is used for the communication
between HermitCore application and its proxy. The second port is used to create
a connection via telnet to QEMU's system monitor. With the environment variable
`HERMIT_PORT`, the default port (18766) can be changed for the communication
between the HermitCore application and its proxy. The connection to the system
monitor used automatically `HERMIT_PORT+1`, i.e., the default port is 18767.

The following command starts the stream benchmark in a virtual machine, which
has 4 cores and 6GB memory.

```bash
$ HERMIT_ISLE=qemu HERMIT_CPUS=4 HERMIT_MEM=6G bin/proxy extra/benchmarks/stream
```


### As multi-kernel on a real machine

*Note*: to launch HermitCore applications, root privileges are required.

A [modified Linux kernel](https://github.com/RWTH-OS/linux) has to be installed.
Afterwards switch to the branch `hermit` for a relative new vanilla kernel or to
`centos`, which is compatible to the current CentOS 7 kernel. Configure the
kernel with `make menuconfig` for your system. Be sure, that the option
`CONFIG_HERMIT_CORE` in `Processor type and features` is enabled. 

```bash
$ git clone https://github.com/RWTH-OS/linux
$ cd linux
$ # see comments above
$ git checkout hermit
$ make menuconfig
$ make
```

Install the Linux kernel and its initial ramdisk on your system (see
descriptions of your Linux distribution). We recommend to disable Linux NO_HZ
feature by setting the kernel parameter `nohz=off`.

Install HermitCore to your system (by default to `/opt/hermit`):

```bash
$ cd build
$ sudo make install
$ ls -l /opt/hermit
```

After a reboot of the system, register the HermitCore loader at your system with
following command:

```bash
$ sudo -c sh 'echo ":hermit:M:7:\\x42::/opt/hermit/bin/proxy:" > /proc/sys/fs/binfmt_misc/register'
```

The IP device between HermitCore and Linux currently does not support IPv6.
Consequently, disable it (might be slightly different on your distribution):

```bash
$ echo 'net.ipv6.conf.mmnif.disable_ipv6 = 1' | sudo tee /etc/sysctl.conf
```

Per default, the IP device uses a static IP address range. Linux has to use
`162.168.28.1`, where HermitCore isles start with `192.168.28.2` (isle 0). The
interface is `mmnif`.

Please configure your network accordingly. For CentOS, you have to create the
file `/etc/sysconfig/network-scripts/ifcfg-mmnif`:

```
DEVICE=mmnif
BOOTPROTO=none
ONBOOT=yes
NETWORK=192.168.28.0
NETMASK=255.255.255.0
IPADDR=192.168.28.1
NM_CONTROLLED=yes
```

You can now start applications the same way as from within a virtual machine
(see description above).


## Building your own HermitCore applications

You can take `usr/tests` as a starting point to build your own applications. All
that is required is that you include
`[...]/HermitCore/cmake/HermitCore-Application.cmake` in your application's
`CMakeLists.txt`. It doesn't have to reside inside the HermitCore repository.
Other than that, it should behave like normal CMake.


## Profiling

We provide profiling support via the XRay profiler. See `usr/xray/README.md` for
more information on how to use it.


## Debugging

If the application is started via `make qemu`, debugging via GDB is enabled by
default on port 1234. When run via proxy (`HERMIT_ISLE=qemu`), set
`HERMIT_DEBUG=1`.

```
$ gdb extra/tests/hello
(gdb) target extended-remote :1234
Remote debugging using :1234
0xffffffff8100b542 in ?? ()
```


## Tips

### Optimization

You can configure the `-mtune=name` compiler flag by adding `-DMTUNE=name` to
the `cmake` command when configuring the project.

Please note, if the applications is started within a VM, the hypervisor has to
support the specified architecture name.

If QEMU is started by our proxy and the environment variable `HERMIT_KVM` is set
to `0`, the virtual machine will be not accelerated by KVM. In this case, the
`-mtune` flag should be avoided.

### TCP connections

With the environment variable `HERMIT_APP_PORT`, an additional port can be open
to establish an TCP/IP connection with your application.

### Dumping the kernel log

By setting the environment variable `HERMIT_VERBOSE` to `1`, the proxy prints at
termination the kernel log messages onto the screen.

### Network tracing

By setting the environment variable `HERMIT_CAPTURE_NET` to `1` and
`HERMIT_ISLE` to `qemu`, QEMU captures the network traffic and creates the trace
file *qemu-vlan0.pcap*. For instance with [Wireshark](https://www.wireshark.org)
you are able to analyze the file.

### Monitor

If `HERMIT_MONITOR` is set to `1` and `HERMIT_ISLE` to `qemu`, QEMU establishes
a monitor which is available via telnet at port 18767.
