// ������ ���������� �� ����� ����� � ���������
// �����: igrik
// ����: 09/11/2018

#include "..\..\include\homm3.h"

Patcher* _P;
PatcherInstance* _PI;

//	�������: ���� �� ������ ����� � �������� (���������� ���������� ������ � ���������)
_int_ isGuardNearTheObject(_AdvMgr_* advMng, _int_ xyz)
{
	int xd = b_unpack_x(xyz);
	int yd = b_unpack_y(xyz);
	int z = b_unpack_z(xyz);

	if (!(0x0100 & CALL_3(WORD, __fastcall, 0x4F8040, xd, yd, z)) ) {
		return 0; // �������� ������ ������ ���
	} 

	int x, y;
	int size = advMng->map->size;
	// ��� �� ������ ���� --------------------------------------------------------------------------------------
	x = xd -1; y = yd;
	if ( x >= 0 && x < size && y >= 0 && y < size) {
		if (advMng->map->GetItem(x,y,z)->object_type == 54) {
			   return b_pack_xyz(x,y,z); } }

	x = xd +1; y = yd;
	if ( x >= 0 && x < size && y >= 0 && y < size) {
		if (advMng->map->GetItem(x,y,z)->object_type == 54) {
			   return b_pack_xyz(x,y,z); } }
	// ��� ���� ���� --------------------------------------------------------------------------------------------
	x = xd -1; y = yd +1;
	if ( x >= 0 && x < size && y >= 0 && y < size) {
		if (advMng->map->GetItem(x,y,z)->object_type == 54) {
			   return b_pack_xyz(x,y,z); } }

	x = xd;	y = yd +1;
	if ( x >= 0 && x < size && y >= 0 && y < size) {
		if (advMng->map->GetItem(x,y,z)->object_type == 54) {
			   return b_pack_xyz(x,y,z); } }

	x = xd +1; y = yd +1;
	if ( x >= 0 && x < size && y >= 0 && y < size) {
		if (advMng->map->GetItem(x,y,z)->object_type == 54) {
			   return b_pack_xyz(x,y,z); } }

	// ��� ���� ���� --------------------------------------------------------------------------------------------  	
	x = xd -1; y = yd -1;
	if ( x >= 0 && x < size && y >= 0 && y < size) {
		if (advMng->map->GetItem(x,y,z)->object_type == 54) {
			   return b_pack_xyz(x,y,z); } }

	x = xd; y = yd -1;
	if ( x >= 0 && x < size && y >= 0 && y < size) {
		if (advMng->map->GetItem(x,y,z)->object_type == 54) {
			   return b_pack_xyz(x,y,z); } }

	x = xd +1; y = yd -1;
	if ( x >= 0 && x < size && y >= 0 && y < size) {
		if (advMng->map->GetItem(x,y,z)->object_type == 54) {
			   return b_pack_xyz(x,y,z); } }
	// -----------------------------------------------------------------------------------------------------
	return 0; // �������� ������ ������ ���
}

// ��� �������� ���� �� ��
_int_ __stdcall Y_GetCursorFrameOnAdvMap(HiHook* hook, _AdvMgr_* advMng, _MapItem_* obj)
{ 
	switch (obj->object_type) {
		case 5: case 6: case 81: case 93: // ���� ���������� ��������
			if ( isGuardNearTheObject(advMng, advMng->pack_xyz) ) { 
				return 5; // ���������� ������ ������
			}	break;

		default: break;
	}
	return CALL_2(_int_, __thiscall, hook->GetDefaultFunc(), advMng, obj);
} 

void __stdcall Y_Hero_Enter_To_Object(HiHook* hook, _AdvMgr_* advMng, _Hero_* hero, _MapItem_* obj, _int_ xyz, _int_ isPlayer)
{
	int xyz_54 = 0; // ������������� ���������� - ���������� ���������� 

	switch (obj->object_type) {
		// case 5: case 6: case 12: case 79: case 81: case 82: case 93: case 101:  // ���� ���������� ��������
		case 5:		// ��������
		case 6:		// ���� �������
		case 81:	// ������
		case 93:	// ������ � ������������
			xyz_54 = isGuardNearTheObject(advMng, xyz); // �������� ������ ������� � �������� �������
			if ( xyz_54 ){  // ���� ������ ����
				_MapItem_* obj_54 = advMng->map->GetItem(b_unpack_x(xyz_54), b_unpack_y(xyz_54), b_unpack_z(xyz_54)); // �������� ��������� ������� "������"
				//advMng->monBattle_type = (_word_)obj_54->os_type; // (advMng+<536>)
				//advMng->monBattle_num = (_word_)obj_54->draw_num; // (advMng+<540>)
				//advMng->monBattle_side = hero->x < (_word_)((_word_)(xyz_54 << 6) >> 6) ? 1 : 0; // ������� ������� ��� ����� (advMng+<544>)

				//CALL_5(void, __thiscall, 0x4A73B0, advMng, obj_54, hero, xyz_54, isPlayer); // �������� �� ������� ����� � �����

				//advMng->monBattle_type = -1; // ����� ����� ���������� ���������
				//advMng->monBattle_num = -1;  // ����� ����� ���������� ���������
				// CALL_5(void, __thiscall, 0x4A8160, advMng, hero, obj_54, xyz_54, isPlayer);
				CALL_5(void, __thiscall, hook->GetDefaultFunc(), advMng, hero, obj_54, xyz_54, isPlayer);

				if (hero->owner_id < 0) {    // ���� ����� ���� ��� ������ (�.�. ����� ������� ����� ���� == -1)
					return; // ����� �� ������� ��������� �������
				}
			}	break;

		default: break;
	}

	CALL_5(void, __thiscall, hook->GetDefaultFunc(), advMng, hero, obj, xyz, isPlayer);
	return;
}


BOOL APIENTRY DllMain( HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
    static _bool_ plugin_On = 0;
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        if (!plugin_On)
        {
			plugin_On = 1;    

			_P = GetPatcher();
			_PI = _P->CreateInstance("ArtsGuard"); 			

			_PI->WriteHiHook(0x40E97F, CALL_, EXTENDED_, THISCALL_, Y_GetCursorFrameOnAdvMap);
			// _PI->WriteHiHook(0x4AA766, CALL_, EXTENDED_, THISCALL_, Y_Hero_Enter_To_Object);

			// SPLICE_ ������ ������������, ������ ��� ������� ����������� ������� !?OB, � ����� ������ ��� ����� SPLICE_
			// _PI->WriteHiHook(0x4A8160, SPLICE_, EXTENDED_, THISCALL_, Y_Hero_Enter_To_Object);
			 _PI->WriteHiHook(0x4ACA04, CALL_, EXTENDED_, THISCALL_, Y_Hero_Enter_To_Object); // �� 2 ���� ���� �������� ���������
			 _PI->WriteHiHook(0x4AA766, CALL_, EXTENDED_, THISCALL_, Y_Hero_Enter_To_Object); // �� 2 ���� ���� �������� ���������

			// ��������� ����������! ���������� ����������� ����� ����!
			//int ptr_hooks[2] = {0x4ACA04, 0x4AA766};
			//for (int i=0; i<2; i++) {
			//	_PI->WriteHiHook(ptr_hooks[i], CALL_, EXTENDED_, THISCALL_, Y_Hero_Enter_To_Object);
			//}

        }
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
