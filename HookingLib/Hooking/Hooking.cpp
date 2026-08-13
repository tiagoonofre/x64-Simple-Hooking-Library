#include "Hooking.h"

VTHooking::VTHooking() {

}

VTHooking::~VTHooking() {

}

bool VTHooking::Setup(std::uintptr_t* pObject) {

	size_t TotalSize = GetVMTSize(pObject);

	if (!TotalSize)
		return false;

	m_pBase = reinterpret_cast< std::uintptr_t** >(pObject);
	m_pNewTable = new std::uintptr_t[TotalSize + 1]();
	m_pOldTable = *( std::uintptr_t** )pObject;

	std::memcpy(m_pNewTable, ( void* )&m_pOldTable[-1], (TotalSize * sizeof(std::uintptr_t)) + sizeof(std::uintptr_t));

	*( std::uintptr_t** )m_pBase = &m_pNewTable[1];

	return true;
}

size_t VTHooking::GetVMTSize(std::uintptr_t* pObject) {

	size_t Size = 0;
	MEMORY_BASIC_INFORMATION MemBasicInfo;

	std::uintptr_t* pVMT = *( std::uintptr_t** )pObject;

	while (true) {
		if (VirtualQuery(reinterpret_cast< LPCVOID >(pVMT[Size]), &MemBasicInfo, sizeof(MemBasicInfo)) == 0)
			break;

		if (MemBasicInfo.Protect == PAGE_EXECUTE_READ || MemBasicInfo.Protect == PAGE_EXECUTE_READWRITE) {
			Size++;
		}
		else {
			break;
		}
	}

	return Size;
}

void Hooking::Init() { 

	MH_STATUS MHStatus = MH_Initialize();

	if (MHStatus != MH_OK)
		return;
}

InlineHooking::InlineHooking() {

}

InlineHooking::~InlineHooking() {

}

void Hooking::Shutdown() {

	MH_DisableHook(MH_ALL_HOOKS);
	MH_Uninitialize();
}