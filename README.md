<h2>Source code for the AMHx boards</h2>
This repository contain source code for develop application based on Hydrogen SoC.<br>

Boards supported:<br>
&nbsp;&nbsp;AHM2D<br>
&nbsp;&nbsp;AHM3D<br>
&nbsp;&nbsp;AHM2DSC<br>
&nbsp;&nbsp;AHM3DSC<br>
&nbsp;&nbsp;AHM2DL<br>

Toolchain:<a href="https://github.com/pulp-platform/pulp-riscv-gnu-toolchain.git">pulp-riscv-gnu-toolchain</a><br>
Minimal build requirement is:<br>
configure --with-arch=rv32imfcxpulpv3 --with-abi=ilp32 --enable-multilib

Precompiled toolchain: <a href="https://drive.google.com/drive/folders/14DKVhOZQEVnwNfB2EFMPyWA22nspnS2o?usp=drive_link">precompiled toolchain</a><br>
The toolchain is compiled under linux and includes scripts to build under Windows by exploiting WSL. Tested with Ubuntu WSL<br>
The scripts convert the compiler call to a wsl call to Linux compiled toolchain 
<h4>Windows/WSL compile</h4>
Install a WSL (Ubuntu tested)<br>
Into project's toolchain option, set<br> 
&nbsp;&nbsp;"suffix" .bat<br>
&nbsp;&nbsp;"prefix" riscv32-unknown-elf-<br>
&nbsp;&nbsp;"Toolchain path" path-to-toolchain-bin-folder<br>


<h4>Windows OpenOCD debug</h4>
GDB program: "win_bin/riscv32-unknown-elf-gdb.exe"<br>
Into the Debugger > Commands, add "set substitute-path /mnt/c/ C:\"<br>
<br><br>




<b>Processor configuration ISA options:</b> rv32imfc_xcorev_xfhalf

<h3>Global preprocessor defines</h3>
Assembler<br>
&nbsp;&nbsp;INCLUDE_COPRBIN<br>
<br>
C preprocessor<br>
&nbsp;&nbsp;HWREV=0x00010001<br>
&nbsp;&nbsp;CORE_CV<br>

<h4>Include file list</h3>
&nbsp;&nbsp;"common"<br>
&nbsp;&nbsp;"ipc"<br>
&nbsp;&nbsp;"comunication"<br>
&nbsp;&nbsp;"comunication_SPIS"<br>
&nbsp;&nbsp;"driverUser"<br>
&nbsp;&nbsp;"dsp/inc"<br>
&nbsp;&nbsp;"hal/inc"<br>
&nbsp;&nbsp;"hw"<br>
&nbsp;&nbsp;"processing"<br>
&nbsp;&nbsp;"reconstruction"<br>
&nbsp;&nbsp;"application"<br>
&nbsp;&nbsp;"startup"<br>
&nbsp;&nbsp;"utils"<br>
<br>

<h4>Linker scripts</h4>
&nbsp;&nbsp;"ld/link_sectors.ld"<br>
&nbsp;&nbsp;"ld/link_hydr_cv32e40_wcopr.ld"<br>


<h4>Libraries</h4>
&nbsp;&nbsp;m<br>
&nbsp;&nbsp;hal_libs_cv32e40_priv_periph<br>


<h2>Debug</h2>

Openocd script provided into "openocd" folder<br>
The script is for FTDI cable as USB-JTAG converter (tested with C232HM-DDHSL-0 cable, see cable's datasheet for JTAG pin wiring)<br><br>
&nbsp;&nbsp;<b>TRST and RST pins (custom mapping)</b><br>
&nbsp;&nbsp;TRST (jtag reset): GPIOL3<br>
&nbsp;&nbsp;RST (master reset): GPIOL0<br>


<h4>Debug AHM-DK</h4>
See devkit user manual for device setup and connection

<h2>Documentation</h2>

Doxygen documentation configuration file is available inside "doc" folder
