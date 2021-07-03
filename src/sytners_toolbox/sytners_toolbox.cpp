#include "swg/graphics/directx9.h"
#include "swg/game/game.h"
#include "swg/camera/camera.h"
#include "swg/graphics/graphics.h"
#include "swg/scene/ground_scene.h"
#include "plugin_framework/utinni_plugin.h"
#include "swg/ui/imgui_impl.h"
#include "swg/misc/swg_math.h"
#include "imGuIZMO.quat/imGuIZMOquat.h"
#include "imgui/imgui.h"

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
    auto depthTex = directX::getDepthTexture();
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
    auto colorTex = directX::getDepthTexture();
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

void SytnersUtinniUI()
{
    static bool showDepthWindow = false;
    static bool showColorWindow = false;

    ImGui::GetStyle().Colors[ImGuiCol_WindowBg].w = 0.6f;
    ImGui::GetStyle().ItemSpacing.y = 1.0f;
    ImGui::GetStyle().FramePadding.y = 1.0f;

    ImGui::Begin("Sytner's Toolbox", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse); // ImVec2(250, 300), 0.9f,  ImGuiWindowFlags_NoResize |
    {
        auto camera = Game::getCamera();
        if (camera != nullptr)
        {
            auto transform = *camera->getTransform();
            swg::math::Vector pos = transform.getPosition();
            swg::math::Vector oldPos = transform.getPosition();
            if (ImGui::DragFloat3("Position", &pos.X))
            {
                transform.setPosition(pos);
                camera->setTransform_o2w(transform);
                camera->positionAndRotationChanged(false, oldPos);
            }
        }

        auto depthTex = directX::getDepthTexture();
        if (depthTex != nullptr)
        {
            if (ImGui::Checkbox("Show Depth Window", &showDepthWindow)) {}
            if (ImGui::Checkbox("Show Color Window", &showColorWindow)) {}

            int stage = depthTex->getStage();
            if (ImGui::SliderInt("Stage", &stage, 0, 11)) { depthTex->setStage(stage); }
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

class SytnersToolboxPlugin : public UtinniPlugin
{
public:
    virtual void init() override
    {
        imgui_impl::addRenderCallback([]() {
            SytnersUtinniUI();
            });
    }

    virtual const Information& getInformation() const override
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