//---------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2010-2015 LenMus project
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

#ifndef __LENMUS_DLG_PROPERTIES_H__        //to avoid nested includes
#define __LENMUS_DLG_PROPERTIES_H__

//lenmus
#include "lenmus_injectors.h"

//wxWidgets
#include <wx/wxprec.h>

#include <wx/intl.h>
#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/notebook.h>
#include <wx/button.h>
#include <wx/dialog.h>

//lomse
#include <lomse_internal_model.h>
#include <lomse_im_note.h>
using namespace lomse;

//other
#include <list>
using namespace std;


namespace lenmus
{

//forward declarations
class CommandGenerator;
class DlgProperties;


//---------------------------------------------------------------------------------------
// PropertiesPage: base class for any property page to include in DlgProperties
class PropertiesPage : public wxPanel
{
public:
    virtual ~PropertiesPage() {}

    //handlers
    virtual void OnAcceptChanges(CommandGenerator* pExecuter, bool fCurrentPage)=0;
    virtual void OnCancelChanges() {}
    virtual void OnEnterPage() {}

    virtual void EnableAcceptButton(bool fEnable);

protected:
    PropertiesPage(DlgProperties* parent);

    DlgProperties* m_pParent;
};



//---------------------------------------------------------------------------------------
// DlgProperties: the dialog to display property pages
class DlgProperties : public wxDialog
{
public:
	DlgProperties(wxWindow* pParent, ApplicationScope& appScope,
                  CommandGenerator* pExecuter);
    ~DlgProperties();

    // event handlers
    void OnAccept(wxCommandEvent& WXUNUSED(event));
    void OnCancel(wxCommandEvent& WXUNUSED(event));
    void OnPageChanged(wxNotebookEvent& event);

    //panels
    inline wxNotebook* GetNotebook() { return m_pNotebook; }
    void AddPanel(PropertiesPage* pPanel, const wxString& sTabName);
    void add_specific_panels_for(ImoObj* pImo);

    //info
    inline ApplicationScope& get_app_scope() { return m_appScope; }

    inline void EnableAcceptButton(bool fEnable) { m_pBtAccept->Enable(fEnable); }


protected:
    void CreateControls();
    void add_general_panel_for(ImoObj* pImo);

    ApplicationScope& m_appScope;
    CommandGenerator* m_pExecuter;
    wxNotebook*     m_pNotebook;
    wxButton*       m_pBtAccept;
    wxButton*       m_pBtCancel;
    wxBoxSizer*     m_pMainSizer;
    list<PropertiesPage*>   m_pPages;
    ImoObj*         m_pOwnerImo;


    wxDECLARE_EVENT_TABLE();
};


}   //namespace lenmus

#endif //__LENMUS_DLG_PROPERTIES_H__
