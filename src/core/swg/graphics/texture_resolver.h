#pragma once

#include "utinni.h"
#include <d3d9.h>
#include <functional>
#include <vector>

namespace directX
{
extern UTINNI_API std::vector<std::function<void(IDirect3DDevice9*, IDirect3DTexture9* depth, IDirect3DTexture9* color)>> resolveCallbacks;

class TextureResolver
{
private:
	 IDirect3DTexture9* pTextureDepth;
	 IDirect3DTexture9* pTextureColor;
	 bool m_isRESZ;
	 bool m_isINTZ;
	 bool m_isNVAPI;
	 bool m_isSupported;
	 IDirect3DSurface9* pRegisteredDSS;
	 IDirect3DDevice9* _pDevice;
	 int stage;

public:
	 TextureResolver();
	 ~TextureResolver();
	 void release();
	 void createTexture(LPDIRECT3DDEVICE9 pDevice, int width, int height);
	 void resolveDepth();
	 void resolveDepth(LPDIRECT3DDEVICE9 pDevice, IDirect3DSurface9* surface);

	 LPDIRECT3DTEXTURE9 getTextureDepth() { return pTextureDepth; }
	 LPDIRECT3DTEXTURE9 getTextureColor() { return pTextureColor; }
	 bool isINTZ() { return m_isINTZ; }
	 bool isSupported() { return m_isSupported; }
	 bool isNvidia() { return m_isNVAPI; }

	 int getStage() { return stage; }
	 void setStage(int value) { stage = value; }
	
	 template<typename T>
     void addCallback(T func)
     {
         resolveCallbacks.emplace_back(func);
     }
};

}
