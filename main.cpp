#include <iostream>
#include "VarTable.h"

using namespace std;

int main()
{
    VarTable<const char*, double, int, const char*> vt({ "Name", "Weight", "Age", "Brother" }, 10);
    //vt.setColumnFormat({VarTableColumnFormat::AUTO, VarTableColumnFormat::SCIENTIFIC, VarTableColumnFormat::SCIENTIFIC, VarTableColumnFormat::AUTO});
    //vt.setColumnPrecision({1, 2, 1, 2});
    vt.addRow("HanMei", 160.2, 16, "HanHan");
    vt.addRow("Jim Green", 175.3, 17, "Hart Green");
    vt.addRow("Yeqian", 100.3, 4, "Yeyicheng");
    cout << "\nBASIC Style:" << endl;
    vt.print(std::cout);
    cout << "\nEMPTY Style:" << endl;
    vt.setPrintStyle(PrintStyle::EMPTY);
    vt.print(std::cout);
    cout << "\nSIMPLE Style:" << endl;
    vt.setPrintStyle(PrintStyle::SIMPLE);
    vt.print(std::cout);
    cout << "\nFULL Style:" << endl;
    vt.setPrintStyle(PrintStyle::FULL);
    vt.print(std::cout);
}
