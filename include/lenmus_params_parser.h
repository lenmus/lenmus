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

#ifndef __LENMUS_PARAMSPARSER_H__        //to avoid nested includes
#define __LENMUS_PARAMSPARSER_H__

#include "lenmus_constrains.h"            //KeyConstrains


namespace lenmus
{


extern string ParseKeys(const string& sParamValue, wxString sFullParam,
                          KeyConstrains* pKeys);
extern string ParseChords(const string& sParamValue, wxString sFullParam,
                            bool* pfValidChords);
extern string ParseScales(const string& sParamValue, wxString sFullParam,
                            bool* pfValidScales);
extern string ParseClef(const string& sParamValue, EClefExercise* pClef);


}  //namespace lenmus

#endif    // __LENMUS_PARAMSPARSER_H__
