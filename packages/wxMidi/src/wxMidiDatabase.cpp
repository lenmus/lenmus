//=====================================================================================
// wxMidi: A MIDI interface based on PortMidi, the Portable Real-Time MIDI Library
// --------------------------------------------------------------------------------
//
// Author:      Cecilio Salmeron <s.cecilio@gmail.com>
// Copyright:   (c) 2005-2015 Cecilio Salmeron
// Licence:     wxWidgets license, version 3.1 or later at your choice.
//=====================================================================================
#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "wxMidiDatabase.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wxMidi.h"

/*
wxMidiDatabaseGM
Methods to load and manage midi instruments names and codes

The General MIDI (GM) Specification, published by the International MIDI Association,
defines a set of general capabilities for General MIDI Instruments. The General MIDI
Specification includes the definition of a General MIDI Sound Set (a patch map), a General
MIDI Percussion map (mapping of percussion sounds to note numbers), and a set of General
MIDI Performance capabilities (number of voices, types of MIDI messages recognized, etc.).
A MIDI sequence which has been generated for use on a General MIDI Instrument should play
correctly on any General MIDI synthesizer or sound module.

The General MIDI system utilizes MIDI channels 1-9 and 11-16 for chromatic instrument sounds,
while channel number 10 is utilized for "key-based" percussion sounds.
The General MIDI Sound set for channels 1-9 and 11-16 is given in table 1.
These instrument sounds are grouped into "sets" of related sounds. For example, program
numbers 1-8 are piano sounds, 6-16 are chromatic percussion sounds, 17-24 are organ sounds,
25-32 are guitar sounds, etc.

For the instrument sounds on channels 1-9 and 11-16, the note number in a Note On message
is used to select the pitch of the sound which will be played. For example if the Vibraphone
instrument (program number 12) has been selected on channel 3, then playing note number 60
on channel 3 would play the middle C note (this would be the default note to pitch assignment
on most instruments), and note number 59 on channel 3 would play B below middle C.
Both notes would be played using the Vibraphone sound.

The General MIDI percussion map used for channel 10 is given in table 2. For these "key-based"
sounds, the note number data in a Note On message is used differently. Note numbers on
channel 10 are used to select which drum sound will be played. For example, a Note On
message on channel 10 with note number 60 will play a Hi Bongo drum sound. Note number 59
on channel 10 will play the Ride Cymbal 2 sound.

It should be noted that the General MIDI system specifies sounds using program numbers 1
through 128. The MIDI Program Change message used to select these sounds uses an 8-bit byte,
which corresponds to decimal numbering from 0 through 127, to specify the desired program
number. Thus, to select GM sound number 10, the Glockenspiel, the Program Change message
will have a data byte with the decimal value 9.

The General MIDI system specifies which instrument or sound corresponds with each
program/patch number, but General MIDI does not specify how these sounds are produced.
Thus, program number 1 should select the Acoustic Grand Piano sound on any General MIDI
instrument. However, the Acoustic Grand Piano sound on two General MIDI synthesizers
which use different synthesis techniques may sound quite different.

*/

//instruments table
static wxString m_sInstrName[128] = {
      "Acoustic Grand Piano"
    , "Bright Acoustic Piano"
    , "Electric Grand Piano"
    , "Honky-tonk Piano"
    , "Electric Piano 1"
    , "Electric Piano 2"
    , "Harpsichord"
    , "Clavinet"
    , "Celesta"
    , "Glockenspiel"
    , "Music Box"
    , "Vibraphone"
    , "Marimba"
    , "Xylophone"
    , "Tubular Bells"
    , "Dulcimer"
    , "Drawbar Organ"
    , "Percussive Organ"
    , "Rock Organ"
    , "Church Organ"
    , "Reed Organ"
    , "Accordion"
    , "Harmonica"
    , "Tango Accordion"
    , "Acoustic Guitar (nylon)"
    , "Acoustic Guitar (steel)"
    , "Electric Guitar (jazz)"
    , "Electric Guitar (clean)"
    , "Electric Guitar (muted)"
    , "Overdriven Guitar"
    , "Distortion Guitar"
    , "Guitar harmonics"
    , "Acoustic Bass"
    , "Electric Bass (finger)"
    , "Electric Bass (pick)"
    , "Fretless Bass"
    , "Slap Bass 1"
    , "Slap Bass 2"
    , "Synth Bass 1"
    , "Synth Bass 2"
    , "Violin"
    , "Viola"
    , "Cello"
    , "Contrabass"
    , "Tremolo Strings"
    , "Pizzicato Strings"
    , "Orchestral Harp"
    , "Timpani"
    , "String Ensemble 1"
    , "String Ensemble 2"
    , "SynthStrings 1"
    , "SynthStrings 2"
    , "Choir Aahs"
    , "Voice Oohs"
    , "Synth Voice"
    , "Orchestra Hit"
    , "Trumpet"
    , "Trombone"
    , "Tuba"
    , "Muted Trumpet"
    , "French Horn"
    , "Brass Section"
    , "SynthBrass 1"
    , "SynthBrass 2"
    , "Soprano Sax"
    , "Alto Sax"
    , "Tenor Sax"
    , "Baritone Sax"
    , "Oboe"
    , "English Horn"
    , "Bassoon"
    , "Clarinet"
    , "Piccolo"
    , "Flute"
    , "Recorder"
    , "Pan Flute"
    , "Blown Bottle"
    , "Shakuhachi"
    , "Whistle"
    , "Ocarina"
    , "Lead 1 (square)"
    , "Lead 2 (sawtooth)"
    , "Lead 3 (calliope)"
    , "Lead 4 (chiff)"
    , "Lead 5 (charang)"
    , "Lead 6 (voice)"
    , "Lead 7 (fifths)"
    , "Lead 8 (bass + lead)"
    , "Pad 1 (new age)"
    , "Pad 2 (warm)"
    , "Pad 3 (polysynth)"
    , "Pad 4 (choir)"
    , "Pad 5 (bowed)"
    , "Pad 6 (metallic)"
    , "Pad 7 (halo)"
    , "Pad 8 (sweep)"
    , "FX 1 (rain)"
    , "FX 2 (soundtrack)"
    , "FX 3 (crystal)"
    , "FX 4 (atmosphere)"
    , "FX 5 (brightness)"
    , "FX 6 (goblins)"
    , "FX 7 (echoes)"
    , "FX 8 (sci-fi)"
    , "Sitar"
    , "Banjo"
    , "Shamisen"
    , "Koto"
    , "Kalimba"
    , "Bag pipe"
    , "Fiddle"
    , "Shanai"
    , "Tinkle Bell"
    , "Agogo"
    , "Steel Drums"
    , "Woodblock"
    , "Taiko Drum"
    , "Melodic Tom"
    , "Synth Drum"
    , "Reverse Cymbal"
    , "Guitar Fret Noise"
    , "Breath Noise"
    , "Seashore"
    , "Bird Tweet"
    , "Telephone Ring"
    , "Helicopter"
    , "Applause"
    , "Gunshot"
};

//section names
static wxString	m_sSectName[] = {
	"Keyboards",    "Chromatic percussion", "Organs and harmonicas",    "Guitars",
	"Basses",       "Strings",              "Ensembles",                "Brass",
	"Reed",         "Pipe",                 "Synth. lead",              "Synth. pad",
	"Synth. F/X",   "Ethnic",               "Percussive",               "Sound F/X"
};


//initialize the only instance
wxMidiDatabaseGM* wxMidiDatabaseGM::m_pInstance = (wxMidiDatabaseGM*)nullptr;

// ----------------------------------------------------------------------------
// wxMidiDatabaseGM ctor/dtor
// ----------------------------------------------------------------------------

wxMidiDatabaseGM::wxMidiDatabaseGM()
{
}

wxMidiDatabaseGM::~wxMidiDatabaseGM()
{
}

wxMidiDatabaseGM* wxMidiDatabaseGM::GetInstance()
{
	if (!m_pInstance) {
		m_pInstance = new wxMidiDatabaseGM();
		m_pInstance->Initialize();
	}
	return m_pInstance;
}

int wxMidiDatabaseGM::GetNumSections()
{
	return NUM_SECTIONS;
}

wxString wxMidiDatabaseGM::GetInstrumentName(int nInstr)
{
	wxASSERT(nInstr >=0 && nInstr < 128);
	return wxGetTranslation( m_sInstrName[nInstr] );
}

wxString wxMidiDatabaseGM::GetSectionName(int nSect)
{
	wxASSERT(nSect >=0 && nSect < NUM_SECTIONS);
	return wxGetTranslation( m_sSectName[nSect] );
}

int wxMidiDatabaseGM::GetNumInstrumentsInSection(int nSect)
{
	wxASSERT(nSect >=0 && nSect < NUM_SECTIONS);
	return m_nNumInstrInSection[nSect];
}

int wxMidiDatabaseGM::GetInstrFromSection(int nSect, int i)
{
	wxASSERT(nSect >=0 && nSect < NUM_SECTIONS);
	wxASSERT(i >=0 && i < m_nNumInstrInSection[nSect]);
	return m_nSectInstr[nSect][i];
}

void wxMidiDatabaseGM::PopulateWithInstruments(wxControlWithItems* pCtrol, int nSection,
                                               int nInstr, bool fAddNumber)
{
	//Populate a wxControlWithItems (wxListBox, wxCheckListBox, wxChoice, wxComboBox, ...)
	//with the list of intruments in a section.
	//Leave selected instrument number nInstr (default: the first one of the section)
    //If fAddNumber is true, added name is preceeded by instrument number

	pCtrol->Clear();
	int iSel=0;
    if (nSection < 0 || nSection > NUM_SECTIONS) {
		//error in section number. Leave control cleared
        return;
    } else {
		//populate control
		int nCurInstr;
		for (int i=0; i < m_nNumInstrInSection[nSection]; i++ )
        {
			nCurInstr = m_nSectInstr[nSection][i];
            wxString sName = (fAddNumber ? wxString::Format(_T("%d - "), nCurInstr) : _T(""));
            sName += wxGetTranslation( m_sInstrName[nCurInstr] );
            pCtrol->Append(sName);
			if (nCurInstr == nInstr) iSel = i;
        }
    }
    pCtrol->SetStringSelection( pCtrol->GetString(iSel) );

}

void wxMidiDatabaseGM::PopulateWithAllInstruments(wxControlWithItems* pCtrol, int nInstr)
{
	/*
	Populate a wxControlWithItems (wxListBox, wxCheckListBox, wxChoice, wxComboBox, ...)
	with the list of all intruments in GM standard.
	Leave selected instrument number nInstr.
	*/

	pCtrol->Clear();
	int iSel = 0;
	if (nInstr >=0 && nInstr < 128) iSel = nInstr;

	//populate control
	for (int i=0; i < 128; i++ ) {
        pCtrol->Append( wxGetTranslation(m_sInstrName[i]) );
    }
    pCtrol->SetStringSelection( pCtrol->GetString(iSel) );

}

int wxMidiDatabaseGM::PopulateWithSections(wxControlWithItems* pCtrol, int nSelInstr)
{
	/*
	Populate a wxControlWithItems (wxListBox, wxCheckListBox, wxChoice, wxComboBox, ...)
	with the list of section names.

	If nSelInstr != -1 select the section which includes instrument nSelInstr. Otherwise,
	select first section

	Returns the number of the section selected
	*/

    //load section names
	pCtrol->Clear();
    pCtrol->Append( _("Keyboards") );
    pCtrol->Append( _("Chromatic percussion") );
    pCtrol->Append( _("Organs and harmonicas") );
    pCtrol->Append( _("Guitars") );
    pCtrol->Append( _("Basses") );
    pCtrol->Append( _("Strings") );
    pCtrol->Append( _("Ensembles") );
    pCtrol->Append( _("Brass") );
    pCtrol->Append( _("Reed") );
    pCtrol->Append( _("Pipe") );
    pCtrol->Append( _("Synth. lead") );
    pCtrol->Append( _("Synth. pad") );
    pCtrol->Append( _("Synth. F/X") );
    pCtrol->Append( _("Ethnic") );
    pCtrol->Append( _("Percussive") );
    pCtrol->Append( _("Sound F/X") );

    //Find section for instrument nSelInstr
    ;
	int iSel = 0;
    if (nSelInstr != -1) {
		bool fFound = false;
		for (int iSec=0; !fFound && iSec < NUM_SECTIONS; iSec++) {
			for (int j=0; j < m_nNumInstrInSection[iSec]; j++) {
				if (m_nSectInstr[iSec][j] == nSelInstr) {
					iSel = iSec;
					fFound = true;
					break;
				}
			}
        }
    }
    pCtrol->SetStringSelection( pCtrol->GetString(iSel) );

    return iSel;

}

void wxMidiDatabaseGM::PopulateWithPercusionInstr(wxControlWithItems* pCtrol, int iSel)
{
	pCtrol->Clear();
	pCtrol->Append( _("35-Acoustic Bass Drum") );
	pCtrol->Append( _("36-Bass Drum 1") );
	pCtrol->Append( _("37-Side Stick") );
	pCtrol->Append( _("38-Acoustic Snare") );
	pCtrol->Append( _("39-Hand clap") );
	pCtrol->Append( _("40-Electric Snare") );
	pCtrol->Append( _("41_Low Floor Tom") );
	pCtrol->Append( _("42_Closed High-Hat") );
	pCtrol->Append( _("43-High Floor Tom") );
	pCtrol->Append( _("44-Pedal High-Hat") );
	pCtrol->Append( _("45-Low Tom") );
	pCtrol->Append( _("46-Open High-Hat") );
	pCtrol->Append( _("47-Low-Mid Tom") );
	pCtrol->Append( _("48-High-Mid Tom") );
	pCtrol->Append( _("49-Crash Cymbal 1") );
	pCtrol->Append( _("50-High Tom") );
	pCtrol->Append( _("51-Ride Cymbal 1") );
	pCtrol->Append( _("52-Chinese Cymbal") );
	pCtrol->Append( _("53-Ride Bell") );
	pCtrol->Append( _("54-Tambourine") );
	pCtrol->Append( _("55-Splash Cymbal") );
	pCtrol->Append( _("56-Cowbell") );
	pCtrol->Append( _("57-Crash Cymbal 2") );
	pCtrol->Append( _("58-Vibraslap") );
	pCtrol->Append( _("59-Ride Cymbal 2") );
	pCtrol->Append( _("60-High Bongo") );
	pCtrol->Append( _("61-Low Bongo") );
	pCtrol->Append( _("62-Mute High Conga") );
	pCtrol->Append( _("63-Open High Conga") );
	pCtrol->Append( _("64-Low Conga") );
	pCtrol->Append( _("65-High Timbale") );
	pCtrol->Append( _("66-Low Timbale") );
	pCtrol->Append( _("67-High Agogo") );
	pCtrol->Append( _("68-Low Agogo") );
	pCtrol->Append( _("69-Casaba") );
	pCtrol->Append( _("70-Maracas") );
	pCtrol->Append( _("71-Short Whistle") );
	pCtrol->Append( _("72-Long Whistle") );
	pCtrol->Append( _("73-Short Guiro") );
	pCtrol->Append( _("74-Long Guiro") );
	pCtrol->Append( _("75-Claves") );
	pCtrol->Append( _("76-High Wood Block") );
	pCtrol->Append( _("77-Low Wood Block") );
	pCtrol->Append( _("78-Mute Cuica") );
	pCtrol->Append( _("79-Open Cuica") );
	pCtrol->Append( _("80-Mute Triangle") );
	pCtrol->Append( _("81-Open Triangle") );

	//select item
    int i = iSel - 35;
    if (i < 0 || i > 46)  i=0;
    pCtrol->SetStringSelection( pCtrol->GetString(i) );

}

void wxMidiDatabaseGM::Initialize()
{
	//organize instruments into sections
	//All code is prepared to deal with an arbitray number of sections, each
	//containing a variable number of instruments. But for now lets use the
	//MIDI groups with 8 instruments in each group

	// WARNING: If you change this, modify constants NUM_SECTIONS and NUM_INSTRS in
	// header file!!!

	//section "Keyboards"
    m_nSectInstr[0][0] = 0;				//0 - Acoustic Grand Piano
    m_nSectInstr[0][1] = 1;				//1 - Bright Acoustic Piano
    m_nSectInstr[0][2] = 2;				//2 - Electric Grand Piano
	m_nSectInstr[0][3] = 3;				//3 - Honky-tonk Piano
	m_nSectInstr[0][4] = 4;				//4 - Electric Piano 1
	m_nSectInstr[0][5] = 5;				//5 - Electric Piano 2
	m_nSectInstr[0][6] = 6;				//6 - Harpsichord
	m_nSectInstr[0][7] = 7;				//7 - Clavinet
	m_nNumInstrInSection[0] = 8;		//number of instruments in this section

	//section "Chromatic percussion"
	m_nSectInstr[1][0] = 8; 			//8  - Celesta");			//"Celesta"
	m_nSectInstr[1][1] = 9; 			//9  - Glockenspiel");		//"Glockenspiel"
	m_nSectInstr[1][2] = 10;			//10 - Music Box");			//"Caja de música"
	m_nSectInstr[1][3] = 11;			//11 - Vibraphone");		//"Vibráfono"
	m_nSectInstr[1][4] = 12;			//12 - Marimba");			//"Marimba"
	m_nSectInstr[1][5] = 13;			//13 - Xylophone");			//"Xilófono"
	m_nSectInstr[1][6] = 14;			//14 - Tubular Bells");		//"Campanas"
	m_nSectInstr[1][7] = 15;			//15 - Dulcimer");			//"Dulcemer (salterio alemán)"
	m_nNumInstrInSection[1] = 8;		//number of instruments in this section

    //section "Organs and harmonicas"
    m_nSectInstr[2][0] = 16;			//16 - Drawbar Organ");		//"Organo Hammond"
    m_nSectInstr[2][1] = 17;			//17 - Percussive Organ");	//"Organo de percusión"
    m_nSectInstr[2][2] = 18;			//18 - Rock Organ");			//"Organo de rock"
    m_nSectInstr[2][3] = 19;			//19 - Church Organ");		//"Organo de iglesia"
    m_nSectInstr[2][4] = 20;			//20 - Reed Organ");			//"Armonio"
    m_nSectInstr[2][5] = 21;			//21 - Accordion");			//"Acordeón"
    m_nSectInstr[2][6] = 22;			//22 - Harmonica");			//"Armónica"
    m_nSectInstr[2][7] = 23;			//23 - Tango Accordion");    //"Bandoneón"
	m_nNumInstrInSection[2] = 8;		//number of instruments in this section

    //section "Guitars"
    m_nSectInstr[3][0] = 24;			//24 - Acoustic Guitar (nylon)");    //"Guitarra cuerdas de nylon"
	m_nSectInstr[3][1] = 25;			//25 - Acoustic Guitar (steel)");    //"Guitarra cuerdas metálicas"
    m_nSectInstr[3][2] = 26;			//26 - Electric Guitar (jazz)");     //"Guitarra eléctrica de jazz"
    m_nSectInstr[3][3] = 27;			//27 - Electric Guitar (clean)");    //"Guitarra eléctrica"
    m_nSectInstr[3][4] = 28;			//28 - Electric Guitar (muted)");    //"Guitarra eléctrica (con sordina)"
    m_nSectInstr[3][5] = 29;			//29 - Overdriven Guitar");			//"Guitarra eléctrica (overdriven)"
    m_nSectInstr[3][6] = 30;			//30 - Distortion Guitar");			//"Guitarra eléctrica (con distorsión)"
    m_nSectInstr[3][7] = 31;			//31 - Guitar harmonics");			//"Guitarra de armónicos"
	m_nNumInstrInSection[3] = 8;		//number of instruments in this section

    //section "Bass"
    m_nSectInstr[4][0] = 32;			//32 - Acoustic Bass");				//"Bajo acústico"
    m_nSectInstr[4][1] = 33;			//33 - Electric Bass (finger)");     //"Bajo eléctrico (finger)"
    m_nSectInstr[4][2] = 34;			//34 - Electric Bass (pick)");	    //"Bajo eléctrico (pick)"
    m_nSectInstr[4][3] = 35;			//35 - Fretless Bass");				//"Bajo sin trastes"
    m_nSectInstr[4][4] = 36;			//36 - Slap Bass 1");				//"Slap Bass 1"
    m_nSectInstr[4][5] = 37;			//37 - Slap Bass 2");				//"Slap Bass 2"
    m_nSectInstr[4][6] = 38;			//38 - Synth Bass 1");				//"Synth Bass 1"
    m_nSectInstr[4][7] = 39;			//39 - Synth Bass 2");				//"Synth Bass 2"
	m_nNumInstrInSection[4] = 8;		//number of instruments in this section

    //section "Strings"
    m_nSectInstr[5][0] = 40;			//40 - Violin");				//"Violín"
    m_nSectInstr[5][1] = 41;			//41 - Viola");				//"Viola"
    m_nSectInstr[5][2] = 42;			//42 - Cello");				//"Violoncelo"
    m_nSectInstr[5][3] = 43;			//43 - Contrabass");			//"Contrabajo"
    m_nSectInstr[5][4] = 44;			//44 - Tremolo Strings");	//"Tremolo strings"
    m_nSectInstr[5][5] = 45;			//45 - Pizzicato Strings");	//"Pizzicato strings"
    m_nSectInstr[5][6] = 46;			//46 - Orchestral Harp");	//"Arpa"
    m_nSectInstr[5][7] = 47;			//47 - Timpani");			//"Timbales"
	m_nNumInstrInSection[5] = 8;		//number of instruments in this section

    //section "Ensembles"
    m_nSectInstr[6][0] = 48;			//48 - String Ensemble 1");     //"Cuerdas"
    m_nSectInstr[6][1] = 49;			//49 - String Ensemble 2");     //"Slow Str."
    m_nSectInstr[6][2] = 50;			//50 - SynthStrings 1");     //"Syn Str. 1"
    m_nSectInstr[6][3] = 51;			//51 - SynthStrings 2");     //"Syn Str. 2"
    m_nSectInstr[6][4] = 52;			//52 - Choir Aahs");     //"Coro de ¡aahs!"
    m_nSectInstr[6][5] = 53;			//53 - Voice Oohs");     //"Coro de Oohs"
    m_nSectInstr[6][6] = 54;			//54 - Synth Voice");     //"Voces sintetizadas"
    m_nSectInstr[6][7] = 55;			//55 - Orchestra Hit");     //"Orchestra Hit"
	m_nNumInstrInSection[6] = 8;		//number of instruments in this section

    //section "Brass"
    m_nSectInstr[7][0] = 56;			//56 - Trumpet");     //"Trompeta"
    m_nSectInstr[7][1] = 57;			//57 - Trombone");     //"Trombón"
    m_nSectInstr[7][2] = 58;			//58 - Tuba");     //"Tuba"
    m_nSectInstr[7][3] = 59;			//59 - Muted Trumpet");     //"Trompeta con sordina"
    m_nSectInstr[7][4] = 60;			//60 - French Horn");     //"Trompa"
    m_nSectInstr[7][5] = 61;			//61 - Brass Section");     //"Grupo de viento-metal"
    m_nSectInstr[7][6] = 62;			//62 - SynthBrass 1");     //"Synth Br.1"
    m_nSectInstr[7][7] = 63;			//63 - SynthBrass 2");     //"Synth Br.2"
	m_nNumInstrInSection[7] = 8;		//number of instruments in this section

    //section "Reed"
    m_nSectInstr[8][0] = 64;			//64 - Soprano Sax");     //"Saxofón soprano"
    m_nSectInstr[8][1] = 65;			//65 - Alto Sax");     //"Saxofón contralto"
    m_nSectInstr[8][2] = 66;			//66 - Tenor Sax");     //"Saxofón tenor"
    m_nSectInstr[8][3] = 67;			//67 - Baritone Sax");     //"Saxofón barítono"
    m_nSectInstr[8][4] = 68;			//68 - Oboe");     //"Oboe"
    m_nSectInstr[8][5] = 69;			//69 - English Horn");     //"Corno ingles"
    m_nSectInstr[8][6] = 70;			//70 - Bassoon");     //"Fagot"
    m_nSectInstr[8][7] = 71;			//71 - Clarinet");     //"Clarinete"
	m_nNumInstrInSection[8] = 8;		//number of instruments in this section

    //section "Pipe"
    m_nSectInstr[9][0] = 72;			//72 - Piccolo");     //"Falutín"
    m_nSectInstr[9][1] = 73;			//73 - Flute");     //"Flauta travesera"
    m_nSectInstr[9][2] = 74;			//74 - Recorder");     //"Flauta dulce"
    m_nSectInstr[9][3] = 75;			//75 - Pan Flute");     //"Flauta de Pan"
    m_nSectInstr[9][4] = 76;			//76 - Blown Bottle");     //"Botella soplada"
    m_nSectInstr[9][5] = 77;			//77 - Shakuhachi");     //"Shakuhachi"
    m_nSectInstr[9][6] = 78;			//78 - Whistle");     //"Silbato"
    m_nSectInstr[9][7] = 79;			//79 - Ocarina");     //"Ocarina"
	m_nNumInstrInSection[9] = 8;		//number of instruments in this section

    //section "Synth. lead"
    m_nSectInstr[10][0] = 80;			//80 - Lead 1 (square)");     //"Onda cuadrada"
    m_nSectInstr[10][1] = 81;			//81 - Lead 2 (sawtooth)");     //"Onda en diente de sierra"
    m_nSectInstr[10][2] = 82;			//82 - Lead 3 (calliope)");     //"Calliope"
    m_nSectInstr[10][3] = 83;			//83 - Lead 4 (chiff)");     //"Chiffer Lead"
    m_nSectInstr[10][4] = 84;			//84 - Lead 5 (charang)");     //"Charanga"
    m_nSectInstr[10][5] = 85;			//85 - Lead 6 (voice)");     //"Voz"
    m_nSectInstr[10][6] = 86;			//86 - Lead 7 (fifths)");     //"Quintas"
    m_nSectInstr[10][7] = 87;			//87 - Lead 8 (bass + lead)");     //"Bass& Lead"
	m_nNumInstrInSection[10] = 8;		//number of instruments in this section

    //section "Synth. pad"
    m_nSectInstr[11][0] = 88;			//88 - Pad 1 (new age)");     //"Fantasia"
    m_nSectInstr[11][1] = 89;			//89 - Pad 2 (warm)");     //"Warm Pad"
    m_nSectInstr[11][2] = 90;			//90 - Pad 3 (polysynth)");     //"PolySynth"
    m_nSectInstr[11][3] = 91;			//91 - Pad 4 (choir)");     //"Space Voice"
    m_nSectInstr[11][4] = 92;			//92 - Pad 5 (bowed)");     //"Bowed Glass"
    m_nSectInstr[11][5] = 93;			//93 - Pad 6 (metallic)");     //"Metal Pad"
    m_nSectInstr[11][6] = 94;			//94 - Pad 7 (halo)");     //"Halo Pad"
    m_nSectInstr[11][7] = 95;			//95 - Pad 8 (sweep)");     //"Sweep Pad"
	m_nNumInstrInSection[11] = 8;		//number of instruments in this section

    //section "Synth. F/X"
    m_nSectInstr[12][0] = 96;			//96 - FX 1 (rain)");     //"Ice Rain"
    m_nSectInstr[12][1] = 97;			//97 - FX 2 (soundtrack)");     //"Sound Track"
    m_nSectInstr[12][2] = 98;			//98 - FX 3 (crystal)");     //"Crystal"
    m_nSectInstr[12][3] = 99;			//99 - FX 4 (atmosphere)");     //"Atmosphere"
    m_nSectInstr[12][4] = 100;			//100 - FX 5 (brightness)");    //"Brightness"
    m_nSectInstr[12][5] = 101;			//101 - FX 6 (goblins)");    //"Goblin"
    m_nSectInstr[12][6] = 102;			//102 - FX 7 (echoes)");    //"Echo Drops"
    m_nSectInstr[12][7] = 103;			//103 - FX 8 (sci-fi)");    //"Star Theme"
	m_nNumInstrInSection[12] = 8;		//number of instruments in this section

    //section "Ethnic"
    m_nSectInstr[13][0] = 104;			//104 - Sitar");    //"Sitar"
    m_nSectInstr[13][1] = 105;			//105 - Banjo");    //"Banjo"
    m_nSectInstr[13][2] = 106;			//106 - Shamisen");    //"Shamisen"
    m_nSectInstr[13][3] = 107;			//107 - Koto");    //"Koto"
    m_nSectInstr[13][4] = 108;			//108 - Kalimba");    //"Kalimba"
    m_nSectInstr[13][5] = 109;			//109 - Bag pipe");    //"Gaita"
    m_nSectInstr[13][6] = 110;			//110 - Fiddle");    //"Fiddle"
    m_nSectInstr[13][7] = 111;			//111 - Shanai");    //"Shanai"
	m_nNumInstrInSection[13] = 8;		//number of instruments in this section

    //section "Percussive"
    m_nSectInstr[14][0] = 112;			//112 - Tinkle Bell");    //"Tinkle Bell"
    m_nSectInstr[14][1] = 113;			//113 - Agogo");    //"Agogo"
    m_nSectInstr[14][2] = 114;			//114 - Steel Drums");    //"Steel Drums"
    m_nSectInstr[14][3] = 115;			//115 - Woodblock");    //"Woodblock"
    m_nSectInstr[14][4] = 116;			//116 - Taiko Drum");    //"Taiko"
    m_nSectInstr[14][5] = 117;			//117 - Melodic Tom");    //"Melo Tom 1"
    m_nSectInstr[14][6] = 118;			//118 - Synth Drum");    //"Synth Drum"
    m_nSectInstr[14][7] = 119;			//119 - Reverse Cymbal");    //"Reverse Cym."
	m_nNumInstrInSection[14] = 8;		//number of instruments in this section

    //section "Sound F/X"
    m_nSectInstr[15][0] = 120;			//120 - Guitar Fret Noise");    //"Gt.FretNoise"
    m_nSectInstr[15][1] = 121;			//121 - Breath Noise");    //"Breath Noise"
    m_nSectInstr[15][2] = 122;			//122 - Seashore");    //"Seashore"
    m_nSectInstr[15][3] = 123;			//123 - Bird Tweet");    //"Bird"
    m_nSectInstr[15][4] = 124;			//124 - Telephone Ring");    //"Telephone 1"
    m_nSectInstr[15][5] = 125;			//125 - Helicopter");    //"Helicopter"
    m_nSectInstr[15][6] = 126;			//126 - Applause");    //"Applause"
    m_nSectInstr[15][7] = 127;			//127 - Gunshot");    //"Gun Shot"
	m_nNumInstrInSection[15] = 8;		//number of instruments in this section

}

/*

The Roland General Synthesizer (GS) Standard
The Roland General Synthesizer (GS) functions are a superset of those specified for
General MIDI. The GS system includes all of the GM sounds (which are referred to as
"capital instrument" sounds), and adds new sounds which are organized as variations
of the capital instruments.

Variations are selected using the MIDI Control Change message in conjunction with the
Program Change message. The Control Change message is sent first, and it is used to set
controller number 0 to some specified nonzero value indicating the desired variation
(some capital sounds have several different variations). The Control Change message
is followed by a MIDI Program Change message which indicates the program number of the
related capital instrument. For example, Capital instrument number 25 is the Nylon String
Guitar. The Ukulele is a variation of this instrument. The Ukulele is selected by sending
a Control Change message which sets controller number 0 to a value of 8, followed by a
program change message on the same channel which selects program number 25. Sending
the Program change message alone would select the capital instrument, the Nylon String
Guitar. Note also that a Control Change of controller number 0 to a value of 0 followed
by a Program Change message would also select the capital instrument.

The GS system also includes adjustable reverberation and chorus effects. The effects
depth for both reverb and chorus may be adjusted on an individual MIDI channel basis
using Control Change messages. The type of reverb and chorus sounds employed may also
be selected using System Exclusive messages.

*/
