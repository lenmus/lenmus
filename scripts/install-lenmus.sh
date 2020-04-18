#! /bin/bash
#------------------------------------------------------------------------------
# Local installation of LenMus package
# This script MUST BE RUN from <root>/scripts/ folder
#
# usage: sudo ./install-lenmus.sh
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# main line starts here

E_SUCCESS=0         # success
E_NOARGS=65         # no arguments
E_BADPATH=66        # not running from <root>/scripts

enhanced="\e[7m"
reset="\e[0m"

#get current directory and check we are running from <root>/scripts.
#For this I just check that "src" folder exists
scripts_path="${PWD}"
root_path=$(dirname "${PWD}")
if [[ ! -e "${root_path}/src" ]]; then
    echo "Error: not running from <root>/scripts"
    exit $E_BADPATH
fi

#path for building
build_path="${root_path}/zz_build-area"

echo -e "${enhanced}Local installation of LenMus program${reset}"

#find new package name
cd ${build_path}
echo "Now in ${PWD}"
app=`ls | grep 'lenmus-all_[0-9]*.[0-9]*.[0-9]*_[a-zA-Z0-9]*.deb'`
echo "New package to install: ${app}"

#uninstall current version of LenMus, if installed
installed=`dpkg -l | grep 'lenmus-all_[0-9]*.[0-9]*.[0-9]*'`
oldapp=$(echo $installed | egrep -o 'lenmus-all_[0-9]*.[0-9]*.[0-9]*' | head -n1)

if [ -n "$oldapp" ]; then
    echo -e "${enhanced}Removing old lenmus package ${oldapp}${reset}"
    sudo dpkg -r ${oldapp}
    echo "-- Done"
fi

#install new version of lenmus
echo -e "${enhanced}Installing new package ${app} ${reset}"
sudo dpkg -i "${app}"
echo "-- Done"

cd ${scripts_path}

exit $E_SUCCESS

