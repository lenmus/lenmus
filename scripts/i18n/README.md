# Scripts for managing translation

## langpacks

Purpose: create a language pack, that is, a ZIP archive with all .pot files to translate.
This script MUST BE RUN from <root>/langtool/scripts/i18n/ folder, in source tree

usage: cd scripts/i18n/
	   ./langpacks 5.2            (to create packages for all languages)
       ./langpacks 5.2 gl_ES      (to create package for gl_ES language)
       ./langpacks 5.2 xx		  (to create pot package for new languages)
#


## mem-update

Purpose: Update/generate compendium_xx.po file
This script MUST BE RUN from <root>/langtool/scripts/i18n/ folder, in source tree
usage: ./mem-update <po-file> [<lang>]
   <po-file> = { GeneralExercises | L1_MusicReading | L2_MusicReading 
                 | TheoryHarmony | lenmus }
 
   If no language specified, all languages are updated.

  example: ./mem-update TheoryHarmony de



## po2mo

Purpose: generate new MO files from PO files for books
This script MUST BE RUN from <root>/langtool/scripts/i18n/ folder, in source tree

usage: ./po2mo book [language]
  book = GeneralExercises | L1_MusicReading | L2_MusicReading | TheoryHarmony

example: ./po2mo TheoryHarmony



## po-lenmus

Purpose: Update/generate lenmus.pot file and update lenmus_xx.po files for all languages
This script MUST BE RUN from scripts/i18n/ folder, in source tree

 usage: ./po-lenmus version



## po-statistics.pl

Purpose: This script creates the website page to show the status of each po file.
I took the idea from an script written by Naba Kumar Singh
See: http://svn.gnome.org/viewvc/anjuta/trunk/po/status.pl?revision=3079&view=markup
     http://mail.gnome.org/archives/gnome-i18n/2002-March/msg00017.html

Usage
   cd /datos/cecilio/lm/projects/lenmus/trunk/scripts/web
   ./po-statistics.pl



## po-update

Purpose: Generate new POT files and update all PO files for books
This script MUST BE RUN from <root>/langtool/scripts/i18n/ folder, in source tree

 usage: ./po-update version book [language]
   book = GeneralExercises | L1_MusicReading | L2_MusicReading | TheoryHarmony

 example: ./po-update 5.2 TheoryHarmony



## upload-packs.sftp

Purpose: upload language packs to Sourceforge, to files to download
 usage:
    cd /datos/cecilio/lm/projects/lenmus/scripts/i18n
    sftp -o "batchmode no" -b upload-packs.sftp cecilios@frs.sourceforge.net

 for debugging add option -v: 
     sftp -v -o "batchmode no" -b upload-packs.sftp cecilios@frs.sourceforge.net



## users-guide scripts

These scripts are for managing the translation of the User's Guide. The scripts must be executed in order. And each script performs an step in the translation:

- Step 1: Extract document’s translatable messages into pot files.
- Step 2: Create/update the .po files, for the specified language.
- Step 3: Merge all .po files for language xx in a single one: users-guide-xx.po
- Step 4: Transfer the translated strings in compendium users-guide-xx.po to each original po file.
- Step 5: Generate all .mo files for Users' Guide.
- Step 6: Generate the translated document for Users' Guide.
- Step 7: Copy translated Users Guide and the wxhelp.map file to lenmus locale folder (trunk/locale/xx/help).


## z_compendium

Purpose: Update/generate compendium_xx.po file
This script MUST BE RUN from <root>/langtool/scripts/i18n/ folder, in source tree

usage: ./compendium [<po-file>] [<lang>]
   <po-file> = { GeneralExercises | L1_MusicReading | L2_MusicReading 
                 | TheoryHarmony | lenmus }
 
    If no PO file is specified, all PO files are processed.
    If no language specified, all languages are processed.

 example1: ./compendium TheoryHarmony de
   will update compendium_de.po with all the translated, non-fuzzy strings
   in TheoryHarmony_de.po

 example2: ./compendium de
   will update compendium_de.po with all the translated, non-fuzzy strings
   in project PO files


