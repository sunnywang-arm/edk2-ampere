#!/bin/bash
OPATH=$PATH
. edk2.sh
make -f $WORKSPACE/edk2-ampere-tools/Makefile \
    BOARD_SETTING=$WORKSPACE/srp-ac01-fw-aptv-bin-r1.08.20210830/altra_firmware_sdk/bin/board_settings/jade_board_setting_1.08.20210825.bin \
    VM_SHARED_DIR=$HOME/AmpereR \
    PACKAGES_PATH=$WORKSPACE/adlink:"${PACKAGES_PATH}" \
    ATF_SLIM=$WORKSPACE/srp-ac01-fw-aptv-bin-r1.08.20210830/altra_firmware_sdk/bin/atf/altra_atf_signed_1.08.20210825.slim \
    SCP_SLIM=$WORKSPACE/srp-ac01-fw-aptv-bin-r1.08.20210830/altra_firmware_sdk/bin/scp/altra_scp_signed_1.08.20210825.slim \
    LINUXBOOT_BIN=$WORKSPACE/flashkernel \
    VER=1.07 BUILD=300 \
    tianocore_img # linuxboot_img # tianocore_capsule # clean # all # 
export PATH=$OPATH
