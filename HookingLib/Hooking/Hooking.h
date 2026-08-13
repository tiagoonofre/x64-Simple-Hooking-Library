#pragma once
#include "../Thirdparty/Minhook/MinHook.h"
#include <Windows.h>
#include <vector>

inline std::vector<uintptr_t*> vecExistingHooks;

namespace Hooking {

	void Init();
	void Shutdown();
}

class VTHooking
{
public:
	VTHooking();
	~VTHooking();

	bool Setup(std::uintptr_t* pObject);

	template<typename T>
	void HookIndex(int iIndex, T hkFunc);

	template<typename T>
	void UnHookIndex(int iIndex);

	template<typename T>
	inline T GetOriginal(int iIndex);

	inline void Restore();

	std::uintptr_t* m_pNewTable = nullptr;
	std::uintptr_t* m_pOldTable = nullptr;
	std::uintptr_t** m_pBase = nullptr;

private:
	size_t GetVMTSize(std::uintptr_t* pVmt);
};

template<typename T>
void VTHooking::HookIndex(int iIndex, T hkFunc) {
	m_pNewTable[iIndex + 1] = reinterpret_cast< std::uintptr_t >(hkFunc);
}

template<typename T>
inline void VTHooking::UnHookIndex(int iIndex)
{
	m_pNewTable[iIndex + 1] = m_pOldTable[iIndex];
}

template<typename T>
inline T VTHooking::GetOriginal(int iIndex)
{
	return reinterpret_cast<T>(m_pOldTable ? m_pOldTable[iIndex] : 0);
}

inline void VTHooking::Restore()
{
	if (m_pBase && m_pOldTable) {
		*m_pBase = m_pOldTable;

		if (m_pNewTable) {
			delete[] m_pNewTable; 
			m_pNewTable = nullptr;
		}
	}
}

class InlineHooking
{
public:
	InlineHooking();
	~InlineHooking();

	template<typename T>
	bool Setup(std::uintptr_t* pTargetFunc, T pHook);

	template<typename T>
	inline T GetOriginal();

private:
	uintptr_t* m_pOldFunc = nullptr;

};

template<typename T>
inline bool InlineHooking::Setup(std::uintptr_t* pTargetFunc, T pHook) {

	if (!pTargetFunc) 
		return false;
	
	MH_STATUS MHStatus = MH_CreateHook(pTargetFunc, pHook, reinterpret_cast<LPVOID*>(&m_pOldFunc));

	if (MHStatus != MH_OK)
		return false;

	MHStatus = MH_EnableHook(pTargetFunc);

	if (MHStatus != MH_OK)
		return false;

	vecExistingHooks.push_back(pTargetFunc);

	return true;
}

template<typename T>
inline T InlineHooking::GetOriginal()
{
	return reinterpret_cast<T>(m_pOldFunc ? m_pOldFunc : 0);
}
