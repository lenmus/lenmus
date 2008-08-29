//--------------------------------------------------------------------------------------
//    LenMus Phonascus: The teacher of music
//    Copyright (c) 2002-2008 Cecilio Salmeron
//
//    The lmUndoData class is based on class 'UndoPar' taken from the Undo/Redo 
//    framework proposed by Alexandre Komyak in the article at 
//    http://www.codeguru.com/cpp/cpp/algorithms/general/article.php/c6361/
//    The code was originally developed by Al Stevens and printed in DDJ #11, 1998. It
//    was later adapted to be used with functors by A. Komyak, Nov 2003.
//    Original licence: free software.
//
//    All other code is original.
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
//-------------------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "UndoRedo.h"
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
#include "EditCmd.h"
#include "UndoRedo.h"


//-----------------------------------------------------------------------------------------
// lmUndoData implementation
//-----------------------------------------------------------------------------------------

inline lmUndoData::lmUndoData(int nChunkSize)
{
	// nChunkSize is the buffer reallocation chunk size

    m_nChunkSize = nChunkSize;
	m_buffer.reserve(nChunkSize);
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




//-----------------------------------------------------------------------------------------
// lmUndoItem implementation
//----------------------------------------------------------------------------------------

lmUndoItem::lmUndoItem(lmUndoLog* pUndoLog, lmEditCmd* pCmd, int nChunkSize)
{
    m_pUndoLog = pUndoLog;
    m_pECmd = pCmd;
    m_pData = new lmUndoData(nChunkSize);
}

lmUndoItem::~lmUndoItem()
{
    if (m_pData) delete m_pData;
    if (m_pECmd) delete m_pECmd;
}



//-----------------------------------------------------------------------------------------
// lmUndoLog implementation
//----------------------------------------------------------------------------------------
lmUndoLog::lmUndoLog()
{
}

lmUndoLog::~lmUndoLog()
{
    Clear();
}

void lmUndoLog::Clear()
{
    //delete the lmUndoItem collection
    std::list<lmUndoItem*>::iterator it;
    for (it = m_items.begin(); it != m_items.end(); ++it)
        delete *it;
    m_items.clear();
}

void lmUndoLog::UndoAll()
{
    //commands must be undone in reverse order
    std::list<lmUndoItem*>::reverse_iterator it;
    for (it = m_items.rbegin(); it != m_items.rend(); ++it)
    {
        (*it)->GetUndoData()->Rewind();
        (*it)->GetCommand()->RollBack(*it);
    }
    Clear();
}

void lmUndoLog::LogCommand(lmEditCmd* pECmd, lmUndoItem* pUndoItem)
{
    pUndoItem->SetCommand(pECmd);
    m_items.push_back(pUndoItem);
}
