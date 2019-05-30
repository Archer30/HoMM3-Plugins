#include "..\..\include\homm3.h"

Patcher* _P;
PatcherInstance* _PI;

#define o_TimeClick (*(_int_*)0x6977D4)
_bool_ isNeedReplay, ifCanReplay;

////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////// 
int __stdcall Y_SkipRedrawAdvMap(LoHook* h, HookContext* c)
{
	if (isNeedReplay) {
		//b_MsgBox("���������� ����� (����������)", 1);
		c->return_address = 0x4B0AC4; 
		return NO_EXEC_DEFAULT;
	}
	//b_MsgBox("���������� ����� (�� ����������)", 1);
	return EXEC_DEFAULT; 
}

int __stdcall Y_SkipALL(LoHook* h, HookContext* c)
{
	if (isNeedReplay) {
		//b_MsgBox("��� ���������� (����������)", 1);
		// CALL_4(void, __thiscall, 0x417540, o_AdvMgr, 0, 0, 1); // advManager__MobilizeCurrHero
		c->return_address = 0x4AE67C;
		return NO_EXEC_DEFAULT;
	}
	//b_MsgBox("��� ���������� (�� ����������)", 1);
	return EXEC_DEFAULT; 
}

#define o_Dlg_BattleResults (*(_int_*)0x69500C)

int __stdcall Y_SkipAddExp(LoHook* h, HookContext* c)
{
	if (isNeedReplay) {
		//b_MsgBox("���������� ����� (����������)", 1);
		c->return_address = 0x477303;
		return NO_EXEC_DEFAULT;
	}
	//b_MsgBox("���������� ����� (�� ����������)", 1);
	return EXEC_DEFAULT; 
}

////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////// 

_Dlg_* __stdcall Y_Dlg_BattleResults_Create(HiHook* hook, _Dlg_* dlg, _Hero_* heroA, _Hero_* heroD, _int_ dlgDef, _int_ sideWin, _int_ a6, _int_ allHPLoserLose)
{
	CALL_7(_Dlg_*, __thiscall, hook->GetDefaultFunc(), dlg, heroA, heroD, dlgDef, sideWin, a6, allHPLoserLose);

	if (ifCanReplay) {
		dlg->AddItem(_DlgStaticPcx8_::Create(20, 506, 0, "Box64x30.pcx"));
		dlg->AddItem(_DlgButton_::Create(21, 507, 64, 30, 30723, "iCancel.def", 0, 1, 0, 1, 2)); 
	}
	return dlg;
}

int __stdcall Y_Dlg_BattleResults_Proc(HiHook* hook, _EventMsg_* msg)
{
	if (ifCanReplay) {
		isNeedReplay = false;
		if (msg->type == MT_MOUSEBUTTON && msg->type != MT_KEYDOWN) {
			if (msg->subtype == MST_LBUTTONCLICK) {
				if (msg->item_id == 30723) {
					o_TimeClick = 15000; 
					msg->item_id == 30722;
					isNeedReplay = true;
				}
			}
		}
	}
	return CALL_1(int, __thiscall, hook->GetDefaultFunc(), msg);
}

////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////// 

int __stdcall Y_ReplayBattle(HiHook* hook, _AdvMgr_* advMng, _dword_ MixedPos, _Hero_* HrA, _Army_* MArrA, _int_ OwnerD, _dword_ townD, _Hero_* HrD, _Army_* MArrD, _int_ Pv3, _dword_ Pv2, _dword_ Pv1)
{	
	int ret = 0; 
	ifCanReplay = false;   

	_GameMgr_* gm = o_GameMgr;
	char isAHuman = 0;
	char isDHuman = 0;
	if (HrA) { isAHuman = gm->GetPlayer(HrA->owner_id)->IsHuman(); }
	if (OwnerD >= 0 ) {	isDHuman = gm->GetPlayer(OwnerD)->IsHuman(); }

	if ( o_NetworkGame || o_IsOnlineGame || o_GameMgrType == 2 || (!isAHuman && !isDHuman) ) { 
		// b_MsgBox("������������� ����� ���������", 1);
		return CALL_11(int, __thiscall, hook->GetDefaultFunc(), advMng, MixedPos, HrA, MArrA, OwnerD, townD, HrD, MArrD, Pv3, Pv2, Pv1);
	} 

	ifCanReplay = true;
	_int_ saveParamQB = o_QuickBattle;

	_Hero_* HrAS;
	_Hero_* HrDS;
	_Npc_* npcA;
	_Npc_* npcAS;
	_bool_ isNpcASCreate = false;
	_bool_ isNpcDSCreate = false;
	_Npc_* npcD;
	_Npc_* npcDS;
	_Army_* armyAS;
	_Army_* armyDS;

	if (HrA) {
		HrAS = (_Hero_*)o_New(1170);
		MemCopy(HrAS, HrA, 1170);
		npcA = GetNpc(HrA->id);                       
		if (npcA->on == 1 && npcA->alive != 1 ) {
			npcAS = (_Npc_*)o_New(296);
			MemCopy(npcAS, npcA, 296);             
			isNpcASCreate = true;
		}
	} else {
		armyAS = (_Army_*)o_New(56);
		MemCopy(armyAS, MArrA, 56);
	}

	if (HrD) {
		HrDS = (_Hero_*)o_New(1170);
		MemCopy(HrDS, HrD, 1170);                    
		npcD = GetNpc(HrD->id);        
		if (npcD->on == 1 && npcD->alive != 1) {
			npcDS = (_Npc_*)o_New(296);
			MemCopy(npcDS, npcD, 296);             
			isNpcDSCreate = true;
		}
	} else {
		armyDS = (_Army_*)o_New(56);
		MemCopy(armyDS, MArrD, 56);
	}


	do {
		if (HrA) {
			MemCopy(HrA, HrAS, 1170);
			if ( isNpcASCreate ) { 
				if ( npcAS->on == 1 && npcAS->alive != 1 ) {
					MemCopy(npcA, npcAS, 296);
				}
			}
		} else MemCopy(MArrA, armyAS, 56);

		if (HrD) {
			MemCopy(HrD, HrDS, 1170);
			if ( isNpcDSCreate ) {
				if (npcDS->on == 1 && npcDS->alive != 1) {
					MemCopy(npcD, npcDS, 296);
				}
			}
		} else MemCopy(MArrD, armyDS, 56);

		if (isNeedReplay) {
			o_QuickBattle = 0;
			CALL_1(void, __cdecl, 0x74CE30, 870520); // ������ !!FU87052:P (�����! ����� ��������� ��� ����������)
		}

		ret = CALL_11(int, __thiscall, hook->GetDefaultFunc(), advMng, MixedPos, HrA, MArrA, OwnerD, townD, HrD, MArrD, Pv3, Pv2, Pv1);

		CALL_1(void, __cdecl, 0x74CE30, 870530); // ������ !!FU87053:P (�����! ����� ��������� ��� ����������)
	} while ( isNeedReplay );

	o_QuickBattle = saveParamQB;
	ifCanReplay = false;

	return ret; 
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    static _bool_ plugin_On = 0;
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        if (!plugin_On)
        {
			plugin_On = 1;    

			_P = GetPatcher();
			_PI = _P->CreateInstance("ERABattleReplay"); 

			_PI->WriteHiHook(0x46FE20, SPLICE_, EXTENDED_, THISCALL_, Y_Dlg_BattleResults_Create);
			_PI->WriteHiHook(0x4716E0, SPLICE_, EXTENDED_, THISCALL_, Y_Dlg_BattleResults_Proc);

			_PI->WriteLoHook(0x477254, Y_SkipAddExp);       // ���������� ���������� ����� �����������
			_PI->WriteLoHook(0x4B0A9F, Y_SkipRedrawAdvMap); // ���������� ���������� ���� ����� ����� �����
			_PI->WriteLoHook(0x4ADFE8, Y_SkipALL);          // ���������� �� ����� ����� �����

			_PI->WriteHexPatch(0x75AEB0, "E8 AB22D5FF 90 90");	// CALL 0x4AD160 (BATTLE) + 2NOPs
			_PI->WriteHiHook(0x75AEB0, CALL_, EXTENDED_, THISCALL_, Y_ReplayBattle);


        }
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
