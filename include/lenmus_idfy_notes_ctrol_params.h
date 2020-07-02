//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2020 LenMus project
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

#ifndef __LENMUS_IDFYNOTESCTROLPARAMS_H__        //to avoid nested includes
#define __LENMUS_IDFYNOTESCTROLPARAMS_H__

//wxWidgets
#include <wx/wxprec.h>
#include <wx/wx.h>

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_exercise_params.h"
#include "lenmus_notes_constrains.h"


namespace lenmus
{

// This class pack all parameters to set up a notes identification exercise,
// The settings must be read/setup by the IdfyNotesCtrol object.

class IdfyNotesCtrolParams : public ExerciseParams
{
public:
    IdfyNotesCtrolParams(EBookCtrolOptions* pConstrains);
    ~IdfyNotesCtrolParams();

protected:
    void do_final_settings() override;
    void process(ImoParamInfo* pParam) override;

};



IdfyNotesCtrolParams::IdfyNotesCtrolParams(EBookCtrolOptions* pConstrains)
    : ExerciseParams(pConstrains)
{
}


IdfyNotesCtrolParams::~IdfyNotesCtrolParams()
{
}

void IdfyNotesCtrolParams::process(ImoParamInfo* pParam)
{
    // Params for IdfyNotesCtrol - html object type="Application/LenMusIdfyNotes"
    //
    //    notes         list of notes. Octave doesn't matter.
    //                  Example: "c4,+d4,f4,g4,-b4"
    //                  Default: natural C major notes "c4,d4,e4,f4,g4,a4,b4"
    //
    //    notes_range   lowest and highest notes. Example: "a3,b4"
    //                  Default: "c4,b4"
    //
    //    clef          'G | F4 | F3 | C4 | C3 | C2 | C1'
    //                  Default: 'G'
    //
    //    mode          'theory' | 'earTraining'. Keyword indicating type of exercise
    //
    //    play_start    'A4 | notes'. Default: 'A4'
    //
    //    control_settings    Value="[key for storing the settings]"
    //                        By coding this param it is forced the inclusion of
    //                        the 'settings' link. Its value will be used
    //                        as the key for saving the user settings.
    //
    //    Example:
    //    ------------------------------------
    //    <exercise type="IdfyNotes" width="100%" height="300" border="0">
    //        <control_settings>EarIdfyNotes</control_settings>
    //    </exercise>


    NotesConstrains* pConstrains = dynamic_cast<NotesConstrains*>( m_pConstrains );

    string& name = pParam->get_name();
    string& value = pParam->get_value();


    // clef        G | F4 | F3 | C4 | C3 | C2 | C1
    if (name == "clef")
    {
        EClef nClef = k_clef_G2;        //default value
        parse_clef(value, &nClef);
        pConstrains->SetClef(nClef);
    }


    // Unknown param
    else
        ExerciseParams::process(pParam);


}

void IdfyNotesCtrolParams::do_final_settings()
{
    //Nothing to validate or initializate in m_pConstrains
}



}   // namespace lenmus

#endif  // __LENMUS_IDFYNOTESCTROLPARAMS_H__
