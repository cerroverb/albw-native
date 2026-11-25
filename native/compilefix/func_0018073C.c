/* Ghidra wrote the compiler's saved literal-pool words (DAT_xxxxxxxx = reg, ...) as assignments in loop conditions; those words are read-only here, so the write faults. Removed. */
#include "ghidra_types.h"

extern int func_00297298(int param_1,int param_2);
extern void func_0048B740(float *param_1,uint param_2,uint param_3,uint param_4);
extern int func_00588BE4(int param_1);
extern int DAT_0018084c;
extern int *DAT_00180850;
extern int *DAT_00180854;
extern int *DAT_00180858;
extern int DAT_0018085c;
extern int *DAT_00180860;

// USA 0018073C <- EU 0018073C (Ghidra, non-matching)
// GLOBAL DAT_0018084c int
// GLOBAL DAT_00180850 int *
// GLOBAL DAT_00180854 int *
// GLOBAL DAT_00180858 int *
// GLOBAL DAT_0018085c int
// GLOBAL DAT_00180860 int *

undefined4 func_0018073C(int *param_1)

{
  int *piVar1;
  int *piVar2;
  int iVar3;
  int *piVar4;
  int iVar5;
  int *piVar6;
  
  iVar5 = DAT_0018084c;
  param_1[0xf7] = DAT_0018084c;
  param_1[0xf8] = iVar5;
  param_1[0xf9] = iVar5;
  *(undefined1 *)(param_1 + 0x2fb) = 0;
  *(undefined1 *)((int)param_1 + 0x853) = 0;
  piVar2 = DAT_00180858;
  piVar1 = DAT_00180854;
  piVar6 = *(int **)(*DAT_00180850 + 0x3dbc8);
  iVar5 = DAT_0018085c;
  for (piVar4 = *(int **)(*DAT_00180850 + 0x3dbc0); piVar4 != piVar6;
      piVar4 = piVar4 + 1) {
    iVar5 = *piVar4;
    if ((iVar5 != 0) &&
       ((iVar3 = func_00588BE4(iVar5), iVar3 == *piVar1 ||
        (iVar3 = func_00588BE4(iVar5), iVar3 == *piVar2)))) {
      *(undefined4 *)(iVar5 + 0x988) = 0;
      *(undefined4 *)(iVar5 + 0x908) = 0;
    }
    iVar5 = DAT_0018085c;
  }
  *(undefined1 *)((int)param_1 + 0xb95) = 0;
  param_1[0x26a] = 0xe;
  param_1[0x26b] = iVar5;
  param_1[0x269] = 0;
  (**(code **)(*param_1 + 0xac))(param_1,5,0xffffffff);
  param_1[0x22b] = param_1[0x22b] | 4;
  func_0048B740((float *)(param_1 + 0x2f1),0,param_1[0xfa],0);
  return func_00297298(*DAT_00180860,0xf);
}

