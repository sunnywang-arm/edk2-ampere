#!/bin/bash
OPATH=$PATH
. edk2.sh
OEM_SRC_DIR=$WORKSPACE/adlink-platforms
BOARD_NAME=ComHpcAlt
EDK2_PLATFORMS_PKG_DIR=$OEM_SRC_DIR/Platform/Ampere/"$BOARD_NAME"Pkg
make -f $WORKSPACE/edk2-ampere-tools/Makefile \
    EDK2_PLATFORMS_PKG_DIR=$EDK2_PLATFORMS_PKG_DIR \
    BOARD_NAME=$BOARD_NAME \
    VM_SHARED_DIR=$HOME/AmpereR \
    CHECKSUM_TOOL=$OEM_SRC_DIR/toolchain/checksum \
    PACKAGES_PATH=$OEM_SRC_DIR:$WORKSPACE/OpenPlatformPkg:"${PACKAGES_PATH}" \
    ATF_SLIM=$WORKSPACE/srp-ac01-fw-aptv-bin-r1.08.20210830/altra_firmware_sdk/bin/atf/altra_atf_signed_1.08.20210825.slim \
    SCP_SLIM=$WORKSPACE/srp-ac01-fw-aptv-bin-r1.08.20210830/altra_firmware_sdk/bin/scp/altra_scp_signed_1.08.20210825.slim \
    LINUXBOOT_BIN=$WORKSPACE/flashkernel \
    SPI_SIZE_MB=32 \
    VER=1.07 BUILD=300 \
    tianocore_capsule # linuxboot_img # all # 
# below setting is for module not reworked fo VR circuit, insert them to above making arguments 
# to support the modules without reworking.    
#    FAILSAFE_WORKAROUND=1 \
#    BOARD_SETTING=$OEM_SRC_DIR/Platform/Ampere/ComHpcAltPkg/ComHpcAltBoardSettingVRWA.cfg \
export PATH=$OPATH
