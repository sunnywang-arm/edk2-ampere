# Initilize Buiding Environment

Ampere Mountain Jade codebase installation & building below procedure was tested with OS ubuntu 20.04 and CnetOS 8.2 (8.3 is not working yet)
1. copy 'setup.sh' and replace the comment string "your_comment" for the SSH key if you want brfore running the script.
2. follow its lead to install 'git' and establish SSH connection to gitlab.
 * "copy" the SSH key generated.
 * firefox will lead you to log in GitLab.
 * get into your "Settings"->"SSH keys" to add this key via "paste" it.
 * close the browser and follow the lead to install source code and toolchain.
 * reply "yes" for the connection authentication question followed.

# Folders
* adlink: source of ADLink.
* edk2: submodule from Ampere
* edk2-ampere-tools: modified base onf Ampere's
* edk2-platforms: submodule from Ampere
* srp-ac01-fw-aptv-bin-rx.xx.xxxxxxxx: slim & board_settings from Ampere
  
# Scripts/Files
* buildshell.sh: build uefi shell for AA64.
* clean.sh: clean temporary files.
* edk2.sh: set environment variables and run edksetup.sh.
* flashkernel: tiny linux kernel for embedded.
* gen_keys.sh: generate SSH keys.
* make_ComHpcAlt.sh: make ADLink COM-HPC-ALT.
* make_jade.sh: make CRB Ampere Mt. Jade.
* setup.sh: copy this script to $HOME after Ubuntu/CentOS installed and run it to setup codebase and tools. a optional argument to assign the tatget folder:
    * setup.sh [TARGET_FOLDER default=Ampere_Altra]