//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
//
//    This code is based on the Undo/Redo framework proposed by Alexandre Komyak in an
//    article at http://www.codeguru.com/cpp/cpp/algorithms/general/article.php/c6361/
//    The code was originally developed by Al Stevens and printed in DDJ #11, 1998. It
//    was later adapted to be used with functors by A. Komyak, Nov 2003.
//    Original licence: free software
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

#ifndef __LM_UNDOREDO_H__        //to avoid nested includes
#define __LM_UNDOREDO_H__


#include <vector>


//-----------------------------------------------------------------------------------------
// lmUndoData
//      A container for parameters. Paremeters are stored in dynamic buffer with AddParam<>()
//      method, like this:
//		    AddParam<double>( 2. );
//		    AddParam<int>( 15 );
//
//      To get parameters:
//		    Rewind();
//		    double d = GetParam<double>();
//		    int i = GetParam<int>();
//----------------------------------------------------------------------------------------
class lmUndoData
{
public:
	lmUndoData();
	lmUndoData(const lmUndoData& param);
	lmUndoData& operator=( const lmUndoData& param);

	// rewind current parameter position
	void Rewind() {m_it = m_buffer.begin(); }

	template <typename T> void	AddParam( T param );
	template <typename T> T		GetParam();

private:
	std::vector<char> m_buffer;				// parameters buffer
	std::vector<char>::iterator m_it;	    // current parameter position
	static const size_t chunk_sz = 128;	    // buffer reallocation chunk
};



//-----------------------------------------------------------------------------------------
// lmUndoData implementation
//-----------------------------------------------------------------------------------------

inline lmUndoData::lmUndoData()
{
	m_buffer.reserve( chunk_sz );
	Rewind();
}

inline lmUndoData::lmUndoData(const lmUndoData& param)
{
	if(this != &param)
	{
		m_buffer = param.m_buffer;
		m_it  = param.m_it;
	}
}

inline lmUndoData& lmUndoData::operator=( const lmUndoData& param)
{
	if(this != &param)
	{
		m_buffer = param.m_buffer;
		m_it  = param.m_it;
	}
	return *this;
}

template <typename T>
inline void lmUndoData::AddParam( T param )
{
	// resize buffer if needed
	size_t cap0, cap;
	cap0 = cap = m_buffer.capacity();
	while(m_buffer.size() + sizeof(T) > cap)
		cap += chunk_sz;
	if(cap != cap0) m_buffer.reserve( cap );

	// append parameters
	for(int i=0; i < (int)sizeof(T); i++)
		m_buffer.push_back( *((char*)&param+i) );
	m_it += sizeof(T);
}

template <typename T>
inline T lmUndoData::GetParam( )
{
	T ret;
	std::copy( m_it, m_it + sizeof(T), (char*)&ret );
	m_it += sizeof(T);
	return ret;
}


#endif    // __LM_UNDOREDO_H__        //to avoid nested includes
