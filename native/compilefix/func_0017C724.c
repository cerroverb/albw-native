/* Ghidra wrote the compiler's saved literal-pool words (DAT_xxxxxxxx = reg, ...) as assignments in loop conditions; those words are read-only here, so the write faults. Removed. */
#include "ghidra_types.h"

extern int func_0010BEB4(int param_1,int *param_2);
extern int func_00115C54(float *param_1,float *param_2,float *param_3);
extern int func_00115C70(float *param_1,float *param_2,float *param_3);
extern int func_00115D70(float *param_1,float *param_2,float fparam_0);
extern int func_0017B378(int param_1,int *param_2,undefined4 param_3,int param_4);
extern int func_001DDDE8(int param_1,float *param_2,int param_3,float fparam_0);
extern int func_0052FBA0(int param_1,uint param_2);
extern float DAT_0017ca3c;
extern int *DAT_0017ca40;
extern undefined4 DAT_0017ca44;
extern float DAT_0017ca48;
extern undefined4 DAT_0017ca4c;
extern float *DAT_0017ca50;
extern undefined4 DAT_0017ca54;
extern float DAT_0017cab0;
extern int *DAT_0017cab4;

// USA 0017C724 <- EU 0017C724 (Ghidra, non-matching)
// GLOBAL DAT_0017ca3c float
// GLOBAL DAT_0017ca40 int *
// GLOBAL DAT_0017ca44 undefined4
// GLOBAL DAT_0017ca48 float
// GLOBAL DAT_0017ca4c undefined4
// GLOBAL DAT_0017ca50 float *
// GLOBAL DAT_0017ca54 undefined4
// GLOBAL DAT_0017cab0 float
// GLOBAL DAT_0017cab4 int *

undefined4 func_0017C724(int param_1)

{
  char cVar1;
  undefined1 uVar2;
  undefined4 uVar3;
  int iVar4;
  int *piVar5;
  int iVar6;
  uint uVar7;
  code *pcVar8;
  int iVar9;
  undefined4 *puVar10;
  undefined4 *puVar11;
  float *pfVar12;
  undefined4 uVar13;
  uint in_fpscr;
  float fVar14;
  ulonglong uVar15;
  float fVar16;
  _Alignas(8) unsigned char albw_frame[0x70];
  #define local_6c (*(float *)(albw_frame + 0x4))
  #define iStack_68 (*(int *)(albw_frame + 0x8))
  #define uStack_64 (*(undefined4 *)(albw_frame + 0xc))
  #define local_60 (*(float *)(albw_frame + 0x10))
  #define local_5c (*(int *)(albw_frame + 0x14))
  #define uStack_58 (*(undefined4 *)(albw_frame + 0x18))
  #define local_4c (*(float *)(albw_frame + 0x24))
  #define uStack_48 (*(undefined4 *)(albw_frame + 0x28))
  #define uStack_44 (*(undefined4 *)(albw_frame + 0x2c))
  #define local_40 (*(float *)(albw_frame + 0x30))
  #define uStack_3c (*(undefined4 *)(albw_frame + 0x34))
  #define uStack_38 (*(undefined4 *)(albw_frame + 0x38))
  #define local_34 (*(float *)(albw_frame + 0x3c))
  #define iStack_30 (*(int *)(albw_frame + 0x40))
  #define uStack_2c (*(undefined4 *)(albw_frame + 0x44))
  piVar5 = DAT_0017ca40;
  pfVar12 = (float *)(param_1 + 0x838);
  cVar1 = *(char *)(param_1 + 0x880);
  uVar15 = (ulonglong)(uint)ALBW_AS_U32(DAT_0017ca3c);
  if (cVar1 == '\0') {
    puVar10 = *(undefined4 **)(param_1 + 0x7dc);
    uVar13 = *(undefined4 *)(param_1 + 0x2cc);
    puVar11 = puVar10 + *(int *)(param_1 + 0x7d4);
    uVar3 = DAT_0017ca44;
    for (; puVar10 != puVar11; puVar10 = puVar10 + 1) {
      iVar6 = func_0052FBA0(*piVar5 + 0x3dbbc,*(uint *)*puVar10);
      if (iVar6 != 0) {
        if (*(int *)(iVar6 + 0x478) == 0) {
          fVar16 = *(float *)(iVar6 + 0x3d0);
          *pfVar12 = fVar16;
          *(undefined4 *)(param_1 + 0x83c) = *(undefined4 *)(iVar6 + 0x3d4);
          fVar14 = *(float *)(iVar6 + 0x3d8);
          *(float *)(param_1 + 0x840) = fVar14;
          *(float *)(param_1 + 0x83c) = *(float *)(param_1 + 0xc) + ALBW_AS_F32(uVar15);
          fVar16 = *(float *)(param_1 + 0x3d0) - fVar16;
          fVar14 = *(float *)(param_1 + 0x3d8) - fVar14;
          if ((int)ALBW_AS_U32(SQRT(fVar16 * fVar16 + fVar14 * fVar14)) < 0x3f800000) {
            *(undefined4 *)(iVar6 + 0x478) = uVar13;
            iVar4 = (int)*(char *)(iVar6 + 0x418);
            if (iVar4 != -1) {
              iVar9 = *(int *)(iVar6 + 0x434);
              pcVar8 = *(code **)(iVar9 + iVar4 * 8);
              if ((pcVar8 != (code *)0x0) ||
                 ((uVar7 = *(uint *)(iVar9 + iVar4 * 8 + 4), (uVar7 & 1) != 0 && (uVar7 != 0)))) {
                uVar7 = *(uint *)(iVar9 + iVar4 * 8 + 4);
                piVar5 = (int *)(*(int *)(iVar6 + 0x428) + ((int)uVar7 >> 1));
                if ((uVar7 & 1) != 0) {
                  pcVar8 = *(code **)(pcVar8 + *piVar5);
                }
                (*pcVar8)(piVar5,2);
              }
            }
            uVar2 = *(undefined1 *)(iVar6 + 0x418);
            *(undefined1 *)(iVar6 + 0x418) = 2;
            *(undefined4 *)(iVar6 + 0x414) = 0;
            *(undefined1 *)(iVar6 + 0x419) = uVar2;
            if (*(int *)(iVar6 + 0x424) != 0) {
              if (*(uint *)(iVar6 + 0x41c) < 3) {
                piVar5 = *(int **)(iVar6 + 0x420);
              }
              else {
                piVar5 = (int *)(*(int *)(iVar6 + 0x420) + 0x48);
              }
              (**(code **)(*piVar5 + 8))(piVar5);
              local_5c = piVar5[1];
              local_60 = DAT_0017ca48;
              func_0010BEB4(*(int *)(iVar6 + 0x424),(int *)&local_60);
            }
            iVar9 = (int)*(char *)(iVar6 + 0x418);
            iVar4 = *(int *)(iVar6 + 0x42c);
            pcVar8 = *(code **)(iVar4 + iVar9 * 8);
            if ((pcVar8 != (code *)0x0) ||
               ((uVar7 = *(uint *)(iVar4 + iVar9 * 8 + 4), (uVar7 & 1) != 0 && (uVar7 != 0)))) {
              uVar7 = *(uint *)(iVar4 + iVar9 * 8 + 4);
              if ((uVar7 & 1) != 0) {
                pcVar8 = *(code **)(pcVar8 + *(int *)(*(int *)(iVar6 + 0x428) + ((int)uVar7 >> 1)));
              }
              (*pcVar8)((int)(*(int *)(iVar6 + 0x428)) + ((int)uVar7 >> 1));
            }
            uVar13 = DAT_0017ca4c;
            *(undefined4 *)(param_1 + 0x868) = 0;
            *(undefined4 *)(param_1 + 0x888) = uVar13;
          }
          goto LAB_0017ca78;
        }
        uVar13 = *(undefined4 *)(iVar6 + 0x2cc);
      }
      uVar3 = DAT_0017ca44;
    }
    *(char *)(param_1 + 0x880) = *(char *)(param_1 + 0x880) + '\x01';
    *(undefined4 *)(param_1 + 0x854) = 0x1e;
    *(undefined4 *)(param_1 + 0x858) = uVar3;
    *(undefined4 *)(param_1 + 0x850) = 0;
    goto LAB_0017ca78;
  }
  if (cVar1 == '\x01') {
    fVar16 = DAT_0017ca3c;
    func_00115C54(&local_4c,(float *)(param_1 + 8),DAT_0017ca50);
    iVar6 = *(int *)(param_1 + 0x854) - *(int *)(param_1 + 0x850);
    if (iVar6 < 2) {
      iVar6 = 1;
    }
    local_40 = local_4c;
    uStack_3c = uStack_48;
    uStack_38 = uStack_44;
    fVar14 = (float)VectorSignedToFloat(iVar6,(byte)(in_fpscr >> 0x15) & 3);
    fVar16 = fVar16 / fVar14;
    func_00115C70(&local_34,&local_40,pfVar12);
    local_60 = local_34;
    local_5c = iStack_30;
    uStack_58 = uStack_2c;
    func_00115D70(&local_34,&local_60,fVar16);
    local_6c = local_34;
    iStack_68 = iStack_30;
    uStack_64 = uStack_2c;
    func_00115C54(pfVar12,pfVar12,&local_6c);
    uVar13 = DAT_0017ca54;
    uVar7 = *(uint *)(param_1 + 0x850);
    if (*(uint *)(param_1 + 0x854) <= uVar7) {
      *(char *)(param_1 + 0x880) = *(char *)(param_1 + 0x880) + '\x01';
      *(undefined4 *)(param_1 + 0x854) = 0x3c;
      *(undefined4 *)(param_1 + 0x858) = uVar13;
      *(undefined4 *)(param_1 + 0x850) = 0;
      goto LAB_0017ca78;
    }
  }
  else {
    if (cVar1 != '\x02') goto LAB_0017ca78;
    uVar7 = *(uint *)(param_1 + 0x850);
    if (*(uint *)(param_1 + 0x854) <= uVar7) {
      *(undefined1 *)(param_1 + 0x883) = 1;
      goto LAB_0017ca78;
    }
  }
  *(uint *)(param_1 + 0x850) = uVar7 + 1;
LAB_0017ca78:
  func_0017B378(param_1,(int *)0x0,0,0);
  return func_001DDDE8(*DAT_0017cab4,(float *)(param_1 + 0x3d0),3,DAT_0017cab0);
}

