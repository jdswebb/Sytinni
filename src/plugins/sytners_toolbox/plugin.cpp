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
#include "imGuIZMO.quat/imGuIZMOquat.h"
#include "imgui/imgui.h"
#include "plugin_framework/utinni_plugin.h"
#include <DirectXMath.h>

using namespace utinni;

int GetWidth()
{
    return Graphics::getCurrentRenderTargetWidth() / 3;
}

int GetHeight()
{
    return Graphics::getCurrentRenderTargetHeight() / 3;
}

void DrawDepthWindow()
{
    auto depthTex = directX::getTextureResolver();
    if (depthTex == nullptr || depthTex->getTextureColor() == nullptr || utinni::Game::getPlayer() == nullptr)
        return;

    ImVec2 size2(GetWidth() + 5.0f, GetHeight() + 31.0f);
    ImGui::SetNextWindowSize(size2);
    if (ImGui::Begin("Depth", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoCollapse))
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        const ImVec2 pos = ImGui::GetCursorScreenPos();
        const ImVec2 posMax(pos.x + GetWidth(), pos.y + GetHeight());

        ImGui::SetNextWindowSize(size2);
        ImGui::BeginChild("GameWindow");

        ImGui::GetWindowDrawList()->AddImage((void*)depthTex->getTextureDepth(), pos, posMax);

        ImGui::EndChild();

        ImGui::SetWindowSize(size2);
        ImGui::PopStyleVar();
    }
}

void DrawColorWindow()
{
    auto colorTex = directX::getTextureResolver();
    if (colorTex == nullptr || colorTex->getTextureColor() == nullptr || utinni::Game::getPlayer() == nullptr)
        return;

    ImVec2 size2(GetWidth() + 5.0f, GetHeight() + 31.0f);
    ImGui::SetNextWindowSize(size2);
    if (ImGui::Begin("Color", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysUseWindowPadding | ImGuiWindowFlags_NoCollapse))
    {
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        const ImVec2 pos = ImGui::GetCursorScreenPos();
        const ImVec2 posMax(pos.x + GetWidth(), pos.y + GetHeight());

        ImGui::SetNextWindowSize(size2);
        ImGui::BeginChild("GameWindow");
        ImGui::GetWindowDrawList()->AddImage((void*)colorTex->getTextureColor(), pos, posMax);
        ImGui::EndChild();

        ImGui::SetWindowSize(size2);
        ImGui::PopStyleVar();
    }
}

inline bool ends_with(std::string const& value, std::string const& ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

class SytnersToolboxPlugin : public UtinniPlugin
{
public:
    void init() override
    {
        imgui_impl::addRenderCallback([this]() {
                this->drawUI();
            });
    }

    void drawUI()
    {
        static bool showDepthWindow = false;
        static bool showColorWindow = false;

        ImGui::GetStyle().Colors[ImGuiCol_WindowBg].w = 0.6f;

        ImGui::Begin("Sytner's Toolbox", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse); // ImVec2(250, 300), 0.9f,  ImGuiWindowFlags_NoResize |
        {
            ImGui::Text("Press F1 to toggle");

            auto scene = GroundScene::get();
            if (scene != nullptr)
            {
                auto player = scene->playerObject;
                if (player != nullptr)
                {
                    ImGui::CollapsingHeader("Player", ImGuiTreeNodeFlags_DefaultOpen);

                    auto transform = *player->getTransform();
                    swg::math::Vector pos = transform.getPosition();
                    swg::math::Vector oldPos = transform.getPosition();
                    if (ImGui::DragFloat3("Player position", &pos.X))
                    {
                        transform.setPosition(pos);
                        player->setTransform_o2w(transform);
                        player->positionAndRotationChanged(false, oldPos);
                    }

                    swg::math::Quaternion q(transform);
                    DirectX::XMFLOAT4 xfm{ q.X,q.Y,q.Z,q.W };
                    XMStoreFloat4(&xfm, DirectX::XMQuaternionConjugate(XMLoadFloat4(&xfm)));
                    if (ImGui::gizmo3D("Free-cam rotation", *(quat*)&xfm.x, 200))
                    {
                        XMStoreFloat4(&xfm, DirectX::XMQuaternionConjugate(XMLoadFloat4(&xfm)));
                        DirectX::XMFLOAT3X3 m;
                        XMStoreFloat3x3(&m, DirectX::XMMatrixRotationQuaternion(XMLoadFloat4(&xfm)));
                        transform.matrix[0][0] = m._11;
                        transform.matrix[0][1] = m._12;
                        transform.matrix[0][2] = m._13;
                        transform.matrix[1][0] = m._21;
                        transform.matrix[1][1] = m._22;
                        transform.matrix[1][2] = m._23;
                        transform.matrix[2][0] = m._31;
                        transform.matrix[2][1] = m._32;
                        transform.matrix[2][2] = m._33;
                        player->setTransform_o2w(transform);
                        player->positionAndRotationChanged(false, oldPos);
                    }

                    if (ImGui::Button("Toggle appearance"))
                    {
                        playerObject::togglePlayerAppearance();
                    }

                    float speed = playerObject::getSpeed();
                    if (ImGui::SliderFloat("Speed", &speed, 0.0001f, 100.0f))
                    {
                        playerObject::setSpeed(speed);
                    }
                }

                ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen);
                bool freeCam = scene->isFreeCameraActive();
                if (ImGui::Checkbox("Free camera", &freeCam))
                {
                    scene->toggleFreeCamera();
                }
                if (freeCam)
                {
                    auto camera = scene->getCurrentCamera();
                    if (camera != nullptr)
                    {
                        auto transform = *camera->getTransform();
                        swg::math::Vector pos = transform.getPosition();
                        swg::math::Vector oldPos = transform.getPosition();
                        if (ImGui::DragFloat3("Camera position", &pos.X))
                        {
                            transform.setPosition(pos);
                            camera->setTransform_o2w(transform);
                            camera->positionAndRotationChanged(false, oldPos);
                        }

                        swg::math::Quaternion q(transform);
                        DirectX::XMFLOAT4 xfm{ q.X,q.Y,q.Z,q.W };
                        XMStoreFloat4(&xfm, DirectX::XMQuaternionConjugate(XMLoadFloat4(&xfm)));
                        if (ImGui::gizmo3D("Free-cam rotation", *(quat*)&xfm.x, 200))
                        {
                            XMStoreFloat4(&xfm, DirectX::XMQuaternionConjugate(XMLoadFloat4(&xfm)));
                            DirectX::XMFLOAT3X3 m;
                            XMStoreFloat3x3(&m, DirectX::XMMatrixRotationQuaternion(XMLoadFloat4(&xfm)));
                            transform.matrix[0][0] = m._11;
                            transform.matrix[0][1] = m._12;
                            transform.matrix[0][2] = m._13;
                            transform.matrix[1][0] = m._21;
                            transform.matrix[1][1] = m._22;
                            transform.matrix[1][2] = m._23;
                            transform.matrix[2][0] = m._31;
                            transform.matrix[2][1] = m._32;
                            transform.matrix[2][2] = m._33;
                            camera->setTransform_o2w(transform);
                            camera->positionAndRotationChanged(false, oldPos);
                        }
                    }
                }

                ImGui::CollapsingHeader("Scene", ImGuiTreeNodeFlags_DefaultOpen);
                if (ImGui::Button("Reload terrain"))
                {
                    scene->reloadTerrain();
                }

                auto terrain = Terrain::get();
                auto repo = Game::getRepository();
                if (repo != nullptr && terrain != nullptr)
                {
                    auto terrains = repo->getDirectoryFilenames("terrain");
                    auto currTerrainName = std::string(terrain->getFilename());

                    static int currTerrain = 0;
                    std::vector<const char*> cstrings;
                    for (size_t i = 0; i < terrains.size(); ++i)
                    {
                        if (ends_with(terrains[i], ".trn"))
                        {
                            cstrings.push_back(terrains[i].c_str());
                            // stupid search because terrain name has extra stuff on it potentially
                            if (currTerrainName.find(terrains[i]) != std::string::npos)
                            {
                                currTerrain = cstrings.size() - 1;
                            }
                        }
                    }

                    if (ImGui::Combo("Terrain", &currTerrain, cstrings.data(), (int)cstrings.size()))
                    {
                        Game::loadScene(cstrings[currTerrain]);
                    }
                }

                if (terrain != nullptr)
                {
                    auto time = terrain->getTimeOfDay();
                    if (ImGui::SliderFloat("Time", &time, 0, 1))
                    {
                        terrain->setTimeOfDay(time);
                    }
                    int weather = terrain->getWeatherIndex();
                    if (ImGui::SliderInt("Weather", &weather, 0, 4))
                    {
                        terrain->setWeatherIndex(weather);
                    }
                }

                if (ImGui::Button("Reload snapshot"))
                {
                    WorldSnapshot::reload();
                }
                if (repo != nullptr && terrain != nullptr)
                {
                    auto snapshots = repo->getDirectoryFilenames("snapshot");

                    static int currSnapshot = 0;
                    std::vector<const char*> cstrings(snapshots.size());
                    for (size_t i = 0; i < snapshots.size(); ++i)
                    {
                        cstrings[i] = snapshots[i].c_str();
                    }

                    if (ImGui::Combo("Snapshot", &currSnapshot, cstrings.data(), (int)cstrings.size()))
                    {
                        WorldSnapshot::load(cstrings[currSnapshot]);
                    }
                }

            }
            
            ImGui::CollapsingHeader("Graphics", ImGuiTreeNodeFlags_DefaultOpen);
            if (ImGui::Button("Reload textures"))
            {
                Graphics::reloadTextures();
            }
            auto depthTex = directX::getTextureResolver();
            if (!(depthTex == nullptr || depthTex->getTextureColor() == nullptr || utinni::Game::getPlayer() == nullptr))
            {
                if (ImGui::Checkbox("Show Depth Window", &showDepthWindow)) {}
                if (ImGui::Checkbox("Show Color Window", &showColorWindow)) {}

                int stage = depthTex->getStage();
                if (ImGui::SliderInt("Stage", &stage, 0, 11)) { depthTex->setStage(stage); }
            }

            ImGui::CollapsingHeader("UI", ImGuiTreeNodeFlags_DefaultOpen);
            if (ImGui::Button("Reload UI"))
            {
                cuiMisc::reloadUi();
            }
        }

        if (showDepthWindow)
        {
            DrawDepthWindow();
        }

        if (showColorWindow)
        {
            DrawColorWindow();
        }
    }

    const Information& getInformation() const override
    {
        static Information info = {
            "Danger Jim",
            "Better than Utinni, no C#",
            "Sytner's Toolbox"
        };
        return info;
    }
};

extern "C"
{
    UTINNI_PLUGIN
    {
        return new SytnersToolboxPlugin();
    }
}