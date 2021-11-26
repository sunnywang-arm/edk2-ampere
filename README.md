# Initialise Building Environment

Ampere Mountain Jade code base & tools installation.
* Copy 'setup.sh' to local machine and run. OS used for build environment is Ubuntu 20.04. 
* Follow the instructions to install tools and download source code.

# Folders
* .vscode: Visual Studio Code settings for tasks which link to bash scripts.
* adlink-platforms: source of ADLINK.
* edk2: submodule from Ampere.
* edk2-ampere-tools: submodule from Ampere's fork.
* edk2-platforms: submodule from Ampere's fork.
* OpenPlatformPkg: submodule from Linaro.
* srp-ac01-fw-aptv-bin-rx.xx.xxxxxxxx: slim & board_settings from Ampere.
  
# Scripts/Files
* buildshell.sh: build uefi shell for AA64.
* clean.sh: clean temporary files and tools.
* edk2.sh: Sample script to set environment variables and run edksetup.sh.
* flashkernel: tiny linux kernel for embedded.
* make_ComHpcAlt.sh: Sample script to make ADLINK COM-HPC-ALT.
* make_jade.sh: Sample script to make CRB Ampere Mt. Jade.
* setup.sh: Sample script to install all source code and tools. Mind your own SSH connection setting if there is any.
    * setup.sh [TARGET_FOLDER, default=Ampere_Altra]

# Building EDK-II image

1. Enter into edk2-ampere directory.

2. Execute the below command to build EDK-II image for COM-HPC-ALT

   ```
   .make_ComHpcAlt.sh
   ```

3. After successful compilation, the final EDK-II image **ComHpcAlt_tianocore_atf_1.07.300.02.cap** will be found in below path

   ```
   $PWD/BUILDS/ComHpcAlt_tianocore_atf_1.07.300.02/
   ```

# Flashing SCP/EDK-II

1. Copy **CapsuleApp.efi** , **ComHpcAlt_scp_1.07.300.02.cap** , **ComHpcAlt_tianocore_atf_1.07.300.02.cap** files from path *$PWD/BUILDS/ComHpcAlt_tianocore_atf_1.07.300.02/* into FAT32 USB and connect to target board.

2. Power up the target and boot into UEFI shell. 

3. Run **map -r** command to identify the USB device name. Type the USB device name in the shell to enter into to the USB drive.

   For example: fs0 is USB device name in our case.

   ```
   Shell>fs0:
   fs0:\>
   ```

4. Run below command to flash SCP FW 1.07

   ```
   CapsuleApp.efi ComHpcAlt_scp_1.07.300.02.cap
   ```

5. Run below command to flash EDK-II FW v1.07.300.02 (this includes ATF + UEFI + BoardSettings).

   ```
   CapsuleApp.efi ComHpcAlt_tianocore_atf_1.07.300.02.cap
   ```

6. Power cycle the target board.

NOTE:

- The target board by default should have EDK-II firmware version greater than or equal to 1.07.299 (which will support capsule update). 

### Flashing Capsule Supported EDK-II Firmware

1. Go to [Ask a Expert page](https://www.adlinktech.com/en/Askanexpert) or [AVA Developer Platform Forum](https://www.ipi.wiki/community/forum/ava-developer-platform) where you can request us and then we will provide the download link
2. Download & Unzip into FAT32 USB device.
3. Boot into UEFI shell
4. Run fwu.nsh to flash EDK-II to 1.07.299 which will support capsule update.
5. Power cycle the target board.

NOTE: 

- The above steps required only when the target is having EDK-II firmware version lesser than 1.07.299
