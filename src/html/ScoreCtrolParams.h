//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2007 Cecilio Salmeron
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

#ifndef __SCORECTROLPARAMS_H__        //to avoid nested includes
#define __SCORECTROLPARAMS_H__

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "ObjectParams.h"
#include "../exercises/ScoreConstrains.h"
#include "../ldp_parser/AuxString.h"
#include "../xml_parser/MusicXMLParser.h"
#include "HtmlWindow.h"     //to get scale

#include "../app/MainFrame.h"
extern lmMainFrame* g_pMainFrame;

// access to global external variables
extern bool g_fBorderOnScores;            // in TheApp.cpp


enum EHtmlScoreTypes
{
    eHST_short = 1,        //implied: 1 instrument, 1 staff, G clef, C major key
    eHST_pattern,        //
    eHST_full,
    eHST_fileXML,
    eHST_fileLDP
};

class lmScoreCtrolParams : public lmObjectParams
{
public:
    lmScoreCtrolParams(const wxHtmlTag& tag, int nWidth, int nHeight, int nPercent,
        EScoreStyles nStyle);

    ~lmScoreCtrolParams();

    void AddParam(const wxHtmlTag& tag);
    void CreateHtmlCell(wxHtmlWinParser *pHtmlParser);

protected:
    wxString FinishShortScore(wxString sPattern);
    void PrepareScore();


        // Member variables:

    lmScore*                m_pScore;           // the score to display
    wxString                m_sParamErrors;     // error messages
    lmScoreCtrolOptions*    m_pOptions;         // the options for the ScoreCtrol

    // html object window attributes
    EScoreStyles            m_nWindowStyle;

    //attributes
    EHtmlScoreTypes         m_nScoreType;
    long                    m_nVersion;         // for type short: LDP version used
    wxString                m_sLanguage;        // for type short: language used
    wxString                m_sMusic;           // the score in LDP format

    DECLARE_NO_COPY_CLASS(lmScoreCtrolParams)
};



lmScoreCtrolParams::lmScoreCtrolParams(const wxHtmlTag& tag, int nWidth, int nHeight,
                                   int nPercent, EScoreStyles nStyle)
    : lmObjectParams(tag, nWidth, nHeight, nPercent)
{
    m_pScore = (lmScore*) NULL;

    // html object window attributes
    m_nWindowStyle = nStyle;

    // create options object
    m_pOptions = new lmScoreCtrolOptions();

    // default values for attributes
    m_nScoreType = eHST_full;
    m_sMusic = wxEmptyString;

    // other initializations
    m_sParamErrors = _T("");    //no errors

}

lmScoreCtrolParams::~lmScoreCtrolParams()
{
    // WARNING: the score and the options will be deleted by the ScoreControl. They
    // are needed while the ScoreControl is alive, for repaints, playing it, etc.
    // DO NOT DELETE THEM HERE IF THE CONTROL HAS BEEN CREATED
    if (m_sParamErrors != _T("")) {
        if (m_pScore) delete m_pScore;
        if (m_pOptions) delete m_pOptions;
    }

}

void lmScoreCtrolParams::AddParam(const wxHtmlTag& tag)
{
    /*
        Params for lmScoreCtrol - html object type="Application/LenMusScore"

        score_type          'short | short_nn_ss | pattern | full | XMLFile | LDPFile' Default: full
                            In 'short_nn_ss' the meaning of 'nn' is the version number
                            for the LDP language used, and 'ss' is the language (default: 'en')
        music               LDP score | file spec.
        music_border        0 | 1               default: 0 (no border around score)

        control_play        Include 'play' link. Default: do not include it.
                            Value="play label|stop playing label". i.e.: "Play|Stop" Stop label
                            is optional. Default labels: "Play|Stop"

        control_solfa       Include 'solfa' link. Default: do not include it.
                            Value="music read label|stop music reading label". i.e.:
                            "Play|Stop". Stop label is optional.
                            Default labels: "Read|Stop"

        control_measures    Include 'play measure #' links, one per measure.
                            Default: do not include them.
                            Value="play label|stop label". i.e.:
                            "Play|Stop". Stop label is optional.
                            Default labels: "Measure %d|Stop %d"

        metronome       Set a fixed metronome rate to play this piece of music
                        Value="MM number". Default: user value in metronome control


    */

    wxString sName = wxEmptyString;
    wxString sValue = wxEmptyString;
    bool fOK;

    // scan name and value
    if (!tag.HasParam(wxT("NAME"))) return;        // ignore param tag if no name attribute
    sName = tag.GetParam(_T("NAME"));
    sName.UpperCase();        //convert to upper case

    if (!tag.HasParam(_T("VALUE"))) return;        // ignore param tag if no value attribute

    // process value
    if ( sName == _T("MUSIC") ) {
        m_sMusic = tag.GetParam(_T("VALUE"));
        //do some checks
        if (m_sMusic.Find(_T("(g(")) != -1 || m_sMusic.Find(_T("(g (")) != -1) {
            m_sParamErrors += _("Invalid score: old G syntax.\n");
        }
    }

    else if ( sName == _T("SCORE_TYPE") ) {
        wxString sType = tag.GetParam(_T("VALUE"));
        sType.UpperCase();
        if (sType.Left(5) == _T("SHORT")) {
            if (sType == _T("SHORT")) {
                m_nScoreType = eHST_short;
                m_nVersion = 13;    //1.3
                m_sLanguage = _T("es");
            }
            else {
                if (sType.Left(6) != _T("SHORT_")) {
                    m_sParamErrors += wxString::Format(
                        _("Invalid param value in:\n<param %s >\nAcceptable value: 'short_nn'\n"),
                        tag.GetAllParams().c_str() );
                }
                else {
                    wxString sNum = sType.Mid(6, 2);
                    sNum.ToLong(&m_nVersion);
                    m_nScoreType = eHST_short;
                    m_sLanguage = _T("en");
                    if (sType.Length() > 9) {
                        sType = tag.GetParam(_T("VALUE"));     //to take the original case, upper, lower or mixed
                        m_sLanguage = sType.Mid(9);
                    }
                }
            }
        }
        else if (sType == _T("PATTERN"))
            m_nScoreType = eHST_pattern;
        else if (sType == _T("FULL"))
            m_nScoreType = eHST_full;
        else if (sType == _T("XMLFILE"))
            m_nScoreType = eHST_fileXML;
        else if (sType == _T("LDPFILE"))
            m_nScoreType = eHST_fileLDP;
        else
            m_sParamErrors += wxString::Format(
                _("Invalid param value in:\n<param %s >\nAcceptable values: short | full | pattern\n"),
                tag.GetAllParams().c_str() );
    }

    else if ( sName == _T("CONTROL_PLAY") ) {
        m_pOptions->SetControlPlay(true, tag.GetParam(_T("VALUE")) );
    }

    else if ( sName == _T("CONTROL_SOLFA") ) {
        m_pOptions->SetControlSolfa(true, tag.GetParam(_T("VALUE")) );
    }

    else if ( sName == _T("CONTROL_MEASURES") ) {
        m_pOptions->SetControlMeasures(true, tag.GetParam(_T("VALUE")) );
    }

    else if ( sName == _T("MUSIC_BORDER") ) {
        int nBorder;
        fOK = tag.GetParamAsInt(_T("VALUE"), &nBorder);
        if (!fOK)
            m_sParamErrors += wxString::Format(
                _("Invalid param value in:\n<param %s >\nAcceptable values: 1 | 0 \n"),
                tag.GetAllParams().c_str() );
        else
            m_pOptions->fMusicBorder = (nBorder != 0);
    }

    // metronome
    else if ( sName == _T("METRONOME") ) {
        wxString sMM = tag.GetParam(_T("VALUE"));
        long nMM;
        bool fOK = sMM.ToLong(&nMM);
        if (!fOK || nMM < 0 ) {
            m_sParamErrors += wxString::Format(
_("Invalid param value in:\n<param %s >\n \
Invalid value = %s \n \
Acceptable values: numeric, greater than 0\n"),
                tag.GetAllParams().c_str(), tag.GetParam(_T("VALUE")).c_str() );
        }
        else {
            m_pOptions->SetMetronomeMM(nMM);
        }
    }

    // Unknown param
    else
        m_sParamErrors += wxString::Format(
            _("Unknown param: <param %s >\n"),
            tag.GetAllParams().c_str() );

}

void lmScoreCtrolParams::PrepareScore()
{
    if (m_pScore) return;

    //create the score
    lmMusicXMLParser parserXML;
    lmLDPParser parserLDP;

    switch(m_nScoreType) {
        case eHST_fileXML:
            m_pScore = parserXML.ParseMusicXMLFile(m_sMusic,
                                                   sbDO_NOT_START_NEW_LOG,
                                                   sbDO_NOT_SHOW_LOG_TO_USER );
            break;

        case eHST_fileLDP:
            m_pScore = parserLDP.ParseFile(m_sMusic);
            break;

        case eHST_short:
            {
            m_sMusic = FinishShortScore(m_sMusic);
            if (!parserLDP.ParenthesisMatch(m_sMusic)) {
                m_sParamErrors += _("Invalid score: unmatched parenthesis.\n");
            }
            else {
                lmLDPNode* pRoot = parserLDP.ParseText(m_sMusic);
                if (pRoot) m_pScore = parserLDP.AnalyzeScore(pRoot);
            }
            break;
            }

        case eHST_full:
            if (!parserLDP.ParenthesisMatch(m_sMusic)) {
                m_sParamErrors += _("Invalid score: unmatched parenthesis.\n");
            }
            else {
                lmLDPNode* pRoot = parserLDP.ParseText(m_sMusic);
                if (pRoot) m_pScore = parserLDP.AnalyzeScore(pRoot);
            }
            break;

        case eHST_pattern:
            //! @todo
    //        Dim nMetricaPatron As ETimeSignature
    //        nMetricaPatron = MetricaQueDura(SrcDuracionPatron(sPatron))
    //        sPatron = "(metrica " & GetNombreMetrica(nMetricaPatron) & ")" & sPatron
    //        m_oAjPartitura(m_iPartitura).sMusica = FinishShortScore(sPatron)
    //        CrearObjetoPartitura m_iPartitura
            break;

        default:
            //wxASSERT(false);
            //! @todo
            m_pScore = parserXML.ParseMusicXMLFile(_T("../Scores/MusicXML/02. Triplet.xml"));
    }

}

void lmScoreCtrolParams::CreateHtmlCell(wxHtmlWinParser *pHtmlParser)
{
    wxWindow* wnd;

    PrepareScore();

    //if errors display a text box with an error message and finish
    if (!m_pScore || m_sParamErrors != _T("")) {
        m_sParamErrors += wxString::Format(
            _("Errors in score: it can not be created. Score:\n%s\n"), m_sMusic.c_str());
        wnd = new wxTextCtrl((wxWindow*)pHtmlParser->GetWindowInterface()->GetHTMLWindow(), -1, m_sParamErrors,
            wxPoint(0,0), wxSize(300, 100), wxTE_MULTILINE);
        wnd->Show(true);
        pHtmlParser->GetContainer()->InsertCell(new wxHtmlWidgetCell(wnd, m_nPercent));
        return;
    }

    //todo: create a parameter to enable border around the score?
    m_pOptions->fBorder = (m_nWindowStyle == eSIMPLE_BORDER);      //around control
    m_pOptions->fMusicBorder = g_fBorderOnScores;                   //around score

    //set scale as a function of current font size
    m_pOptions->rScale = g_pMainFrame->GetHtmlWindow()->GetScale() * 1.2;
    int nHeight = (int)((double)m_nHeight * m_pOptions->rScale);

    // create the lmScoreCtrol
    int nStyle = 0;
    if (m_pOptions->fBorder) nStyle |= wxSIMPLE_BORDER;
    wnd = new lmScoreCtrol((wxWindow*)g_pMainFrame->GetHtmlWindow(), -1, m_pScore,
        m_pOptions, wxPoint(0,0), wxSize(m_uWidth, nHeight), nStyle );
    wnd->Show(true);
    pHtmlParser->GetContainer()->InsertCell(new wxHtmlWidgetCell(wnd, m_nPercent));



}

//===============================================================================================
// Helper methods to generate the score
//===============================================================================================

wxString lmScoreCtrolParams::FinishShortScore(wxString sPattern)
{
    //prepare the score adding the begining and closing it.
    wxString sPart;
    if (m_nVersion == 15)
    {
        if (m_sLanguage == _T("es")) {
            sPart = _T("(score (vers 1.5)(language es iso-8859-1)")
                    _T("(instrumento ")
                    _T("(datosMusica (clave Sol)(tonalidad Do)");
        }
        else {
            sPart = _T("(score (vers 1.5)(language en iso-8859-1)")
                    _T("(instrument ")
                    _T("(musicData (clef treble)(key Do)");
        }
        sPart += sPattern;
        sPart += _T(" )))");
    }
    else if (m_nVersion == 13)
    {
        sPart = _T("(Score (Vers 1.3)(NumInstrumentos 1)")
                _T("(Instrumento 1 (NumPartes 1)")
                _T("(Parte 1 ")
                    _T("(c 1 (Clave Sol)")
                        _T("(Tonalidad Do)");

        sPart += sPattern;
        sPart += _T(" )))");
    }
    else
    {
        wxString sMsg = wxString::Format(
                _T("lmScoreCtrolParams::FinishShortScore]. Score type 'short': ")
                _T("Invalid version number (%d)"), m_nVersion );
        wxMessageBox(sMsg);
        wxLogMessage(sMsg);
    }

    //replace note pitch '*' by 'a4'
    sPart.Replace(_T("*"), _T("a4"));

    return sPart;

}


#endif  // __SCORECTROLPARAMS_H__
