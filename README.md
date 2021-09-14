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
