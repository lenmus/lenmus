#! /bin/bash
#--------------------------------------------------------------------
# Pack all sources
#--------------------------------------------------------------------

E_XCD=66       # Can't change directory

SRC_DIR=/media/sdb1/usr/Desarrollo_wx/lenmus/src
PACK_DIR=/media/sdb1/usr/Desarrollo_wx/lenmus/temp/backup.tar.bz2

function ChangeToDir()
{
    cd $1
    if [ `pwd` != "$1" ]  # or  if [ "$PWD" != "$1" ]
	then
      echo "Can't change to $1."
      exit $E_XCD
	fi
}

ChangeToDir $SRC_DIR
tar --bzip2 --exclude=".svn" -cvvf $PACK_DIR *

exit 0      # sucess

