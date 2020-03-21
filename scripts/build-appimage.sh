#! /bin/bash
#
# From: https://docs.appimage.org/packaging-guide/from-source/native-binaries.html#id2

#------------------------------------------------------------------------------
# Building LenMus Phonascus
# This script MUST BE RUN from <root>/scripts/ folder
#
# usage:
#   ./build-appimage.sh
#------------------------------------------------------------------------------

#set the value some of shell attributes. See $ help set
set -x      #Exit immediately if a command exits with a non-zero status
set -e      #Exit immediately if a command exits with a non-zero status

#------------------------------------------------------------------------------
# Display the help message
function DisplayHelp()
{
    echo "Usage: ./bulid-lenmus.sh [option]*"
    echo ""
    echo "Options:"
    echo "    -h --help        Print this help text."
    echo ""
}


#------------------------------------------------------------------------------
# main line starts here

E_SUCCESS=0         # success
E_NOARGS=65         # no arguments
E_BADPATH=66        # not running from lenmus/scripts
E_BADARGS=67        # bad arguments
E_BUIL_ERROR=68

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

#parse command line parameters
# See: https://stackoverflow.com/questions/192249/how-do-i-parse-command-line-arguments-in-bash
#
while [[ $# -gt 0 ]]
do
    key="$1"

    case $key in
        -h|--help)
        DisplayHelp
        exit 1
        ;;
        *) # unknown option 
        DisplayHelp
        exit 1
        ;;
    esac
done


#path for building
build_path="${root_path}/zz_build-appimage"
sources="${root_path}"

#create or clear build folder
if [[ ! -e ${build_path} ]]; then
    #create build folder
    echo -e "${enhanced}Creating build folder${reset}"
    mkdir ${build_path}
    echo "-- Build folders created"
    echo ""
elif [[ ! -d ${build_path} ]]; then
    #path exists but it is not a folder
    echo "Folder for building (${build_path}) already exists but is not a directory!"
    echo "Build aborted"
    exit $E_BUIL_ERROR
else
    # clear build folders
    echo -e "${enhanced}Removing last build${reset}"
    cd "${build_path}" || exit $E_BADPATH
    rm * -rf
    echo "-- Build folders now empty"
    echo ""
fi

# create makefile
# we need to explicitly set the install prefix, as CMake's default is /usr/local for some reason...
cd "${build_path}"
echo -e "${enhanced}Creating makefile${reset}"
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_PKG_BOUNDLE:BOOL=ON \
    -DLENMUS_INSTALL_SOUNDFONT:BOOL=ON \
    -DLENMUS_INSTALL_BRAVURA_FONT:BOOL=ON \
    -DCMAKE_INSTALL_PREFIX=/usr ${sources}  || exit 1
echo ""

#build program
num_jobs=`getconf _NPROCESSORS_ONLN`	#number of jobs to create (as many as the number of processors)
echo -e "${enhanced}Building LenMus. This will use ${num_jobs} jobs.${reset}"
start_time=$(date -u +"%s")
make -j$num_jobs || exit 1
end_time=$(date -u +"%s")
secs=$(($end_time-$start_time))
echo "Build time: $(($secs / 60))m:$(($secs % 60))s"

# install files into AppDir
echo -e "${enhanced}Creating package${reset}"
cd "${build_path}" || exit $E_BADPATH
make install DESTDIR=./AppDir || exit 1
echo "-- Done"

# get linuxdeploy's AppImage and make it executable
wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
chmod +x linuxdeploy-x86_64.AppImage

# run linuxdeploy to initialize AppDir and build AppImage, all in one single command
./linuxdeploy-x86_64.AppImage --appdir AppDir --output appimage

exit $E_SUCCESS

