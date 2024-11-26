#include "gui.h"

#include "../offsets/globals.h"

#include "../../imgui/imgui.h"
#include "../../imgui/imgui_impl_dx9.h"
#include "../../imgui/imgui_impl_win32.h"
#include <cmath>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
    HWND window,
    UINT message,
    WPARAM wideParameter,
    LPARAM longParameter
);

LRESULT CALLBACK WindowProcess(
    HWND window,
    UINT message,
    WPARAM wideParameter,
    LPARAM longParameter)
{
    if (ImGui_ImplWin32_WndProcHandler(window, message, wideParameter, longParameter))
        return true;

    switch (message)
    {
    case WM_SIZE: {
        if (gui::device && wideParameter != SIZE_MINIMIZED)
        {
            gui::presentParameters.BackBufferWidth = LOWORD(longParameter);
            gui::presentParameters.BackBufferHeight = HIWORD(longParameter);
            gui::ResetDevice();
        }
        return 0;
    }

    case WM_SYSCOMMAND: {
        if ((wideParameter & 0xfff0) == SC_KEYMENU)
            return 0;
        break;
    }

    case WM_DESTROY: {
        PostQuitMessage(0);
        return 0;
    }

    case WM_LBUTTONDOWN: {
        gui::position = MAKEPOINTS(longParameter);
        return 0;
    }

    case WM_MOUSEMOVE: {
        if (wideParameter == MK_LBUTTON)
        {
            const auto points = MAKEPOINTS(longParameter);
            auto rect = RECT{};

            GetWindowRect(gui::window, &rect);

            rect.left += points.x - gui::position.x;
            rect.top += points.y - gui::position.y;

            if (gui::position.x >= 0 &&
                gui::position.x <= gui::WIDTH &&
                gui::position.y >= 0 && gui::position.y <= 19)
                SetWindowPos(
                    gui::window,
                    HWND_TOPMOST,
                    rect.left,
                    rect.top,
                    0, 0,
                    SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOZORDER
                );
        }
        return 0;
    }

    }

    return DefWindowProc(window, message, wideParameter, longParameter);
}

void gui::CreateHWindow(const char* windowName) noexcept
{
    WNDCLASSEXA windowClass = { sizeof(WNDCLASSEXA), CS_CLASSDC, WindowProcess, 0L, 0L,
                                GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr,
                                "class001", nullptr };
    RegisterClassExA(&windowClass);

    window = CreateWindowExA(
        0,
        "class001",
        windowName,
        WS_POPUP,
        500,
        500,
        WIDTH,
        HEIGHT,
        0,
        0,
        windowClass.hInstance,
        0
    );

    SetWindowPos(window, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);

    ShowWindow(window, SW_SHOWDEFAULT);
    UpdateWindow(window);
}

void gui::DestroyHWindow() noexcept
{
    DestroyWindow(window);
    UnregisterClassA("class001", GetModuleHandle(nullptr));
}

bool gui::CreateDevice() noexcept
{
    d3d = Direct3DCreate9(D3D_SDK_VERSION);

    if (!d3d)
        return false;

    ZeroMemory(&presentParameters, sizeof(presentParameters));

    presentParameters.Windowed = TRUE;
    presentParameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
    presentParameters.BackBufferFormat = D3DFMT_UNKNOWN;
    presentParameters.EnableAutoDepthStencil = TRUE;
    presentParameters.AutoDepthStencilFormat = D3DFMT_D16;
    presentParameters.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

    if (d3d->CreateDevice(
        D3DADAPTER_DEFAULT,
        D3DDEVTYPE_HAL,
        window,
        D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &presentParameters,
        &device) < 0)
        return false;

    return true;
}

void gui::ResetDevice() noexcept
{
    ImGui_ImplDX9_InvalidateDeviceObjects();

    const auto result = device->Reset(&presentParameters);

    if (result == D3DERR_INVALIDCALL)
        IM_ASSERT(0);

    ImGui_ImplDX9_CreateDeviceObjects();
}

void gui::DestroyDevice() noexcept
{
    if (device)
    {
        device->Release();
        device = nullptr;
    }

    if (d3d)
    {
        d3d->Release();
        d3d = nullptr;
    }
}

void gui::CreateImGui() noexcept
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = NULL;
    ImGui::StyleColorsDark();

    const char* fontPath = "C:\\Windows\\Fonts\\segoeui.ttf";
    float fontSize = 18.0f;

    if (!io.Fonts->AddFontFromFileTTF(fontPath, fontSize)) {
        io.Fonts->AddFontDefault();
    }

    ImGui_ImplWin32_Init(window);
    ImGui_ImplDX9_Init(device);
}

void gui::DestroyImGui() noexcept
{
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

void gui::BeginRender() noexcept
{
    MSG message;
    while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);

        if (message.message == WM_QUIT)
        {
            isRunning = !isRunning;
            return;
        }
    }

    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void gui::EndRender() noexcept
{
    ImGui::EndFrame();

    device->SetRenderState(D3DRS_ZENABLE, FALSE);
    device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    device->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);

    device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

    if (device->BeginScene() >= 0)
    {
        ImGui::Render();
        ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
        device->EndScene();
    }

    const auto result = device->Present(0, 0, 0, 0);

    if (result == D3DERR_DEVICELOST && device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
        ResetDevice();
}


void gui::SetupImGuiStyle() noexcept {
    ImGuiStyle* style = &ImGui::GetStyle();
    ImVec4* colors = style->Colors;

    // Rounded corners
    style->WindowRounding = 8.0f;
    style->FrameRounding = 10.0f;
    style->ScrollbarRounding = 10.0f;
    style->GrabRounding = 10.0f;

    // Padding and spacing
    style->WindowPadding = ImVec2(15, 15);
    style->FramePadding = ImVec2(10, 5);
    style->ItemSpacing = ImVec2(10, 10);

    // Dark theme with accent colors
    colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
    colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.14f, 0.15f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.20f, 0.29f, 0.37f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.41f, 0.55f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.15f, 0.25f, 0.34f, 1.00f);

    colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.30f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.35f, 0.45f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.20f, 0.29f, 0.39f, 1.00f);

    // Title bar
    colors[ImGuiCol_TitleBg] = ImVec4(0.14f, 0.18f, 0.22f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.20f, 0.24f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.13f, 0.17f, 0.21f, 1.00f);
}

std::string GetKeyName(int vk) {
    UINT scanCode = MapVirtualKey(vk, MAPVK_VK_TO_VSC);
    char keyName[128];
    int result = GetKeyNameTextA(scanCode << 16, keyName, sizeof(keyName));
    if (result > 0) {
        return std::string(keyName);
    }
    else {
        return "Unknown";
    }
}

void gui::Render() noexcept {
    static int currentTab = 0;

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 10.0f;
    style.ChildRounding = 8.0f;
    style.FrameRounding = 6.0f;
    style.ScrollbarRounding = 6.0f;
    style.GrabRounding = 6.0f;
    style.FramePadding = ImVec2(12.0f, 6.0f);
    style.ItemSpacing = ImVec2(14.0f, 14.0f);

    ImVec4 blueAccent = ImVec4(0.1f, 0.3f, 0.8f, 1.0f);
    ImVec4 lightBlue = ImVec4(0.2f, 0.4f, 0.9f, 1.0f);
    ImVec4 blueBackground = ImVec4(0.05f, 0.05f, 0.15f, 1.0f);
    ImVec4 menuColor = globals::MenuAccentColor;

    // Apply styles
    ImGui::PushStyleColor(ImGuiCol_WindowBg, blueBackground);
    ImGui::PushStyleColor(ImGuiCol_Button, menuColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(menuColor.x + 0.1f, menuColor.y + 0.1f, menuColor.z + 0.1f, menuColor.w));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(menuColor.x - 0.1f, menuColor.y - 0.1f, menuColor.z - 0.1f, menuColor.w));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.1f, 0.2f, 0.4f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, lightBlue);
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.3f, 0.3f, 0.5f, 1.0f));

    // Main window setup
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(750, 750), ImGuiCond_Always);

    ImGui::Begin(
        "VoidWare | External",
        &globals::isRunning,
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoMove
    );

    // Left sidebar
    ImGui::BeginChild("##LeftSide", ImVec2(200, ImGui::GetContentRegionAvail().y), true);
    {
        ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("VoidWare").x) * 0.5f);
        ImGui::TextColored(lightBlue, "VoidWare");
        ImGui::Separator();

        const char* tabs[] = { "Aimbot", "Triggerbot", "Visual", "Misc" };
        for (int i = 0; i < IM_ARRAYSIZE(tabs); ++i) {
            ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - 140) * 0.5f);
            if (ImGui::Button(tabs[i], ImVec2(140, 40))) {
                currentTab = i;
            }
            ImGui::Spacing();
        }

        // Exit button at bottom
        ImGui::SetCursorPosY(ImGui::GetContentRegionAvail().y - 60);
        ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - 140) * 0.5f);
        if (ImGui::Button("Exit", ImVec2(140, 40))) {
            exit(0);
        }
    }
    ImGui::EndChild();

    ImGui::SameLine();

    // Right content area
    ImGui::BeginChild("##RightSide", ImVec2(0, 0), true);
    {
        switch (currentTab) {
        case 0: // Aimbot Tab
        {
            ImGui::TextColored(blueAccent, "Aimbot Settings");
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Checkbox("Enable Aimbot", &AimbotSettings::Enabled);
            ImGui::SliderFloat("FOV", &AimbotSettings::FOV, 1.0f, 180.0f, "FOV: %.1f");
            ImGui::SliderFloat("Smoothness", &AimbotSettings::Smoothness, 1.0f, 20.0f, "Smooth: %.1f");

            ImGui::Spacing();
            ImGui::TextWrapped("Made By Dante.");
            break;
        }

        case 1: // Triggerbot Tab
        {
            ImGui::TextColored(lightBlue, "Triggerbot Settings");
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Checkbox("Enable Triggerbot", &globals::TriggerBot);

            if (globals::TriggerBot) {
                ImGui::Text("Key:");
                ImGui::SameLine();
                if (ImGui::Button(globals::TriggerBotKeyName)) {
                    ImGui::OpenPopup("Select Key");
                }

                if (ImGui::BeginPopup("Select Key")) {
                    for (int i = 0; i < 256; ++i) {
                        if (ImGui::IsKeyPressed(i)) {
                            globals::TriggerBotKey = i;
                            snprintf(globals::TriggerBotKeyName, sizeof(globals::TriggerBotKeyName), "%s", GetKeyName(i).c_str());
                            ImGui::CloseCurrentPopup();
                        }
                    }
                    ImGui::Text("Press a key to select.");
                    ImGui::EndPopup();
                }

                ImGui::Spacing();
                if (ImGui::CollapsingHeader("Settings", ImGuiTreeNodeFlags_DefaultOpen)) {
                    ImGui::SliderInt("Delay (ms)", &globals::TriggerBotDelay, 1, 1000);
                    ImGui::SliderInt("Hitchance %", &globals::TriggerHitChance, 1, 100);
                    ImGui::Checkbox("Team Check", &globals::TriggerBotTeamCheck);
                    ImGui::Checkbox("Ignore Flash", &globals::TriggerBotIgnoreFlash);
                }
            }
            break;
        }

        case 2: // Visual Tab
        {
            ImGui::TextColored(lightBlue, "Visual Settings");
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::SliderInt("FOV", &globals::FOV, 0, 160, "FOV: %d");

            ImGui::Checkbox("Glow", &globals::Glow);
            if (globals::Glow) {
                ImGui::ColorEdit4("Glow Color", (float*)&globals::GlowColor);
            }

            ImGui::Checkbox("No Flash", &globals::NoFlashEnabled);
            break;
        }

        case 3: // Misc Tab
        {
            ImGui::TextColored(lightBlue, "Misc Settings");
            ImGui::Separator();
            ImGui::Spacing();

            ImGui::Text("Menu Color");
            ImGui::ColorEdit4("##AccentColor", (float*)&globals::MenuAccentColor,
                ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_NoOptions);
            break;
        }
        }
    }
    ImGui::EndChild();

    ImGui::End();
    ImGui::PopStyleColor(7);
}
