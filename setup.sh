#!/bin/bash
# 
# detect install system
# 
APT_CMD=$(which apt)
APT_GET_CMD=$(which apt-get)
DNF_CMD=$(which dnf)
YUM_CMD=$(which yum)
if [[ ! -z $DNF_CMD ]]; then
  INSTALLER=dnf
  UUID_LIB=libuuid-devel
elif [[ ! -z $YUM_CMD ]]; then
  INSTALLER=yum
  UUID_LIB=libuuid-devel
elif [[ ! -z $APT_CMD ]]; then
  INSTALLER=apt
  UUID_LIB=uuid-dev
elif [[ ! -z $APT_GET_CMD ]]; then
  INSTALLER=apt-get
  UUID_LIB=uuid-dev
else
  echo "error can't install tools"
  exit 1;
fi
echo "==========================================================================="
echo "update system before install tools"
echo "==========================================================================="
sudo $INSTALLER upgrade -y
sudo $INSTALLER autoremove -y
#
# for platform build, do not extract into VirtualBox shared folder, 
# or it will cause sumbolic link fail such as liblto_plugin.so cannot be linked from liblto_plugin.so.0.0.0.0
#
echo "==========================================================================="
echo "install tools" 
echo "==========================================================================="
if [[ $INSTALLER == "apt" || $INSTALLER == "apt-get" ]]; then
  sudo $INSTALLER install -y libssl-dev build-essential lzma uuid-dev
else
  sudo $INSTALLER install -y openssl-devel libuuid-devel
  wget http://mirror.centos.org/centos/8/PowerTools/x86_64/os/Packages/xz-lzma-compat-5.2.4-3.el8.x86_64.rpm
  sudo $INSTALLER localinstall -y xz-lzma-compat-5.2.4-3.el8.x86_64.rpm
  rm xz-lzma-compat-5.2.4-3.el8.x86_64.rpm
fi
sudo $INSTALLER install -y flex bison git
echo "==========================================================================="
echo "install python 3.6 sample"
echo "==========================================================================="
version=$(python -V 2>&1 | grep -Po '(?<=Python )(.+)')
echo $version
parsedVersion=${version:0:3}
if [[ -z "$version" || "$parsedVersion" != "3.6" ]] ; then
  if [[ $INSTALLER == "apt" || $INSTALLER == "apt-get" ]]; then
      sudo add-apt-repository -y ppa:deadsnakes/ppa
      sudo apt update -y
      sudo apt install -y python3.6 # 3.6 works for tag 1.07
  else
    sudo dnf install -y python3
  fi
  plink=/usr/bin/python
  if [ -L ${plink} ]; then
    sudo unlink ${plink}
  fi 
  sudo ln -s /usr/bin/python3.6 ${plink}
fi
echo "==========================================================================="
echo "test connection to SSH host"
echo "==========================================================================="
if [ "eval $(ssh -T git@github.com-adlink | grep -q "authenticated")" != "" ] ; then
  echo "establish SSH connection to GitHub already"
else
  echo "==========================================================================="
  echo "establishing SSH connection to GitHub"
  echo "==========================================================================="
  ssh-keygen -t rsa -b 4096 -C "GitHub-ADLINK" -f ~/.ssh/id_rsa_github_adlink -N ""
  if [ $? == 0 ] ; then
    cat ~/.ssh/id_rsa_github_adlink.pub
    echo ""
    echo "1. copy above public key to clipboard"
    echo "2. press ENTER to continue, "
    echo "   will lead you to gitlab to add this public key to your SSH Key setting, "
    echo "   close the brower to continue following procedure"
    echo "wait for the browser..."
    firefox http://git@github.com
    echo ""
    while ! ssh -T git@github.com-adlink | grep -q "authenticated"
    do 
      echo "wait for the ssh..."
      sleep 1
    done  
  fi
fi

echo "==========================================================================="
echo "clone edk2-ampere"
echo "==========================================================================="
SILLICON_FAMILY=$1
if [ -z "$SILLICON_FAMILY" ]; then
  SILLICON_FAMILY="edk2-ampere"
fi
if [ -d "$SILLICON_FAMILY" ]; then
  read -p "Do you wish to override current $SILLICON_FAMILY folder?" yn
  case $yn in
    [Yy]* )   rm -rf $SILLICON_FAMILY;;
    * ) return 1;;
  esac
fi

git clone https://github.com/ADLINK/edk2-ampere.git $SILLICON_FAMILY
echo "==========================================================================="
echo "fetch submodules recursively"
echo "==========================================================================="
cd $SILLICON_FAMILY
git submodule update --init --recursive
cd edk2-ampere-tools
git checkout adlink
cd .. 
cd edk2-platforms
git checkout adlink
cd .. 
cd edk2
git checkout 4b0c88346a6c57ddf73dd12eb55efd99b1d793e1
cd .. 
cd OpenPlatformPkg
git checkout master
cd .. 
if [ "eval $(ssh -T git@github.com-adlink | grep -q "authenticated")" != "" ] ; then
  echo "==========================================================================="
  echo "replace HTTPS access with SSH access if authenticated"
  echo "==========================================================================="
  git remote set-url origin git@github.com-adlink:ADLINK/edk2-ampere.git
  cd edk2-platforms
  git remote set-url origin git@github.com-adlink:ADLINK/edk2-platforms.git
  cd .. 
  cd edk2-ampere-tools
  git remote set-url origin git@github.com-adlink:ADLINK/edk2-ampere-tools.git
  cd .. 
fi
echo "==========================================================================="
echo "set building environment"
echo "==========================================================================="
. edk2/edksetup.sh --reconfig
. edk2.sh
echo "==========================================================================="
echo "Ready to build !!!"
echo "==========================================================================="
