//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2014 LenMus project
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
#include "lenmus_dyncontrol.h"

//#include "lenmus_exercise.h"
#include "lenmus_theo_intervals_ctrol.h"
#include "lenmus_ear_intervals_ctrol.h"
#include "lenmus_idfy_notes_ctrol.h"
#include "lenmus_ear_compare_intv_ctrol.h"
#include "lenmus_theo_key_sign_ctrol.h"
#include "lenmus_idfy_scales_ctrol.h"
#include "lenmus_idfy_chords_ctrol.h"
#include "lenmus_idfy_cadences_ctrol.h"
#include "lenmus_idfy_tonality_ctrol.h"
#include "lenmus_theo_music_reading_ctrol.h"
#include "lenmus_dictation_ctrol.h"

//lomse
#include <lomse_doorway.h>
#include <lomse_internal_model.h>
#include <lomse_document.h>
using namespace lomse;


namespace lenmus
{


//=======================================================================================
// DynControlFactory implementation
//=======================================================================================
DynControl* DynControlFactory::create_dyncontrol(ApplicationScope& appScope,
                                                 const string& classid,
                                                 DocumentWindow* pCanvas)
{
    if (classid == "IdfyIntervals")
        return LENMUS_NEW IdfyIntervalsCtrol(new_id(), appScope, pCanvas);

    else if (classid == "BuildIntervals")
        return LENMUS_NEW BuildIntervalsCtrol(new_id(), appScope, pCanvas);

    else if (classid == "EarIntervals")
        return LENMUS_NEW EarIntervalsCtrol(new_id(), appScope, pCanvas);

    else if (classid == "TheoKeySignatures")
        return LENMUS_NEW TheoKeySignCtrol(new_id(), appScope, pCanvas);

    else if (classid == "EarCompareIntervals")
        return LENMUS_NEW EarCompareIntvCtrol(new_id(), appScope, pCanvas);

    else if (classid == "TheoMusicReading")
        return LENMUS_NEW TheoMusicReadingCtrol(new_id(), appScope, pCanvas);

    else if (classid == "IdfyChord")
        return LENMUS_NEW IdfyChordCtrol(new_id(), appScope, pCanvas);

    else if (classid == "IdfyScales")
        return LENMUS_NEW IdfyScalesCtrol(new_id(), appScope, pCanvas);

    else if (classid == "IdfyCadences")
        return LENMUS_NEW IdfyCadencesCtrol(new_id(), appScope, pCanvas);

    else if (classid == "IdfyTonality")
        return LENMUS_NEW IdfyTonalityCtrol(new_id(), appScope, pCanvas);

    else if (classid == "IdfyNotes")
        return LENMUS_NEW IdfyNotesCtrol(new_id(), appScope, pCanvas);

    else if (classid == "RhythmicDictation")
        return LENMUS_NEW RhythmicDictationCtrol(new_id(), appScope, pCanvas);

    else if (classid == "MelodicDictation")
        return LENMUS_NEW MelodicDictationCtrol(new_id(), appScope, pCanvas);

    else if (classid == "HarmonicDictation")
        return LENMUS_NEW HarmonicDictationCtrol(new_id(), appScope, pCanvas);

////    else if (classid == "EarTunning")
////        return LENMUS_NEW EarTunningCtrol(new_id(), appScope, pCanvas);
////
//    else if (classid == "TheoHarmony")
//        return LENMUS_NEW TheoHarmonyCtrol(new_id(), appScope, pCanvas);

    else
        return LENMUS_NEW DummyControl(new_id(), appScope, classid);
}


//=======================================================================================
// DummyControl implementation
//=======================================================================================
DummyControl::DummyControl(long dynId, ApplicationScope& appScope, const string& classid)
    : DynControl(dynId, appScope)
    , m_classid(classid)
{
}

//---------------------------------------------------------------------------------------
void DummyControl::generate_content(ADynamic dyn, ADocument doc)
{
    ImoDynamic* pDyn = dyn.internal_object();
    Document* pDoc = doc.internal_object();
    ImoStyle* pErrorStyle = pDoc->create_private_style();
    pErrorStyle->color( Color(255,0,0) );
    pErrorStyle->font_weight( ImoStyle::k_font_weight_bold );

    ImoParagraph* pPara = pDyn->add_paragraph(pErrorStyle);
    pPara->add_text_item("*** Dynamic object error: unknown classid ***",
                         pErrorStyle);

//    std::unique_ptr<IStyle> errorStyle = doc.new_private_style();
//    errorStyle->color( Color(255,0,0) );
//    errorStyle->font_weight( ImoStyle::k_font_weight_bold );
//
//    std::unique_ptr<IParagraph> para = dyn.append_paragraph(*errorStyle);
//    para.append_text_item("*** Dynamic object error: unknown classid ***", *errorStyle);
}


}  //namespace lenmus
