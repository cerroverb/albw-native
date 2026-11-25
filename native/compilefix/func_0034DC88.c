/* Ghidra wrote the compiler's saved literal-pool words (DAT_xxxxxxxx = reg, ...) as assignments in loop conditions; those words are read-only here, so the write faults. Removed. */
#include "ghidra_types.h"

extern int func_00100174(int *param_1);
extern int func_00541C38(int param_1,float *param_2,undefined1 *param_3,undefined4 *param_4,float *param_5, int param_6);
extern int func_00559C64(int param_1,undefined4 *param_2,undefined4 *param_3,float *param_4,int param_5);
extern int func_005841A8(int param_1,undefined4 param_2,int *param_3);
extern int *DAT_0034de7c;
extern int DAT_0034de80;
extern undefined4 *DAT_0034de84;
extern undefined4 DAT_0034de88;
extern uint *DAT_0034de8c;
extern undefined4 *DAT_0034de90;
extern undefined4 *DAT_0034de94;
extern int DAT_0034de98;
extern uint *DAT_0034de9c;
extern undefined4 *DAT_0034dea0;

// USA 0034DC88 <- EU 0034DC64 (Ghidra, non-matching)
// GLOBAL DAT_0034de7c int *
// GLOBAL DAT_0034de80 int
// GLOBAL DAT_0034de84 undefined4 *
// GLOBAL DAT_0034de88 undefined4
// GLOBAL DAT_0034de8c uint *
// GLOBAL DAT_0034de90 undefined4 *
// GLOBAL DAT_0034de94 undefined4 *
// GLOBAL DAT_0034de98 int
// GLOBAL DAT_0034de9c uint *
// GLOBAL DAT_0034dea0 undefined4 *

void func_0034DC88(int param_1,float *param_2,int param_3)

{
  int *piVar1;
  undefined4 uVar2;
  uint *puVar3;
  int iVar4;
  undefined4 *puVar5;
  undefined4 *puVar6;
  undefined4 *puVar7;
  
  iVar4 = *DAT_0034de7c;
  puVar7 = *(undefined4 **)(DAT_0034de80 + iVar4);
  puVar5 = (undefined4 *)func_005841A8(iVar4,*DAT_0034de84,*(int **)(DAT_0034de80 + -8 + iVar4));
  puVar3 = DAT_0034de8c;
  uVar2 = DAT_0034de88;
  piVar1 = DAT_0034de7c;
  puVar6 = DAT_0034de94;
  while (puVar5 != puVar7) {
    puVar6 = (undefined4 *)*puVar5;
    if (((*puVar3 & 1) == 0) && (iVar4 = func_00100174((int *)DAT_0034de8c), iVar4 != 0)) {
      *DAT_0034de90 = uVar2;
    }
    if ((puVar6 == (undefined4 *)0x0) ||
       (iVar4 = (**(code **)*puVar6)(puVar6,DAT_0034de90), iVar4 == 0)) {
      puVar6 = (undefined4 *)0x0;
    }
    if (puVar6 != (undefined4 *)0x0) {
      iVar4 = func_00541C38((int)puVar6,(float *)(param_1 + 0x158),(undefined1 *)(param_1 + 0x148),
                            (undefined4 *)(param_1 + 0x14c),param_2,param_3);
      *(char *)(param_1 + 0x137) = (char)iVar4;
      if (iVar4 != 0) {
        iVar4 = puVar6[0xb3];
        if (*(int *)(param_1 + 0x144) != 0 && iVar4 != *(int *)(param_1 + 0x144)) {
          *(undefined1 *)(param_1 + 0x138) = 0;
          *(undefined1 *)(param_1 + 0x136) = 1;
        }
        *(int *)(param_1 + 0x144) = iVar4;
        return;
      }
    }
    puVar5 = (undefined4 *)func_005841A8(*DAT_0034de7c,*DAT_0034de84,puVar5 + 1);
    piVar1 = DAT_0034de7c;
    puVar6 = DAT_0034de94;
  }
  *(undefined4 *)(param_1 + 0x144) = 0;
  *(undefined1 *)(param_1 + 0x136) = 0;
  puVar5 = (undefined4 *)func_005841A8(*piVar1,*puVar6,*(int **)(DAT_0034de98 + *piVar1));
  puVar3 = DAT_0034de9c;
  if (puVar5 != puVar7) {
    do {
      puVar6 = (undefined4 *)*puVar5;
      if (((*puVar3 & 1) == 0) && (iVar4 = func_00100174((int *)DAT_0034de9c), iVar4 != 0)) {
        *DAT_0034dea0 = uVar2;
      }
      if ((puVar6 == (undefined4 *)0x0) ||
         (iVar4 = (**(code **)*puVar6)(puVar6,DAT_0034dea0), iVar4 == 0)) {
        puVar6 = (undefined4 *)0x0;
      }
      if (puVar6 != (undefined4 *)0x0) {
        iVar4 = func_00559C64((int)puVar6,(undefined4 *)(param_1 + 0x13c),
                              (undefined4 *)(param_1 + 0x140),param_2,param_3);
        *(char *)(param_1 + 0x139) = (char)iVar4;
        if (iVar4 != 0) {
          return;
        }
      }
      puVar5 = (undefined4 *)func_005841A8(*DAT_0034de7c,*DAT_0034de94,puVar5 + 1);
    } while (puVar5 != puVar7);
    return;
  }
  return;
}

