#! /bin/bash
#------------------------------------------------------------------------------
# Update LenMus date & version information in the following places:
#   debian/changelog
#
# This script MUST BE RUN from <root>/scripts/ folder
#
# usage: ./update-version.sh <major> <minor> <patch>
#------------------------------------------------------------------------------


#------------------------------------------------------------------------------
# main line starts here

E_SUCCESS=0         # success
E_NOARGS=65         # no arguments
E_BADPATH=66        # not running from lenmus/scripts

#get current directory and check we are running from <root>/scripts.
#For this I just check that "src" folder exists
scripts_path="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
lenmus_path=$(dirname "${scripts_path}")
if [[ ! -e "${lenmus_path}/src" ]]; then
    echo "Error: cannot find src folder. Not running from scripts folder?" 1>&2
    exit $E_BADPATH
fi

source ${scripts_path}/helper.sh

if [ 1 -eq 0 ]; then   #------------------------------------------------
# get lenmus version from repo tags
echo "Getting lenmus version"
cd "${lenmus_path}"

description="$(git describe --tags --long)"
parseDescription "$description"

echo "-- git description = ${description}"
echo "-- package = ${package}"
echo "-- major=${major}, minor=${minor}, patch=${patch}, sha1=${sha1}"

fi #--------------------------------------------------------------------

# get lenmus version from command line
major=$1
minor=$2
patch=$3
package="${major}.${minor}.${patch}"
echo "-- package = ${package}"
echo "-- major=${major}, minor=${minor}, patch=${patch}"


if [ 1 -eq 0 ]; then   #------------------------------------------------
# update version file from latest tag
file="${lenmus_path}/build-version.cmake"
if [ -f $file ]; then
    echo "Updating version in file ${file}"
    sed -i -e 's/\(set( LENMUS_VERSION_MAJOR \)\([01-9]*\)\(.*\)/\1'$major'\3/' \
	-e 's/\(set( LENMUS_VERSION_MINOR \)\([01-9]*\)\(.*\)/\1'$minor'\3/' \
	-e 's/\(set( LENMUS_VERSION_PATCH \)\([01-9]*\)\(.*\)/\1'$patch'\3/' \
	$file
    echo "-- Done"
else
    echo "ERROR: File ${file} not found. Aborted."
    echo ""    
    exit $E_BADPATH
fi

fi #--------------------------------------------------------------------

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

