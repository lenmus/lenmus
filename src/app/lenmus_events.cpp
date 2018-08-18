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

//lenmus
#include "lenmus_events.h"

//wxWidgets
#include <wx/wxprec.h>

namespace lenmus
{

DEFINE_EVENT_TYPE( lmEVT_UPDATE_VIEWPORT )
DEFINE_EVENT_TYPE( lmEVT_SCORE_HIGHLIGHT )
DEFINE_EVENT_TYPE( lmEVT_END_OF_PLAYBACK )
DEFINE_EVENT_TYPE( lmEVT_MOVE_TEMPO_LINE )
DEFINE_EVENT_TYPE( EVT_COUNTERS_DLG )
DEFINE_EVENT_TYPE( lmEVT_PAGE_REQUEST )
DEFINE_EVENT_TYPE( lmEVT_UPDATE_UI )
DEFINE_EVENT_TYPE( lmEVT_SHOW_CONTEXTUAL_MENU )


}   // namespace lenmus
