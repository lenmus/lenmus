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

#include <iostream>
#include <UnitTest++.h>

//classes related to these tests
#include "Tree.h"


using namespace UnitTest;
using namespace std;
using namespace lenmus;


class Element : virtual public RefCounted, virtual public NodeInTree<Element>
{
public:
    Element(const std::string& v) : m_value(v) {}

    std::string m_value;
};

class SpObj : public SmartPtr<Element>
{
public:
    SpObj(const std::string& v) : SmartPtr<Element>( new Element(v) ) {}
};


class TreeTestFixture
{
public:

    TreeTestFixture()     //SetUp fixture
    {
    }

    ~TreeTestFixture()    //TearDown fixture
    {
    }

    Tree<Element> m_tree;

    void CreateTree()
    {
        SpObj a("A");
        SpObj b("B");
        SpObj c("C");
        SpObj d("D");
        SpObj e("E");
        SpObj f("F");
        SpObj g("G");
        SpObj h("H");
        SpObj i("I");
        SpObj j("J");
        SpObj k("K");
        SpObj l("L");
        SpObj m("M");
        SpObj n("N");
        SpObj o("O");
        SpObj p("P");
        SpObj q("Q");
        SpObj r("R");
        SpObj s("S");
        SpObj t("T");
        m_tree.set_root(a);
        a->append_child(b);
        b->append_child(c);
        a->append_child(d);
        d->append_child(e);
        e->append_child(f);
        e->append_child(g);
        d->append_child(h);
        a->append_child(i);
        i->append_child(j);
        j->append_child(k);
        j->append_child(l);
        l->append_child(m);
        l->append_child(n);
        i->append_child(o);
        i->append_child(p);
        p->append_child(q);
        p->append_child(r);
        r->append_child(s);
        p->append_child(t);
    }

    void DeleteTestData()
    {
    }

};

SUITE(TreeTest)
{
    TEST_FIXTURE(TreeTestFixture, TreeCreateTree)
    {
        Tree<Element> tree;
        SpObj root("animal");
        tree.set_root(root);
        CHECK( root->is_root() );
        CHECK( root->m_value == "animal" );
    }

    TEST_FIXTURE(TreeTestFixture, TreeCreateTreeFromNode)
    {
        SpObj root("animal");
        Tree<Element> tree(root);
        CHECK( root->is_root() );
        CHECK( root->m_value == "animal" );
    }

    TEST_FIXTURE(TreeTestFixture, TreeAppendChild)
    {
        SpObj root("animal");
        Tree<Element> tree(root);
        SpObj two("mammal");
        root->append_child(two);
        CHECK( root->get_num_children() == 1 );
    }

    TEST_FIXTURE(TreeTestFixture, TreeBeginChildren)
    {
        SpObj root("animal");
        Tree<Element> tree(root);
        SpObj two("mammal");
        root->append_child(two);
        NodeInTree<Element>::children_iterator it = root->begin_children();
        CHECK( (*it)->m_value == "mammal" );
    }

    TEST_FIXTURE(TreeTestFixture, TreeGetNumChildren)
    {
        SpObj root("animal");
        SpObj two1("mammal");
        root->append_child(two1);
        SpObj two2("bird");
        root->append_child(two2);
        SpObj two3("fish");
        root->append_child(two3);
        CHECK( root->get_num_children() == 3 );
        CHECK( two3->get_num_children() == 0 );
    }

    TEST_FIXTURE(TreeTestFixture, TreeStartChildrenIteratorInGivenNode)
    {
        SpObj root("animal");
        SpObj two1("mammal");
        root->append_child(two1);
        SpObj two2("bird");
        root->append_child(two2);
        SpObj two3("fish");
        NodeInTree<Element>::children_iterator it(two2);
        CHECK( (*it)->m_value == "bird" );
    }

    TEST_FIXTURE(TreeTestFixture, TreeGetChild)
    {
        SpObj root("animal");
        SpObj two1("mammal");
        root->append_child(two1);
        SpObj two2("bird");
        root->append_child(two2);
        SpObj two3("fish");
        CHECK( root->get_child(2)->m_value == "bird" );
    }

    TEST_FIXTURE(TreeTestFixture, TreeGetChildOutOfRange)
    {
        SpObj root("animal");
        SpObj two1("mammal");
        root->append_child(two1);
        SpObj two2("bird");
        root->append_child(two2);
        SpObj two3("fish");
        bool fOk = false;
        try {
            root->get_child(5);
        }
        catch(exception& e)
        {
            e.what();       //compiler happy
            //cout << e.what() << endl;
            fOk = true;
        }

        CHECK( fOk );
    }

    TEST_FIXTURE(TreeTestFixture, TreeDepthFirstTraversal)
    {
        CreateTree();

        stringstream path;
        Tree<Element>::depth_first_iterator it;
        for (it=m_tree.begin(); it != m_tree.end(); ++it)
            path << (*it)->m_value;
        //cout << path.str() << endl;
        CHECK( path.str() == "ABCDEFGHIJKLMNOPQRST" );
    }

    TEST_FIXTURE(TreeTestFixture, TreeIsEmpty)
    {
        //checks that the test tree, built in previous test, has been destroyed and
        //doesn't interfere with new tests on that tree
        stringstream path;
        Tree<Element>::depth_first_iterator it;
        for (it=m_tree.begin(); it != m_tree.end(); ++it)
            path << (*it)->m_value;
        //cout << path.str() << endl;
        CHECK( path.str() == "" );
    }

    TEST_FIXTURE(TreeTestFixture, TreeIsBuiltAgain)
    {
        //checks that the test tree can be re-built again witout problems
        CreateTree();

        stringstream path;
        Tree<Element>::depth_first_iterator it;
        for (it=m_tree.begin(); it != m_tree.end(); ++it)
            path << (*it)->m_value;
        //cout << path.str() << endl;
        CHECK( path.str() == "ABCDEFGHIJKLMNOPQRST" );
    }

    TEST_FIXTURE(TreeTestFixture, TreeEraseNode)
    {
        CreateTree();
        Tree<Element>::depth_first_iterator it = m_tree.begin();
        ++it;   //B
        ++it;   //C
        ++it;   //D
        Tree<Element>::depth_first_iterator itNext = m_tree.erase(it);

        stringstream path;
        for (it=m_tree.begin(); it != m_tree.end(); ++it)
            path << (*it)->m_value;
        //cout << path.str() << endl;
        CHECK( path.str() == "ABCIJKLMNOPQRST" );
        //cout << (*itNext)->m_value << endl;
        CHECK( (*itNext)->m_value == "I" );
    }

    TEST_FIXTURE(TreeTestFixture, TreeEraseTerminalNode)
    {
        CreateTree();
        Tree<Element>::depth_first_iterator it = m_tree.begin();
        ++it;   //B
        ++it;   //C
        ++it;   //D
        ++it;   //E
        ++it;   //F
        ++it;   //G
        Tree<Element>::depth_first_iterator itNext = m_tree.erase(it);

        stringstream path;
        for (it=m_tree.begin(); it != m_tree.end(); ++it)
            path << (*it)->m_value;
        //cout << path.str() << endl;
        CHECK( path.str() == "ABCDEFHIJKLMNOPQRST" );
        //cout << (*itNext)->m_value << endl;
        CHECK( (*itNext)->m_value == "H" );
    }

    TEST_FIXTURE(TreeTestFixture, TreeEraseWholeTree)
    {
        CreateTree();
        Tree<Element>::depth_first_iterator it = m_tree.begin();
        Tree<Element>::depth_first_iterator itNext = m_tree.erase(it);

        stringstream path;
        for (it=m_tree.begin(); it != m_tree.end(); ++it)
            path << (*it)->m_value;
        //cout << path.str() << endl;
        CHECK( path.str() == "" );
        CHECK( itNext == m_tree.end() );
    }

    TEST_FIXTURE(TreeTestFixture, TreeReplaceNode)
    {
        CreateTree();
        Tree<Element>::depth_first_iterator it = m_tree.begin();
        ++it;   //B
        ++it;   //C
        ++it;   //D
        SpObj elm("(DEFGH)");
        Tree<Element>::depth_first_iterator itNext = m_tree.replace_node(it, elm);

        stringstream path;
        for (it=m_tree.begin(); it != m_tree.end(); ++it)
            path << (*it)->m_value;
        //cout << path.str() << endl;
        CHECK( path.str() == "ABC(DEFGH)IJKLMNOPQRST" );
        //cout << (*itNext)->m_value << endl;
        CHECK( (*itNext)->m_value == "(DEFGH)" );
    }

    TEST_FIXTURE(TreeTestFixture, TreeReplaceWholeTree)
    {
        CreateTree();
        Tree<Element>::depth_first_iterator it = m_tree.begin();
        SpObj elm("(ALL)");
        Tree<Element>::depth_first_iterator itNext = m_tree.replace_node(it, elm);

        stringstream path;
        for (it=m_tree.begin(); it != m_tree.end(); ++it)
            path << (*it)->m_value;
        //cout << path.str() << endl;
        CHECK( path.str() == "(ALL)" );
        //cout << (*itNext)->m_value << endl;
        CHECK( (*itNext)->m_value == "(ALL)" );
    }


}
