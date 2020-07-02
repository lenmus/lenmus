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

#ifndef __LENMUS_EXERCISE_PARAMS_H__        //to avoid nested includes
#define __LENMUS_EXERCISE_PARAMS_H__

//lenmus
#include "lenmus_standard_header.h"
#include "lenmus_constrains.h"

//lomse
#include <lomse_internal_model.h>
using namespace lomse;


namespace lenmus
{

//forward declarations
class EBookCtrolOptions;
class KeyConstrains;

//---------------------------------------------------------------------------------------
class EBookCtrolParams
{
protected:
//    // html object window attributes
//    int m_nWidth;
//    int m_nHeight;
//    int m_nPercent;

    EBookCtrolOptions* m_pConstrains;   // control options object
    string m_sParamErrors;              // error string if param parsing error

public:
    virtual ~EBookCtrolParams() {}

    void process_params(std::list<ImoParamInfo*>& params);

    void parse_keys(const string& sParamValue, KeyConstrains* pKeys);
    void parse_chords(const string& sParamValue, bool* pfValidChords);
    void parse_scales(const string& sParamValue, bool* pfValidScales);
    void parse_clef(const string& sParamValue, EClef* pClef);

protected:
    EBookCtrolParams(EBookCtrolOptions* pConstrains);

    virtual void process(ImoParamInfo* pParam);
    virtual void do_final_settings() = 0;

    float get_float_value(const string& value, float rDefault);
    void LogError(const string& sMsg);
    void error_invalid_param(const string& name, const string& value,
                             const string& acceptableValues);

};

//---------------------------------------------------------------------------------------
class ExerciseParams : public EBookCtrolParams
{
public:
    virtual ~ExerciseParams() {}

protected:
    ExerciseParams(EBookCtrolOptions* pConstrains);

    void process(ImoParamInfo* pParam) override;

};


}   // namespace lenmus

#endif  // __LENMUS_EXERCISE_PARAMS_H__
