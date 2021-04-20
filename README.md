# VarTable
A class for printing a table on Shell

# Example
```C++
#include <iostream>  
#include "VarTable.h"  

using namespace std;  

int main()  
{  
    VarTable<const char*, double, int, const char*> vt({"Name", "Weight", "Age", "Brother"}, 16);  
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
```

# Display

BASIC Style:  
```
+------------------+------------------+-----+------------------+  
|       Name       |      Weight      | Age |      Brother     |  
+------------------+------------------+-----+------------------+  
| HanMei           |            160.2 |  16 | HanHan           |  
| Jim Green        |            175.3 |  17 | Hart Green       |  
| Yeqian           |            100.3 |   4 | Yeyicheng        |  
+------------------+------------------+-----+------------------+  
```

EMPTY Style:  
```
        Name              Weight        Age        Brother  
  HanMei                        160.2    16   HanHan  
  Jim Green                     175.3    17   Hart Green  
  Yeqian                        100.3     4   Yeyicheng  
```

SIMPLE Style:  
```
        Name              Weight        Age        Brother  
 ------------------ ------------------ ----- ------------------  
  HanMei                        160.2    16   HanHan  
  Jim Green                     175.3    17   Hart Green  
  Yeqian                        100.3     4   Yeyicheng  
```
FULL Style:  
```
+------------------+------------------+-----+------------------+  
|       Name       |      Weight      | Age |      Brother     |  
+------------------+------------------+-----+------------------+  
| HanMei           |            160.2 |  16 | HanHan           |  
+------------------+------------------+-----+------------------+  
| Jim Green        |            175.3 |  17 | Hart Green       |  
+------------------+------------------+-----+------------------+  
| Yeqian           |            100.3 |   4 | Yeyicheng        |  
+------------------+------------------+-----+------------------+  
```
