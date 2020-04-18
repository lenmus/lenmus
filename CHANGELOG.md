# LenMus Phonascus. Log of changes


[Since last version] 5.6.2
=============================

* None


Version [5.6.2] (Apr 18th, 2020)
==================================

* Fixed important bug that only affected to Linux platforms. The path for the
  default sound font was incorrectly set and MIDI sound didn't work without
  the user having to go to Preferences -> Sound and to change the setting.


Version [5.6.1] (March 22nd, 2020)
==================================

From a user point of view, release 5.6.1 does not have any functional changes.
The only differences from version 5.6.0 are technical, oriented to improve
Linux packages and installation:

* A Linux package in [AppImage](https://appimage.org/) format, that run in all
  common Linux distributions is now buildable and distributed.
* LenMus cmake script now allows to split the Linux package in several packages:
    * lenmus: (main package) binaries and related files
    * lenmus-common: common support files not bound to any particular
      release, architecture or language, such as icons, samples and templates.
    * lenmus-i18n: the message catalogues for lenmus and related translated files
    * lenmus-ebooks: the eBooks (all translations).
  In addition, a boundle package containing all can be generated.
* LenMus now uses system installed fonts instead of fonts delivered in LenMus
  source tree (only for Linux and Windows) and will do the necessary substitutions
  if a specific font is not installed. For other operating systems the old
  behavior is maintained.
* There is no longer need to install 'Liberation' and 'WenQuanYi Zen Hei' fonts
  for running LenMus. The only needed font is 'Bravura' music font but it is no
  longer included in source tree. Package maintainers must add an install dependency
  from this font package. As I have not found a Bravura font package for Debian,
  I've created one at https://github.com/lenmus/bravura. This .deb package can
  be downloaded from
  [here](https://github.com/lenmus/bravura/releases/download/1.18-0/fonts-lenmus-bravura_1.18-0_all.deb).
  For Linux a new build dependency is introduced from 'fontconfig' package but
  it is a basic package expected to be installed in most distros.
* All font files have been removed from source tree.


Version [5.6.0] (March 1st, 2020)
==================================

The main objective of version 5.6.0 is to avoid problems with the sound when installing
LenMus. In previous versions, the user had to configure a MIDI synthesizer and sometimes
even install it, and this caused problems. This now has been solved:

* LenMus includes now a preconfigured internal MIDI synthesizer and, therefore, the user
  no longer needs to install a MIDI synthesizer or configure the program to use it.
* The MIDI assistant has been removed, since the user has nothing to configure after
  installing LenMus. But, for greater flexibility, LenMus now includes a MIDI
  configuration option, to allow you to configure the internal synthesizer (for example,
  change SoundFont) or allow the use of an external MIDI synthesizer instead of the
  internal one, in case the user prefers to use specialized hardware or other MIDI
  synthesizers.
* Fix bug preventing to translate the "Cancel" button.
* Fix bug: MIDI instrument names not properly translated when changing the language.


Version [5.5.0] (February 7th, 2020)
===============================================================================

This is a maintenance release to add the translation to the Greek language and some few
small improvements, and to fix bugs. The main changes are:

* The translation to the Greek language has been added. Thanks to
  Avgeris Tsironis (Αυγέρης Τσιρώνης).
* The cadences generator have been re-coded to remove bugs in Cadences exercises.
* Updates the MS Windows installer for Windows 10 x64.
* Improves metronome by adding a metronome settings window and allowing to select
  the beat note value.
* Keyboard buttons Page Up/Down are now operative in eBooks.
* More visual tracking options during playback.
* Fix to display the Study Guide in the selected language.
* Fix bug in options for theo idfy/build interval and for cadences exercises, causing to
  ignore some selected options.
* Includes the latest Lomse library with improvements, mainly in MusicXML import and
  music notation support.
* Links for features not yet fully operational have been disabled.
* Fix error in figure in Theory and Harmony eBook.


Version [5.4.2] (July 26th, 2016)
===============================================================================

This is a maintenance release:

* Fixed #55: Compilation errors when using C++11.
* Fixed #40: Added a man page for lenmus.
* Includes the latest Lomse library with improvements in music renderization.


Version [5.4.1] (December 18th, 2015)
===============================================================================

This is a maintenance release to address an important bug in LenMus installation,
preventing help files to install properly. The only changes are:

* Adds the missing help documentation files.
* Includes the latest Lomse library with improvements, mainly in MusicXML import.
* Fixes a bug in QuestionBox due to wxString 3.0 conversion.
* And, updates some score templates to LDP 2.0.


Version 5.4 (September 26th, 2015)
===============================================================================

The main goal of this release is to simplify LenMus installation. I've changed the way in
which LenMus is built and installed, removing the dependency from Lomse library that 
caused a lot of problems. Unfortunately, it is too costly to prepare a release without 
including all the new code in development and, thus, this release includes a lot of new 
features not yet finished, but this new code is, by default, disabled. The main changes 
are:

* Changes for including a score editor:
    * Changes in main menu:
	    * Added 'Edit' menu item, with some sub-items (undo, redo, insert,
	      enable edition). When opening a document it is, by default,
	      non-editable. You must enable edition by selecting 
	      'Edit > Enable edition'. Edit > Insert' includes test code for 
	      inserting headers, paragraphs and scores.
	    * Added more 'File' menu items:
	        * 'File > New', for creating empty documents
	        * 'File > Save' and 'File > Save as'.
	        * 'File > Close'. Also, when closing a document, checks if it
                  has been modified and asks for saving it before closing.
        	* 'File > Convert' menu item, for converting a Document to latest
              format version.
        	* Added menu option 'View > Show voices in colors' for displaying
              each voice in a different colour (works also in read only mode).
    * More tools added:
    	* A 'Command console', to test edition commands without using GUI.
        * A 'Toolbox' for containing all tools related to document edition:
            * It is automatically displayed/hidden if edition is enabled/disabled. 
	        * It also admits 'mode' (personalized configurations so that
              each exercise or edition mode can have specific tools).
            * It is also automatically reconfigured when the active 
              document changes, so that appropriate tools for each document
              are presented.
        * A 'Score wizard' for creating scores from templates.
    * Shortcuts: 
	    * Added code for a shortcuts system, global managed.
	    * Added option dialog for customizing shortcuts
    * All music elements (notes, rests, barlines, clefts, etc.) can
      be entered from keyboard. Navigation keys also operational.
    * Support for edition via 'drag and click': Barlines, notes & rests can now be
      added by drag and click.

* Changes for preparing more exercises and new features:
    * Added and implemented FullEditorCtrol and DictationCtrol classes, for 
      allowing to use the score editor in music exercises.
    * Added class ScoreComparer for correcting user solutions.
    * Changes in composer for creating pickup measures (Thanks to Camilla Tassi).
    * New dialog for metronome in 'Menu > Tools', to facilitate more features,
      such as setting tempo by tapping.
    * Added help system. Help menu now includes: 'Users guide', 'Study guide'
      and 'Editor quick guide'.
    * Added code for importing MusicXML files. Importing files merged in
      File > Open.
    * Fixed bug in composer resultimg in bad pitch assigned to some notes.
    * Added button to hide or show TOC panel for books.

* Other changes:
    * All the code has been updated for using wxWidgets 3.0.
    * I've designed a new workflow for translating the  users oriented
      documentation, such as 'Users' Guide' and the 'Editor quick guide'.
      Several scripts developed for managing all this.
    * Added option, in preferences dialog, for enabling all the features
      not yet finished ('Preferences > Other > Enable experimental features').



Version 5.3.1 (April 26th, 2013)
===============================================================================

Version 5.3.1 is focused on fixing bugs, in particular on those causing 
unexpected program termination. This version also completes translation to 
Chinese and adds options for clefs reading exercise:

* Full translation to Chinese.
* Changes in exercises to display more messages to guide users.
* Added options to clefs reading exercise for choosing clef and notes range.
* Bug fixes:
       - Occasional program termination when moving to another exercise.
       - In compare interval exercise the option to start both intervals 
         with same note was not working.
       - In interval exercises sometimes an strange error message saying 
         that 'Not possible to generate valid interval' was displayed.
       - Button colour was removed as soon as mouse moved over the button.
       - Other bug fixes and changes related to exercises.
       - Moved to Lomse library version 0.14, as it fixed many bugs.
* Refactoring: Logger class removed and replaced by Lomse logger.


Version 5.3 (November 29th, 2012)
===============================================================================

* Restored all other missing features, present in previous version 4.2, that 
 were disabled in 5.0:
    - All exercise modes (learning, practice, exam, quiz) restored.
    - Right/wrong sounds when answering an exercise restored.
    - After answering an exercise, the answer buttons will play the 
      button sound when clicked.
* Fixed bug #1074909: Exercises on intervals in TheoryHarmony, lesson 4.1:
 not for level one. Settings for these exercises were for level 3 and have 
 been restored for level 1.
* Fixed bug #1082637 Incorrect answer in key signature exercise. There was an 
 error with B flat minor key signature. Now fixed. 


Version 5.2 (October 28th, 2012)
===============================================================================

* Fixed bug #1059064: TheoryHarmony: crash when opening lesson 3.3.
* Fixed bug #1067628: Crash if page change while playing back an score.
* Added German translation (Thanks to Undine Peters).
* "langtool" program for generating and translating books, has been modified
  to generate books in new xml format with LMD files.
* All eBooks converted to new format.
* Removed irrelevant log messages.


Version 5.1 (September 15th, 2012)
===============================================================================

* New dialog for selecting books.
* Added "Study guide" to help students and explain how this program should be used.
* Improvements in all music reading exercises, that include now a checkbox to turn
  metronome on/off.
* Added Chinese translation (Thanks to Wenxi Lu).
* Restored some features, present in previous version 4.2, that were disabled in 5.0:
    * General metronome and metronome synchronized with score playback are again operational.
    * The solution for compare intervals (ear) exercise displays again the name of the intervals.
    * Countoff checkbox is again operational in all music reading exercises.
* Fixed bugs in music scores composer, introduced during source code migration to 5.0.
* Fixed bug preventing to display release notes in current language.
* Fixed bug in key sign. identification preventing to display the answer string.


Version 5.0 (July 8th, 2012)
===============================================================================

Version 5.0 is, mainly, an experimental version using the new Lomse library (http://www.lenmus.org/en/lomse). It is an update of all LenMus Phonascus code, with some addidional improvements:

* Two new exercises are included:
        * Notes identification.
        * Tonality identification.
* All existing problems with zooming, relative size of scores and text, and rendering have been solved.
* The build system has been change to CMake. And there is now a Linux package.
* Now there are independent Windows installation packages for each translation. With this, it is now possible to update a translation at any moment without having to wait for a new program release. This creates more work for me but solves the previous problem of having to wait for all  translators before releasing a new version.

As you can see there are no many new features, as all efforts has been in rewriting LenMus Phonascus code:

* All code related to analysing, parsing and rendering scores has been isolated and moved to a platform independent library (the Lomse library). The idea was that this code could be used by any other projects and that it can be better tested to get the best code quality.

* The eBooks format has been changed so that now they are fully written in LDP.I have redesigned the way in which exercises are created and displayed. Now the LDP score description language has been extended so that it can now describe not only a score but full documents, with any content: texts, images, scores, exercises, etc. The reason for this change was to remove the need to use a web browser to display the eBooks. Until now, the exercises were displayed as HTML pages by using an internal, very limited, web browser. Due to its limitations and to the hacks I had to do to display scores, old LenMus Phonascus releases had some problems of difficult solution that are now solved.

All these changes have implied a lot of work. Due to this, I have not yet finished rewriting all existing code. As a trade-off between features and time to release version 5.0, I've decided to release version 5.0 without including some features existing in previous 4.2 version:

* The score editor is not yet included.
* Reduced functionality in some points:
        * Exercises supporting only exam mode (Quiz counter ctrol in exam mode).
        * No right/wrong sound.

All missing functionality will be restored gradually in future versions and new functionalities and exercises will be added.


Version 4.2.2 (March 21st, 2010)
===============================================================================

* Critical bug fixed in score renderization that could cause a crash in any score when rendering it.
* Updated Basque translation.



Version 4.2.1 (March 8th, 2010)
===============================================================================

* Fixes a critical bug producing a crash at start if monitor resolution is grater than 1024 x 768 pixels.
* Language can now be changed without re-starting program.



Version 4.2 (March 7th, 2010)
===============================================================================

The main changes introduced by release 4.2 are the following:

- Exercises and eBooks:
    * Lessons in eBook TheoryHarmony: Added six lessons on scales, lesson on intervals changed.

- Bug fixes:
    * Score layout algorithm has been rewritten from scratch using a test driven approach. A lot of renderization bugs have been fixed. Automated tests now in place for error detection and quality assurance.
    * Critical bug fixed: sometimes crash when clicking with mouse at end of last bar.
    * Inserting note before barline creates the note at next bar, with incorrect time position.
    * LDP parser incorrectly imported real numbers lower than 1.0 and greater than 0.0.
    * Clefs bounds incorrectly computed, adding more space than necessary for layout and for selection.

- Translation
    * In order to simplify and reduce work for translators, Langtool, the lenmus tool developed for eBooks translation, has been modified to improve it and to add support to translate also help documentation, release notes, web pages and other documents.
    * Also changes to improve program translation by not breaking sentences at certain tags (i.e. emphasis, link, etc); a lot of refactoring to simplify maintenance and addition of more tags. Support for styles (eBook style, variable footers) introduced.


Version 4.1 (January 7th, 2010)
===============================================================================

The main changes introduced by release 4.1 are the following:

The score editor
-----------------

The score editor is much more stable but it is not yet finished and continues in beta status. The editor includes now more features but few of them will be noticeable in this release. The most visible are:

* Data entry using the mouse. The user interface (including the toolbox) has been improved and now data entry using the mouse (point and click) is available. Data entry using keyboard has been restricted to notes and rests (but mouse can also be used). For all other, the mouse is now mandatory.

* Lines, ties, and, in general, geometric figures, are now editable by dragging control points.

* Figured bass symbols. Now it is possible to add figured bass annotations to the score. This was required for the planned exercises on figured bass to be included in a coming version.

* Notes stem direction now editable. The tool box now includes buttons to control stem direction.

* The score being edited is now automatically recovered after a crash. The score editor is much more stable, as a lot of debugging was done and all known bugs have been fixed; nevertheless, it is still not finished and continues in beta status. But now, after a program crash you will not loose (normally) the score being edited and it will be recovered automatically when you run again the program.

* Automatic upload of a 'forensic log'. I need your help to debug the score editor. Take into account that most of my time is devoted to programming, not to use the program. Therefore you probably will catch more errors than me!. In order that you can collaborate in this task, the program now saves data about the score being edited and the action performed, and if the program crashes, it asks for your permission and sends me a "forensic log". It is a simple file, in text format (you can open it at see its contents) and it does not contains any identification data of any nature (personal, equipment, Internet address, etc). It only has the score being edited an the actions being performed. Thanks for your collaboration!

Among the 'hidden' improvements the following should be mentioned:

* Support for 'Score processors'. There is a need for tools to process (or alter) a full score. For example, a tool to transpose a score, to add colours to voices, to make a piano reduction, etc. I call 'score processors' to these type of tools that process the whole score and can alter it. Support to develop and use score processors has been added and it is currently being tested with the four voices harmony and figured bass exercises that will be included in coming releases.

* Support to use the score editor for exercises. The problem is presented as a score that the student has to modify. As the full editor is used, the student can save the score so that he/she can later continue the exercise. This facility is currently being tested with the harmony and figured bass exercises.

* Customizable editor modes. For exercises using the score editor to elaborate the answer (for example: dictation exercises, harmony exercises, etc.) there is a need to customize the score editor for the needs of a particular exercise. For example, in the four-parts harmony exercises the editor is restricted to use only four voices. To implement this idea, the score editor now supports 'editor modes'. An 'editor mode' is just a set of restrictions and customizations that forces the score editor to behave in a predefined way and customizes the toolbox to include different tools.


Exercises and eBooks
---------------------

The interval's exercise has been reviewed and splitted into two exercises, one to identify intervals and another one to build them. The identification exercise has been re-structured into four difficulty levels. The exercise settings has been rebuild and there are now more options.

eBook Theory & harmony now includes a new chapter on intervals. But only the first lesson is included. Sorry! I have no more time!

The eBooks license has been changed to Creative Commons Attribution-Share Alike License. I think this license is clearer than the GNU Free Documentation License and is compatible with it. This changes also simplifies life for using texts from other free public sources or for integrating LenMus texts in other public sources.


The Leitner learning methodology is now included in some exercises.
Originally, LenMus exercises were not based on any particular learning methodology. Questions were selected just at random and no student performance data was saved. Version 4.1 now includes support in some exercises for a spaced repetition
learning technique (see http://en.wikipedia.org/wiki/Spaced_repetition), also known as
Leitner method or flashcards method (http://en.wikipedia.org/wiki/Flashcard).
A database to save user data about exercises and answers has already been added and I have started to modify exercises to include four working modes: 'learning', 'practising', 'exam' and 'quiz'

* In 'learning' mode the program analyses your answers and schedule questions to systematically covering all the subject and to focus on those questions that are troubling you. This mode is the most systematic one and asked questions are adapted your learning needs, to minimize your study time and optimise your learning rate. Your performance data is saved and the next time  you return again to the exercise, the program takes care of asking questions to ensure an optimal learning path. The result is, ideally, a reduction in the amount of time needed to study a subject and the assurance that the subject has been systematically reviewed. This mode is based on the 'spaced repetition' or 'Leitner method'.

* In 'practising' mode the program uses your performance data, saved in learning mode, to choose questions. It selects questions at random but giving more probability to those that are troubling you. Your performance data in this mode is not saved. This mode is useful when you have finished your daily assignment and you would like to practise more.

* In 'exam' mode, neither your saved performance data not your answers to previous questions are taken into account to formulate the next question. At any moment, all possible questions have the same probability of being asked. This mode is useful for testing your knowledge before taking an examination, but is less useful for learning.

* The 'quiz' mode is similar to the 'exam' mode but two answer counters are displayed and questions are accounted in both counters: one in first counter and the next one in the second counter. This mode is useful to work in pairs or in teams at classroom.


User requests and other changes
---------------------------------

* Some users requested to have a **count off before starting any score play back** option. Count off has been added for scores opened in the editor and for scores in music reading exercises.

* I received some reports on problems when using low screen resolutions or big size icons in the toolbar, as the buttons didn't fit on the available space and some of them were not visible on the screen; as the actions performed with these buttons were not available on the main menu the user could not interact with the program. Now:
    
    * all toolbar actions are also on the main menu, and.
    * *he toolbar automatically resizes and splits to fit in the main window.


Version 4.0 (January 17th, 2009)
=======================================================================

As the version number change suggests, release 4.0 is major update in
the program code and functionality. The main changes are:

Inclusion of a score editor (beta status)

The major change from previous release 3.6 is the inclusion of a score
editor (although in beta status). The score editor is not yet finished, 
as much more features have to be added, but the included features are 
operational, and allows to create simple scores, even with several 
instruments and voices. Nevertheless, the editor is in a preliminary state
(beta status) and it is mainly useful deal with simple scores with one 
voice per staff. Many voices per staff are also supported but facilities 
to automate layouting task and taking automated decisions, such as 
automatic voice assignment, automatically setting up stem direction in 
notes to avoid collisions between stems, and other facilities, are not 
fully operational and in some cases produce poor results. Also, at 
current development stage, it is not possible to deal with:

* lyric lines
* slurs
* dynamics, articulations and other symbols
* cue/grace notes
* rehearsal marks, coda, segno and related symbols
* nested tuples
* figures: lines, boxes, circles, other

These missing features will be added in coming releases. The basements 
are now there, so I expect to add much more features in short time.

One of the issues that has demanding me a lot of time is the user interface.
I would like to find a simple and intuitive interface, but the complexities
involved in editing a score makes this difficult to achieve. Therefore, do
not assume the user interface is finished as I will continue experimenting
with different approaches. Your suggestions are extremely welcome,
specially if you have experience in using other score editors.

Change of license to GPL v3+

The extensive code re-writing required to include the score editor and the
fact that including a score editor is a major change in program features are
the main reasons to change release number to 4.0 instead of 3.7.

With release 4.0 I also have changed the program license to the new Free
Software Foundation GPL V3+. With this change, I expect to better ensure
your rights to use LenMus for free, to have access to source code, and to be
able to distribute it to others and to modify it. The change to GPL v3+ is
also a reason to change the release number to 4.0. With this change it will
be more identifiable which license applies to which lenmus version:

* lenmus 1.x - freeware. Source code not available
* lenmus 2.x & 3.x - GPL v2+
* lenmus 4.x - GPL v3+

Welcome page

A welcome page has been added to facilitate access to the different features
and to have easily available on startup:

* eBooks
* The score editor
* Latest open scores
* Useful links and documentation

Other changes

Version 4.0 does not include new music eBooks or exercises. But bugs
detected in previous versions have been fixed.

Three new translations has been added: Basque, Galician and Italian.

 
 

Version 3.6 (December 1st, 2007)
=======================================================================

Some new features and changes are introduced:

* A new eBook with theory and exercises on cadences.
* The automatic composer algorithm has been rewritten to take into
  account harmony rules. Scores in music reading exercises are now 
  'singable' and takes tonality into account.
* Some errors corrected in Music Reading Level 1 eBook
* French translation has been totally reviewed.
* Some bug fixes and other technical improvements:
      o Bug fixed [#1742061]: Occasional crash on Ear Interval exercise.
      o Bug fixed [#1742062]: Intervals: theory exercise. No button 
        for 2dim answer.
      o Bug fixed [#1823932]: Score symbols bad positioned when no
        standard screen resolution.
      o Bug fixed [#1742064]: Scales identification: ambiguity in 
        answer buttons.

Version 3.5 (June 17th, 2007)
=======================================================================

Some new features are introduced that affect the users' experience of 
the software:

* A new eMusicBook is included: Music Reading level 1
* Playback: a rhythmic accent was added. Higher volume on stronger 
  beats.
* The MDI user interface was replaced by a notebook tabbed interface.
* The lenmus program now includes the Dutch language translation.

Additionally, the new version has a lot of technical changes and some 
bug fixes:

* Support was added to include images in eMusicBook texts.
* Score parameters in eBooks: added tag to specify a fixed metronome
  rate to play the score instead of using a user specified value in the
  metronome control.
* Improvements in the automatic music composer:
      o The composer was modified so that in the final measure it never
        adds a note longer that a quarter note if the composed piece
        only uses quarter notes. This was needed for Music Reading Level
        1 eBook, for the initial lessons.
* Improvements in the internal score model to support more features:
      o Support to include graphical objects (arrows, lines, circles,
        etc.) in a score. For now only lines are implemented.
      o Implemented metronome mark notations.
      o Added support to set up the size of a measure by specifying 
        the barline position.
      o Added support for fermatas on notes and rests.
      o Added support to have global, inheritable options in all 
        score elements. Implemented support for the following options:
            + Option 'Staff.DrawLeftBarline' to control left barline 
              drawing.
            + Option 'StaffLines.Hide' to control staff lines drawing.
            + Option 'StaffLines.StopAtFinalBarline' to allow for staff
              lines going to the right paper margin instead of finishing
              at the final barline.
            + Option 'Staff.UpperLegerLines.Displacement' to shift the
              first upper leger line so that it is closer/further than
              normal spacing. The other leger lines spacing is not
              changed.
            + Options 'Render.SpacingMethod' and 'Render.SpacingValue',
              to allow for more control of score renderization.
* Improvements in score renderization:
      o Fixed spacing method implemented.
      o Fixed bug [#1715968]. Proportional spacing of notes is 
        now working.
      o Do not add spacing after barline if barline is not visible.
* Improvements in LDP language to support the new features. New 
  tags added:
      o Implemented simple notation 'fermata', and compound notation
        '(fermata [above | below])' on notes and rests.
      o tag 'metronome' to define metronome notations.
      o tag 'graphic line' to add lines on a score.
      o tag 'spacer' for better controlling staff space.
      o tag 'opt' to add global, inheritable options to all other tags.
      o Added support for strings enclosed in double simple quotes as 
        an alternative to double quotes.
* Several bug fixes:
      o Bug fixed [#1724988]: Crash if program is closed while playing
        intervals in aural training compare intervals exercise.
      o Bug fixed [#1715979]: When you have a score open, the 'show/hide
        navigation panel' button shows as active but you cannot click
        on it.
      o Bug fixed [#1715953]: In the Midi configuration wizard the 
        drop downs in the wizard should not be editable.
      o Bug fixed [#1715959]: Focus problem. In the configuration 
        window when you check the proxy configuration but do not fill it
        in and then change the focus to section 'language', the category
        selector changes but the focus remains on the Internet section.
        Alternatively you could generate a warning message and leave the
        focus on the Internet section.
      o Bug fixed [#1715963]: Repeated quick acceleration letters in
        options, menu labels. The English text has the same letter as
        quick key for bottom and right labels.
      o Bug fixed [#1715965]: Rests in a group of tied notes are not 
        rendered in the right position.
      o Bug fixed [#1715968]: Proportional spacing of notes is 
        not working.
      o Bug fixed [#176634]: Program crash if closed while playing 
        a score.
      o Bug fixed [#176635]: Crash if program is closed while playing
        an exercise (L2_MusicReading).
      o Bug fixed [#1590088]: Program crash if move to another 
        exercise while playing an exercise.
            

Version 3.4 (5/Feb/2007)
=======================================================================

* A new eMusicBooks source format, based on XML and DocBook, has been
  developed with three objectives in mind:
      o To simplify writing eMusicBooks.
      o To simplify the translation of eMusicBooks.
      o To enable new features.
* The controller to display eMusicBooks has been rewritten to adapt to
  the new eMusicBooks format. Now the index panel can also display
  images.
* New buttons to increase/decrease font size on eMusicBooks. They 
  scale also the scores on the books.
* All toolbar icons has been reviewed to improve the disable images.
* A program, named LangTool, has been developed to compile eMusicBooks
  and to automate the translation process.
* Supporting wxWidgets libraries upgraded to version 2.8.0
* Bug fixed: In the options panel, key signature 'D mayor/B minor' is 
  duplicated and 'D b Mayor/B b minor' is missing [1610190].
* Bug fixed: Minor scales, in scales exercises, have wrong key 
  signature [1604535].
* Bug fixed: No ascending intervals in aural training [1601350].
* Bug fixed: Spanish translation error in aural training 
  exercises [1610186].
* Bug fixed: Metronome lose previous run settings. Always initialized
  at 20.
* Bug fixed: Some navigation buttons on eMusicBooks didn't work.

Version 3.3 (10/Nov/2006)
=======================================================================

* A new exercise for aural training: identify scales.
* Theory exercise on scales identification improved: now it is 
  user configurable and more scales have been included.
* Software updater improved: now it takes proxies into account (user
  request, task #130256).
* Options dialog modified to include Internet settings. Updater 
  settings moved from 'Other settings' to 'Internet settings'.
* French and Turkish translations now included.
* Changing to another language is now immediate. In previous versions 
  it was delayed to the next run of the program.
* New option to enable/disable right/wrong answer sounds in exercises
  (user request #1559450).
* New option to do exercises in teams: two answer counters are displayed
  and each new problem is assigned to a different team.
* New option to show the solution also when the answer was right.
* Now, at first run, the program detects the language chosen for 
  installer interface and configures program to use also this language
  as default.
* Screen renderization code modified to deal with fractions of a pixel
  and add anti-aliasing. This technique greatly improves visual quality
  of displayed scores.
* Bug fixed: Metronome not synchronized with the score in compound 
  times (user reported bug #1581743)
* Bug fixed: Occasionally, crash when displaying and exercise.
* Bug fixed: When printing a score, in occasions, it is not adjusted
  to page size.


Version 3.2 (26/Aug/2006)
=======================================================================

* New exercise for ear training: identify chords.
* New theory exercise: identify chords.
* Ear intervals exercise now allows to play intervals while not 
  expecting an answer
* eBooks navigation toolbar now integrated with the main toolbars.
* Print button and menu print option now also works for eBooks.
* Unicode build, to fully support internationalization.


Version 3.1 (4/Aug/2006)
=======================================================================

* First release oriented to support score edition:
      o LenMus scores imported from text files.
      o New toolbars for the score editor: play back, zoom.
      o Export scores as bmp or jpg images.
* Toolbars are now draggable and dockable.
* Improvements in score layout.
* Bug fixed: Frequency to check for program updates changes if 
  language is changed.
* Bug fixed: When playing back a score, accidentals not always
  propagated along the measure.


Version 3.0 (May/2006)
=======================================================================

* All eBooks are now available in English and Spanish.
* Buttons for answers added to 'identify scales' and 'build interval'
  exercises.
* Name of intervals now displayed in ear training exercises when
  requesting to show the solution.
* The exercise on intervals (theory) has been improved.
* The piano keyboard has been removed.
* Now, at start, automatically checks if a new version of the program
  is available.

In addition:

* Project changed to GNU free software, hosted at Sourceforge.net
* Totally reprogrammed, from Microsoft Visual Basic 6.0 to wxWidgets and
  C++. This opens the possibility to launch releases for all Linux
  variants, Mac OS X, and Windows, and relief the project from version
  2.0 technical constrains to improve.
* Teaching material now written in Windows help format

Version 2.0 (Oct/2004):
=======================================================================

* Polyphonic capabilities added.
* Support for scores with several instruments added to external 
  language LDP
* Developed an architecture for integration between scores, interactive
  exercises and written text book material, so that all exercises are
  fully integrated into the text and music scores are not just images or
  pictures but fully interactive operational music scores that the
  student can hear, in whole or just the measures selected by the
  student.
* Internationalization support added. First English release.
* For more details click here.


Version 1.0 (Jan/2004):
=======================================================================

* First release.
* A collection of non-integrated music exercises for ear-training and
  music reading.
* Only Spanish version. No internationalization support



[Since last version]: https://github.com/lenmus/lenmus/compare/Release_5.6.2...HEAD
[5.6.2]: https://github.com/lenmus/lenmus/compare/Release_5.6.1...Release_5.6.2
[5.6.1]: https://github.com/lenmus/lenmus/compare/Release_5.6.0...Release_5.6.1
[5.6.0]: https://github.com/lenmus/lenmus/compare/Release_5.5.0...Release_5.6.0
[5.5.0]: https://github.com/lenmus/lenmus/compare/Release_5.4.2...Release_5.5.0
[5.4.2]: https://github.com/lenmus/lenmus/compare/Release_5.4.1...Release_5.4.2
[5.4.1]: https://github.com/lenmus/lenmus/compare/Release_5.4...Release_5.4.1


