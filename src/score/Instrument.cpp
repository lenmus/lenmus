//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2006 Cecilio Salmeron
//
//    This program is free software; you can redistribute it and/or modify it under the 
//    terms of the GNU General Public License as published by the Free Software Foundation;
//    either version 2 of the License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful, but WITHOUT ANY 
//    WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A 
//    PARTICULAR PURPOSE.  See the GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along with this 
//    program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, 
//    Fifth Floor, Boston, MA  02110-1301, USA.
//
//    For any comment, suggestion or feature request, please contact the manager of 
//    the project at cecilios@users.sourceforge.net
//
//-------------------------------------------------------------------------------------
/*! @file Instrument.cpp
    @brief Implementation file for class lmInstrument
    @ingroup score_kernel
*/
/*! class lmInstrument    A collection of Staves

    An lmInstrument is a collection of Staves. Usually only one staff or two staves
    (piano grand staff) but could be any other number.

*/
#ifdef __GNUG__
// #pragma implementation
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "Score.h"
#include "wx/debug.h"

//implementation of the Instruments List
#include <wx/listimpl.cpp>
WX_DEFINE_LIST(InstrumentsList);


lmInstrument::lmInstrument(lmScore* pScore, wxInt32 nNumStaves, wxInt32 nMIDIChannel, wxInt32 nMIDIInstr)
{
    //constructor: 
    //    pScore    - score to which this instrument belongs
    //    nStaves   - num of VStaves that will have. Usually only one lmVStaff.
    //    nChannel  - MIDI channel that will be used for playing this lmInstrument.
    //    nInstr    - MIDI instrument that will be used for playing this lmInstrument.
                
    m_pScore = pScore;
    m_sNombre = _T("");
    m_sShortName = _T("");
    m_nMidiInstr = nMIDIInstr;
    m_nMidiChannel = nMIDIChannel;
    
    //! @todo Review for lmVStaff with more than one lmStaff
    //¿Qué es esto de más de un lmVStaff en un instrumento?
    //¿Se usa en LenMus?
    for (wxInt32 i = 1; i <= nNumStaves; i++) {
        AddVStaff();
    }
        
}

lmInstrument::~lmInstrument()
{
    m_cStaves.DeleteContents(true);
}

lmVStaff* lmInstrument::AddVStaff()
{
    lmVStaff *pStaff = new lmVStaff(m_pScore, this);
    m_cStaves.Append(pStaff);
    return pStaff;

}

//returns lmVStaff number nStaff (1..n)
lmVStaff* lmInstrument::GetVStaff(wxInt32 nStaff)
{
    wxInt32 i;
    VStavesList::Node *node;
    //iterate over the list to locate lmVStaff nStaff
    for (i=1, node = m_cStaves.GetFirst(); node && i < nStaff; node = node->GetNext(), i++ ) {}
//    wxASSERT_MSG{i != nStaff, _T("No existe el lmVStaff num. nStaff"));
    return node->GetData();
}



////Devuelve el canal MIDI a utilizar con este instrumento
//wxInt32 lmInstrument::GetCanalMIDI()
//{
//    GetCanalMIDI = m_nMidiChannel
//    
//}
//
////Devuelve el instrumento MIDI a utilizar con este instrumento
//wxInt32 lmInstrument::GetInstrumentMIDI()
//{
//    GetInstrumentMIDI = m_nMidiInstr
//    
//}

wxString lmInstrument::Dump()
{
    wxString sDump;
    wxVStavesListNode *pNode;
    lmVStaff* pVStaff;
    int i;
    //iterate over the list of lmVStaff to dump them
    for (i=1, pNode = m_cStaves.GetFirst(); pNode; pNode = pNode->GetNext(), i++)
    {
        pVStaff = (lmVStaff*) pNode->GetData();
        sDump += wxString::Format(_T("\nVStaff %d\n"), i );
        sDump += pVStaff->Dump();
    }
    return sDump;
        
}

wxString lmInstrument::SourceLDP()
{
    wxString sSource = 
        wxString::Format(_T("      (NumPartes %d)\n"), m_cStaves.GetCount() );

    //loop for each lmVStaff
    wxVStavesListNode *pNode;
    lmVStaff* pVStaff;
    int i;
    for (i=1, pNode = m_cStaves.GetFirst(); pNode; pNode = pNode->GetNext(), i++)
    {
        pVStaff = (lmVStaff*) pNode->GetData();
        sSource += wxString::Format(_T("      (Parte %d\n"), i);
        sSource += pVStaff->SourceLDP();
        sSource += _T("      )\n");
    }
    return sSource;

}

wxString lmInstrument::SourceXML()
{
    wxString sSource = _T("TODO: lmInstrument XML Source code generation methods");
    return sSource;

//    Dim i As Long, sFuente As String
//    
//    sFuente = "      (NumPartes " & m_cStaves.Count & ")" & sCrLf
//    for (i = 1 To m_cStaves.Count
//        sFuente = sFuente & "      (Parte " & i & sCrLf
//        sFuente = sFuente & m_cStaves.Item(i).FuenteXML
//        sFuente = sFuente & "      )" & sCrLf
//    }  // i
//    FuenteXML = sFuente

}

