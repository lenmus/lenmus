#! /bin/bash
#------------------------------------------------------------------------------
# Building LenMus Phonascus
# This script MUST BE RUN from <root>/scripts/ folder
#
# usage: ./build-lenmus.sh
#------------------------------------------------------------------------------


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
while [[ $# > 0 ]]
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
    shift # past argument or value
done

#path for building
build_path="${root_path}/zz_build-area"
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
cd "${build_path}"
echo -e "${enhanced}Creating makefile${reset}"
cmake -G "Unix Makefiles" ${sources}  || exit 1
echo ""

#build program
echo -e "${enhanced}Building LenMus${reset}"
start_time=$(date -u +"%s")
make -j2 || exit 1
end_time=$(date -u +"%s")
secs=$(($end_time-$start_time))
echo "Build time: $(($secs / 60))m:$(($secs % 60))s"

exit $E_SUCCESS

