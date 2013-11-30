//---------------------------------------------------------------------------------------
// This file is part of the Lomse library.
// Copyright (c) 2010-2012 Cecilio Salmeron. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright notice, this 
//      list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright notice, this
//      list of conditions and the following disclaimer in the documentation and/or
//      other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
// SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
// TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
// BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
// DAMAGE.
//
// For any comment, suggestion or feature request, please contact the manager of
// the project at cecilios@users.sourceforge.net
//---------------------------------------------------------------------------------------

#include <UnitTest++.h>
#include <sstream>
#include "lomse_config.h"

//classes related to these tests
#include "lomse_injectors.h"
#include "lomse_hit_tester.h"
#include "lomse_gm_basic.h"
#include "lomse_box_system.h"
#include "lomse_shape_staff.h"

#include "lomse_document.h"
#include "lomse_graphic_view.h"
#include "platform/lomse_platform.h"
#include "lomse_screen_drawer.h"

using namespace UnitTest;
using namespace std;
using namespace lomse;




//---------------------------------------------------------------------------------------
class MyPlatformSupport : public PlatformSupport
{
protected:
    bool m_fUpdateWindowInvoked;
    bool m_fSetWindowTitleInvoked;
    std::string m_title;
    RenderingBuffer m_buffer;

public:
    MyPlatformSupport(EPixelFormat format, bool flip_y) 
        : PlatformSupport(format, flip_y)
    { 
    }
    virtual ~MyPlatformSupport() {}

    void update_window() { m_fUpdateWindowInvoked = true; }
    void set_window_title(const std::string& title) {
        m_fSetWindowTitleInvoked = true; 
        m_title = title;
    }
    void force_redraw() {}
    RenderingBuffer& get_window_buffer() { return m_buffer; }

    bool set_window_title_invoked() { return m_fSetWindowTitleInvoked; }
    bool update_window_invoked() { return m_fUpdateWindowInvoked; }
    const std::string& get_title() { return m_title; }
    void start_timer() {}
    double elapsed_time() const { return 0.0; }

};


//---------------------------------------------------------------------------------------
class HitTesterTestFixture
{
public:
    LibraryScope m_libraryScope;

    HitTesterTestFixture()     //SetUp fixture
        : m_libraryScope(cout)
    {
    }

    ~HitTesterTestFixture()    //TearDown fixture
    {
    }
};

SUITE(HitTesterTest)
{

    TEST_FIXTURE(HitTesterTestFixture, HitTester_BoxFound)
    {
        Document doc(m_libraryScope);
        //doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) "
        //    "(instrument (musicData (clef G)(key e)(n c4 q)(r q)(barline simple))))))" );
        doc.from_string("(lenmusdoc (vers 0.0) "
            "(content (score (vers 1.6) "
            "(instrument (staves 2)(musicData )))))" );
        MyPlatformSupport platform(k_pix_format_bgra32, false);
        ScreenDrawer drawer(m_libraryScope);
        VerticalBookView view(m_libraryScope, &doc, &platform, &drawer);
        GraphicModel* pModel = view.get_graphic_model();

        GmoBoxDocPage* pPage = pModel->get_page(0);     //DocPage
        GmoBox* pBDPC = pPage->get_child_box(0);        //DocPageContent
        GmoBox* pBSP = pBDPC->get_child_box(0);         //ScorePage
        GmoBox* pBSys = pBSP->get_child_box(0);         //System
        GmoBox* pBSlice = pBSys->get_child_box(0);          //Slice
        GmoBox* pBSliceInstr = pBSlice->get_child_box(0);   //SliceInsr

        URect bbox = pBSliceInstr->get_bounds();
        //GmoObj* pHit = ht.hit_test(bbox.x + 1.0f, bbox.y + 1.0f);
        GmoObj* pHit = pPage->find_inner_box_at(bbox.x + 1.0f, bbox.y + 1.0f);

        CHECK ( pHit != NULL );
        CHECK ( pHit == pBSliceInstr );
    }

    TEST_FIXTURE(HitTesterTestFixture, HitTester_NoBox)
    {
        Document doc(m_libraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) "
            "(content (score (vers 1.6) "
            "(instrument (staves 2)(musicData )))))" );
        MyPlatformSupport platform(k_pix_format_bgra32, false);
        ScreenDrawer drawer(m_libraryScope);
        VerticalBookView view(m_libraryScope, &doc, &platform, &drawer);
        GraphicModel* pModel = view.get_graphic_model();
        GmoBoxDocPage* pPage = pModel->get_page(0);
        URect bbox = pPage->get_bounds();

        GmoObj* pHit = pPage->find_inner_box_at(bbox.x - 1.0f, bbox.y - 1.0f);

        CHECK ( pHit == NULL );
    }

    TEST_FIXTURE(HitTesterTestFixture, HitTester_ShapeAt)
    {
        Document doc(m_libraryScope);
        doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) "
            "(instrument (musicData (clef G)(key e)(n c4 q)(r q)(barline simple))))))" );
        MyPlatformSupport platform(k_pix_format_bgra32, false);
        ScreenDrawer drawer(m_libraryScope);
        VerticalBookView view(m_libraryScope, &doc, &platform, &drawer);
        GraphicModel* pModel = view.get_graphic_model();

        GmoBoxDocPage* pPage = pModel->get_page(0);     //DocPage
        GmoBox* pBDPC = pPage->get_child_box(0);        //DocPageContent
        GmoBox* pBSP = pBDPC->get_child_box(0);         //ScorePage
        GmoBox* pBSys = pBSP->get_child_box(0);         //System
        GmoBox* pBSlice = pBSys->get_child_box(0);          //Slice
        GmoBox* pBSliceInstr = pBSlice->get_child_box(0);   //SliceInsr
        GmoShape* pShape = pBSliceInstr->get_shape(0);
        CHECK( pShape != NULL );

        URect bbox = pShape->get_bounds();
        GmoObj* pHit = pBSliceInstr->find_shape_at(bbox.x + 1.0f, bbox.y + 1.0f);

        CHECK ( pHit != NULL );
        CHECK ( pHit == pShape );
    }

    //TEST_FIXTURE(HitTesterTestFixture, HitTester_HitTestShape)
    //{
    //    Document doc(m_libraryScope);
    //    doc.from_string("(lenmusdoc (vers 0.0) (content (score (vers 1.6) "
    //        "(instrument (musicData (clef G)(key e)(n c4 q)(r q)(barline simple))))))" );
    //    MyPlatformSupport platform(k_pix_format_bgra32, false);
    //    ScreenDrawer drawer(m_libraryScope);
    //    VerticalBookView view(m_libraryScope, &doc, &platform, &drawer);
    //    GraphicModel* pModel = view.get_graphic_model();

    //    GmoBoxDocPage* pPage = pModel->get_page(0);     //DocPage
    //    GmoBox* pBDPC = pPage->get_child_box(0);        //DocPageContent
    //    GmoBox* pBSP = pBDPC->get_child_box(0);         //ScorePage
    //    GmoBox* pBSys = pBSP->get_child_box(0);         //System
    //    GmoBox* pBSlice = pBSys->get_child_box(0);          //Slice
    //    GmoBox* pBSliceInstr = pBSlice->get_child_box(0);   //SliceInsr
    //    GmoShape* pShape = pBSliceInstr->get_shape(0);
    //    CHECK( pShape != NULL );

    //    URect bbox = pShape->get_bounds();
    //    GmoObj* pHit = pPage->hit_test(bbox.x + 1.0f, bbox.y + 1.0f);

    //    CHECK ( pHit != NULL );
    //    CHECK ( pHit == pShape );
    //}

}


