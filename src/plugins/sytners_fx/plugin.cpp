/**
 * MIT License
 *
 * Copyright (c) 2020 Philip Klatt
 * Copyright (c) 2021 Sytner
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
**/

#include "swg/camera/camera.h"
#include "swg/game/game.h"
#include "swg/graphics/directx9.h"
#include "swg/graphics/graphics.h"
#include "swg/misc/repository.h"
#include "swg/misc/swg_math.h"
#include "swg/object/player_object.h"
#include "swg/scene/ground_scene.h"
#include "swg/scene/terrain.h"
#include "swg/scene/world_snapshot.h"
#include "swg/ui/cui_misc.h"
#include "swg/ui/imgui_impl.h"
#include "imgui/imgui.h"
#include "plugin_framework/utinni_plugin.h"
#include <DirectXMath.h>
#include "d3dx9.h"
#include "swg/graphics/texture_resolver.h"
#include "utility/log.h"

using namespace utinni;
using namespace DirectX;

int getWidth()
{
    return Graphics::getCurrentRenderTargetWidth();
}

int getHeight()
{
    return Graphics::getCurrentRenderTargetHeight();
}

class SytnersFX : public UtinniPlugin
{
public:
    void init() override
    {
        imgui_impl::addRenderCallback([this]() {
            this->drawUI();
            });

        auto resolver = directX::getTextureResolver();
        resolver->addCallback([this](IDirect3DDevice9* device, IDirect3DTexture9* depth, IDirect3DTexture9* color) {
            onCallback(device, depth, color);
            });
    }

    void drawUI()
    {
        ImGui::GetStyle().Colors[ImGuiCol_WindowBg].w = 0.6f;

        ImGui::Begin("Sytner's FX", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse); // ImVec2(250, 300), 0.9f,  ImGuiWindowFlags_NoResize |
        {
            ImGui::Checkbox("Enabled", &m_enabled);
            ImGui::CollapsingHeader("Fun", ImGuiTreeNodeFlags_DefaultOpen);
            ImGui::Checkbox("ASCII mode", &m_ascii_enabled);
            ImGui::Checkbox("8-bit mode", &m_8bit_enabled);
            ImGui::SliderFloat("8-bit pixel size", &m_8bit_settings.x, 50, 500);
            ImGui::SliderFloat("8-bit colours", &m_8bit_settings.y, 2, 16);

            ImGui::CollapsingHeader("Sharpening", ImGuiTreeNodeFlags_DefaultOpen);
            ImGui::Checkbox("Enable sharpening", &m_sharpening);
            ImGui::SliderFloat("Sharpening amount", &m_sharpening_amount, 0, 1);
            ImGui::SliderFloat("Gamma", &m_gamma, 0.1f, 3.0);
            
            ImGui::CollapsingHeader("Colour", ImGuiTreeNodeFlags_DefaultOpen);
            ImGui::Combo("Mode", &m_colouring_mode, "None\0Hue adjustment\0Colour grading");

            if (m_colouring_mode == 1)
            {
                float col[3] = { m_color.x, m_color.y, m_color.z };
                if (ImGui::ColorPicker3("Hue", col))
                {
                    m_color.x = col[0];
                    m_color.y = col[1];
                    m_color.z = col[2];
                }
            }
            if (m_colouring_mode == 2 && m_luts[m_lut_index] != nullptr)
            {
                ImGui::Combo("LUT", &m_lut_index, "Default\0Sepia\0Mono\0Coronet\0Saturated");

                D3DSURFACE_DESC lutDesc;
                m_luts[m_lut_index]->GetLevelDesc(0, &lutDesc);
                const ImVec2 size((float)lutDesc.Width, (float)lutDesc.Height);
                ImGui::Image((void*)m_luts[m_lut_index], size);
            }
        }
    }

    struct UVPosW
    {
        XMFLOAT4 position;
        XMFLOAT2 uv;
    };

    void loadShaderVS(IDirect3DDevice9* device, IDirect3DVertexShader9** ps, const char* name, const char* entry = "main")
    {        
        LPD3DXBUFFER dxShaderCode;
        LPD3DXBUFFER errors = nullptr;
        if (FAILED(D3DXCompileShaderFromFile((getPath() + "/shaders/" + std::string(name)).c_str(), nullptr, nullptr, entry,
            "vs_3_0", 0, &dxShaderCode, &errors, nullptr))) {
            log::error((const char*)errors->GetBufferPointer());
            errors->Release();
        }
        device->CreateVertexShader((DWORD*)dxShaderCode->GetBufferPointer(), ps);
        dxShaderCode->Release();
    }

    void loadShaderPS(IDirect3DDevice9* device, IDirect3DPixelShader9** ps, const char* name, const char* entry = "main")
    {
        LPD3DXBUFFER dxShaderCode;
        LPD3DXBUFFER errors = nullptr;
        if (FAILED(D3DXCompileShaderFromFile((getPath() + "/shaders/" + std::string(name)).c_str(), nullptr, nullptr, entry,
            "ps_3_0", 0, &dxShaderCode, &errors, nullptr))) {
            log::error((const char*)errors->GetBufferPointer());
            errors->Release();
        }
        device->CreatePixelShader((DWORD*)dxShaderCode->GetBufferPointer(), ps);
        dxShaderCode->Release();
    }

    void createResources(IDirect3DDevice9* device)
    {
        auto w = getWidth();
        auto h = getHeight();
        if ((w != m_width || h != m_height) && w > 0 && h > 0)
        {
            m_width = (UINT)w;
            m_height = (UINT)h;
            device->CreateTexture(m_width, m_height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_swap0, nullptr);
            device->CreateTexture(m_width, m_height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &m_swap1, nullptr);
            
            D3DXCreateTextureFromFile(device, (getPath() + "/shaders/luts/lut_default.png").c_str(), &m_luts[0]);
            D3DXCreateTextureFromFile(device, (getPath() + "/shaders/luts/lut_sepia.png").c_str(), &m_luts[1]);
            D3DXCreateTextureFromFile(device, (getPath() + "/shaders/luts/lut_mono.png").c_str(), &m_luts[2]);
            D3DXCreateTextureFromFile(device, (getPath() + "/shaders/luts/lut_coro.png").c_str(), &m_luts[3]);
            D3DXCreateTextureFromFile(device, (getPath() + "/shaders/luts/lut_saturated.png").c_str(), &m_luts[4]);
            D3DVERTEXELEMENT9 simple_decl[] =
            {
                {0,  0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
                {0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
                D3DDECL_END()
            };
            device->CreateVertexDeclaration(simple_decl, &m_fs_vertex_decl);

            UVPosW fsTriangle[3];
            for (int i = 0; i < 3; ++i)
            {
                fsTriangle[i].uv = XMFLOAT2(float((i << 1) & 2), float(i & 2));
                fsTriangle[i].position = XMFLOAT4(fsTriangle[i].uv.x * 2.0f - 1.0f, fsTriangle[i].uv.y * -2.0f + 1.0f, 0.0f, 1.0f);
            }

            loadShaderPS(device, &m_ps_grading, "grading.ps");
            loadShaderPS(device, &m_ps_hue, "hue.ps");
            loadShaderPS(device, &m_ps_gamma, "gamma.ps");
            loadShaderPS(device, &m_ps_sharpen, "sharpen.ps");
            loadShaderPS(device, &m_ps_ascii, "ascii.ps");
            loadShaderPS(device, &m_ps_8bit, "8bit.ps");
            loadShaderVS(device, &m_vs_fs_tri, "fs_posn_uv.vs");

            device->CreateVertexBuffer(3 * sizeof(UVPosW), D3DUSAGE_WRITEONLY, 0, D3DPOOL_DEFAULT, &m_vb_fs_tri, nullptr);
            HRESULT hr;
            void* vb_vertices;
            hr = m_vb_fs_tri->Lock(0, 0, &vb_vertices, 0);
            memcpy(vb_vertices, fsTriangle, 3 * sizeof(UVPosW));
            m_vb_fs_tri->Unlock();
        }
    }

    void postProcess(IDirect3DDevice9* device, IDirect3DTexture9* depth, IDirect3DTexture9* color)
    {
        if (m_vb_fs_tri == nullptr)
        {
            return;
        }

        IDirect3DSurface9* surface;
        device->GetRenderTarget(0, &surface);

        IDirect3DSurface9* swap0surface;
        m_swap0->GetSurfaceLevel(0, &swap0surface);
        IDirect3DSurface9* swap1surface;
        m_swap1->GetSurfaceLevel(0, &swap1surface);

        device->SetRenderTarget(0, swap0surface);
        device->Clear(0, nullptr, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0);
        if (m_colouring_mode == 1)
        {
            XMFLOAT4 texelInfo{ 0.5f / (float)m_width, 0.5f / (float)m_height, 0,0 };
            device->SetStreamSource(0, m_vb_fs_tri, 0, sizeof(UVPosW));
            device->SetVertexShader(m_vs_fs_tri);
            device->SetPixelShader(m_ps_hue);
            device->SetPixelShaderConstantF(85, &texelInfo.x, 1);
            device->SetPixelShaderConstantF(86, &m_color.x, 1);
            device->SetVertexDeclaration(m_fs_vertex_decl);
            device->SetTexture(0, color);

            device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
        }
        else if (m_colouring_mode == 2)
        {
            D3DSURFACE_DESC lutDesc;
            m_luts[m_lut_index]->GetLevelDesc(0, &lutDesc);
            XMFLOAT4 lutInfo{ (float)lutDesc.Width, (float)lutDesc.Height, 0, 0 };

            XMFLOAT4 texelInfo{ 0.5f / (float)m_width, 0.5f / (float)m_height, 0,0 };
            device->SetStreamSource(0, m_vb_fs_tri, 0, sizeof(UVPosW));
            device->SetVertexShader(m_vs_fs_tri);
            device->SetPixelShader(m_ps_grading);
            device->SetPixelShaderConstantF(85, &texelInfo.x, 1);
            device->SetPixelShaderConstantF(87, &lutInfo.x, 1);
            device->SetVertexDeclaration(m_fs_vertex_decl);
            device->SetTexture(0, color);
            device->SetTexture(1, m_luts[m_lut_index]);

            device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
        }
        else
        {
            // no-op with color 1,1,1,1
            XMFLOAT4 white{ 1,1,1,1 };
            XMFLOAT4 texelInfo{ 0.5f / (float)m_width, 0.5f / (float)m_height, 0,0 };
            device->SetPixelShader(m_ps_hue);
            device->SetPixelShaderConstantF(85, &texelInfo.x, 1);
            device->SetPixelShaderConstantF(86, &white.x, 1);
            device->SetVertexDeclaration(m_fs_vertex_decl);
            device->SetTexture(0, color);

            device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
        }

        device->SetRenderTarget(0, swap1surface);
        device->Clear(0, nullptr, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0);
        device->SetStreamSource(0, m_vb_fs_tri, 0, sizeof(UVPosW));
        device->SetVertexShader(m_vs_fs_tri);
        if (m_ascii_enabled)
        {
            // ascii
            XMFLOAT4 texelInfo{ 0.5f / (float)m_width, 0.5f / (float)m_height, (float)m_width, (float)m_height };
            device->SetPixelShader(m_ps_ascii);
            device->SetPixelShaderConstantF(85, &texelInfo.x, 1);
        }
        else if (m_8bit_enabled)
        {
            XMFLOAT4 texelInfo{ 0.5f / (float)m_width, 0.5f / (float)m_height, (float)m_width, (float)m_height };
            device->SetPixelShader(m_ps_8bit);
            device->SetPixelShaderConstantF(85, &texelInfo.x, 1);
            device->SetPixelShaderConstantF(86, &m_8bit_settings.x, 1);
        }
        else
        {
            // sharpen
            XMFLOAT4 texelInfo{ 0.5f / (float)m_width, 0.5f / (float)m_height, m_sharpening ? m_sharpening_amount : 0.0f,0 };
            device->SetPixelShader(m_ps_sharpen);
            device->SetPixelShaderConstantF(85, &texelInfo.x, 1);
        }
        device->SetVertexDeclaration(m_fs_vertex_decl);
        device->SetTexture(0, m_swap0);
        device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);

        device->SetRenderTarget(0, surface);

        //D3DVIEWPORT9 vp;
        //vp.X = 0;
        //vp.Y = 0;
        //vp.Width = 500;
        //vp.Height = 500;
        //vp.MinZ = 0;
        //vp.MaxZ = 1;
        //device->SetViewport(&vp);

        device->SetStreamSource(0, m_vb_fs_tri, 0, sizeof(UVPosW));
        device->SetVertexShader(m_vs_fs_tri);
        XMFLOAT4 texelInfo{ 0.5f / (float)m_width, 0.5f / (float)m_height, m_gamma, 0 };
        device->SetPixelShader(m_ps_gamma);
        device->SetPixelShaderConstantF(85, &texelInfo.x, 1);
        device->SetVertexDeclaration(m_fs_vertex_decl);
        device->SetTexture(0, m_swap1);
        device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
    }

    void postProcessSimple(IDirect3DDevice9* device, IDirect3DTexture9* depth, IDirect3DTexture9* color)
    {
        if (m_vb_fs_tri == nullptr)
        {
            return;
        }/*
        IDirect3DSurface9* surface;
        device->GetRenderTarget(0, &surface);
        device->SetRenderTarget(0, surface);*/

        device->SetStreamSource(0, m_vb_fs_tri, 0, sizeof(UVPosW));
        device->SetVertexShader(m_vs_fs_tri);
        XMFLOAT4 texelInfo{ 0.5f / (float)m_width, 0.5f / (float)m_height, m_gamma, 0 };
        device->SetPixelShader(m_ps_gamma);
        device->SetPixelShaderConstantF(85, &texelInfo.x, 1);
        device->SetVertexDeclaration(m_fs_vertex_decl);
        device->SetTexture(0, color);

        D3DVIEWPORT9 vp;
        vp.X = 0;
        vp.Y = 0;
        vp.Width = 500;
        vp.Height = 500;
        vp.MinZ = 0;
        vp.MaxZ = 1;
        device->SetViewport(&vp);
        device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
    }

    void onCallback(IDirect3DDevice9* device, IDirect3DTexture9* depth, IDirect3DTexture9* color)
    {
        createResources(device);

        // Backup the DX9 state
        IDirect3DStateBlock9* d3d9_state_block = NULL;
        if (device->CreateStateBlock(D3DSBT_ALL, &d3d9_state_block) < 0)
        {
            return;
        }
        if (d3d9_state_block->Capture() < 0)
        {
            d3d9_state_block->Release();
            return;
        }
        // Backup the DX9 transform (DX9 documentation suggests that it is included in the StateBlock but it doesn't appear to)
        D3DMATRIX last_world, last_view, last_projection;
        device->GetTransform(D3DTS_WORLD, &last_world);
        device->GetTransform(D3DTS_VIEW, &last_view);
        device->GetTransform(D3DTS_PROJECTION, &last_projection);

        // Do postprocess
        if (m_enabled)
        {
            postProcess(device, depth, color);
        }

        // Restore the DX9 transform
        device->SetTransform(D3DTS_WORLD, &last_world);
        device->SetTransform(D3DTS_VIEW, &last_view);
        device->SetTransform(D3DTS_PROJECTION, &last_projection);
        // Restore the DX9 state
        d3d9_state_block->Apply();
        d3d9_state_block->Release();

        // Apply some global state for water shaders and stuff
        auto camera = utinni::Game::getCamera();
        auto player = utinni::Game::getPlayer();

        device->SetTexture(14, color);
        device->SetTexture(15, depth);
        if (camera != nullptr && player != nullptr) {
            float tmp[4];
            tmp[0] = camera->nearPlane;
            tmp[1] = camera->farPlane;
            device->SetPixelShaderConstantF(88, &tmp[0], 1);

            auto camPos = camera->getTransform()->getPosition();
            tmp[0] = camPos.X;
            tmp[1] = camPos.Y;
            tmp[2] = camPos.Z;
            device->SetPixelShaderConstantF(89, &tmp[0], 1);

            auto playerPos = player->getTransform()->getPosition();
            tmp[0] = playerPos.X;
            tmp[1] = playerPos.Y;
            tmp[2] = playerPos.Z;
            device->SetPixelShaderConstantF(90, &tmp[0], 1);
        }
    }

    const Information& getInformation() const override
    {
        static Information info = {
            "Danger Jim",
            "Better than Utinni, many FX",
            "Sytner's FX"
        };
        return info;
    }

private:
    UINT m_width;
    UINT m_height;
    bool m_enabled = true;
    int m_colouring_mode = 1;
    XMFLOAT4 m_color = { 1,1,1,1 };
    IDirect3DTexture9* m_swap0 = nullptr;
    IDirect3DTexture9* m_swap1 = nullptr;

    int m_lut_index = 1;
    IDirect3DTexture9* m_luts[5] = {};

    bool m_ascii_enabled = false;
    bool m_8bit_enabled = false;
    XMFLOAT4 m_8bit_settings = { 200, 8, 0, 0 };
    bool m_sharpening = true;
    float m_sharpening_amount = 0.2f;
    float m_gamma = 1.2f;

    IDirect3DVertexBuffer9* m_vb_fs_tri = nullptr;
    IDirect3DVertexShader9* m_vs_fs_tri = nullptr;
    IDirect3DPixelShader9* m_ps_grading = nullptr;
    IDirect3DPixelShader9* m_ps_hue = nullptr;
    IDirect3DPixelShader9* m_ps_sharpen = nullptr;
    IDirect3DPixelShader9* m_ps_ascii = nullptr;
    IDirect3DPixelShader9* m_ps_8bit = nullptr;
    IDirect3DPixelShader9* m_ps_gamma = nullptr;
    IDirect3DVertexDeclaration9* m_fs_vertex_decl = nullptr;
};

extern "C"
{
    UTINNI_PLUGIN
    {
        return new SytnersFX();
    }
}