// test.cpp
#include <iostream>
#include <UnitTest++.h>
#include "../global/StringType.h"

using namespace std;

int main()
{  
    tcout << _T("LenMus Library tests runner") << endl << endl;
    int nErrors = UnitTest::RunAllTests();
    system("pause");
    return nErrors;
}
