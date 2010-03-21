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

#ifndef __LM_LDP_READER_H__ 
#define __LM_LDP_READER_H__

#include <fstream>
#include <iostream>
#include <sstream>
#include "../global/StringType.h"

using namespace std;

namespace lenmus
{

    /*! \brief Base class for any provider of LDP source code to be parsed */
    //-----------------------------------------------------------------------
    class LdpReader
    {
	public:
		LdpReader() {}
		virtual ~LdpReader() {}
		
        //! Returns the next char from the source
		virtual char_type get_next_char()=0;
        //! Instruct reader to repeat last returned char at next invocation of 
        //! get_next_char()
        virtual void repeat_last_char()=0;
        //! The reader is ready for get_next_char(), unget() operations
        virtual bool is_ready()=0;
        //! End of data reached. No more data available
		virtual bool end_of_data()=0;

    };


    /*!
    \brief A LDP reader using a file as origin of source code
    */
    //------------------------------------------------------------------
    class LdpFileReader : public LdpReader
    {
    public:
        LdpFileReader(const string_type& fullFilename);
		virtual ~LdpFileReader() {}

		virtual char_type get_next_char();
        virtual void repeat_last_char();
		virtual bool is_ready();
		virtual bool end_of_data();

    private:
        std::iftstream m_file;
        const string_type& m_filename;
    };


    /*!
    \brief A LDP reader using a string as origin of source code
    */
    //------------------------------------------------------------------
    class LdpTextReader : public LdpReader
    {
    public:
        LdpTextReader(const string_type& sourceText);
		virtual ~LdpTextReader() {}

		virtual char_type get_next_char();
        virtual void repeat_last_char();
		virtual bool is_ready();
		virtual bool end_of_data();

    private:
        tstringstream   m_stream;

    };


}   //namespace lenmus

#endif      //__LM_LDP_READER_H__
