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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "GMObject.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "../app/global.h"
#include "GMObject.h"
#include "../app/Paper.h"
#include "../score/StaffObj.h"
#include "../app/ScoreCanvas.h"

//access to colors
#include "../globals/Colors.h"
extern lmColors* g_pColors;

bool g_fFreeMove = false;		// the shapes can be dragged without restrictions


//========================================================================================
//Implementation of class lmGMObject: the root object for the graphical model
//========================================================================================

static int m_IdCounter = 0;        //to assign unique IDs to GMObjects



lmGMObject::lmGMObject(lmScoreObj* pOwner, lmEGMOType nType, bool fDraggable)
{
    m_nId = m_IdCounter++;      // give it an ID
    m_nType = nType;            // save its type
	m_pOwner = pOwner;

	//initializations
	m_uBoundsBottom = lmUPoint(0.0, 0.0);
    m_uBoundsTop = lmUPoint(0.0, 0.0);
	m_fSelected = false;
	m_fDraggable = fDraggable;
}

lmGMObject::~lmGMObject()
{
}

bool lmGMObject::ContainsPoint(lmUPoint& pointL)
{
    //returns true if point received is within the limits of this Box
    return GetBounds().Contains(pointL);

}

void lmGMObject::DrawBounds(lmPaper* pPaper, wxColour color)
{
    //draw a border around instrSlice region
    pPaper->SketchRectangle(m_uBoundsTop,
                            lmUSize(m_uBoundsBottom.x - m_uBoundsTop.x, m_uBoundsBottom.y - m_uBoundsTop.y),
                            color);

}

wxString lmGMObject::DumpBounds()
{
    return wxString::Format(_T("Bounds=(%.2f, %.2f, %.2f, %.2f)"),
        	m_uBoundsTop.x, m_uBoundsTop.y,
            m_uBoundsBottom.x - m_uBoundsTop.x, m_uBoundsBottom.y - m_uBoundsTop.y);
}   

void lmGMObject::NormaliceBoundsRectangle()
{
	// Ensure that TopLeft point is at left-top of BottomRight point

	if (m_uBoundsBottom.x < m_uBoundsTop.x)
	{
		lmLUnits uxMin = m_uBoundsBottom.x;
		m_uBoundsBottom.x = m_uBoundsTop.x;
		m_uBoundsTop.x = uxMin;
	}
	if (m_uBoundsBottom.y < m_uBoundsTop.y)
	{
		lmLUnits uyMin = m_uBoundsBottom.y;
		m_uBoundsBottom.y = m_uBoundsTop.y;
		m_uBoundsTop.y = uyMin;
	}
}

lmUPoint lmGMObject::GetObjectOrigin()
{
	//returns the origin of this object
	return m_uBoundsTop;
}

void lmGMObject::OnEndDrag(lmController* pCanvas, const lmUPoint& uPos)
{
	// End drag. Receives the command processor associated to the view and the
	// final position of the object (logical units referred to page origin).
	// This method must validate/adjust final position and, if ok, it must move
	// the shape and send a move object command to the controller.

	Shift(uPos.x - GetXLeft(), uPos.y - GetYTop());

	pCanvas->MoveObject(this, uPos);

}

void lmGMObject::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
    ShiftBoundsAndSelRec(xIncr, yIncr);
}

void lmGMObject::ShiftBoundsAndSelRec(lmLUnits xIncr, lmLUnits yIncr)
{
	// Auxiliary method to be used by derived classes to perform common actions when the
	// shape is shifted    
	
	m_uSelRect.x += xIncr;		//AWARE: As it is a rectangle, changing its origin does not change
    m_uSelRect.y += yIncr;		//       its width/height. So no need to adjust bottom right point

	m_uBoundsTop.x += xIncr;
	m_uBoundsBottom.x += xIncr;
	m_uBoundsTop.y += yIncr;
	m_uBoundsBottom.y += yIncr;
}

void lmGMObject::SetSelRectangle(lmLUnits x, lmLUnits y, lmLUnits uWidth, lmLUnits uHeight)
{
    m_uSelRect.x = x;
    m_uSelRect.y = y;
    m_uSelRect.width = uWidth;
    m_uSelRect.height = uHeight;
}

void lmGMObject::DrawSelRectangle(lmPaper* pPaper, wxColour colorC)
{
    pPaper->SketchRectangle(m_uSelRect.GetPosition(), m_uSelRect.GetSize(), colorC);
}

void lmGMObject::OnRightClick(lmController* pCanvas, const lmDPoint& vPos, int nKeys)
{
	WXUNUSED(nKeys);
    m_pOwner->PopupMenu(pCanvas, this, vPos);
}







//========================================================================================
//Implementation of class lmBox: the container objects root
//========================================================================================


lmBox::lmBox(lmScoreObj* pOwner, lmEGMOType nType) : lmGMObject(pOwner, nType)
{
}

lmBox::~lmBox()
{
    //delete shapes collection
    for (int i=0; i < (int)m_Shapes.size(); i++)
    {
        delete m_Shapes[i];
    }
    m_Shapes.clear();
}

void lmBox::AddShape(lmShape* pShape)
{
    m_Shapes.push_back(pShape);
	pShape->SetOwnerBox(this);
}

lmShape* lmBox::FindShapeAtPosition(lmUPoint& pointL)
{
    //loop to look up in the shapes collection
	for(int i=0; i < (int)m_Shapes.size(); i++)
    {
        if (m_Shapes[i]->ContainsPoint(pointL))
			return m_Shapes[i];    //found
    }

    // no shape found.
    return (lmShape*)NULL;      
}

//========================================================================================
// Implementation of class lmShape: any renderizable object, such as a line,
// a glyph, a note head, an arch, etc.
//========================================================================================


lmShape::lmShape(lmEGMOType nType, lmScoreObj* pOwner, wxString sName, bool fDraggable,
				 wxColour color)
	: lmGMObject(pOwner, nType, fDraggable)
{
    m_sShapeName = sName;
	m_pOwnerBox = (lmBox*)NULL;
	m_color = color;
}

lmShape::~lmShape()
{
	//delete attachment data
	for(int i=0; i < (int)m_cAttachments.size(); i++)
    {
		delete m_cAttachments[i];
    }

}

bool lmShape::Collision(lmShape* pShape)
{
    lmURect rect1 = GetBounds();
    return rect1.Intersects( pShape->GetBounds() );
}

void lmShape::RenderCommon(lmPaper* pPaper)
{
	RenderCommon(pPaper, g_pColors->ScoreSelected());
}

void lmShape::RenderCommon(lmPaper* pPaper, wxColour colorC)
{
    // Code common to all shapes renderization. Must be invoked after specific code at
    // each shape renderization method

    // draw selection rectangle
    if (g_fDrawSelRect || IsSelected() )
        DrawSelRectangle(pPaper, g_pColors->ScoreSelected() );

    if (g_fDrawBounds)
        DrawBounds(pPaper, colorC);
}
            
wxString lmShape::DumpSelRect()
{
    return wxString::Format(_T("SelRect=(%.2f, %.2f, %.2f, %.2f)"),
        	m_uSelRect.x, m_uSelRect.y, m_uSelRect.width, m_uSelRect.height);     

}   

int lmShape::Attach(lmShape* pShape, lmEAttachType nTag)
{
	lmAtachPoint* pData = new lmAtachPoint;
	pData->nType = nTag;
	pData->pShape = pShape;

    m_cAttachments.push_back(pData);

	//return index to attached shape
	return (int)m_cAttachments.size() - 1;

}

void lmShape::InformAttachedShapes(lmLUnits ux, lmLUnits uy, lmEParentEvent nEvent)
{
	for(int i=0; i < (int)m_cAttachments.size(); i++)
    {
		lmAtachPoint* pData = m_cAttachments[i];
        pData->pShape->OnAttachmentPointMoved(this, pData->nType, ux, uy, nEvent);
    }
}

//========================================================================================
// Implementation of class lmSimpleShape
//========================================================================================

lmSimpleShape::lmSimpleShape(lmEGMOType nType, lmScoreObj* pOwner, wxString sName,
							 bool fDraggable, wxColour color)
	: lmShape(nType, pOwner, sName, fDraggable, color)
{
}

lmSimpleShape::~lmSimpleShape()
{
}

void lmSimpleShape::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
	//Default behaviour is to shift bounding and selection rectangles

    m_uSelRect.x += xIncr;		//AWARE: As it is a rectangle, changing its origin does not
    m_uSelRect.y += yIncr;		//       change its width/height

	m_uBoundsTop.x += xIncr;
	m_uBoundsBottom.x += xIncr;
	m_uBoundsTop.y += yIncr;
	m_uBoundsBottom.y += yIncr;

}



//========================================================================================
// Implementation of class lmCompositeShape
//========================================================================================


lmCompositeShape::lmCompositeShape(lmScoreObj* pOwner, wxString sName, bool fDraggable,
                                   lmEGMOType nType)
	: lmShape(nType, pOwner, sName, fDraggable)
{
    m_fGrouped = true;	//by default all constituent shapes are grouped
}

lmCompositeShape::~lmCompositeShape()
{
    //delete Components collection
    for (int i=0; i < (int)m_Components.size(); i++)
    {
        delete m_Components[i];
    }
    m_Components.clear();
}

int lmCompositeShape::Add(lmShape* pShape)
{
    m_Components.push_back(pShape);

	if (m_Components.size() == 1)
	{
		//compute new selection rectangle
		m_uSelRect = pShape->GetSelRectangle();

		// compute outer rectangle for bounds
		m_uBoundsTop.x = pShape->GetXLeft();
		m_uBoundsTop.y = pShape->GetYTop();
		m_uBoundsBottom.x = pShape->GetXRight();
		m_uBoundsBottom.y = pShape->GetYBottom();
	}
	else
	{
		//compute new selection rectangle by union of individual selection rectangles
		m_uSelRect.Union(pShape->GetSelRectangle());

		// compute outer rectangle for bounds
		m_uBoundsTop.x = wxMin(m_uBoundsTop.x, pShape->GetXLeft());
		m_uBoundsTop.y = wxMin(m_uBoundsTop.y, pShape->GetYTop());
		m_uBoundsBottom.x = wxMax(m_uBoundsBottom.x, pShape->GetXRight());
		m_uBoundsBottom.y = wxMax(m_uBoundsBottom.y, pShape->GetYBottom());
	}

	//return index to added shape
	return (int)m_Components.size() - 1;

}

void lmCompositeShape::Shift(lmLUnits xIncr, lmLUnits yIncr)
{
	//Default behaviour is to shift all components
    for (int i=0; i < (int)m_Components.size(); i++)
    {
        m_Components[i]->Shift(xIncr, yIncr);
    }

	ShiftBoundsAndSelRec(xIncr, yIncr);
}

wxString lmCompositeShape::Dump(int nIndent)
{
	//TODO
	wxString sDump = _T("");
	sDump.append(nIndent * lmINDENT_STEP, _T(' '));
	sDump += wxString::Format(_T("%04d %s: grouped=%s, "), m_nId, m_sShapeName, 
        (m_fGrouped ? _T("yes") : _T("no")) );
    sDump += DumpBounds();
    sDump += _T("\n");

    //dump all its components
    nIndent++;
    for (int i=0; i < (int)m_Components.size(); i++)
    {
        sDump += m_Components[i]->Dump(nIndent);
    }
	return sDump;
}

void lmCompositeShape::Render(lmPaper* pPaper,  wxColour color)
{
	WXUNUSED(color);
	RenderCommon(pPaper, *wxGREEN);

	//Default behaviour: render all components
    for (int i=0; i < (int)m_Components.size(); i++)
    {
        m_Components[i]->Render(pPaper);
    }
}

bool lmCompositeShape::ContainsPoint(lmUPoint& pointL)
{
    for (int i=0; i < (int)m_Components.size(); i++)
    {
        if (m_Components[i]->ContainsPoint(pointL))
			return true;
    }
	return false;

}

bool lmCompositeShape::Collision(lmShape* pShape)
{
    for (int i=0; i < (int)m_Components.size(); i++)
    {
        if (m_Components[i]->Collision(pShape))
			return true;
    }
	return false;
}

lmShape* lmCompositeShape::GetShape(int nShape)
{
	wxASSERT(nShape < (int)m_Components.size());
	return m_Components[nShape];
}

void lmCompositeShape::RecomputeBounds()
{
	if (m_Components.size() > 0)
	{
		lmShape* pShape = m_Components[0];

		//initilaize selection rectangle
		m_uSelRect = pShape->GetSelRectangle();

		// initialize bounds
		m_uBoundsTop.x = pShape->GetXLeft();
		m_uBoundsTop.y = pShape->GetYTop();
		m_uBoundsBottom.x = pShape->GetXRight();
		m_uBoundsBottom.y = pShape->GetYBottom();
	}

    for (int i=1; i < (int)m_Components.size(); i++)
    {
		lmShape* pShape = m_Components[i];

		//compute new selection rectangle by union of individual selection rectangles
		m_uSelRect.Union(pShape->GetSelRectangle());

		// compute outer rectangle for bounds
		m_uBoundsTop.x = wxMin(m_uBoundsTop.x, pShape->GetXLeft());
		m_uBoundsTop.y = wxMin(m_uBoundsTop.y, pShape->GetYTop());
		m_uBoundsBottom.x = wxMax(m_uBoundsBottom.x, pShape->GetXRight());
		m_uBoundsBottom.y = wxMax(m_uBoundsBottom.y, pShape->GetYBottom());
	}

}


wxBitmap* lmCompositeShape::OnBeginDrag(double rScale)
{
	// A dragging operation is started. The view invokes this method to request the 
	// bitmap to be used as drag image. No other action is required.
	// If no bitmap is returned drag is cancelled.
	//      
	// So this method returns the bitmap to use with the drag image.

    // allocate a memory DC for logical units to pixels conversions
    wxMemoryDC dc1;
    dc1.SetMapMode(lmDC_MODE);
    dc1.SetUserScale(rScale, rScale);

    // allocate a memory DC for drawing onto a bitmap
    wxMemoryDC dc2;
    dc2.SetMapMode(wxMM_TEXT);			// each logical unit is 1 pixel

    // allocate the bitmap
    // convert size to pixels
    int wD = (int)dc1.LogicalToDeviceXRel( GetWidth() );
    int hD = (int)dc1.LogicalToDeviceYRel( GetHeight() );
    wxBitmap bitmap(wD, hD);

	//clear the bitmap
    dc2.SelectObject(bitmap);
    dc2.SetBackground(*wxWHITE_BRUSH);
    dc2.Clear();
    dc2.SetBackgroundMode(wxTRANSPARENT);

    //loop to get each shape bitmap and to merge it
    for (int i=0; i < (int)m_Components.size(); i++)
    {
        //get shape bitmap
        lmShape* pShape = m_Components[i];
		wxBitmap* pBMS = pShape->OnBeginDrag(rScale);

        //merge it
        if (pBMS)
		{
            lmPixels vxPos = dc1.LogicalToDeviceXRel( pShape->GetXLeft() - GetXLeft() );
            lmPixels vyPos = dc1.LogicalToDeviceXRel( pShape->GetYTop() - GetYTop() );
            dc2.DrawBitmap(*pBMS, vxPos, vyPos, true);       //true = transparent

            delete pBMS;    //bitmap no longer needed
        }
    }
    dc2.SelectObject(wxNullBitmap);

    // the common bitmap is prepared. Make it masked
    wxImage image = bitmap.ConvertToImage();
    image.SetMaskColour(255, 255, 255);
    wxBitmap* pBitmap = new wxBitmap(image);
    ////DBG -----------
    //wxString sFileName = _T("CompositeShape.bmp");
    //image.SaveFile(sFileName, wxBITMAP_TYPE_BMP);
    ////END DBG -------

    return pBitmap;
}

lmUPoint lmCompositeShape::OnDrag(lmPaper* pPaper, const lmUPoint& uPos)
{
	// The view informs that the user continues dragging. We receive the new desired
	// shape position and we must return the new allowed shape position.
	//
	// The default behaviour is to return the received position, so the view redraws 
	// the drag image at that position. No action must be performed by the shape on 
	// the score and score objects.
	//
	// The received new desired shape position is in logical units and referred to page
	// origin. The returned new allowed shape position must also be in in logical units
	// and referred to page origin.

	return uPos;

}
