#pragma once 
//////////////////////////////////////////////////////////////////////////
//																		//
//																		//
//							Interfaces									//
//																		//
//																		//
//////////////////////////////////////////////////////////////////////////


__interface IFrameMemory {
	void Copy(void* source,EFrameMemoryDest dest, size_t size)			PURE;
	void SetVariables(ComPtr<ID3D12GraphicsCommandList>& commandList)	PURE;
};