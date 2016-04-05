# HermitCore - A lightweight extreme-scale satellite kernel

The project [HermitCore](http://www.hermitcore.org) is new [unikernel](http://unikernel.org) targeting high-performance computing.
HermitCore extends on the multi-kernel approach with unikernel features to provide better programmability and scalability for hierarchical systems.
By starting HermitCore applications, cores will be split off from the Linux system and the applications run bare-metal on these cores.
This approach achieves a lower OS jitter and a better scalability.
HermitCore applications and the Linux system can communicate via an IP interface (e.g. inter-kernel communication).

HermitCore is the result of a research project at RWTH Aachen University and is currently an experimental approach, i.e.  not production ready. Please use it carefully.

## Requirements

* Netwide Assembler (NASM)
* GNU Make, GNU Binutils
* Tools and libraries to build *linux*, *binutils* and *gcc* (e.g. flex, bison, MPFR library, GMP library, MPC library, ISL library)
* texinfo
* Qemu

## Building and testing HermitCore within a virtual machine

0. Please make sure that you cloned this repository and all its submodules.
1. The build process works currently only on x86-based Linux systems.
2. To configure the system, run the *configure* script in the directory, which contains this *README*. Fine tuning of the installation directories e.g. with the flag `--prefix` is currently not supported. HermitCore, the cross-compiler and the demo applications will be installed in subdirectories of this repository.
3. The command `make` build the Linux kernel, the HermitCore kernel, the corss-compiler and the demo applications.
4. To start a virtual machine and to boot a small Linux version use the command `make qemu`. Per default, the virtual machine has 10 cores, 2 NUMA nodes and 8 GByte RAM. To increase or to decrease the machine size, the label `qemu` in the Makefile has to be modified.
5. Inside the VM runs a small Linux system, which already includes the patches for HermitCore. For each NUMA node (= HermitCore isle) is a directory called `isleX` in `/sys/hermit` , where `X` represents the number of the NUMA node. The demo applications are located in the directories `/hermit/usr/{tests,benchmarks}`. A HermitCore loader is already registered. By starting a HermitCore application, a proxy will be started on the Linux system, while the HermitCore binary will be started on isle 0 with cpu 1. To change the default behavior, the environment variable `HERMIT_ISLE` is used to specify the (memory) location of the isle, while the environment variable `HERMIT_CPUS` is used to specify the cores. For instance, `HERMIT_ISLE=1 HERMIT_CPUS="3-5" /hermit/usr/tests/hello` starts a HelloWorld demo on the HermitCore isle 1, which used the cores 3 to 5. The output messages are forwarded to the Linux proxy and printed on the Linux system.
6. HermitCore's kernel messages of `isleX` are available via `cat /sys/hermit/isleX/log`, where `X` represents the isle.
7. It exists an virtual IP devices between HermitCore isles and the Linux system (see output of `ifconfig`). Per default, the Linux system has the IP address `192.168.28.1`. The HermitCore isles starts with the IP address `192.168.28.2` for isle 0 and is increased by one for every isle.
8. More HermitCore applications are available at `/hermit/usr/{tests,benchmarks}` which is a shared directory between the host and QEmu.

## Building and testing HermitCore on a real machine

1. In principle you have to follow the tutorial above. After the configuration (step 2 in the above tutorial) go to the subdirectory `linux`, which contains the source code of the Linux kernel. Configure the kernel with `make menuconfig` for your system. Be sure, that the option `CONFIG_HERMIT_CORE` in `Processor type and features` is enabled.
2. Go back to the root directory of this repository and build with `make` the Linux kernel, the HermitCore kernel, the cross-compiler and the demo applications.
3. Install the Linux kernel and its initial ramdisk on your system (see descriptions of your Linux distribution).
4. Register the HermitCore loader to your system with following command: `echo ":hermit:M:7:\\x42:/path2proyxy/proxy:" > /proc/sys/fs/binfmt_misc/register`, in which `path2proxy` defines the path to the loader. You find the loader `proxy` after building the HermiCore sources in the subdirectory `hermit/tools` of the directory, which contains this *README*.
5. The IP device between HermitCore and Linux does currently not support IPv6. Consequently, disable IPv6 by adding following line to `/etc/sysctl.conf`: `net.ipv6.conf.eth0.disable_ipv6 = 1`.
6. Per default, the IP device uses a static IP address range. Linux has to use `162.168.28.1`, where HermitCore isles start with `192.168.28.2` (isle 0). The network manager must be configured accordingly and consequently the file `/etc/sysconfig/network-scripts/ifcfg-mmnif` must be created with following contents:
```
DEVICE=mmnif
BOOTPROTO=none
ONBOOT=yes
NETWORK=192.168.28.0
NETMASK=255.255.255.0
IPADDR=192.168.28.1
NM_CONTROLLED=yes
```
Finally, boot your system with the new Linux kernel and follow the above tutorial (*Building and testing HermitCore within a virtual machine*) from point 5.

The demo applications are stored in their subdirectories `hermit/usr/{tests,benchmarks}` of the directory, which contains this *README*.
