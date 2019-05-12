//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2018 LenMus project
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

#ifndef __LENMUS_UTILITIES_H__        //to avoid nested includes
#define __LENMUS_UTILITIES_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_injectors.h"

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>
#include <wx/bmpcbox.h>

//lomse
#include <lomse_pitch.h>
using namespace lomse;


namespace lenmus
{


extern void load_combobox_with_note_names(wxComboBox* pCboBox, DiatonicPitch nSelNote);
extern void load_combobox_with_note_names(wxComboBox* pCboBox, wxString sNoteName);
extern int split_ldp_pattern(const wxString& sSource);
extern bool ldp_pattern_is_rest(const wxString& sElement);


//---------------------------------------------------------------------------------------
// Utility global definitions and functions related to barlines
//---------------------------------------------------------------------------------------

//info about barlines, to centralize data about barlines
class BarlinesDBEntry
{
public:
    BarlinesDBEntry()
        : sBarlineName(wxEmptyString)
        , nBarlineType(k_barline_unknown)
    {
    }
    BarlinesDBEntry(wxString name, EBarline type)
        : sBarlineName(name)
        , nBarlineType(type)
    {
    }

    wxString sBarlineName;
    EBarline nBarlineType;
};

extern void load_barlines_bitmap_combobox(ApplicationScope& appScope,
                                          wxBitmapComboBox* pCtrol,
                                          BarlinesDBEntry tBarlines[]);
extern void select_barline_in_bitmap_combobox(wxBitmapComboBox* pCtrol, EBarline nType);
extern const wxString get_barline_name(int barlineType);


//---------------------------------------------------------------------------------------
// Utility global definitions and functions related to clefs
//---------------------------------------------------------------------------------------

//extern const wxString& get_clef_name(EBarline nBarlineType);


//---------------------------------------------------------------------------------------
// Utility global definitions and functions related to key signatures
//---------------------------------------------------------------------------------------

extern const wxString& get_key_signature_name(EKeySignature nKeySignature);

//---------------------------------------------------------------------------------------
// Utility global definitions and functions related to images generation
//---------------------------------------------------------------------------------------

extern wxBitmap generate_bitmap_for_barline_ctrol(ApplicationScope& appScope,
                                                  wxString& sName, EBarline type);
extern wxBitmap generate_bitmap_for_clef_ctrol(ApplicationScope& appScope,
                                               wxString& sName, EClef type);
extern wxBitmap generate_bitmap_for_key_ctrol(ApplicationScope& appScope,
                                              wxString& sName, EKeySignature type);


//---------------------------------------------------------------------------------------
// Utility global functions related to names for property values
//---------------------------------------------------------------------------------------
extern const wxString get_stem_name(int stemType);



//extern wxString LineStyleToLDP(lmELineStyle nStyle);
//extern wxString LineCapToLDP(lmELineCap nLineCap);
//extern wxString lmTPointToLDP(lmTPoint& tPoint, const wxString& sName=wxEmptyString,
//                              bool fEmptyIfZero=true);
//extern wxString lmColorToLDP(const wxColour& nColor, bool fEmptyIfEqual=true,
//                             const wxColour& nRefColor=*wxBLACK);
//extern wxString lmFloatToLDP(float rValue, const wxString& sName,
//                             bool fEmptyIfEqual=false, float rRefValue=0.0f);
//
//
//// MusicXML related
//extern bool XmlDataToClef(wxString sClefLine, lmEClefType* pClef);
//extern bool XmlDataToBarStyle(wxString sBarStyle, lmEBarline* pType);


}  //namespace lenmus

#endif    // __LENMUS_UTILITIES_H__
