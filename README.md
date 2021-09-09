# Initilize Buiding Environment

Ampere Mountain Jade codebase installation & building below procedure was tested with OS ubuntu 20.04 and CnetOS 8.2 (8.3 is not working yet)
1. copy 'setup.sh' to local machine and run. 
2. follow the lead to install tools and download source code.

# Folders
* .vscode: Visual Studio Code settings for tasks which link to bash scripts.
* adlink-platforms: source of ADLink.
* edk2: submodule from Ampere.
* edk2-ampere-tools: modified base on Ampere's.
* edk2-platforms: submodule from Ampere.
* srp-ac01-fw-aptv-bin-rx.xx.xxxxxxxx: slim & board_settings from Ampere.
  
# Scripts/Files
* buildshell.sh: build uefi shell for AA64.
* clean.sh: clean temporary files.
* edk2.sh: set environment variables and run edksetup.sh.
* flashkernel: tiny linux kernel for embedded, not working yet.
* gen_keys.sh: generate keys for signing, not applied yet.
* make_ComHpcAlt.sh: make ADLink COM-HPC-ALT.
* make_jade.sh: make CRB Ampere Mt. Jade.
* setup.sh: copy this script to $HOME after Ubuntu/CentOS installed and run it to setup codebase and tools. a optional argument to assign the tatget folder:
    * setup.sh [TARGET_FOLDER default=Ampere_Altra]
