//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2011 LenMus project
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
                                                 DocumentCanvas* pCanvas)
{
    if (classid == "IdfyIntervals")
        return new IdfyIntervalsCtrol(new_id(), appScope, pCanvas);

    else if (classid == "BuildIntervals")
        return new BuildIntervalsCtrol(new_id(), appScope, pCanvas);

    else if (classid == "EarIntervals")
        return new EarIntervalsCtrol(new_id(), appScope, pCanvas);

//    else if (classid == "TheoKeySignatures")
//        return new TheoKeySignaturesCtrol(new_id(), appScope, pCanvas);
//
////    else if (classid == "Control")
////        nType = eHO_ControlCtrol(new_id(), appScope, pCanvas);

//    else if (classid == "EarCompareIntervals")
//        return new EarCompareIntervalsCtrol(new_id(), appScope, pCanvas);
//
//    else if (classid == "TheoMusicReading")
//        return new TheoMusicReadingCtrol(new_id(), appScope, pCanvas);
//
//    else if (classid == "IdfyChord")
//        return new IdfyChordCtrol(new_id(), appScope, pCanvas);
//
//    else if (classid == "IdfyScales")
//        return new IdfyScalesCtrol(new_id(), appScope, pCanvas);
//
//    else if (classid == "IdfyCadences")
//        return new IdfyCadencesCtrol(new_id(), appScope, pCanvas);
//
//    else if (classid == "IdfyTonality")
//        return new IdfyTonalityCtrol(new_id(), appScope, pCanvas);

    else if (classid == "IdfyNotes")
        return new IdfyNotesCtrol(new_id(), appScope, pCanvas);

//    else if (classid == "EarTunning")
//        return new EarTunningCtrol(new_id(), appScope, pCanvas);
//
//    else if (classid == "TheoHarmony")
//        return new TheoHarmonyCtrol(new_id(), appScope, pCanvas);

    else
        return new DummyControl(new_id(), appScope, classid);
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
void DummyControl::generate_content(ImoDynamic* pDyn, Document* pDoc)
{
    ImoStyle* pErrorStyle = pDoc->create_private_style();
    pErrorStyle->set_color_property(ImoStyle::k_color, Color(255,0,0) );
    pErrorStyle->set_lunits_property(ImoStyle::k_font_weight, ImoFontStyleDto::k_bold);

    ImoParagraph* pPara = pDyn->add_paragraph(pErrorStyle);
    pPara->add_text_item("*** Dynamic object error: unknown classid ***",
                         pErrorStyle);
}


}  //namespace lenmus
