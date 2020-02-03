#!/usr/bin/perl
##------------------------------------------------------------------------------------
## Copyright (C) 2009-20020 LenMus proyect
##
## This script creates the website page to show the status of each po file.
## I took the idea from an script written by Naba Kumar Singh
## See: 
## http://svn.gnome.org/viewvc/anjuta/trunk/po/status.pl?revision=3079&view=markup
## http://mail.gnome.org/archives/gnome-i18n/2002-March/msg00017.html
##
##
## This program is free software; you can redistribute it and/or modify it under the 
## terms of the GNU General Public License as published by the Free Software Foundation,
## either version 3 of the License, or (at your option) any later version.
##
## This program is distributed in the hope that it will be useful, but WITHOUT ANY 
## WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
## PARTICULAR PURPOSE.  See the GNU General Public License for more details.
##
## You should have received a copy of the GNU General Public License along with this 
## program. If not, see <http://www.gnu.org/licenses/>. 
##
## For any comment, suggestion or feature request, please contact the manager of 
## the project at cecilios@users.sourceforge.net
##
# Usage
#   cd /datos/cecilio/lm/projects/lenmus/trunk/scripts/web
#   ./po-statistics.pl
##------------------------------------------------------------------------------------

    # List of supported laguages and their names
    %languages = (
        "el" => "Greek",
        "de" => "German",
        "es" => "Spanish",
        "eu" => "Basque",
        "fr" => "French",
        "gl_ES" => "Galician",
        "it" => "Italian",
        "nl" => "Dutch",
        "tr" => "Turkish",
        "zh_CN" => "Chinese (simplified)",
    );

    # status of each language
    %lang_status = (
        "el" => "maintained",
        "de" => "unknown",
        "es" => "maintained",
        "eu" => "unknown",
        "fr" => "unknown",
        "gl_ES" => "unknown",
        "it" => "unknown",
        "nl" => "unknown",
        "tr" => "unmaintained",
        "zh_CN" => "unknown",
    );


    # List of eBooks and their descriptions
    %ebooks = (
        "lenmus" => "Program user interface (GUI)",
        "GeneralExercises" => "eBook: General exercises",
        "L1_MusicReading" => "eBook: Music reading. Level 1",
        "L2_MusicReading" => "eBook: Music reading. Level 2",
        "TheoryHarmony" => "eBook: Theory and Harmony",
        "Study Guide" => "Document: Study Guide",
    );
      
    print "\n";

    # html page to create, for translation status
    my $out_file = "/datos/cecilio/WebSite/mws/content/lenmus/phonascus/html/phonascus_en/translation_status.htm";
    open (HTMLFILE, ">$out_file")
    or print STDERR "Can't open $out_file to write.\n";

    WriteHeader("HTMLFILE");


    # Analyse languages, in alphabetical order (English language name)
    foreach $langcode (sort keys %languages)
    {
        my $langname = $languages{$langcode};
        StartLanguage(HTMLFILE, $langname, $langcode);

        # main po files
        my @po_files = glob("../../locale/$langcode/*.po");
        foreach my $poFile (@po_files)
        {
            if ($poFile =~ /(\w+)\_$langcode\.po/)
            {
                my $ebook_name = $ebooks{$1};
                if ($ebook_name ne "")
                {
                    AnalyzeFile(HTMLFILE, $poFile, $langname, $langcode, $ebook_name);
                }
            }
        }

        # eBook po files
        my @po_files = glob("../../../langtool/locale/$langcode/*.po");
        foreach my $poFile (@po_files)
        {
            if ($poFile =~ /(\w+)\_$langcode\.po/)
            {
                my $ebook_name = $ebooks{$1};
                if ($ebook_name ne "")
                {
                    AnalyzeFile(HTMLFILE, $poFile, $langname, $langcode, $ebook_name);
                }
            }
        }
    }

    #terminate page
    WriteFooter("HTMLFILE");



# End of program. Functions start.
#-----------------------------------------------------------------------------

sub AnalyzeFile
{
    my($html_file, $po_file, $language, $country_code, $ebook_name) = @_;

    if (-f $po_file)
    {
        my $output = `msgfmt --statistics $po_file 2>&1`;
     
        #print $output, "\n";    ##dbg

        my ($translated, $fuzzy, $untranslated) = (0,0,0);
        my @strs = split (", ", $output);
        foreach my $value (@strs)
        {
            if ($value =~ /\btranslated messages\b/)    ##mensajes traducidos\b/)
            {
                ($translated) = split (" ", $value);
            } 
            elsif ($value =~ /\bfuzzy translations\b/)  ##traducciones difusas\b/) 
            {
                ($fuzzy) = split (" ", $value);
            } 
            elsif ($value =~ /\buntranslated messages\b/)   ##mensajes sin traducir\b/) 
            {
                ($untranslated) = split (" ", $value);
            }
        }
        my $total_messages = $translated + $fuzzy + $untranslated;

     
        print "trans=", $translated, ", fuzzy=", $fuzzy, ", untrans=", $untranslated, "\n";    ##dbg

        $untranslated = $total_messages - ($translated + $fuzzy);
     
        #print "total: ", $total_messages, "\n";    ##dbg

        if ($total_messages eq 0) 
        {
            $total_messages = 1;
        }
        $untranslated *= 100/$total_messages;
        $untranslated = ($untranslated < 0)? 0:$untranslated;
        $untranslated = ($untranslated > 100)? 100:$untranslated;

        $translated *= 100/$total_messages;
        $translated = ($translated < 0)? 0:$translated;
        $translated = ($translated > 100)? 100:$translated;

        $fuzzy *= 100/$total_messages;
        $fuzzy = ($fuzzy < 0)? 0:$fuzzy;
        $fuzzy = ($fuzzy > 100)? 100:$fuzzy;

        print "Processing $po_file\n";

        WriteTranslation($html_file, $ebook_name, $translated, $fuzzy, $untranslated);
    }

}


#-----------------------------------------------------------------------------

sub WriteTranslation
{
    my($html_file, $ebook_name, $translated, $fuzzy, $untranslated) = @_;

    printf ($html_file "<tr><td class='book'><img src='content/phonascus/images/1ptrans.gif' width='41px' height='2px'>$ebook_name</td>\n");

    printf ($html_file "   <td class='trans'>%6.2f%</td>\n", $translated);
    printf ($html_file "   <!-- Trans=%6.2f, Fuzzy=%6.2f, Untranslated=%6.2f -->\n", $translated, $fuzzy, $untranslated);
    printf ($html_file "   <td class='graph'>\n");

    if ($translated > 0.5)
    {
        printf ($html_file "<img src='content/phonascus/images/bar_green.gif' height='15' width='%.0f'>", $translated);
    }
    if ($fuzzy > 0.5)
    {
        printf ($html_file "<img src='content/phonascus/images/bar_yellow.gif' height='15' width='%.0f'>", $fuzzy);
    }
    if ($untranslated > 0.5)
    {
        printf ($html_file "<img src='content/phonascus/images/bar_red.gif' height='15' width='%.0f'>\n", $untranslated);
    }
    printf ($html_file "</td></tr>\n");

}

#-----------------------------------------------------------------------------

sub StartLanguage
{
    my($html_file, $langname, $lang) = @_;

    my %icons = (
        "maintained" => "ok.png",
        "unknown" => "question.jpg",
        "unmaintained" => "error.jpg",
    );

    my $status = $lang_status{$lang};
    my $icon = $icons{$status};

    print $html_file "<tr>
<td class='langname'>$langname [$lang]</td><td class='center'><a href='###translation_credits####'><img src='content/phonascus/images/$icon' /> <b>$status</b></a></td><td>&nbsp;</td></tr>\n";

}

#-----------------------------------------------------------------------------

sub WriteHeader
{
    my($html_file) = @_;
    my $date_time = gmtime(time());
    print ($html_file "<html>\n");
    print ($html_file "<!-- File automatically generated by po-statistics.pl. Date: $date_time -->

<head>
<meta http-equiv='content-type' content='text/html; charset=UTF-8'>
<title>Translation status</title>
<link rel='stylesheet' media='all' href='singledoc.css' type='text/css' />
</head>
<body>

<h1>Translation status</h1>

<p>This page informs you about the program translation status of the current release version of LenMus. You may find the list of the translators <a href='###translation_credits###'>here</a> .</p>

<p>If you would like to translate LenMus into your language or to help with a translation which already exists please <a href='###translation_instructions###'>go on!</a>. It is quite helpful to have several translators for one language at least for proof reading.<p>


<h2>Translations (ordered by their ISO language code)</h2>

<p>Below is a table containing the available translations. The columns of this table have an obvious meaning: in each row you will see the language and the status of the translations. Click on a language status link to see the details of the translation as well as the names of the current active translators for that language. You should contact them if you would like to submit any corrections to your language translation.</p>

<p>
<b>Legend for graphs:</b>&nbsp;&nbsp;&nbsp; <img src='content/phonascus/images/bar_green.gif' alt='translated' title='translated' width='32' height='15'/> Translated&nbsp;&nbsp;&nbsp;
<img src='content/phonascus/images/bar_yellow.gif' alt='fuzzy' title='fuzzy' width='32' height='15'/> Fuzzy&nbsp;&nbsp;&nbsp;
<img src='content/phonascus/images/bar_red.gif' alt='untranslated' title='untranslated' width='32' height='15' /> Untranslated
</p>

<p>The translations status is as follows:</p>
<ul>
  <li><img src='content/phonascus/images/ok.png' /> <b>maintained</b></span> -
  translator(s) sends updates when requested.</li>
  <li><img src='content/phonascus/images/question.jpg' /> <b>unknown</b></span> - I did not received translation updates and I don't know if the translator(s) is (are) still interested.</li>
  <li><img src='content/phonascus/images/error.jpg' /> <b>unmaintained</b></span> - I haven't heard anything from the translator(s) for two program releases or the translator explicitly informed he/she will not continue to maintain the translation.</li>
</ul>


<p>&nbsp;</p>
<table class='translations' summary='Languages ordered by ISO code'>
<tbody><tr>
<th>Language [ISO code]</th>
<th class='trans'>Status</th>
<th class='graph'>Graph</th>
</tr>

<tr><td colspan='3' height='8px'><img src='content/phonascus/images/RayaAzul.gif' width='500px' height='16px'></td></tr>
\n\n");

}

#-----------------------------------------------------------------------------

sub WriteFooter
{
    my($html_file) = @_;

    my $date_time = gmtime(time());

print $html_file "<tr><td colspan='3' height='8px'><img src='content/phonascus/images/RayaAzul.gif' width='500px' height='16px'></td></tr>

</tbody></table>

<p class='last_update'>Page automatically generated by script po-statistics.pl. Date: $date_time</p>

</body></html>\n";
}

