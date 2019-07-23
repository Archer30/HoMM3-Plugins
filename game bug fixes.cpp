// #include "stdafx.h"
#include "..\..\include\homm3.h"

Patcher* _P;
PatcherInstance* _PI;

// by RoseKavalier ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

int __stdcall AI_split_div0(LoHook *h, HookContext *c)
{
    if (c->ebx == 0) // enemy army value of 0?
        c->ebx = 1; // make it 1 to prevent crash
    return EXEC_DEFAULT;
}

// ����������� ���� ��������� ������ � ������� ���� ������� 
// ����� � ������� ������������ ��������
void __stdcall HH_Show_Hero_Info_Dlg(HiHook *h, int this_hero_id, int a1, int a2)
{
	CALL_3(void, __thiscall, h->GetDefaultFunc(), this_hero_id, a1, a2);
	CALL_4(void, __thiscall, 0x417540, o_AdvMgr, 0, 0, 0); // Mobilize active hero
}

int __stdcall AI_waterwalk_fly(LoHook *h, HookContext *c)
{
    if (c->eax == 0) // no angel wings
    {
        _Hero_* hero = (_Hero_ *)(c->esi);
        if (hero->spell[6] == 0 && hero->spell_level[6] == 0) // this AI hero does not have the means to cast fly (id = 6)
        {
            if (hero->spell[7] != 0 || hero->spell_level[7] != 0) // this AI hero has access to waterwalk (id = 7)
            {
                if (hero->waterwalk_cast_power == -1) // waterwalk is not cast ~ waterwalk field is *(&hero + 0x116) (see 0x4E6040 Cast_waterwalk function)
                    c->return_address = 0x430231; // try to cast waterwalk instead (code checks for Boots of Levitation first...)
                else
                    c->return_address = 0x430540; // skip procedure
                return NO_EXEC_DEFAULT;
            }
        }
    }
    return EXEC_DEFAULT;
}

int __stdcall AI_TP_cursed_check(LoHook *h, HookContext *c)
{
    _Hero_* hero = (_Hero_*)c->esi;
    if (hero->GetSpecialTerrain() == 0x15) // 0x15 = cursed ground, 0x4E5130: __thiscall GetSpecialTerrain()
    {
        c->return_address = 0x56B6F4;
        return NO_EXEC_DEFAULT;
    }
    return EXEC_DEFAULT;
}

// The Castle's Lighthouse building bonus
int __stdcall castleOwnerCheck(LoHook *h, HookContext *c)
{
   _Town_ *town = (_Town_*)(c->ecx);
   _Hero_ *hero = *(_Hero_**)(c->ebp - 4); // _Hero_ is stored in temp variable [LOCAL.1]

   if (hero->owner_id == town->owner_id) // normal
      return EXEC_DEFAULT;
  
   c->return_address = 0x4E4D6C; // skip procedure
   return NO_EXEC_DEFAULT;
}

int __stdcall EarthquakeBug(LoHook *h, HookContext *c)
{
    _BattleStack_ *cre = (_BattleStack_ *)(c->eax - 0x84); // offset is to creature flags
    if (cre->index_on_side == 0) // if creature is in position 0 (1st on the left), then we don't want it to be flagged 0x200000 aka CANNOT_MOVE aka WAR_MACHINE
        return NO_EXEC_DEFAULT;
    return EXEC_DEFAULT;
}

int __stdcall faerie_button(LoHook *h, HookContext *c)
{
    if (c->eax == 15) // item 0xF is spellbook for Faerie Dragon
    {
        c->edi = *(int*)0x6A6A00; // "Cast Spell" text ~ taken from 0x46B4FE
        return NO_EXEC_DEFAULT;
    }
    return EXEC_DEFAULT;
}

int __stdcall faerie_button_RMB(LoHook *h, HookContext *c)
{
   if (c->esi == 15)
   {
      c->esi = *(int*)0x6A6A00; // "Cast Spell" text ~ taken from 0x46B4FE
      return NO_EXEC_DEFAULT;
   }
   return EXEC_DEFAULT;
}

/* // Admiral's hat bug fix + movement fixes
int __stdcall mpSeaToGround(LoHook *h, HookContext *c)
{
    _Hero_* hero = (_Hero_*)c->esi;
    disguise = (disguiseStruct*)&hero->disguise;

    if(disguise->groundMaxMP != 65535)
        c->eax = disguise->groundMaxMP;
    else
        disguise->groundMaxMP = c->eax;
    return EXEC_DEFAULT;
}

int __stdcall mpGroundToSea(LoHook *h, HookContext *c)
{
    _Hero_* hero = (_Hero_*)c->esi;
    disguise = (disguiseStruct*)&hero->disguise;
    if(disguise->groundMaxMP != 65535)
        c->ecx = disguise->groundMaxMP;
    else
        disguise->groundMaxMP = *(int*)(c->esi + 0x49);
    return EXEC_DEFAULT;
}

int __stdcall killedHeroSeaToGround(LoHook *h, HookContext *c)
{
    _Hero_* hero = (_Hero_*)c->esi;
    int currentMaxLandMovement;

    disguise = (disguiseStruct*)&hero->disguise;
    if(disguise->groundMaxMP != 65535)
        currentMaxLandMovement = disguise->groundMaxMP;
    else
        currentMaxLandMovement = hero->MaxLandMovement();
    
    hero->movement_points = hero->movement_points * currentMaxLandMovement / hero->MaxWaterMovement();

    return EXEC_DEFAULT;
} */ 

// by igrik ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
int __stdcall setActStack(LoHook* h, HookContext* c)
{
	_BattleStack_* mon = (_BattleStack_*)(int)(c->esi -656);

	// ���� ��������� � ���������� ��� ������� ������ ���
	// �� ������ �������� �������� ���� ���� �������
	if (mon->creature_id == 145 || mon->creature_id == 146) {
		if ( mon->creature.shots < 1 ) {
			c->return_address = 0x464DAD;
			return NO_EXEC_DEFAULT;
		}	
	}
	// ���� ��������� � �������� ������ ���
	// �� �������� ���� "����������"
	if (mon->creature_id == 94 || mon->creature_id == 95) {
		if ( mon->creature.shots < 1 ) {
			if ( mon->creature.flags & 32 ) {
				mon->creature.flags -= 32;
			}
		}
	}
	return EXEC_DEFAULT;
} 

// �������� ���������� ��������� ��� ����� ������� ���� "����������"
int __stdcall catapultaShoot(LoHook* h, HookContext* c)
{
	_BattleStack_* mon = (_BattleStack_*)c->edi;
	mon->creature.shots -= 1;

	return EXEC_DEFAULT;
} 

// ������ �� ������ �������, ���� �������� ����������
// ��� �������� � ��������
int __stdcall monstreShoot(LoHook* h, HookContext* c)
{
	_BattleStack_* mon = (_BattleStack_*)c->esi;

	if (mon->creature.shots < 1) {
		c->return_address = 0x43FFFC;
		return NO_EXEC_DEFAULT;		
	}

	return EXEC_DEFAULT;
} 

_int_ __stdcall Y_FixBagCreatureGredeOfNeutrals(HiHook* hook, _Army_* army, _int_ creature_id)
{
    _int_ count = 0;
    _int_ i = 0;
    _int_ crGrade_id = GetCreatureGrade(creature_id);
    do {
        if (army->type[i] == creature_id || army->type[i] == crGrade_id) {
            count += army->count[i];
        }
        i++;
    } while ( i<7 );

    return count;
}

int __stdcall fixHarpyBinds(LoHook* h, HookContext* c)
{
    if (*(int*)(c->ebx + 696)) {
        c->return_address = 0x478365;
        return NO_EXEC_DEFAULT;
    }
    return EXEC_DEFAULT;
}

int __stdcall ERM_Fix_EA_E(HiHook* hook, _BattleStack_* stack )
{
    int ret = 0;
    _int32_ spell_duration[81]; // ��� ���������� ������������ ����������
    _int32_ spells_power[81];   // ��� ���������� ���� �������� ����������

    if (stack) {
        for (int i=0; i<80; i++) {
            spell_duration[i] = stack->active_spell_duration[i];
            spells_power[i] = stack->active_spells_power[i];

            if (spell_duration[i] > 0 ) // ���� ���������� �������� �� ����, �� ���������� ��� ������
                CALL_2(int, __thiscall, 0x444230, stack, i); // ResetSpellFromStack 0x444230
        }
    }

    ret = CALL_1(int, __cdecl, hook->GetDefaultFunc(), stack);

    for (int i=0; i<80; i++) {
        if (spell_duration[i] > 0) { // ���� ���������� ����� ���� ��������, �� ��������������� ���
            CALL_5(int, __thiscall, 0x444610, stack, i, spell_duration[i], spells_power[i], 0); // ApplySpell 0x444610
        }
    }

    return ret;
}

// �� ������� ������������� ����� ��� ������
_int_ __stdcall Y_AntiKavalierAndFly(LoHook* h, HookContext* c)
{
    if ( *(_dword_*)(c->ebx +132) >> 1 & 1 ) { // ��������� ���� ���������� �� �����
        c->return_address = 0x4430A3; // ������� ������ �������������� ������ (�� �� ����� �� ��������)
        return NO_EXEC_DEFAULT;
    }
    return EXEC_DEFAULT;
} 

int __stdcall Y_SetCanselWitchHut(HiHook* hook, _Hero_* hero, _int_ skill, _byte_ skill_lvl) 
{
	if (o_WndMgr->result_dlg_item_id == DIID_CANCEL) {
		return 0;
	}

	return CALL_3(int, __thiscall, hook->GetDefaultFunc(), hero, skill, skill_lvl);
}

int __stdcall Y_SetCanselScholarlySS(LoHook* h, HookContext* c)
{
	int sskill = c->edi;
	_Hero_* hero = (_Hero_*)c->ebx;

	if (!b_MsgBoxD(o_ADVEVENT_TXT->GetString(116), 2, 20, hero->second_skill[sskill] +3*sskill +2) ) 
	{
		if (hero->second_skill[sskill] == 1 ) {
			hero->second_skill_count -= 1;
			hero->second_skill_show[sskill] = 0;
		}
		hero->second_skill[sskill] -= 1;
	}
	c->return_address = 0x4A4B86;
	return NO_EXEC_DEFAULT;
}

//#define Wog_FOH_Monstr (*(int*)0x27718CC)
//
//int __stdcall Y_FixWoG_GetCreatureGrade(HiHook* hook, int mon_id) 
//{
//	int ret = CALL_1(int, __fastcall, hook->GetDefaultFunc(), mon_id);
//
//	int ret2 = 25;
//	/* Wog_FOH_Monstr = mon_id;
//	int ret2 = CALL_1(int, __fastcall, 0x74ECD3, mon_id); // ��� ����� ����� ���� �������� ECX*/ 
//
//	sprintf(o_TextBuffer, "����� ��� �������� \n ret = %d (%d)", ret, ret2);
//	b_MsgBox(o_TextBuffer, 1);
//
//	return ret;
//} 

int __stdcall Y_FixWoG_GetCreatureGrade(LoHook* h, HookContext* c)
{
	int ID = *(int*)(c->ebp +8);
	int mon_id = c->ecx; 
	int mon_idGr =  *(int*)(0xA49590 +  mon_id*4);

	if (mon_idGr == -2) {
		return EXEC_DEFAULT;
	} else if (mon_idGr == -1) {
		mon_idGr = CALL_1(int, __fastcall, 0x47AAD0, mon_id);
	}

	int mon_idGr2 = *(int*)(0xA49590 +  mon_idGr*4);

	if (mon_idGr2 == -2) {
		return EXEC_DEFAULT;
	} else if (mon_idGr2 == -1) {
		mon_idGr = CALL_1(int, __fastcall, 0x47AAD0, mon_id);
	}
	
	if (ID == mon_idGr2 ) { c->ecx = mon_idGr; }

	// sprintf(o_TextBuffer, "����� ��� �������� \n %d = %d (%d) ((%d))", ID, mon_id, mon_idGr, mon_idGr2 );
	// b_MsgBox(o_TextBuffer, 1);
	// c->return_address = 0x4E64FF;
	return EXEC_DEFAULT;
}

_int_ __stdcall Y_FixNewRoundCountInTactics(LoHook* h, HookContext* c)
{
	o_BattleMgr->round = -1;
	return EXEC_DEFAULT;
}
#define BACall_Day  (*(_int_*)0x79F0B8)
#define BACall_Turn  (*(_int_*)0x79F0BC)
_int_ __stdcall Y_FixRoundCount_WoG(LoHook* h, HookContext* c)
{
	BACall_Day = o_BattleMgr->round;
	if (o_BattleMgr->isTactics) {
		BACall_Turn--;
	} else {
		BACall_Turn = BACall_Day;
	}
	c->return_address = 0x76099A;
	return NO_EXEC_DEFAULT;
}

// ����������� �������� WoG'�� ������� ��������� ���������
_dword_ __stdcall Y_WoG_MixedPos_Fix(HiHook* hook, int x, int y, int z)
{
	_dword_ xyz = b_pack_xyz(x, y, z);
	// b_MsgBox("����������", 1);
	return xyz; 
}

// ����������� �������� WoG'�� ������� ������������� ���������
void __stdcall Y_WoG_UnMixedPos_Fix(HiHook* hook, _dword_ x, _dword_ y, _dword_ z, _dword_ xyz)
{
	// CALL_4(void, __cdecl, hook->GetDefaultFunc(), x, y, z, xyz);	
	*(_dword_*)x = b_unpack_x(xyz);
	*(_dword_*)y = b_unpack_y(xyz);
	*(_dword_*)z = b_unpack_z(xyz);
	// sprintf(o_TextBuffer, "%d (%d) \n %d (%d) \n %d (%d) \n\n 0x%X \n\n 0x%X", x, *(_dword_*)x, y, *(_dword_*)y, z, *(_dword_*)z, hook->GetDefaultFunc(), hook->GetReturnAddress() );
	// b_MsgBox(o_TextBuffer, 1);
	return; 
}

// ������������� WoG ��������� �������
// ��������� � ������� 8-�� ������
// � ������� ��������� ���� �����������
_int_ __stdcall Y_SetWogHates(LoHook* h, HookContext* c)
{
	int mult = 0; // ����� ����� ��������� � ���������
	int amtype = *(int*)(c->ebp -4);
	int dmtype = *(int*)(c->ebp -8);
	 
	switch(amtype){
		case 12:  // �����
		case 13:  // ��������
		case 150: // ��������� ��������
			if(dmtype == 54 || dmtype == 55 || dmtype == 153) mult = 50; break;
		case 36:  // ����
		case 37:  // ������ ����
			if(dmtype == 52 || dmtype == 53) mult = 50; break; 
		case 41:  // �����
		case 152: // �����������
			if(dmtype == 83 || dmtype == 155) mult = 50; break;
		case 52:  // �����
		case 53:  // ����� ������
			if(dmtype == 36 || dmtype == 37) mult = 50; break;
		case 54:  // ������
		case 55:  // ����������
		case 153: // ����� ���
			if(dmtype == 12 || dmtype == 13 || dmtype == 150) mult = 50; break;
		case 83:  // ������ ������
		case 155: // Ҹ���� ������
			if(dmtype == 41 || dmtype == 152) mult = 50; break;
		default: break;
  }
	// ������ ����� ����� ����� ������� � EDX
	c->edx = mult;
	c->return_address = 0x766EEB;
	return NO_EXEC_DEFAULT;
} 

void __stdcall Y_SelectNewMonsterToAct(HiHook* hook, _BattleMgr_* bm, int side, int stack_id_in_side)
{
	if (o_NetworkGame) {
		if ( o_GameMgr->IsPlayerHuman(bm->owner_id[1]) ) { 
			if ( o_GameMgr->IsPlayerHuman(bm->owner_id[0]) ) { 
				_int32_ net[14197];  // 56748 +40 = 56788 / 4 = 14197
				net[0] = -1;
				net[1] = NULL;
				net[2] = 1987;
				net[3] = 56788;
				net[4] = 0;
				net[5] = side; 
				net[6] = stack_id_in_side;

				int bmStart = (int)o_BattleMgr +21708 -40;
				for (int i=10; i<14197; i++) {
					net[i] = *(int*)(bmStart +i*4);
				}

				_int32_ isGood = CALL_4(_int32_, __fastcall, 0x5549E0, (int)&net, *(int*)(0x697790 +4*bm->current_side), 0, 1); 
				if ( !isGood ) CALL_1(char, __thiscall, 0x4F3D20, 0);
			}
		}
	}
	CALL_3(void, __thiscall, hook->GetDefaultFunc(), bm, side, stack_id_in_side);
}

int __stdcall Y_BM_ReceNetData(LoHook* h, HookContext* c)
{
	int id = *(int*)(c->esi +8);
	if ( id == 1987 ) {
		//sprintf(o_TextBuffer, "%d %d", *(int*)(c->esi +8), *(int*)(c->esi +12) );
		//b_MsgBox(o_TextBuffer, 1);
		_int32_ netData = c->esi;

		/* int bmStart = (int)o_BattleMgr +21708 -40; // ������ ������
		for (int i=10; i<14197; i++) {
			*(int*)(bmStart +i*4) = *(int*)(netData +i*4);
		} */ 
		int bmStart = (int)o_BattleMgr +21708; // ������ ������
		int netDSt = netData +40;
		for (int i=0; i<42; i++) {
			for (int k=0; k<1352; k++) { 
				if (k>=40 && k<44) continue;
				if (k>=116 && k<172) continue;
				if (k>=272 && k<404) continue;				
				if (k>=1056 && k<1108) continue;
				if (k>=1264) continue;

				*(char*)(bmStart +i*1352 +k) = *(char*)(netDSt +i*1352 +k);
			}
			// sprintf(o_TextBuffer, "���� %d �������� \n", *(int*)(netData +1100) );
			// b_MsgBox(o_TextBuffer, 1);
		}
		// _BattleMgr_* bm = o_BattleMgr;
		CALL_3(void, __thiscall, 0x464F10, o_BattleMgr, *(int*)(netData +20), *(int*)(netData +24));

		CALL_1(void*, __thiscall, 0x555D00, netData); // 0x555D00 void __thiscall Delete(void *this)
	}

	return EXEC_DEFAULT; 
}

// ������������� �������� ���������� � ����� (�� ����������� ������ ������������ �� �����������)
_int64_ __stdcall Y_DlgSpellBook_FixDecription_SpellPower(HiHook* hook, _Hero_ *hero, int spell, signed int damage, _BattleStack_ *stack)
{
	_int64_ power = CALL_4(_int64_, __thiscall, hook->GetDefaultFunc(), hero, spell, damage, stack);
	power += (_int64_)hero->GetSpell_Specialisation_Bonuses(spell, 0, (_int_)power);

	return power;
}


// ������ ������������� ��������� � ��������� ���� � ������� ���������� �������� ����� � �������
_bool_ isVisible_Monoliths;
_bool_ isVisible_Gates;

int __stdcall Y_Fix_ViewEarthOrAirSpell_Add_Monoliths_Show(LoHook* h, HookContext* c)
{
	c->edi = c->ecx;					// mov edi, ecx
	c->ecx = *(_dword_*)(c->esi +30);	// mov ecx, [esi+1Eh]
	int obj_id = c->ecx; // ����� �������
	_byte_ isVisible = *(_byte_*)(c->ebp +8 +3);

	int type = 0;
	if ( obj_id == 43 || obj_id == 44 || obj_id == 45 ) { // ��������
		if ( isVisible_Monoliths || isVisible ) {
			type = 3;
		}
	}

	if ( obj_id == 103 ) { // ��������� �����
		if ( isVisible_Gates || isVisible) {
			type = 4;
		}
	}

	if ( type == 3 || type == 4 ) { // ��������� ������� ����������� ��������� ��� ��������� ���� ��� ����� ��������� ����� ��� �������
		CALL_6(_int_, __fastcall, 0x5F7760, *(int*)0x6AAC88, c->esi, type, *(int*)0x6AAC80 + (*(int*)(c->ebp +0x14) * *(int*)0x68C70C), c->edi +8, *(int*)(c->ebp +0x12) );
	}

	c->return_address = 0x5F854A;
	return NO_EXEC_DEFAULT;
}

int __stdcall Y_Fix_ViewEarthOrAirSpell_Add_Monoliths_Prepare(LoHook* h, HookContext* c)
{
	int spell = *(int*)(c->ebp +8);
	int power = *(int*)(c->ebp +12);

	isVisible_Gates = false;
	isVisible_Monoliths = false;

	if ( spell == 5 ) // �������� �������
	{
		if ( power >= 2 ) { // ���������� ��������� �����
			isVisible_Gates = true;
		}
		if ( power >= 3 ) { // ���������� ��������
			isVisible_Monoliths = true;
		}
	}

	return EXEC_DEFAULT;
}

// ���� ������������� ����������� �������� ����� � ���� ������� ����� ��� ����� ���������� ����������
_int_ __stdcall Y_Fix_ReportStatusMsg_CastArmageddonSpell(HiHook* hook, _BattleMgr_ *bm, _int_ damage, _int_ spell, _Hero_ *heroA, _Hero_ *heroD, _BattleStack_ *stack, _int_ a7) 
{
	if ( !heroA ) { // � ���� ����� ��������� ���������� ����� �� �������. ������� �� ������ � ����
		heroA = bm->hero[bm->current_side];
	}

	_int_ ret = CALL_7(_int_, __thiscall, hook->GetDefaultFunc(), bm, damage, spell, heroA, heroD, stack, a7);
	return ret;
}

// ##############################################################################################################################
// ##############################################################################################################################
// ##############################################################################################################################

void startPlugin(Patcher* _P, PatcherInstance* _PI)
{
// by RoseKavalier ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	
	_PI->WriteLoHook(0x42DDA6, AI_split_div0); // Prevent crash when AI attacks "ghost" hero			
	_PI->WriteByte(0x49E4EC +1, 99); // ���������� ������ ������ � �����

	// ��  ��� ��� (�������� �������� �� �������)
	_PI->WriteByte(0x49C021, 183);
	_PI->WriteByte(0x4A763F, 183);
	_PI->WriteByte(0x4A9423, 183);
	_PI->WriteByte(0x4AA82E, 183);
	_PI->WriteByte(0x4C9662, 183);
	_PI->WriteByte(0x4FD164, 183);
	_PI->WriteByte(0x505C9F, 183);
	_PI->WriteByte(0x52CD36, 183);
	_PI->WriteJmp(0x5A365D, 0x5A3666);
	_PI->WriteJmp(0x5A37B9, 0x5A37C2);
	_PI->WriteJmp(0x464DF1, 0x464DFB);
	_PI->WriteHexPatch(0x4FD12A, "0F BF 78 24 83 FF FF 0F 84");
	_PI->WriteHexPatch(0x505C75, "0F BF 77 24 83 FE FF 74");
	_PI->WriteHexPatch(0x52CD26, "0F BF 7F 24 83 FF FF 74");
	_PI->WriteHexPatch(0x44AA4F, "90 8B 7C 81 1C");
	_PI->WriteHexPatch(0x44AA58, "90 89 F8 89 44 B2 1C");
	_PI->WriteHexPatch(0x42D922, "90 8B 56 1C 89 45 DC 89 55 E4 90");
	_PI->WriteHexPatch(0x42DA39, "8B 4D E4 90 57 51");
	_PI->WriteHexPatch(0x42DAD9, "8B 4D E4 90 57 51");

	// ����������� ���� ��������� ������ � ������� ���� ������� (����� � ������� ������������� ��������) � RoseKavalier
	_PI->WriteHiHook(0x5D52CA, CALL_, EXTENDED_, THISCALL_, HH_Show_Hero_Info_Dlg); // alternative 2 - should

	////////////////////////// AI BAGs ///////////////////////////////////////
	_PI->WriteLoHook(0x56B344, AI_TP_cursed_check);
	_PI->WriteLoHook(0x43020E, AI_waterwalk_fly);


// by Ben80 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Arrow towers damage bug fix
    // for cases:
    // Armorer sec.skill, Stoning of creature, Air shield spell
    _PI->WriteByte(0x41E3A4, 0x4D);
    _PI->WriteByte(0x41E4DF, 0x4D);
    _PI->WriteByte(0x465944, 0x4D);

	// fix Fortress attack and defense bonuses for defended Hero
    _PI->WriteByte(0x4639FE, 0x77);
    _PI->WriteByte(0x463A31, 0x76);

	// The Castle's Lighthouse building bonus
    // is now only applied to the Castle's owner
    _PI->WriteLoHook(0x4E4D40, castleOwnerCheck);

	// Earthquake Bug will no longer kill creatures and end battle
    _PI->WriteLoHook(0x465656, EarthquakeBug);

	// Admiral's hat bug fix + movement fixes
    // _PI->WriteLoHook(0x4A0CE6, mpGroundToSea);
    // _PI->WriteLoHook(0x49E334, mpSeaToGround);
    // _PI->WriteLoHook(0x4DA268, killedHeroSeaToGround);

// by igrik ///////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// ������������� ����������� �� ��� � ������ �� ������ ����� ����� (����� ������� ������ �����)
	_PI->WriteHexPatch(0x5D47B3, "0F BF 57 18  8B 4F 24 B8  FF FF FF FF  90");

	// ��������� ���������� ������ ��������� ��������
	_PI->WriteDword(0x5F3D9F, 235);  // �������� ���.Y
	_PI->WriteByte(0x5F3DA4, 21);    // �������� ���.X
	_PI->WriteDword(0x5F3DF5, 235);  // ������   ���.Y
	_PI->WriteByte(0x5F3DFA, 21);    // ������   ���.X
    _PI->WriteLoHook(0x5F5320, faerie_button);
    _PI->WriteLoHook(0x5F4C99, faerie_button_RMB);

	// ����������� ������ ERM � �������� IF:N1, ������ �������� �������� 
	// �� ����� ����������, ����������� � �������������� ����������� z, � �� ������ � z1
	_PI->WriteByte(0x749093, 0xB0);
	_PI->WriteByte(0x74909C, 0xB0);
	_PI->WriteByte(0x7490B0, 0xB0);
	_PI->WriteByte(0x7490B6, 0xB0);
	_PI->WriteByte(0x7490CD, 0xB0);

	// ����������� ������������ ������ ������ ������� (��������� � ������)
	_PI->WriteDword(0x79984C, 63);
	_PI->WriteDword(0x799850, 57);

	// ����������� ������������ ������
	// � ������� �������
	_PI->WriteDword(0x5D7ACA, 0x682A24); // iCN6432.def
	// � ������� ���������� �������
	_PI->WriteDword(0x449A41, 0x682A24); // iCN6432.def

	// ����������� ���� ����� ���������, ����� ������ ������ ��-�� ����� "� ������"
	_PI->WriteCodePatch(0x76E7D7, "%n", 24); // 15 nop 
	_PI->WriteCodePatch(0x76E80B, "%n", 13); // 13 nop
	_PI->WriteHexPatch(0x76E7D7, "8B4D 08 C601 01 C641 02 04");	

	// ����������� ������ �� ����� ����������� ����
	// ������� WoG ���������, ������� �������� ���������� ������
	_PI->WriteHexPatch(0x76D4B3, "EB17");

	// ��������� ����� ������ ���������� ��� �������� ����������
	// � �������� � ������� ������ 134
	_PI->WriteHexPatch(0x492A56, "81FF B7000000 90 7747");
	_PI->WriteDword(0x492A63, *(_int_*)0x44825F);

	// ����������� ���� �������, ����� �� � ���� ���������� ������� ��� ������� ������ ��������
	_PI->WriteByte(0x75C82C, 0xEB);
	// ����������� �������� � ������� ����� ��� ������� ����� � ����� � �����-�����
	_PI->WriteHexPatch(0x5CEA83, "EB74");
	_PI->WriteHexPatch(0x5CEACD, "2800");

	// ����������� ���� � ������������� ���������� ����� ��� �������������
	_PI->WriteByte(0x5029C0, 0xEB);

	// ����������� �������� ��� �������.���������
	// ��� �������� ���� �����
	_PI->WriteLoHook(0x464D75, setActStack);
	// ��� �������� �� ������ (����������, �������)
	_PI->WriteLoHook(0x445CF9, catapultaShoot);	
	// ������ ������� ���������
	_PI->WriteLoHook(0x43FF92, monstreShoot);	
	// ������ ������� ���������
	_PI->WriteLoHook(0x43FFF4, monstreShoot);	

	// ������� ���� (��� � SoD) ������������ ����������� ����� ��� ����� � ����������� ���������
	_PI->WriteHiHook(0x4AC5F5, CALL_, EXTENDED_, THISCALL_, Y_FixBagCreatureGredeOfNeutrals);

	// ���� ������ ������, ����� ����� ����� ��� ������� ������� ����������
    _PI->WriteLoHook(0x47835B, fixHarpyBinds);

	// �� ������� ������������� ����� ��� ������
	_PI->WriteLoHook(0x44307A, Y_AntiKavalierAndFly);

	// ������� ���� ����, ����� � ��� ������������� ���� ����� EA:E � �����, ������, �����, ��������, ��������� � �.�. ������ ���������������.
	// ��-�� ����� �������� ������ ���������� ���������� (�������� ����� �������� �� ���������)
	_PI->WriteHiHook(0x726DE4, CALL_, EXTENDED_, CDECL_, ERM_Fix_EA_E);

	// ������ ������ ������ � ������ ������ 
	_PI->WriteDword(0x4A7E63 +1, 2);
	_PI->WriteHiHook(0x4A7E8A, CALL_, EXTENDED_, THISCALL_, Y_SetCanselWitchHut);

	// ������ ������ ������ � �������, ������������� ����.�����
	_PI->WriteLoHook(0x4A4AFE, Y_SetCanselScholarlySS);	

	// ������ �������� ����� ������ ������������ �� ��������� ����� ���������
	// _PI->WriteHiHook(0x4E64FA, CALL_, EXTENDED_, FASTCALL_, Y_FixWoG_GetCreatureGrade);
	_PI->WriteLoHook(0x4E64D1, Y_FixWoG_GetCreatureGrade);

	// o_BattleMgr->Round = 1; ������ ������ � �������� ������� SOD.
	// ����� ����������� ���� ������ ����� ������ ��� = 1
	// � ��� ����������� ���� ������ ����� ������ ��� = 0
	// ������ ������ ������ ����� ����� = 0
	_PI->WriteLoHook(0x473E73, Y_FixNewRoundCountInTactics);
	_PI->WriteLoHook(0x474B79, Y_FixNewRoundCountInTactics);
	_PI->WriteLoHook(0x4758B3, Y_FixNewRoundCountInTactics);
	_PI->WriteDword(0x75D125, 0);
	_PI->WriteLoHook(0x760973, Y_FixRoundCount_WoG);

	// ����������� �������� WoG'�� ������� ��������� ���������
	_PI->WriteHiHook(0x711E7F, SPLICE_, EXTENDED_, CDECL_, Y_WoG_MixedPos_Fix);
	_PI->WriteHiHook(0x711F49, SPLICE_, SAFE_, CDECL_, Y_WoG_UnMixedPos_Fix);

	// ������������� WoG ��������� �������
	// ��������� � ������� 8-�� ������
	_PI->WriteLoHook(0x766E4E, Y_SetWogHates);

	// ��������� ����������� ���������������� 
	// ������� ����������� ���������� ������ � �����
	_PI->WriteHiHook(0x464F10, SPLICE_, EXTENDED_, THISCALL_, Y_SelectNewMonsterToAct);
	_PI->WriteLoHook(0x473D41, Y_BM_ReceNetData);

	// ������ �������� �� ��������� ������ �������
	// ������ ������� ������ ������, 
	// ����� � ������ ������� �� ������������ ���� �3
	_PI->WriteByte(0x767A05 +1, 55); 
	_PI->WriteByte(0x767A2A +1, 89); 

	// ���� ������ ���� ����� ��� ����� �� vs ������� (������� �� ��� ������� ��� �����)
	// ���� � ���, ��� ���� �������� �� ���� V997, � ������ ���� V998
	_PI->WriteByte(0x762601 +3, 0xC5);

	// ������� �������� ����������� ��������� ����� (� ������� �����������) � ������� ����� ��������.
	// ���� � ������� ���� ����������� �������� �� ��������� �� ����������� ( MOVSX -> MOVZX )
	_PI->WriteByte(0x71F3FC, 0xB6);
	_PI->WriteByte(0x71F5BA, 0xB6);
	_PI->WriteByte(0x71F5D3, 0xB6);
	_PI->WriteByte(0x723657, 0xB6);
	_PI->WriteByte(0x723219, 0xB6);
	_PI->WriteByte(0x7238D8, 0xB6);
	_PI->WriteByte(0x7217BB, 0xB6);
	_PI->WriteByte(0x723CBD, 0xB6);
	_PI->WriteByte(0x721B03, 0xB6);
	_PI->WriteByte(0x722792, 0xB6); 
	_PI->WriteByte(0x723ACB, 0xB6);
	_PI->WriteByte(0x723F1C, 0xB6);

	// ������������� �������� ���������� � ����� (�� ����������� ������ ������������ �� �����������)
	_PI->WriteHiHook(0x59BFFD, CALL_, SAFE_, THISCALL_, Y_DlgSpellBook_FixDecription_SpellPower);

	// ������ ������������� ��������� � ��������� ���� � ������� ���������� �������� ����� � �������		
	_PI->WriteLoHook(0x5FC3EC, Y_Fix_ViewEarthOrAirSpell_Add_Monoliths_Prepare);
	_PI->WriteLoHook(0x5F8545, Y_Fix_ViewEarthOrAirSpell_Add_Monoliths_Show);

	// ���� ������������� ����������� �������� ����� � ���� ������� ����� ��� ����� ���������� ����������
	_PI->WriteHiHook(0x5A5522, CALL_, EXTENDED_, THISCALL_, Y_Fix_ReportStatusMsg_CastArmageddonSpell);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
    static _bool_ bug_fixes_On = 0;
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        if (!bug_fixes_On)
        {
			bug_fixes_On = 1;    

			_P = GetPatcher();
			_PI = _P->CreateInstance("ERA_bug_fixes"); 

			startPlugin(_P, _PI);

		}
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}


