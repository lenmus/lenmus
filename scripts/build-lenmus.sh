#! /bin/bash
#------------------------------------------------------------------------------
# Building LenMus Phonascus
# This script MUST BE RUN from <root>/scripts/ folder
#
# usage:
#   ./build-lenmus.sh [-p {main | common | i18n | ebooks}]*
# Example:
#   ./build-lenmus.sh -p ebooks -p i18n
#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# Display the help message
function DisplayHelp()
{
    echo "Usage: ./bulid-lenmus.sh [option]*"
    echo ""
    echo "Options:"
    echo "    -h --help        Print this help text."
    echo "    -p --packages    Selects the packages to generate."
    echo "                     If option -p is not specified, it will generate"
    echo "                     the 'boundle' package containing all."
    echo "                     Valid options: one or more values"
    echo "                     { main | common | i18n | ebooks | boundle | all}* "
    echo "                     If boundle package is requested all other"
    echo "                     requested packages are ignored"
    echo ""
}

#------------------------------------------------------------------------------
# Check value and save in array
function SavePackage()
{
    #validate package
    if [ "$1" != "main" -a      \
         "$1" != "common" -a    \
         "$1" != "i18n" -a      \
         "$1" != "ebooks" -a      \
         "$1" != "all" -a      \
         "$1" != "boundle" ]
    then
        echo "Invalid package name '$1'. Generation cancelled."
        exit $E_NOARGS
    fi

    #save value
    packages+=("$1")
}

#------------------------------------------------------------------------------
# Avoid incompatible requests
function SanitizePackages()
{
    if [ "${#packages[*]}" -eq "0" ]; then
        packages+="boundle"
        return 0
    else
        for ix in ${!packages[*]}
        do
            if [ "${packages[$ix]}" == "boundle" ]; then
                packages=("boundle")
                return 0
            fi
            if [ "${packages[$ix]}" == "all" ]; then
                packages=("main" "common" "i18n" "ebooks")
                return 0
            fi
        done
    fi
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

declare -a packages=()   #declare empty array

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
        -p|--packages)
        SavePackage "$2"
        shift       # past argument
        shift       # past value
        ;;
        *) # unknown option 
        DisplayHelp
        exit 1
        ;;
    esac
done

#print message with packages to generate
SanitizePackages
num_packages=${#packages[*]}
echo "Packages to generate: ${num_packages}"
for ix in ${!packages[*]}
do
    printf "   %s\n" "lenmus-${packages[$ix]}"
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

#prepare package variables for cmake
for ix in ${!packages[*]}
do
    pkg="$(echo ${packages[$ix]} | tr '[a-z]' '[A-Z]')"
    package+="-DBUILD_PKG_${pkg}:BOOL=ON "
done
echo "package='${package}'"

# create makefile
cd "${build_path}"
echo -e "${enhanced}Creating makefile${reset}"
cmake -G "Unix Makefiles" ${package} ${sources}  || exit 1
echo ""

#build program
num_jobs=`getconf _NPROCESSORS_ONLN`	#number of jobs to create (as many as the number of processors)
echo -e "${enhanced}Building LenMus. This will use ${num_jobs} jobs.${reset}"
start_time=$(date -u +"%s")
make -j$num_jobs || exit 1
end_time=$(date -u +"%s")
secs=$(($end_time-$start_time))
echo "Build time: $(($secs / 60))m:$(($secs % 60))s"

#prepare package:
echo -e "${enhanced}Creating package${reset}"
cd "${build_path}" || exit $E_BADPATH
make package || exit 1
echo "-- Done"

exit $E_SUCCESS

