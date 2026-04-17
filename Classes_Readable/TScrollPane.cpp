#include "TScrollPane.h"

// Decompiled methods and structure for class: TScrollPane



// Function at 004cefb0

undefined4 TScrollPane::meth_0x4cefb0()

{
  (**(code **)(this->scrolldata + 0x40))(this->scrollfont | 0x40);
  if (this->line != this->numlines) {
    (**(code **)(this->scrolldata + 0x218))(this->numlines,0,0);
  }
  return 1;
}



