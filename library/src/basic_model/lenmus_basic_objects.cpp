//--------------------------------------------------------------------------------------
//  LenMus Library
//  Copyright (c) 2010 LenMus project
//
//  This program is free software; you can redistribute it and/or modify it under the
//  terms of the GNU General Public License as published by the Free Software Foundation,
//  either version 3 of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful, but WITHOUT ANY
//  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
//  PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License along
//  with this library; if not, see <http://www.gnu.org/licenses/> or write to the
//  Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston,
//  MA  02111-1307,  USA.
//
//  For any comment, suggestion or feature request, please contact the manager of
//  the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------

#include "lenmus_basic_objects.h"

#include <algorithm>
#include "lenmus_internal_model.h"

using namespace std;

namespace lenmus
{


//-------------------------------------------------------------------------------------
// DtoObj implementation
//-------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------
// DtoBinaryRelObj implementation
//-------------------------------------------------------------------------------------

//DtoBinaryRelObj::DtoBinaryRelObj(DtoDocObj* pOwner, long id, int type,
//                               DtoStaffObj* pStartSO, DtoStaffObj* pEndSO)
//    : DtoRelObj(pOwner, id, type)
//    , m_pStartSO(pStartSO)
//    , m_pEndSO(pEndSO)
//{
//}
//
//DtoBinaryRelObj::~DtoBinaryRelObj()
//{
//}

//void DtoBinaryRelObj::remove(DtoStaffObj* pSO)
//{
//    if (m_pStartSO == pSO)
//        m_pStartSO = (DtoStaffObj*)NULL;
//    else if (m_pEndSO == pSO)
//        m_pEndSO = (DtoStaffObj*)NULL;
//}



//-------------------------------------------------------------------------------------
// DtoMultiRelObj implementation
//-------------------------------------------------------------------------------------

//DtoMultiRelObj::DtoMultiRelObj(DtoDocObj* pOwner, long id, int type)
//    : DtoRelObj(pOwner, id, type)
//{
//}

//DtoMultiRelObj::~DtoMultiRelObj()
//{
//    m_relatedObjects.clear();
//}

//void DtoMultiRelObj::remove(DtoStaffObj* pSO)
//{
//    //remove StaffObj.
//	//AWARE: This method is always invoked by a SO. Therefore it will
//	//not inform back the SO, as this is unnecessary and causes problems when
//	//deleting the relationship object
//
//    std::list<DtoStaffObj*>::iterator it;
//    it = find(m_relatedObjects.begin(), m_relatedObjects.end(), pSO);
//    m_relatedObjects.erase(it);
//    on_relationship_modified();
//}

//void DtoMultiRelObj::push_back(DtoStaffObj* pSO)
//{
//    m_relatedObjects.push_back(pSO);
//    //on_relationship_modified();
//}

//void DtoMultiRelObj::include(DtoStaffObj* pSO, int index)
//{
//    // Add a note to the relation. index is the position that the added StaffObj
//    // must occupy (0..n). If index == -1, StaffObj will be added at the end.
//
//	//add the StaffObj
//	if (index == -1 || index == get_num_objects())
//		m_relatedObjects.push_back(pSO);
//	else
//	{
//		int iN;
//		std::list<DtoStaffObj*>::iterator it;
//		for(iN=0, it=m_relatedObjects.begin(); it != m_relatedObjects.end(); ++it, iN++)
//		{
//			if (iN == index)
//			{
//				//insert before current item
//				m_relatedObjects.insert(it, pSO);
//				break;
//			}
//		}
//	}
//
//    on_relationship_modified();
//}
//
//int DtoMultiRelObj::get_object_index(DtoStaffObj* pSO)
//{
//	//returns the position in the StaffObjs list (0..n)
//
//	int iN;
//    std::list<DtoStaffObj*>::iterator it;
//    for(iN=0, it=m_relatedObjects.begin(); it != m_relatedObjects.end(); ++it, iN++)
//	{
//		if (pSO == *it)
//            return iN;
//	}
//	return 0;			//compiler happy
//}



//-------------------------------------------------------------------------------------
// DtoComponentObj implementation
//-------------------------------------------------------------------------------------

//void DtoComponentObj::set_color(DtoColorInfo* pColor)
//{
//    m_color = rgba16(pColor->red(), pColor->green(), pColor->blue(), pColor->alpha());
//    delete pColor;
//}

void DtoComponentObj::set_color(rgba16 color)
{
    m_color = color;
}


////-------------------------------------------------------------------------------------
//// DtoBeamInfo implementation
////-------------------------------------------------------------------------------------
//
//DtoBeamInfo::DtoBeamInfo()
//    : DtoObj(DtoObj::k_beam_info)
//    , m_beamNum(0)
//    , m_line(0)
//    , m_pNR(NULL)
//{
//    for (int level=0; level < 6; level++)
//    {
//        m_beamType[level] = DtoBeam::k_none;
//        m_repeat[level] = false;
//    }
//}
//
//void DtoBeamInfo::set_beam_type(int level, int type)
//{
//    m_beamType[level] = type;
//}
//
//int DtoBeamInfo::get_beam_type(int level)
//{
//    return m_beamType[level];
//}
//
//bool DtoBeamInfo::is_end_of_beam()
//{
//    for (int level=0; level < 6; level++)
//    {
//        if (m_beamType[level] == DtoBeam::k_begin
//            || m_beamType[level] == DtoBeam::k_forward
//            || m_beamType[level] == DtoBeam::k_continue
//            )
//            return false;
//    }
//    return true;
//}
//
//void DtoBeamInfo::set_repeat(int level, bool value)
//{
//    m_repeat[level] = value;
//}
//
//bool DtoBeamInfo::get_repeat(int level)
//{
//    return m_repeat[level];
//}



////-------------------------------------------------------------------------------------
//// DtoContent implementation
////-------------------------------------------------------------------------------------
//
//DtoContent::~DtoContent()
//{
//    std::list<DtoDocObj*>::iterator it;
//    for (it=m_contents.begin(); it != m_contents.end(); ++it)
//        delete *it;
//    m_contents.clear();
//}
//
//DtoDocObj* DtoContent::get_content_item(int iItem)    //iItem = 0..n-1
//{
//    std::list<DtoDocObj*>::iterator it = m_contents.begin();
//    for (; it != m_contents.end() && iItem > 0; ++it, --iItem);
//    if (it != m_contents.end())
//        return *it;
//    return NULL;
//}
//
//
////-------------------------------------------------------------------------------------
//// DtoDocObj implementation
////-------------------------------------------------------------------------------------
//
//DtoDocObj::~DtoDocObj()
//{
//    std::list<DtoAuxObj*>::iterator it;
//    for (it=m_attachments.begin(); it != m_attachments.end(); ++it)
//        delete *it;
//    m_attachments.clear();
//}
//
//DtoAuxObj* DtoDocObj::get_attachment(int i)       //0..n-1
//{
//    std::list<DtoAuxObj*>::iterator it = m_attachments.begin();
//    for (; it != m_attachments.end() && i > 0; ++it, --i);
//    if (it != m_attachments.end())
//        return *it;
//    return NULL;
//}

//-------------------------------------------------------------------------------------
// DtoBarline implementation
//-------------------------------------------------------------------------------------

DtoBarline::DtoBarline(int barlineType)
    : DtoStaffObj()
    , m_barlineType(barlineType)
{
}

////-------------------------------------------------------------------------------------
//// DtoDocument implementation
////-------------------------------------------------------------------------------------
//
//DtoDocument::~DtoDocument()
//{
//    if (m_pContent)
//        delete m_pContent;
//}
//
//

//-------------------------------------------------------------------------------------
// DtoFermata implementation
//-------------------------------------------------------------------------------------

DtoFermata::DtoFermata() 
    : DtoAuxObj()
    , m_placement(ImoFermata::k_above)
    , m_symbol(ImoFermata::k_normal) 
{
}

////-------------------------------------------------------------------------------------
//// DtoInstrument implementation
////-------------------------------------------------------------------------------------
//
//DtoInstrument::DtoInstrument()
//    : DtoContainerObj(DtoObj::k_instrument)
//    , m_nStaves(1)
//    , m_pMusicData(NULL)
//{
////	m_midiChannel = g_pMidi->DefaultVoiceChannel();
////	m_midiInstr = g_pMidi->DefaultVoiceInstr();
//}
//
//DtoInstrument::~DtoInstrument()
//{
//    if (m_pMusicData)
//        delete m_pMusicData;
//}
//
//void DtoInstrument::set_name(DtoTextString* pText)
//{
//    m_name = *pText;
//    delete pText;
//}
//
//void DtoInstrument::set_abbrev(DtoTextString* pText)
//{
//    m_abbrev = *pText;
//    delete pText;
//}
//
//void DtoInstrument::set_midi_info(DtoMidiInfo* pInfo)
//{
//    m_midi = *pInfo;
//    delete pInfo;
//}
//
//
////-------------------------------------------------------------------------------------
//// DtoInstrGroup implementation
////-------------------------------------------------------------------------------------
//
//
//DtoInstrGroup::~DtoInstrGroup()
//{
//    delete_instruments();
//}
//
//void DtoInstrGroup::delete_instruments()
//{
//    std::list<DtoInstrument*>::iterator it;
//    for (it= m_instruments.begin(); it != m_instruments.end(); ++it)
//        delete *it;
//    m_instruments.clear();
//}
//
//void DtoInstrGroup::set_name(DtoTextString* pText)
//{
//    m_name = *pText;
//    delete pText;
//}
//
//void DtoInstrGroup::set_abbrev(DtoTextString* pText)
//{
//    m_abbrev = *pText;
//    delete pText;
//}
//
//
////-------------------------------------------------------------------------------------
//// DtoMusicData implementation
////-------------------------------------------------------------------------------------
//
//DtoMusicData::~DtoMusicData()
//{
//    std::list<DtoStaffObj*>::iterator it;
//    for (it=m_staffobjs.begin(); it != m_staffobjs.end(); ++it)
//        delete *it;
//    m_staffobjs.clear();
//}
//
//

//-------------------------------------------------------------------------------------
// DtoNoteRest implementation
//-------------------------------------------------------------------------------------

DtoNoteRest::DtoNoteRest()
    : DtoStaffObj()
    , m_nDots(0)
    , m_nVoice(1)
    , m_fInTuplet(false)
{
}

//int DtoNoteRest::get_beam_type(int level)
//{
//    return m_beamInfo.get_beam_type(level);
//}
//
//void DtoNoteRest::set_beam_type(int level, int type)
//{
//    m_beamInfo.set_beam_type(level, type);
//}

void DtoNoteRest::set_note_type_and_dots(int noteType, int dots)
{
    m_nNoteType = noteType;
    m_nDots = dots;
}

//bool DtoNoteRest::is_beamed()
//{
//    return m_beamInfo.get_beam_type(0) != DtoBeam::k_none;
//}


//-------------------------------------------------------------------------------------
// DtoRest implementation
//-------------------------------------------------------------------------------------



//-------------------------------------------------------------------------------------
// DtoNote implementation
//-------------------------------------------------------------------------------------

DtoNote::DtoNote()
    : DtoNoteRest()
    , m_step(0)
    , m_octave(4)
    , m_accidentals(0)
    , m_stemDirection(0)
    , m_tiedNext(false)
    , m_tiedPrev(false)
    , m_inChord(false)
    //, m_pTieNext(NULL)
    //, m_pTiePrev(NULL)
{
}


////-------------------------------------------------------------------------------------
//// DtoScore implementation
////-------------------------------------------------------------------------------------
//
//DtoScore::DtoScore()
//    : DtoContainerObj(DtoObj::k_score)
//    , m_version("")
//    , m_pSystemLayoutFirst(NULL)
//    , m_pSystemLayoutOther(NULL)
//{
//}
//
//DtoScore::~DtoScore()
//{
//    delete_options();
//    delete_instruments();
//    delete_systems_layout();
//    delete_staffobjs_collection();
//}
//
//void DtoScore::delete_options()
//{
//    std::list<DtoOption*>::iterator it;
//    for (it= m_options.begin(); it != m_options.end(); ++it)
//        delete *it;
//    m_options.clear();
//}
//
//void DtoScore::delete_instruments()
//{
//    std::list<DtoInstrument*>::iterator it;
//    for (it= m_instruments.begin(); it != m_instruments.end(); ++it)
//        delete *it;
//    m_instruments.clear();
//}
//
//void DtoScore::delete_systems_layout()
//{
//    if (m_pSystemLayoutFirst)
//        delete m_pSystemLayoutFirst;
//    if (m_pSystemLayoutOther)
//        delete m_pSystemLayoutOther;
//}
//
//void DtoScore::delete_staffobjs_collection()
//{
//}
//
//DtoInstrument* DtoScore::get_instrument(int iInstr)    //iInstr = 0..n-1
//{
//    std::list<DtoInstrument*>::iterator it = m_instruments.begin();
//    for (; it != m_instruments.end() && iInstr > 0; ++it, --iInstr);
//    if (it != m_instruments.end())
//        return *it;
//    return NULL;
//}
//
//DtoOption* DtoScore::get_option(const std::string& name)
//{
//    std::list<DtoOption*>::iterator it;
//    for (it= m_options.begin(); it != m_options.end(); ++it)
//    {
//        if ((*it)->get_name() == name)
//            return *it;
//    }
//    return NULL;
//}
//
//void DtoScore::add_sytem_layout(DtoSystemLayout* pSL)
//{
//    if (pSL->is_first())
//    {
//        if (m_pSystemLayoutFirst)
//            delete m_pSystemLayoutFirst;
//        m_pSystemLayoutFirst = pSL;
//    }
//    else
//    {
//        if (m_pSystemLayoutOther)
//            delete m_pSystemLayoutOther;
//        m_pSystemLayoutOther = pSL;
//    }
//}
//
//
////-------------------------------------------------------------------------------------
//// DtoSystemLayout implementation
////-------------------------------------------------------------------------------------
//
//DtoSystemLayout::~DtoSystemLayout()
//{
//    if (m_pMargins)
//        delete m_pMargins;
//}
//
//
//
////-------------------------------------------------------------------------------------
//// DtoTie implementation
////-------------------------------------------------------------------------------------
//
//DtoTie::~DtoTie()
//{
//    if (m_pStartBezier)
//        delete m_pStartBezier;
//    if (m_pEndBezier)
//        delete m_pEndBezier;
//}
//
//
////-------------------------------------------------------------------------------------
//// DtoTieInfo implementation
////-------------------------------------------------------------------------------------
//
//DtoTieInfo::~DtoTieInfo()
//{
//    if (m_pBezier)
//        delete m_pBezier;
//}


////-------------------------------------------------------------------------------------
//// DtoTupletInfo implementation
////-------------------------------------------------------------------------------------
//
//DtoTupletInfo::DtoTupletInfo()
//    : DtoObj(DtoObj::k_tuplet_info)
//    , m_fStartOfTuplet(true)
//    , m_nActualNum(0)
//    , m_nNormalNum(0)
//    , m_fShowBracket(true)
//    , m_fShowNumber(true)
//    , m_nPlacement(k_default)
//    , m_pNR(NULL)
//{
//}
//
//
//
////-------------------------------------------------------------------------------------
//// DtoColorInfo implementation
////-------------------------------------------------------------------------------------
//
//DtoColorInfo::DtoColorInfo(int16u r, int16u g, int16u b, int16u a)
//    : DtoObj(DtoObj::k_color_info)
//    , m_color(r, g, b, a)
//    , m_ok(true)
//{
//}
//
//int16u DtoColorInfo::convert_from_hex(const std::string& hex)
//{
//    int value = 0;
//
//    int a = 0;
//    int b = static_cast<int>(hex.length()) - 1;
//    for (; b >= 0; a++, b--)
//    {
//        if (hex[b] >= '0' && hex[b] <= '9')
//        {
//            value += (hex[b] - '0') * (1 << (a * 4));
//        }
//        else
//        {
//            switch (hex[b])
//            {
//                case 'A':
//                case 'a':
//                    value += 10 * (1 << (a * 4));
//                    break;
//
//                case 'B':
//                case 'b':
//                    value += 11 * (1 << (a * 4));
//                    break;
//
//                case 'C':
//                case 'c':
//                    value += 12 * (1 << (a * 4));
//                    break;
//
//                case 'D':
//                case 'd':
//                    value += 13 * (1 << (a * 4));
//                    break;
//
//                case 'E':
//                case 'e':
//                    value += 14 * (1 << (a * 4));
//                    break;
//
//                case 'F':
//                case 'f':
//                    value += 15 * (1 << (a * 4));
//                    break;
//
//                default:
//                    m_ok = false;
//                    //cout << "Error: invalid character '" << hex[b] << "' in hex number" << endl;
//                    break;
//            }
//        }
//    }
//
//    return static_cast<int16u>(value);
//}
//
//rgba16& DtoColorInfo::get_from_rgb_string(const std::string& rgb)
//{
//    m_ok = true;
//
//    if (rgb[0] == '#')
//    {
//        m_color.r = convert_from_hex( rgb.substr(1, 2) );
//        m_color.g = convert_from_hex( rgb.substr(3, 2) );
//        m_color.b = convert_from_hex( rgb.substr(5, 2) );
//        m_color.a = 255;
//    }
//
//    if (!m_ok)
//        m_color = rgba16(0,0,0,255);
//
//    return m_color;
//}
//
//rgba16& DtoColorInfo::get_from_rgba_string(const std::string& rgba)
//{
//    m_ok = true;
//
//    if (rgba[0] == '#')
//    {
//        m_color.r = convert_from_hex( rgba.substr(1, 2) );
//        m_color.g = convert_from_hex( rgba.substr(3, 2) );
//        m_color.b = convert_from_hex( rgba.substr(5, 2) );
//        m_color.a = convert_from_hex( rgba.substr(7, 2) );
//    }
//
//    if (!m_ok)
//        m_color = rgba16(0,0,0,255);
//
//    return m_color;
//}
//
//rgba16& DtoColorInfo::get_from_string(const std::string& hex)
//{
//    if (hex.length() == 7)
//        return get_from_rgb_string(hex);
//    else if (hex.length() == 9)
//        return get_from_rgba_string(hex);
//    else
//    {
//        m_ok = false;
//        m_color = rgba16(0,0,0,255);
//        return m_color;
//    }
//}


}  //namespace lenmus
