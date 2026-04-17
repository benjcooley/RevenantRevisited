#include "TVideoCapture.h"

// Decompiled methods and structure for class: TVideoCapture



// Function at 0049a380

void TVideoCapture::meth_0x49a380()

{
  dword dVar1;
  uint uVar2;
  int iVar3;
  dword dVar4;
  
  dVar4 = this->bufsize;
  if (dVar4 == 0) {
    if (this->framesize != 0) {
      this->bufsize = this->framesize;
      this->framesize = 0;
    }
    goto LAB_0049a408;
  }
  if ((int)dVar4 < 1) {
    uVar2 = dVar4 + this->bufframe;
    uVar2 = uVar2 & ((int)uVar2 < 1) - 1;
    this->bufframe = uVar2;
    if (uVar2 == 0) goto LAB_0049a3c6;
  }
  else {
    dVar4 = this->bufframe + dVar4;
    dVar1 = this->totframes;
    if ((int)dVar1 <= (int)dVar4) {
      dVar4 = dVar1;
    }
    this->bufframe = dVar4;
    if (dVar4 == dVar1) {
LAB_0049a3c6:
      this->bufsize = 0;
    }
  }
  dVar4 = this->bufframe;
  if (this->saving != 0) {
    if ((int)dVar4 < 0x60) {
      if ((int)dVar4 < 0) {
        dVar4 = 0;
      }
    }
    else {
      dVar4 = 0x60;
    }
    if (this->frame != 0) {
      _AIL_redbook_set_volume_8(this->saving,dVar4);
      dVar4 = _AIL_redbook_volume_4(this->saving);
      this->bufframe = dVar4;
    }
  }
LAB_0049a408:
  if (0 < (int)this->frameaccum) {
    this->frameaccum = this->frameaccum - 1;
  }
  if ((this->frameaccum == 0) && (this->frameaccum = 1000, this->bufsize == 0)) {
    if (this->saving == 0) {
      this->frame = 0;
    }
    else if (this->frame != 0) {
      iVar3 = _AIL_redbook_status_4(this->saving);
      this->frame = (uint)(iVar3 == 1);
    }
    if (((this->frame != 0) && (iVar3 = _AIL_redbook_volume_4(this->saving), iVar3 == 0)) &&
       (this->saving != 0)) {
      if ((this->frame != 0) && (iVar3 = _AIL_redbook_status_4(this->saving), iVar3 == 1)) {
        _AIL_redbook_stop_4(this->saving);
      }
      this->frame = 0;
    }
  }
  return;
}



