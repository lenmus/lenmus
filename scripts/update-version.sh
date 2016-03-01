#! /bin/bash
#------------------------------------------------------------------------------
# Update LenMus version information in several places:
#   include/lenmus_version.h
#   debian/changelog
#
# This script MUST BE RUN from <root>/scripts/ folder
#
# usage: ./update-version.sh
#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# main line starts here

E_SUCCESS=0         # success
E_NOARGS=65         # no arguments
E_BADPATH=66        # not running from lenmus/trunk/scripts

#get current directory and check we are running from <root>/scripts.
#For this I jaust check that "src" folder exists
scripts_path="${PWD}"
lenmus_path=$(dirname "${PWD}")
if [[ ! -e "${lenmus_path}/src" ]]; then
    echo "Error: not running from <root>/scripts"
    exit $E_BADPATH
fi

#get lenmus version
echo "Getting LenMus version"
cd "${lenmus_path}"
description=`git describe --tags`
pattern="([0-9]+)\.*"
if [[ $description =~ $pattern ]]; then
    major=${BASH_REMATCH[1]}
fi
pattern="[0-9]+\.([0-9]+)\.*"
if [[ $description =~ $pattern ]]; then
    minor=${BASH_REMATCH[1]}
fi
pattern="[0-9]+\.[0-9]+\-([0-9]+)\-*"
if [[ $description =~ $pattern ]]; then
    patch=${BASH_REMATCH[1]}
fi
pattern="[0-9]+\.[0-9]+\-[0-9]+\-g([a-f0-9]+)"
if [[ $description =~ $pattern ]]; then
    sha1=${BASH_REMATCH[1]}
fi
package="${major}.${minor}.${patch}"
echo "-- git description = ${description}"
echo "-- package = ${package}"
echo "-- major=${major}, minor=${minor}, patch=${patch}, sha1=${sha1}"

#update version file
file="${lenmus_path}/include/lenmus_version.h"
if [ -f $file ]; then
    echo "Updating version in file ${file}"
    FILE=`sed -n '1,2p' ${file}`
    FILE+=$'\n'
    FILE+="#define LENMUS_VERSION_MAJOR   ${major}"
    FILE+=$'\n'
    FILE+="#define LENMUS_VERSION_MINOR   ${minor}"
    FILE+=$'\n'
    FILE+="#define LENMUS_VERSION_PATCH   ${patch}"
    FILE+=$'\n'
    echo "$FILE" > ${file}
    echo "-- Done"
else
    echo "ERROR: File ${file} not found. Aborted."
    echo ""    
    exit $E_BADPATH
fi

#Update version and date in debian changelog
file="${lenmus_path}/debian/changelog"
if [ -f $file ]; then
    echo "Updating version and date in ${file}"
    cd "${lenmus_path}/debian"
    FILE="lenmus (${package}) stable; urgency=low"
    FILE+=$'\n'
    FILE+=$'\n'
    FILE+="  * Latest release"
    FILE+=$'\n'
    FILE+=$'\n'
    today=`date -R`
    FILE+=" -- Cecilio Salmeron <s.cecilio@gmail.com>  ${today}"
    FILE+=$'\n'
    echo "$FILE" > ${file}
    echo "-- Done"
else
    echo "ERROR: File ${file} not found. Aborted."
    echo ""    
    exit $E_BADPATH
fi

exit $E_SUCCESS

