#pragma once
#include "sdk.h"
#include "window.h"
#include <cmath>

namespace W2S {

    inline bool WorldToScreen(const RBX::Vec3& worldPos, const RBX::Mat4& viewMatrix,
        const RBX::Vec2& dimensions, RBX::Vec2& screen) {
        RBX::Vec4 quat{};

        float screenW = dimensions.X;
        float screenH = dimensions.Y;
        if (screenW <= 0.0f || screenH <= 0.0f)
            return false;


        quat.X = (worldPos.X * viewMatrix.data[0]) + (worldPos.Y * viewMatrix.data[1]) + (worldPos.Z * viewMatrix.data[2]) + viewMatrix.data[3];
        quat.Y = (worldPos.X * viewMatrix.data[4]) + (worldPos.Y * viewMatrix.data[5]) + (worldPos.Z * viewMatrix.data[6]) + viewMatrix.data[7];
        quat.Z = (worldPos.X * viewMatrix.data[8]) + (worldPos.Y * viewMatrix.data[9]) + (worldPos.Z * viewMatrix.data[10]) + viewMatrix.data[11];
        quat.W = (worldPos.X * viewMatrix.data[12]) + (worldPos.Y * viewMatrix.data[13]) + (worldPos.Z * viewMatrix.data[14]) + viewMatrix.data[15];

        if (!std::isfinite(quat.W) || quat.W <= 0.1f)
            return false;


        RBX::Vec3 ndc;
        ndc.X = quat.X / quat.W;
        ndc.Y = quat.Y / quat.W;
        ndc.Z = quat.Z / quat.W;

        if (!std::isfinite(ndc.X) || !std::isfinite(ndc.Y) || !std::isfinite(ndc.Z))
            return false;

        screen.X = (screenW * 0.5f * ndc.X) + (screenW * 0.5f);
        screen.Y = -(screenH * 0.5f * ndc.Y) + (screenH * 0.5f);


        return std::isfinite(screen.X) && std::isfinite(screen.Y);
    }

    inline bool WorldToScreen(const RBX::Vec3& worldPos, const RBX::Mat4& viewMatrix, RBX::Vec2& screen) {
        HWND robloxWindow = WindowUtil::FindForProcess(memory->get_process_id());
        RECT clientRect{};
        if (!robloxWindow || !GetClientRect(robloxWindow, &clientRect))
            return false;

        const RBX::Vec2 dimensions{
            static_cast<float>(clientRect.right - clientRect.left),
            static_cast<float>(clientRect.bottom - clientRect.top)
        };
        return WorldToScreen(worldPos, viewMatrix, dimensions, screen);
    }

    inline RBX::Vec2 WorldToScreen(const RBX::Vec3& worldPos, const RBX::Mat4& viewMatrix) {
        RBX::Vec2 screen{};
        WorldToScreen(worldPos, viewMatrix, screen);
        return screen;
    }
}
