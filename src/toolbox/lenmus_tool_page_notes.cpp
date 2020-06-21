//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2015 LenMus project
//
//    This program is free software; you can redistribute it and/or modify it under the
//    terms of the GNU General Public License as published by the Free Software Foundation,
//    either version 3 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this
//    program. If not, see <http://www.gnu.org/licenses/>.
//
//    For any comment, suggestion or feature request, please contact the manager of
//    the project at cecilios@users.sourceforge.net
//
//---------------------------------------------------------------------------------------

//lenmus
#include "lenmus_tool_box.h"
#include "lenmus_tool_page_notes.h"
#include "lenmus_tool_group.h"
#include "lenmus_tool_box_events.h"
#include "lenmus_button.h"

//lomse
#include <lomse_selections.h>
#include <lomse_document_cursor.h>
#include <lomse_internal_model.h>
using namespace lomse;

//wxWidgets
#include <wx/wxprec.h>
#ifndef WX_PRECOMP
    #include <wx/wx.h>
    #include <wx/xrc/xmlres.h>
    #include <wx/bmpcbox.h>
    #include <wx/statline.h>
    #include <wx/clrpicker.h>
#endif



namespace lenmus
{


#define lmSPACING 5

//event IDs
#define lm_NUM_NR_BUTTONS   2       //note or rest
#define lm_NUM_DUR_BUTTONS  10
#define lm_NUM_ACC_BUTTONS  8
#define lm_NUM_DOT_BUTTONS  3
#define lm_NUM_OCTAVE_BUTTONS 10
#define lm_NUM_VOICE_BUTTONS 8
//#define lm_NUM_MOUSE_MODE_BUTTONS 2

enum {
	lmID_BT_NoteRest = 2600,
	lmID_BT_NoteDuration = lmID_BT_NoteRest + lm_NUM_NR_BUTTONS,
    lmID_BT_NoteAcc = lmID_BT_NoteDuration + lm_NUM_DUR_BUTTONS,
    lmID_BT_NoteDots = lmID_BT_NoteAcc + lm_NUM_ACC_BUTTONS,
    lmID_BT_Tie = lmID_BT_NoteDots + lm_NUM_DOT_BUTTONS,
    lmID_BT_Tuplet,
    lmID_BT_ToggleStem,
    lmID_BT_Beam_Cut,
    lmID_BT_Beam_Join,
    lmID_BT_Beam_Flatten,
    lmID_BT_Beam_Subgroup,
	lmID_BT_Octave,
	lmID_BT_Voice = lmID_BT_Octave + lm_NUM_OCTAVE_BUTTONS,
    //lmID_BT_MouseMode_Pointer,
    //lmID_BT_MouseMode_DataEntry,
};


//=======================================================================================
// ToolPageNotes implementation
//=======================================================================================

IMPLEMENT_ABSTRACT_CLASS(ToolPageNotes, ToolPage)


//---------------------------------------------------------------------------------------
ToolPageNotes::ToolPageNotes(wxWindow* parent)
{
    Create(parent);
}

//---------------------------------------------------------------------------------------
void ToolPageNotes::Create(wxWindow* parent)
{
    //base class
    ToolPage::CreatePage(parent, k_page_notes);

    //other data initialization
    m_sPageToolTip = _("Edit tools for notes and rests");
    m_sPageBitmapName = "tool_notes";
    m_title = _("Notes and rests");
    m_selector = m_title;
}

////---------------------------------------------------------------------------------------
//ENoteHeads ToolPageNotes::GetNoteheadType()
//{
//    return k_notehead_quarter; //(ENoteHeads)m_pCboNotehead->GetSelection();
//}

////---------------------------------------------------------------------------------------
//wxString ToolPageNotes::GetToolShortDescription()
//{
//    //returns a short description of the selected tool. This description is used to
//    //be displayed in the status bar
//
//    if (IsNoteSelected())
//        return _("Add note");
//    else
//        return _("Add rest");
//}

////---------------------------------------------------------------------------------------
//void ToolPageNotes::synchronize_with_selection(bool fEnable, SelectionSet* pSelection)
//{
//    //enable toolbox options depending on current selected objects
//
//    //flags to enable/disable tools
//    bool fEnableTie = false;
//    bool fCheckTie = false;
//    bool fEnableTuplet = false;
//    bool fCheckTuplet = false;
//    bool fEnableJoinBeam = false;
//    bool fEnableToggleStem = false;
//
//    if (fEnable && !pSelection->empty())
//    {
//        //find common values for all selected notes, if any.
//        //This is necessary for highlighting the accidentals, dots and voice tools
//        bool fNoteFound = false;
//        int nAcc, nDots, nDuration;
//        ColStaffObjs* pCollection = pSelection->get_staffobjs_collection();
//        if (pCollection)
//        {
//            ColStaffObjsIterator it;
//            for (it = pCollection->begin(); it != pCollection->end(); ++it)
//            {
//                ImoObj* pImo = (*it)->imo_object();
//                if (pImo->is_note_rest())
//                {
//                    ImoNoteRest* pNR = static_cast<ImoNoteRest*>(pImo);
//                    int nThisDuration = (int)pNR->get_note_type() - 1;
//                    int nThisDots = pNR->get_dots() - 1;
//                    int nThisAcc = -10;
//                    if (pImo->is_note())
//                    {
//                        ImoNote* pNote = static_cast<ImoNote*>(pImo);
//                        nThisAcc = pNote->get_notated_accidentals();
//                    }
//                    if (!fNoteFound)
//                    {
//                        fNoteFound = true;
//                        nDuration = nThisDuration;
//                        nDots = nThisDots;
//                        nAcc = nThisAcc;
//                    }
//                    else
//                    {
//                        if (nDuration != nThisDuration)
//                            nDuration = -1;
//                        if (nDots != nThisDots)
//                            nDots = -1;
//                        if (nAcc != nThisAcc)
//                            nAcc = -10;
//                    }
//                }
//            }
//
//            //if any note found, proceed to sync. the toolbox buttons for
//            //note type, accidentals and dots
//            if (fNoteFound)
//            {
////                //save current options
////                if (!m_fToolBoxSavedOptions)
////                {
////                    m_fToolBoxSavedOptions = true;
////                    m_nTbAcc = pPage->GetNoteAccButton();
////                    m_nTbDots = pPage->GetNoteDotsButton();
////                    m_nTbDuration = pPage->GetNoteDurationButton();
////                }
////                //translate Acc
////                switch(nAcc)
////                {
////                    case -2:  nAcc = 3;  break;
////                    case -1:  nAcc = 1;  break;
////                    case  0:  nAcc = -1; break;
////                    case  1:  nAcc = 2;  break;
////                    case  2:  nAcc = 4;  break;
////                    default:
////                        nAcc = -1;
////                }
//
//                SetNoteDotsButton(nDots);
//                SetNoteAccButton(nAcc - 1);
//                SetNoteDurationButton( nDuration );
//            }
//        }
//
//
//        //Ties status
//        if (pSelection->is_valid_to_add_tie())
//        {
//            fEnableTie = true;
//            fCheckTie = false;
//        }
//        else if (pSelection->is_valid_to_remove_tie())
//        {
//            fEnableTie = true;
//            fCheckTie = true;
//        }
//
//        //add/remove tuplet
//        if (pSelection->is_valid_to_add_tuplet())
//        {
//            fEnableTuplet = true;
//            fCheckTuplet = false;
//        }
//        else if (pSelection->is_valid_to_remove_tuplet())
//        {
//            fEnableTuplet = true;
//            fCheckTuplet = true;
//        }
//
//        //toggle stems
//        fEnableToggleStem = pSelection->is_valid_for_toggle_stem();
//
//        //Join beams
//        fEnableJoinBeam = pSelection->is_valid_for_join_beam();
//
//    }
//
//    //proceed to enable/disable tools
//
//    //Group Note Modifiers
//        //Ties
//    m_pGrpModifiers->EnableTool(k_tool_note_tie, fEnableTie);
//    if (fEnableTie)
//        SetToolTie(fCheckTie);
//
//        //Tuples
//    m_pGrpModifiers->EnableTool(k_tool_note_tuplet, fEnableTuplet);
//    if (fEnableTuplet)
//        SetToolTuplet(fCheckTuplet);
//
//        //Toggle stems
//    m_pGrpModifiers->EnableTool(k_tool_note_toggle_stem, fEnableToggleStem);
//    SetToolToggleStem(false);
//
//    //Group Beams
//        //Join beams
//    m_pGrpBeams->EnableTool(k_tool_beams_join, fEnableJoinBeam);
//}
//
////---------------------------------------------------------------------------------------
//bool ToolPageNotes::is_valid_for_cut_beam(ImoStaffObj* pSO)
//{
//    //Returns TRUE if object pointed by cursor is valid for breaking a beam.
//
//    //Conditions to be valid:
//    //  The object must be a note/rest in a beam
//    //  It must not be the first one in the beam
//
//    if (pSO && pSO->is_note_rest())
//    {
//        ImoNoteRest* pNR = static_cast<ImoNoteRest*>(pSO);
//        if (pNR->is_beamed())
//        {
//            //verify that it is not the first object in the beam
//            ImoBeam* pBeam = pNR->get_beam();
//            if (pSO != pBeam->get_start_object())
//                return true;
//        }
//    }
//    return false;
//}


//=======================================================================================
// GrpNoteDuration implementation
//=======================================================================================
GrpNoteDuration::GrpNoteDuration(ToolPage* pParent, wxBoxSizer* pMainSizer)
        : ToolButtonsGroup(pParent, k_group_type_options, lm_NUM_DUR_BUTTONS,
                             lmTBG_ONE_SELECTED, pMainSizer,
                             lmID_BT_NoteDuration, k_tool_note_duration, pParent->GetColors())
{
}

//---------------------------------------------------------------------------------------
void GrpNoteDuration::create_controls_in_group(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    set_group_title(_("Duration"));
    wxBoxSizer* pCtrolsSizer = create_main_sizer_for_group(pMainSizer);

    //create the specific controls for this group
    wxBoxSizer* pButtonsSizer = nullptr;
	for (int iB=0; iB < lm_NUM_DUR_BUTTONS; iB++)
	{
		if (iB % 5 == 0) {
			pButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
			pCtrolsSizer->Add(pButtonsSizer);
		}

		m_pButton[iB] = new CheckButton(this, lmID_BT_NoteDuration+iB, wxBitmap(24, 24),
                                        wxDefaultPosition, wxSize(24, 24));
		pButtonsSizer->Add(m_pButton[iB], wxSizerFlags(0).Border(wxALL, 2) );
	}
    SetButtonsBitmaps(true);    //true: bitmaps for notes
	this->Layout();

	SelectButton(3);	//select quarter note
}

//---------------------------------------------------------------------------------------
ENoteType GrpNoteDuration::GetNoteDuration()
{
    //AWARE: this assumes that buttons are ordered and that first button is 'double whole'
    //m_nSelButton
    //              k_longa = 0
    //   0          k_breve = 1
    //   1          k_whole = 2
    //   2          k_half = 3
    //   3          k_quarter = 4
    //   4          k_eighth = 5
    //   5          k_16th = 6
    //   6          k_32nd = 7
    //   7          k_64th = 8
    //   8          k_128th = 9
    //   9          k_256th = 10
    return static_cast<ENoteType>(m_nSelButton + 1);
}

//---------------------------------------------------------------------------------------
void GrpNoteDuration::SetButtonsBitmaps(bool fNotes)
{
    //Set buttons bitmap for rest or for notes, depending on flag fNotes

    const wxString sNoteBmps[lm_NUM_DUR_BUTTONS] = {
        "note_0",
        "note_1",
        "note_2",
        "note_4",
        "note_8",
        "note_16",
        "note_32",
        "note_64",
        "note_128",
        "note_256",
    };
    const wxString sRestBmps[lm_NUM_DUR_BUTTONS] = {
        "rest_0",
        "rest_1",
        "rest_2",
        "rest_4",
        "rest_8",
        "rest_16",
        "rest_32",
        "rest_64",
        "rest_128",
        "rest_256",
    };

    wxSize btSize(24, 24);
    if (fNotes)
	    for (int iB=0; iB < lm_NUM_DUR_BUTTONS; iB++)
	    {
            m_pButton[iB]->SetBitmapUp(sNoteBmps[iB], "", btSize);
            m_pButton[iB]->SetBitmapDown(sNoteBmps[iB], "button_selected_flat", btSize);
            m_pButton[iB]->SetBitmapOver(sNoteBmps[iB], "button_over_flat", btSize);
	    }
    else
	    for (int iB=0; iB < lm_NUM_DUR_BUTTONS; iB++)
	    {
            m_pButton[iB]->SetBitmapUp(sRestBmps[iB], "", btSize);
            m_pButton[iB]->SetBitmapDown(sRestBmps[iB], "button_selected_flat", btSize);
            m_pButton[iB]->SetBitmapOver(sRestBmps[iB], "button_over_flat", btSize);
	    }
}

//---------------------------------------------------------------------------------------
bool GrpNoteDuration::process_command(int cmd)
{
    switch(cmd)
    {
        case k_cmd_note_duration_longa:
            return false;
        case k_cmd_note_duration_breve:
            SelectButton(0);
            return true;
        case k_cmd_note_duration_whole:
            SelectButton(1);
            return true;
        case k_cmd_note_duration_half:
            SelectButton(2);
            return true;
        case k_cmd_note_duration_quarter:
            SelectButton(3);
            return true;
        case k_cmd_note_duration_eight:
            SelectButton(4);
            return true;
        case k_cmd_note_duration_16th:
            SelectButton(5);
            return true;
        case k_cmd_note_duration_32nd:
            SelectButton(6);
            return true;
        case k_cmd_note_duration_64th:
            SelectButton(7);
            return true;
        case k_cmd_note_duration_128th:
            SelectButton(8);
            return true;
        case k_cmd_note_duration_256th:
            SelectButton(9);
            return true;
        default:
            return false;   //not processed
    }
}

//---------------------------------------------------------------------------------------
void GrpNoteDuration::update_tools_info(ToolsInfo* pInfo)
{
    pInfo->noteType = GetNoteDuration();
}

//---------------------------------------------------------------------------------------
void GrpNoteDuration::synchronize_with_cursor(bool fEnable, DocCursor* WXUNUSED(pCursor))
{
    //TODO
    EnableGroup(fEnable);
}

//---------------------------------------------------------------------------------------
void GrpNoteDuration::synchronize_with_selection(bool fEnable, SelectionSet* pSelection)
{
    //enable toolbox options depending on current selected objects
    if (fEnable && !pSelection->empty())
    {
        //find common values for all selected notes, if any.
        //This is necessary for highlighting the duration
        bool fNoteFound = false;
        int  nDuration;
        ColStaffObjs* pCollection = pSelection->get_staffobjs_collection();
        if (pCollection)
        {
            ColStaffObjsIterator it;
            for (it = pCollection->begin(); it != pCollection->end(); ++it)
            {
                ImoObj* pImo = (*it)->imo_object();
                if (pImo->is_note_rest())
                {
                    ImoNoteRest* pNR = static_cast<ImoNoteRest*>(pImo);
                    int nThisDuration = (int)pNR->get_note_type() - 1;
                    if (!fNoteFound)
                    {
                        fNoteFound = true;
                        nDuration = nThisDuration;
                    }
                    else
                    {
                        if (nDuration != nThisDuration)
                            nDuration = -1;
                    }
                }
            }

            //if any note found, proceed to sync. the toolbox buttons for
            //note type
            if (fNoteFound)
                SelectButton( nDuration );
        }
    }
}


//=======================================================================================
// GrpOctave implementation
//=======================================================================================
GrpOctave::GrpOctave(ToolPage* pParent, wxBoxSizer* pMainSizer)
        : ToolButtonsGroup(pParent, k_group_type_options, lm_NUM_OCTAVE_BUTTONS,
                             lmTBG_ONE_SELECTED, pMainSizer,
                             lmID_BT_Octave, k_tool_octave, pParent->GetColors())
{
}

//---------------------------------------------------------------------------------------
void GrpOctave::create_controls_in_group(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    wxString sTitle = _("Octave");
    set_group_title(sTitle + " (Ctrl)");
    wxBoxSizer* pCtrolsSizer = create_main_sizer_for_group(pMainSizer);

    wxBoxSizer* pButtonsSizer = nullptr;
    wxSize btSize(16, 16);
	for (int iB=0; iB < lm_NUM_OCTAVE_BUTTONS; iB++)
	{
		if (iB % 9 == 0) {
			pButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
			pCtrolsSizer->Add(pButtonsSizer);
		}

		wxString sBtName = wxString::Format("opt_num%1d", iB);
		wxString sToolTip = wxString::Format(_("Select octave %d"), iB);
        sToolTip += ". (Ctrl + num/+/-)";
		m_pButton[iB] = new CheckButton(this, lmID_BT_Octave+iB, wxBitmap(16, 16),
                                        wxDefaultPosition, wxSize(16, 16));
        m_pButton[iB]->SetBitmapUp(sBtName, "", btSize);
        m_pButton[iB]->SetBitmapDown(sBtName, "button_selected_flat", btSize);
        m_pButton[iB]->SetBitmapOver(sBtName, "button_over_flat", btSize);
		m_pButton[iB]->SetToolTip(sToolTip);
		pButtonsSizer->Add(m_pButton[iB], wxSizerFlags(0).Border(wxALL, 0) );
	}
	this->Layout();

	SelectButton(4);	//select octave 4
}

//---------------------------------------------------------------------------------------
void GrpOctave::SetOctave(bool fUp)
{
    if (fUp)
    {
        if (m_nSelButton < 9)
            SelectButton(++m_nSelButton);
    }
    else
    {
        if (m_nSelButton > 0)
            SelectButton(--m_nSelButton);
    }
}

//---------------------------------------------------------------------------------------
bool GrpOctave::process_key(wxKeyEvent& event)
{
    //increment/decrement octave: up (ctrl +), down (ctrl -)
    //Select octave:            ctrl + digits 0..9

    int nKeyCode = event.GetKeyCode();
    if (event.CmdDown())
    {
        if (nKeyCode == int('+') || nKeyCode == int('-'))
        {
            SetOctave(nKeyCode == int('+'));
            return true;
        }
        else if (nKeyCode >= int('0') && nKeyCode <= int('9'))
        {
            SetOctave(nKeyCode - int('0'));
            return true;
        }
    }
	return false;
}

//---------------------------------------------------------------------------------------
void GrpOctave::update_tools_info(ToolsInfo* pInfo)
{
    pInfo->octave = GetOctave();
}

//---------------------------------------------------------------------------------------
void GrpOctave::synchronize_with_cursor(bool WXUNUSED(fEnable),
                                        DocCursor* WXUNUSED(pCursor))
{
    EnableGroup(true);
}

//---------------------------------------------------------------------------------------
void GrpOctave::synchronize_with_selection(bool WXUNUSED(fEnable),
                                           SelectionSet* WXUNUSED(pSelection))
{
    EnableGroup(true);
}



//=======================================================================================
// GrpVoice implementation
//=======================================================================================
GrpVoice::GrpVoice(ToolPage* pParent, wxBoxSizer* pMainSizer, int nNumButtons)
        : ToolButtonsGroup(pParent, k_group_type_options, nNumButtons, lmTBG_ONE_SELECTED,
                             pMainSizer, lmID_BT_Voice, k_tool_voice,
                             pParent->GetColors())
{
}

//---------------------------------------------------------------------------------------
void GrpVoice::SetVoice(bool fUp)
{
    if (fUp)
    {
        if (m_nSelButton < 8)
            SelectButton(++m_nSelButton);
    }
    else
    {
        if (m_nSelButton > 0)
            SelectButton(--m_nSelButton);
    }
}

//---------------------------------------------------------------------------------------
bool GrpVoice::process_key(wxKeyEvent& event)
{
    //increment/decrement voice: up (alt +), down (alt -)
    //select voice: Atl+<num>

    int nKeyCode = event.GetKeyCode();
    if (event.AltDown())
    {
        if (nKeyCode == int('+') || nKeyCode == int('-'))
        {
            SetVoice(nKeyCode == int('+'));
            return true;
        }
        else if (nKeyCode >= int('0') && nKeyCode <= int('9'))
        {
            SetVoice(nKeyCode - int('0'));
            return true;
        }
    }
	return false;
}

//---------------------------------------------------------------------------------------
void GrpVoice::update_tools_info(ToolsInfo* pInfo)
{
    pInfo->voice = GetVoice();
}

//---------------------------------------------------------------------------------------
void GrpVoice::synchronize_with_cursor(bool WXUNUSED(fEnable),
                                       DocCursor* WXUNUSED(pCursor))
{
    EnableGroup(true);
}

//---------------------------------------------------------------------------------------
void GrpVoice::synchronize_with_selection(bool WXUNUSED(fEnable),
                                          SelectionSet* WXUNUSED(pSelection))
{
    EnableGroup(true);
}


//=======================================================================================
// Group for voice number: standard group
//=======================================================================================
GrpVoiceStd::GrpVoiceStd(ToolPage* pParent, wxBoxSizer* pMainSizer)
        : GrpVoice(pParent, pMainSizer, lm_NUM_VOICE_BUTTONS)
{
}

//---------------------------------------------------------------------------------------
void GrpVoiceStd::create_controls_in_group(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    wxString sTitle = _("Voice");
    set_group_title(sTitle + " (Alt)");
    wxBoxSizer* pCtrolsSizer = create_main_sizer_for_group(pMainSizer);

    wxBoxSizer* pButtonsSizer = nullptr;
    wxSize btSize(16, 16);
	for (int iB=0; iB < lm_NUM_VOICE_BUTTONS; iB++)
	{
		if (iB % 9 == 0) {
			pButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
			pCtrolsSizer->Add(pButtonsSizer);
		}

		wxString sBtName = wxString::Format("opt_num%1d", iB+1);
        m_pButton[iB] = new CheckButton(this, lmID_BT_Voice+iB, wxBitmap(16, 16),
                                        wxDefaultPosition, wxSize(16, 16));
        wxString sTip = wxString::Format(_("Select voice %d"), iB+1);
        sTip += ". (Alt + num/+/-)";
        m_pButton[iB]->SetToolTip(sTip);
        m_pButton[iB]->SetBitmapUp(sBtName, "", btSize);
        m_pButton[iB]->SetBitmapDown(sBtName, "button_selected_flat", btSize);
        m_pButton[iB]->SetBitmapOver(sBtName, "button_over_flat", btSize);
		pButtonsSizer->Add(m_pButton[iB], wxSizerFlags(0).Border(wxALL, 0) );
	}
	this->Layout();

	SelectButton(0);	//select voice 1
}



//=======================================================================================
// Group for voice number: for harmony exercises
//=======================================================================================
GrpVoiceHarmony::GrpVoiceHarmony(ToolPage* pParent, wxBoxSizer* pMainSizer)
        : GrpVoice(pParent, pMainSizer, 4)
{
}

//---------------------------------------------------------------------------------------
void GrpVoiceHarmony::create_controls_in_group(wxBoxSizer* pMainSizer)
{
    //voice names
    static const wxString sBtName[4] = { "opt_voice_S", "opt_voice_A",
                                         "opt_voice_T", "opt_voice_B" };
    const wxString sTipStr[4] = { _("Select voice Soprano"), _("Select voice Alto"),
                                  _("Select voice Tenor"), _("Select voice Bass") };

    //create the common controls for a group
    set_group_title(_("Voice (Alt)"));
    wxBoxSizer* pCtrolsSizer = create_main_sizer_for_group(pMainSizer);

    wxBoxSizer* pButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
    pCtrolsSizer->Add(pButtonsSizer);

    wxSize btSize(16, 16);
	for (int iB=0; iB < 4; iB++)
	{
		m_pButton[iB] = new CheckButton(this, lmID_BT_Voice+iB, wxBitmap(16, 16),
                                        wxDefaultPosition, wxSize(16, 16));
        wxString sTip = sTipStr[iB] + ". (Alt + num/+/-)";
		m_pButton[iB]->SetToolTip(sTip);

        m_pButton[iB]->SetBitmapUp(sBtName[iB], "", btSize);
        m_pButton[iB]->SetBitmapDown(sBtName[iB], "button_selected_flat", btSize);
        m_pButton[iB]->SetBitmapOver(sBtName[iB], "button_over_flat", btSize);
		pButtonsSizer->Add(m_pButton[iB], wxSizerFlags(0).Border(wxALL, 0) );
	}
	this->Layout();

	SelectButton(0);	//select voice Soprano
}



//=======================================================================================
// GrpNoteAcc implementation
//=======================================================================================
GrpNoteAcc::GrpNoteAcc(ToolPage* pParent, wxBoxSizer* pMainSizer)
        : ToolButtonsGroup(pParent, k_group_type_options, lm_NUM_ACC_BUTTONS,
                             lmTBG_ALLOW_NONE, pMainSizer,
                             lmID_BT_NoteAcc, k_tool_accidentals, pParent->GetColors())
{
}

//---------------------------------------------------------------------------------------
void GrpNoteAcc::create_controls_in_group(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    set_group_title(_("Accidentals"));
    wxBoxSizer* pCtrolsSizer = create_main_sizer_for_group(pMainSizer);

    //create the specific controls for this group
    const wxString sButtonBmps[lm_NUM_ACC_BUTTONS] = {
	    "acc_natural",
	    "acc_flat",
	    "acc_sharp",
	    "acc_flat_flat",
	    "acc_double_sharp",
	    "acc_sharp_sharp",
	    "acc_natural_flat",
	    "acc_natural_sharp",
    };

    wxBoxSizer* pButtonsSizer = nullptr;
    wxSize btSize(24, 24);
	for (int iB=0; iB < lm_NUM_ACC_BUTTONS; iB++)
	{
		if (iB % 5 == 0) {
			pButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
			pCtrolsSizer->Add(pButtonsSizer);
		}
		m_pButton[iB] = new CheckButton(this, lmID_BT_NoteAcc+iB, wxBitmap(24,24),
                                        wxDefaultPosition, wxSize(24, 24));
        m_pButton[iB]->SetBitmapUp(sButtonBmps[iB], "", btSize);
        m_pButton[iB]->SetBitmapDown(sButtonBmps[iB], "button_selected_flat", btSize);
        m_pButton[iB]->SetBitmapOver(sButtonBmps[iB], "button_over_flat", btSize);
		pButtonsSizer->Add(m_pButton[iB], wxSizerFlags(0).Border(wxALL, 2) );
	}
	this->Layout();

	SelectButton(-1);	//select none
}

//---------------------------------------------------------------------------------------
EAccidentals GrpNoteAcc::GetNoteAcc()
{
    return (EAccidentals)(m_nSelButton+1);
}

//---------------------------------------------------------------------------------------
bool GrpNoteAcc::process_command(int cmd)
{
    switch(cmd)
    {
        case k_cmd_note_next_accidental:
            SelectNextButton();
            return true;
        case k_cmd_note_prev_accidental:
            SelectPrevButton();
            return true;
        default:
            return false;   //not processed
    }
}

//---------------------------------------------------------------------------------------
void GrpNoteAcc::update_tools_info(ToolsInfo* pInfo)
{
    pInfo->acc = GetNoteAcc();
}

//---------------------------------------------------------------------------------------
void GrpNoteAcc::synchronize_with_cursor(bool fEnable, DocCursor* WXUNUSED(pCursor))
{
    //TODO
    EnableGroup(fEnable);
}

//---------------------------------------------------------------------------------------
void GrpNoteAcc::synchronize_with_selection(bool fEnable, SelectionSet* pSelection)
{
    if (fEnable && !pSelection->empty())
    {
        //find common values for all selected notes, if any.
        //This is necessary for highlighting the accidentals, dots and voice tools
        bool fNoteFound = false;
        int nAcc;
        ColStaffObjs* pCollection = pSelection->get_staffobjs_collection();
        if (pCollection)
        {
            ColStaffObjsIterator it;
            for (it = pCollection->begin(); it != pCollection->end(); ++it)
            {
                ImoObj* pImo = (*it)->imo_object();
                if (pImo->is_note())
                {
                    ImoNote* pNote = static_cast<ImoNote*>(pImo);
                    int nThisAcc = pNote->get_notated_accidentals();
                    if (!fNoteFound)
                    {
                        fNoteFound = true;
                        nAcc = nThisAcc;
                    }
                    else
                    {
                        if (nAcc != nThisAcc)
                            nAcc = -10;
                    }
                }
            }

            //if any note found, proceed to sync. the toolbox buttons for
            //note type
            if (fNoteFound)
                SelectButton(nAcc - 1);
        }
    }
}


//=======================================================================================
// GrpNoteDots implementation
//=======================================================================================
GrpNoteDots::GrpNoteDots(ToolPage* pParent, wxBoxSizer* pMainSizer)
        : ToolButtonsGroup(pParent, k_group_type_options, lm_NUM_DOT_BUTTONS,
                             lmTBG_ALLOW_NONE, pMainSizer,
                             lmID_BT_NoteDots, k_tool_dots, pParent->GetColors())
{
}

//---------------------------------------------------------------------------------------
void GrpNoteDots::create_controls_in_group(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    set_group_title(_("Dots"));
    wxBoxSizer* pCtrolsSizer = create_main_sizer_for_group(pMainSizer);

    //create the specific controls for this group
    const wxString sButtonBmps[lm_NUM_DOT_BUTTONS] = {
	    "dot_1",
	    "dot_2",
	    "dot_3",
    };

    wxBoxSizer* pButtonsSizer = nullptr;
    wxSize btSize(24, 24);
	for (int iB=0; iB < lm_NUM_DOT_BUTTONS; iB++)
	{
		if (iB % 5 == 0) {
			pButtonsSizer = new wxBoxSizer(wxHORIZONTAL);
			pCtrolsSizer->Add(pButtonsSizer);
		}
		m_pButton[iB] = new CheckButton(this, lmID_BT_NoteDots+iB, wxBitmap(24,24),
                                        wxDefaultPosition, wxSize(24, 24));
        m_pButton[iB]->SetBitmapUp(sButtonBmps[iB], "", btSize);
        m_pButton[iB]->SetBitmapDown(sButtonBmps[iB], "button_selected_flat", btSize);
        m_pButton[iB]->SetBitmapOver(sButtonBmps[iB], "button_over_flat", btSize);
		pButtonsSizer->Add(m_pButton[iB], wxSizerFlags(0).Border(wxALL, 2) );
	}
	this->Layout();

	SelectButton(-1);       //select none
}

//---------------------------------------------------------------------------------------
int GrpNoteDots::GetNoteDots()
{
    return m_nSelButton + 1;
}

//---------------------------------------------------------------------------------------
bool GrpNoteDots::process_command(int cmd)
{
    switch(cmd)
    {
        case k_cmd_note_next_dot:
            SelectNextButton();
            return true;
        case k_cmd_note_prev_dot:
            SelectPrevButton();
            return true;
        default:
            return false;   //not processed
    }
}

//---------------------------------------------------------------------------------------
void GrpNoteDots::update_tools_info(ToolsInfo* pInfo)
{
    pInfo->dots = GetNoteDots();
}

//---------------------------------------------------------------------------------------
void GrpNoteDots::synchronize_with_cursor(bool fEnable, DocCursor* WXUNUSED(pCursor))
{
    //TODO
    EnableGroup(fEnable);
}

//---------------------------------------------------------------------------------------
void GrpNoteDots::synchronize_with_selection(bool fEnable, SelectionSet* pSelection)
{
    //enable toolbox options depending on current selected objects

    if (fEnable && !pSelection->empty())
    {
        //find common values for all selected notes, if any.
        //This is necessary for highlighting the dots tools
        bool fNoteFound = false;
        int nDots;
        ColStaffObjs* pCollection = pSelection->get_staffobjs_collection();
        if (pCollection)
        {
            ColStaffObjsIterator it;
            for (it = pCollection->begin(); it != pCollection->end(); ++it)
            {
                ImoObj* pImo = (*it)->imo_object();
                if (pImo->is_note_rest())
                {
                    ImoNoteRest* pNR = static_cast<ImoNoteRest*>(pImo);
                    int nThisDots = pNR->get_dots() - 1;
                    if (!fNoteFound)
                    {
                        fNoteFound = true;
                        nDots = nThisDots;
                    }
                    else
                    {
                        if (nDots != nThisDots)
                            nDots = -1;
                    }
                }
            }

            //if any note found, proceed to sync. the toolbox buttons for dots
            if (fNoteFound)
                SelectButton(nDots);
        }
    }
}


//=======================================================================================
// GrpNoteModifiers implementation
//=======================================================================================

wxBEGIN_EVENT_TABLE(GrpNoteModifiers, ToolGroup)
    EVT_BUTTON  (lmID_BT_Tie, GrpNoteModifiers::OnTieButton)
    EVT_BUTTON  (lmID_BT_Tuplet, GrpNoteModifiers::OnTupletButton)
    EVT_BUTTON  (lmID_BT_ToggleStem, GrpNoteModifiers::OnToggleStemButton)
wxEND_EVENT_TABLE()


//---------------------------------------------------------------------------------------
GrpNoteModifiers::GrpNoteModifiers(ToolPage* pParent, wxBoxSizer* WXUNUSED(pMainSizer))
        : ToolGroup(pParent, k_group_type_options, pParent->GetColors())
        , m_nSelectedToolID(k_tool_none)
{
}

//---------------------------------------------------------------------------------------
void GrpNoteModifiers::create_controls_in_group(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    set_group_title(_("Modifiers"));
    wxBoxSizer* pCtrolsSizer = create_main_sizer_for_group(pMainSizer);

    //create the specific controls for this group

    // Tie button
	wxBoxSizer* pRow1Sizer = new wxBoxSizer( wxHORIZONTAL );

    wxSize btSize(24, 24);
	m_pBtnTie = new CheckButton(this, lmID_BT_Tie, wxBitmap(24,24),
                                wxDefaultPosition, wxSize(24, 24));
    m_pBtnTie->SetBitmapUp("tie", "", btSize);
    m_pBtnTie->SetBitmapDown("tie", "button_selected_flat", btSize);
    m_pBtnTie->SetBitmapOver("tie", "button_over_flat", btSize);
    m_pBtnTie->SetBitmapDisabled("tie_dis", "", btSize);
    m_pBtnTie->SetToolTip(_("Add/remove a tie to/from selected notes"));
	pRow1Sizer->Add( m_pBtnTie, wxSizerFlags(0).Border(wxALL, 2) );

    // Tuplet button
	m_pBtnTuplet = new CheckButton(this, lmID_BT_Tuplet, wxBitmap(24,24),
                                   wxDefaultPosition, wxSize(24, 24));
    m_pBtnTuplet->SetBitmapUp("tuplet", "", btSize);
    m_pBtnTuplet->SetBitmapDown("tuplet", "button_selected_flat", btSize);
    m_pBtnTuplet->SetBitmapOver("tuplet", "button_over_flat", btSize);
    m_pBtnTuplet->SetBitmapDisabled("tuplet_dis", "", btSize);
    m_pBtnTuplet->SetToolTip(_("Add/remove tuplet to/from selected notes"));
	pRow1Sizer->Add( m_pBtnTuplet, wxSizerFlags(0).Border(wxALL, 2) );

    // Toggle stem button
	m_pBtnToggleStem = new CheckButton(this, lmID_BT_ToggleStem, wxBitmap(24,24),
                                       wxDefaultPosition, wxSize(24, 24));
    m_pBtnToggleStem->SetBitmapUp("toggle_stem", "", btSize);
    m_pBtnToggleStem->SetBitmapDown("toggle_stem", "button_selected_flat", btSize);
    m_pBtnToggleStem->SetBitmapOver("toggle_stem", "button_over_flat", btSize);
    m_pBtnToggleStem->SetBitmapDisabled("toggle_stem_dis", "", btSize);
    m_pBtnToggleStem->SetToolTip(_("Toggle stem in selected notes"));
	pRow1Sizer->Add( m_pBtnToggleStem, wxSizerFlags(0).Border(wxALL, 2) );

    pCtrolsSizer->Add( pRow1Sizer, 0, wxEXPAND, 5 );

	this->Layout();
}

//---------------------------------------------------------------------------------------
void GrpNoteModifiers::OnTieButton(wxCommandEvent& event)
{
    m_nSelectedToolID = k_tool_note_tie;
    PostToolBoxEvent(k_tool_note_tie, event.IsChecked());
}

//---------------------------------------------------------------------------------------
void GrpNoteModifiers::OnTupletButton(wxCommandEvent& event)
{
    m_nSelectedToolID = k_tool_note_tuplet;
    PostToolBoxEvent(k_tool_note_tuplet, event.IsChecked());
}

//---------------------------------------------------------------------------------------
void GrpNoteModifiers::OnToggleStemButton(wxCommandEvent& event)
{
    m_nSelectedToolID = k_tool_note_toggle_stem;
    PostToolBoxEvent(k_tool_note_toggle_stem, event.IsChecked());
}

//---------------------------------------------------------------------------------------
void GrpNoteModifiers::SetToolTie(bool fChecked)
{
    fChecked ? m_pBtnTie->Press() : m_pBtnTie->Release();
}

//---------------------------------------------------------------------------------------
void GrpNoteModifiers::SetToolTuplet(bool fChecked)
{
    fChecked ? m_pBtnTuplet->Press() : m_pBtnTuplet->Release();
}

//---------------------------------------------------------------------------------------
void GrpNoteModifiers::SetToolToggleStem(bool fChecked)
{
    fChecked ? m_pBtnToggleStem->Press() : m_pBtnToggleStem->Release();
}

//---------------------------------------------------------------------------------------
void GrpNoteModifiers::EnableTool(EToolID nToolID, bool fEnabled)
{
    switch (nToolID)
    {
        case k_tool_note_tie:
            m_pBtnTie->Enable(fEnabled);
            break;

        case k_tool_note_tuplet:
            m_pBtnTuplet->Enable(fEnabled);
            break;

        case k_tool_note_toggle_stem:
            m_pBtnToggleStem->Enable(fEnabled);
            break;

        default:
            wxASSERT(false);
    }

    //enable /disable group
    bool fEnableGroup = m_pBtnTie->IsEnabled() || m_pBtnTuplet->IsEnabled()
                        || m_pBtnToggleStem->IsEnabled();
    EnableGroup(fEnableGroup);
}

//---------------------------------------------------------------------------------------
void GrpNoteModifiers::update_tools_info(ToolsInfo* WXUNUSED(pInfo))
{
    //Nothing to do. This is an 'action tools' group
}

//---------------------------------------------------------------------------------------
void GrpNoteModifiers::synchronize_with_cursor(bool fEnable, DocCursor* WXUNUSED(pCursor))
{
    //TODO
    EnableGroup(fEnable);
}

//---------------------------------------------------------------------------------------
void GrpNoteModifiers::synchronize_with_selection(bool fEnable, SelectionSet* pSelection)
{
    //enable toolbox options depending on current selected objects

    //flags to enable/disable tools
    bool fEnableTie = false;
    bool fCheckTie = false;
    bool fEnableTuplet = false;
    bool fCheckTuplet = false;
    bool fEnableToggleStem = false;

    if (fEnable && !pSelection->empty())
    {
        //Ties status
        if (pSelection->is_valid_to_add_tie())
        {
            fEnableTie = true;
            fCheckTie = false;
        }
        else if (pSelection->is_valid_to_remove_tie())
        {
            fEnableTie = true;
            fCheckTie = true;
        }

        //add/remove tuplet
        if (pSelection->is_valid_to_add_tuplet())
        {
            fEnableTuplet = true;
            fCheckTuplet = false;
        }
        else if (pSelection->is_valid_to_remove_tuplet())
        {
            fEnableTuplet = true;
            fCheckTuplet = true;
        }

        //toggle stems
        fEnableToggleStem = pSelection->is_valid_for_toggle_stem();
    }

    //proceed to enable/disable tools

    //Ties
    EnableTool(k_tool_note_tie, fEnableTie);
    if (fEnableTie)
        SetToolTie(fCheckTie);

    //Tuples
    EnableTool(k_tool_note_tuplet, fEnableTuplet);
    if (fEnableTuplet)
        SetToolTuplet(fCheckTuplet);

    //Toggle stems
    EnableTool(k_tool_note_toggle_stem, fEnableToggleStem);
    SetToolToggleStem(false);
}



//=======================================================================================
// GrpBeams implementation
//=======================================================================================

wxBEGIN_EVENT_TABLE(GrpBeams, ToolGroup)
    EVT_BUTTON  (lmID_BT_Beam_Cut, GrpBeams::OnButton)
    EVT_BUTTON  (lmID_BT_Beam_Join, GrpBeams::OnButton)
    EVT_BUTTON  (lmID_BT_Beam_Flatten, GrpBeams::OnButton)
    EVT_BUTTON  (lmID_BT_Beam_Subgroup, GrpBeams::OnButton)
wxEND_EVENT_TABLE()


//---------------------------------------------------------------------------------------
GrpBeams::GrpBeams(ToolPage* pParent, wxBoxSizer* WXUNUSED(pMainSizer))
        : ToolGroup(pParent, k_group_type_options, pParent->GetColors())
        , m_nSelectedToolID(k_tool_none)
{
}

//---------------------------------------------------------------------------------------
void GrpBeams::create_controls_in_group(wxBoxSizer* pMainSizer)
{
    //create the common controls for a group
    set_group_title(_("Beams"));
    wxBoxSizer* pCtrolsSizer = create_main_sizer_for_group(pMainSizer);

    //create the specific controls for this group

    // cut beam button
	wxBoxSizer* pRow1Sizer = new wxBoxSizer( wxHORIZONTAL );

    wxSize btSize(24, 24);
	m_pBtnBeamCut = new BitmapButton(this, lmID_BT_Beam_Cut, wxBitmap(24,24),
                                     wxDefaultPosition, wxSize(24, 24));
    m_pBtnBeamCut->SetBitmapUp("tool_beam_cut", "", btSize);
    m_pBtnBeamCut->SetBitmapDown("tool_beam_cut", "button_selected_flat", btSize);
    m_pBtnBeamCut->SetBitmapOver("tool_beam_cut", "button_over_flat", btSize);
    m_pBtnBeamCut->SetBitmapDisabled("tool_beam_cut_dis", "", btSize);
    m_pBtnBeamCut->SetToolTip(_("Break beam at current cursor position"));
	pRow1Sizer->Add( m_pBtnBeamCut, wxSizerFlags(0).Border(wxALL, 2) );

    // beam join button
	m_pBtnBeamJoin = new BitmapButton(this, lmID_BT_Beam_Join, wxBitmap(24,24),
                                      wxDefaultPosition, wxSize(24, 24));
    m_pBtnBeamJoin->SetBitmapUp("tool_beam_join", "", btSize);
    m_pBtnBeamJoin->SetBitmapDown("tool_beam_join", "button_selected_flat", btSize);
    m_pBtnBeamJoin->SetBitmapOver("tool_beam_join", "button_over_flat", btSize);
    m_pBtnBeamJoin->SetBitmapDisabled("tool_beam_join_dis", "", btSize);
    m_pBtnBeamJoin->SetToolTip(_("Beam together all selected notes"));
	pRow1Sizer->Add( m_pBtnBeamJoin, wxSizerFlags(0).Border(wxALL, 2) );

    // beam subgroup button
	m_pBtnBeamSubgroup = new BitmapButton(this, lmID_BT_Beam_Subgroup, wxBitmap(24,24),
                                          wxDefaultPosition, wxSize(24, 24));
    m_pBtnBeamSubgroup->SetBitmapUp("tool_beam_subgroup", "", btSize);
    m_pBtnBeamSubgroup->SetBitmapDown("tool_beam_subgroup", "button_selected_flat", btSize);
    m_pBtnBeamSubgroup->SetBitmapOver("tool_beam_subgroup", "button_over_flat", btSize);
    m_pBtnBeamSubgroup->SetBitmapDisabled("tool_beam_subgroup_dis", "", btSize);
    m_pBtnBeamSubgroup->SetToolTip(_("Subdivide beamed group at current cursor position"));
	pRow1Sizer->Add( m_pBtnBeamSubgroup, wxSizerFlags(0).Border(wxALL, 2) );

    // beam flatten button
	m_pBtnBeamFlatten = new BitmapButton(this, lmID_BT_Beam_Flatten, wxBitmap(24,24),
                                         wxDefaultPosition, wxSize(24, 24));
    m_pBtnBeamFlatten->SetBitmapUp("tool_beam_flatten", "", btSize);
    m_pBtnBeamFlatten->SetBitmapDown("tool_beam_flatten", "button_selected_flat", btSize);
    m_pBtnBeamFlatten->SetBitmapOver("tool_beam_flatten", "button_over_flat", btSize);
    m_pBtnBeamFlatten->SetBitmapDisabled("tool_beam_flatten_dis", "", btSize);
    m_pBtnBeamFlatten->SetToolTip(_("Adjust selected beam to draw it horizontal"));
	pRow1Sizer->Add( m_pBtnBeamFlatten, wxSizerFlags(0).Border(wxALL, 2) );


	pCtrolsSizer->Add( pRow1Sizer, 0, wxEXPAND, 5 );
	this->Layout();

    //disable buttons not yet used
    m_pBtnBeamFlatten->Enable(false);
    m_pBtnBeamSubgroup->Enable(false);
}

//---------------------------------------------------------------------------------------
void GrpBeams::OnButton(wxCommandEvent& event)
{
    switch(event.GetId())
    {
        case lmID_BT_Beam_Cut:      m_nSelectedToolID = k_tool_beams_cut;         break;
        case lmID_BT_Beam_Join:     m_nSelectedToolID = k_tool_beams_join;        break;
        case lmID_BT_Beam_Flatten:  m_nSelectedToolID = k_tool_beams_flatten;     break;
        case lmID_BT_Beam_Subgroup: m_nSelectedToolID = k_tool_beams_subgroup;    break;
        default:
            wxASSERT(false);
    }
    PostToolBoxEvent(m_nSelectedToolID, event.IsChecked());
}

//---------------------------------------------------------------------------------------
void GrpBeams::EnableTool(EToolID nToolID, bool fEnabled)
{
    switch (nToolID)
    {
        case k_tool_beams_cut:
            m_pBtnBeamCut->Enable(fEnabled);
            break;

        case k_tool_beams_join:
            m_pBtnBeamJoin->Enable(fEnabled);
            break;

        case k_tool_beams_flatten:
            //m_pBtnBeamFlatten->Enable(fEnabled);
            break;

        case k_tool_beams_subgroup:
            //m_pBtnBeamSubgroup->Enable(fEnabled);
            break;

        default:
            wxASSERT(false);
    }

    //enable /disable group
    bool fEnableGroup = m_pBtnBeamCut->IsEnabled() || m_pBtnBeamJoin->IsEnabled() ||
                        m_pBtnBeamFlatten->IsEnabled() || m_pBtnBeamSubgroup->IsEnabled();
    EnableGroup(fEnableGroup);
    //disable buttons not yet used
    m_pBtnBeamFlatten->Enable(false);
    m_pBtnBeamSubgroup->Enable(false);
}

//---------------------------------------------------------------------------------------
void GrpBeams::update_tools_info(ToolsInfo* WXUNUSED(pInfo))
{
    //Nothing to do. This is an 'action tools' group
}

//---------------------------------------------------------------------------------------
void GrpBeams::synchronize_with_cursor(bool fEnable, DocCursor* pCursor)
{
    ImoStaffObj* pSO = nullptr;
    if (fEnable)
        pSO = static_cast<ImoStaffObj*>( pCursor->get_pointee() );

    //cut beams tool
    bool fCut = (fEnable && pSO ? is_valid_for_cut_beam(pSO) : false);
    EnableTool(k_tool_beams_cut, fCut);
}

//---------------------------------------------------------------------------------------
void GrpBeams::synchronize_with_selection(bool fEnable, SelectionSet* pSelection)
{
    bool fEnableJoinBeam = fEnable
                           && !pSelection->empty()
                           && pSelection->is_valid_for_join_beam();

    EnableTool(k_tool_beams_join, fEnableJoinBeam);
}

//---------------------------------------------------------------------------------------
bool GrpBeams::is_valid_for_cut_beam(ImoStaffObj* pSO)
{
    //Returns TRUE if object pointed by cursor is valid for breaking a beam.

    //Conditions to be valid:
    //  The object must be a note/rest in a beam
    //  It must not be the first one in the beam

    if (pSO && pSO->is_note_rest())
    {
        ImoNoteRest* pNR = static_cast<ImoNoteRest*>(pSO);
        if (pNR->is_beamed())
        {
            //verify that it is not the first object in the beam
            ImoBeam* pBeam = pNR->get_beam();
            if (pSO != pBeam->get_start_object())
                return true;
        }
    }
    return false;
}



//=======================================================================================
// ToolPageNotesStd implementation
//=======================================================================================

wxIMPLEMENT_DYNAMIC_CLASS(ToolPageNotesStd, ToolPageNotes);


//---------------------------------------------------------------------------------------
ToolPageNotesStd::ToolPageNotesStd()
{
}

//---------------------------------------------------------------------------------------
ToolPageNotesStd::ToolPageNotesStd(wxWindow* parent)
{
    Create(parent);
}

//---------------------------------------------------------------------------------------
void ToolPageNotesStd::Create(wxWindow* parent)
{
    ToolPageNotes::Create(parent);
}

//---------------------------------------------------------------------------------------
ToolPageNotesStd::~ToolPageNotesStd()
{
}

//---------------------------------------------------------------------------------------
void ToolPageNotesStd::create_tool_groups()
{
    //Create the groups for this page

    wxBoxSizer *pMainSizer = GetMainSizer();

	add_group( LENMUS_NEW GrpOctave(this, pMainSizer) );
	add_group( LENMUS_NEW GrpVoiceStd(this, pMainSizer) );
    add_group( LENMUS_NEW GrpNoteDuration(this, pMainSizer) );
    add_group( LENMUS_NEW GrpNoteAcc(this, pMainSizer) );
    add_group( LENMUS_NEW GrpNoteDots(this, pMainSizer) );
    add_group( LENMUS_NEW GrpNoteModifiers(this, pMainSizer) );
    add_group( LENMUS_NEW GrpBeams(this, pMainSizer) );

	create_layout();
	select_group(k_grp_NoteDuration);
}



//=======================================================================================
// ToolPageNotesHarmony implementation
//=======================================================================================

wxIMPLEMENT_DYNAMIC_CLASS(ToolPageNotesHarmony, ToolPageNotes);


//---------------------------------------------------------------------------------------
ToolPageNotesHarmony::ToolPageNotesHarmony()
    : m_pMenu(nullptr)
{
}

//---------------------------------------------------------------------------------------
ToolPageNotesHarmony::ToolPageNotesHarmony(wxWindow* parent)
    : m_pMenu(nullptr)
{
    Create(parent);
}

//---------------------------------------------------------------------------------------
void ToolPageNotesHarmony::Create(wxWindow* parent)
{
    ToolPageNotes::Create(parent);
    m_pMenu = nullptr;

    m_title = _("Harmony");
    m_selector = _("Harmony exercise");
}

//---------------------------------------------------------------------------------------
ToolPageNotesHarmony::~ToolPageNotesHarmony()
{
    delete m_pMenu;
}

//---------------------------------------------------------------------------------------
void ToolPageNotesHarmony::create_tool_groups()
{
    //Create the groups for this page

    wxBoxSizer *pMainSizer = GetMainSizer();

	add_group( LENMUS_NEW GrpOctave(this, pMainSizer) );
	add_group( LENMUS_NEW GrpVoiceHarmony(this, pMainSizer) );
    add_group( LENMUS_NEW GrpNoteDuration(this, pMainSizer) );
    add_group( LENMUS_NEW GrpNoteAcc(this, pMainSizer) );
    add_group( LENMUS_NEW GrpNoteDots(this, pMainSizer) );
    add_group( LENMUS_NEW GrpNoteModifiers(this, pMainSizer) );
    add_group( LENMUS_NEW GrpBeams(this, pMainSizer) );

	create_layout();
	select_group(k_grp_NoteDuration);
}

//---------------------------------------------------------------------------------------
wxMenu* ToolPageNotesHarmony::GetContextualMenuForToolPage()
{
	if (m_pMenu)
        return m_pMenu;

	m_pMenu = new wxMenu();

//TODO TB
//	m_pMenu->Append(lmTOOL_VOICE_SOPRANO, _("&Soprano"));
//	m_pMenu->Append(lmTOOL_VOICE_ALTO, _("&Alto"));
//	m_pMenu->Append(lmTOOL_VOICE_TENOR, _("&Tenor"));
//	m_pMenu->Append(lmTOOL_VOICE_BASS, _("Bass"));

	return m_pMenu;
}

////---------------------------------------------------------------------------------------
//void ToolPageNotesHarmony::OnPopUpMenuEvent(wxCommandEvent& event)
//{
////TODO TB
////    int nID = event.GetId();
////    if (nID >= lmTOOL_VOICE_SOPRANO && nID <= lmTOOL_VOICE_BASS)
////    {
////        m_pGrpVoice->SelectButton(nID - lmTOOL_VOICE_SOPRANO);
////        event.Skip();
////    }
//}


}   //namespace lenmus
