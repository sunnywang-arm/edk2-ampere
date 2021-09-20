# Initialize Building Environment

Ampere Mountain Jade codebase & tools installation.
* Copy 'setup.sh' to local machine and run, tested with OS Ubuntu 20.04. 
* follow the instructions to install tools and download source code.

# Folders
* .vscode: Visual Studio Code settings for tasks which link to bash scripts.
* adlink-platforms: source of ADLink.
* edk2: submodule from Ampere.
* edk2-ampere-tools: submodule from Ampere's fork.
* edk2-platforms: submodule from Ampere's fork.
* OpenPlatformPkg: submodule from Linaro.
* srp-ac01-fw-aptv-bin-rx.xx.xxxxxxxx: slim & board_settings from Ampere.
  
# Scripts/Files
* buildshell.sh: build uefi shell for AA64.
* clean.sh: clean temporary files and tools.
* edk2.sh: Sample script to set environment variables and run edksetup.sh.
* gen_keys.sh: Sample script to generate keys for signing, not applied yet.
* make_ComHpcAlt.sh: Sample script to make ADLink COM-HPC-ALT.
* make_jade.sh: Sample script to make CRB Ampere Mt. Jade.
* setup.sh: Sample script to install all source code and tools. Mind your own SSH connection setting if there is any.
    * setup.sh [TARGET_FOLDER, default=Ampere_Altra]

# Building EDK-II image

1. Enter into edk2-ampere directory.

2. Execute the below command to build EDK-II image for COM-HPC-ALT

   ```
   .make_ComHpcAlt.sh
   ```

3. After successful compilation, the final EDK-II image **ComHpcAlt_tianocore_atf_1.07.300.cap** will be found in below path

   ```
   $PWD/BUILDS/ComHpcAlt_tianocore_atf_1.07.300/
   ```

# Flashing SCP/EDK-II

1. Copy **CapsuleApp.efi** , **ComHpcAlt_scp_1.07.300.cap** , **ComHpcAlt_tianocore_atf_1.07.300.cap** files from path *$PWD/BUILDS/ComHpcAlt_tianocore_atf_1.07.300/* into FAT32 USB and connect to target board.

2. Power up the target and boot into UEFI shell. 

3. Run **map -r** command to identify the USB device name. Type the USB device name in the shell to enter into to the USB drive.

   For example: fs0 is USB device name in our case.

   ```
   Shell>fs0:
   fs0:\>
   ```

4. Run below command to flash SCP FW 1.08

   ```
   CapsuleApp.efi ComHpcAlt_scp_1.07.300.cap
   ```

5. Run below command to flash EDK-II FW v1.07.300 (this includes ATF + UEFI + BoardSettings).

   ```
   CapsuleApp.efi ComHpcAlt_tianocore_atf_1.07.300.cap
   ```

6. Power cycle the target board.

NOTE:

- The target board by default should have EDK-II firmware version greater than or equal to 1.07.299 (which will support capsule update). 

### Flashing Capsule Supported EDK-II Firmware

1. Click [here](https://hq0epm0west0us0storage.blob.core.windows.net/development/ampere/Capsule_Supported_EDK-II_img.zip)  to download the package for capsule supported EDK-II firmware.
2. Unzip into FAT32 USB device.
3. Boot into UEFI shell
4. Run fwu.nsh to flash EDK-II to 1.07.299 which will support capsule update.
5. Power cycle the target board.

NOTE: 

- The above steps required only when the target is having EDK-II firmware version lesser than 1.07.299
