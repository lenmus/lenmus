# Scripts for managing LenMus

Folders:
- i18n. Scripts for managing translations
- release. Scripts for preapring a nuew release of the program
- web. Scripts for uploading things to website

The scripts in this root folder are related to building and installing LenMus:



## build-lendmus.sh

Purpose: Builds the LenMus Phonascus program
This script MUST BE RUN from folder <root>/scripts
usage: ./build-lenmus.sh



## cp-lomse
Purpose: This is an old auxiliary script that was used to copy lomse sources to lenmus tree.



## helper.sh
Purpose: helper Bash functions used in other scripts.



## install-lenmus.sh

Purpose: installation of LenMus package
This script MUST BE RUN from <root>/scripts/ folder
usage: sudo ./install-lenmus.sh




## update-version.sh

Purpose: Update LenMus date & version information in the following places:
- debian/changelog

This script MUST BE RUN from folder <root>/scripts/
usage: ./update-version.sh <major> <minor> <patch>
