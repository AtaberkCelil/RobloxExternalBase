#pragma once
#include "../../../../src/sdk/w2s.h"
#include "../../../../src/core/cache/cache.h"
#include "../../../../src/core/variables/variables.h"
#include "../../../../ext/imgui/imgui.h"
#include <string>
#include <string_view>
#include <algorithm>
#include <cmath>

namespace Visuals {
    // thanks to (@whowould) on github
    inline auto is_r6_body(std::string_view name) -> bool
    {
        return name == "Head" || name == "Torso"
            || name == "Left Arm" || name == "Right Arm"
            || name == "Left Leg" || name == "Right Leg";
    }
    // thanks to (@whowould) on github
    inline auto is_r15_body(std::string_view name) -> bool
    {
        return name == "Head" || name == "UpperTorso" || name == "LowerTorso"
            || name == "LeftUpperArm" || name == "LeftLowerArm" || name == "LeftHand"
            || name == "RightUpperArm" || name == "RightLowerArm" || name == "RightHand"
            || name == "LeftUpperLeg" || name == "LeftLowerLeg" || name == "LeftFoot"
            || name == "RightUpperLeg" || name == "RightLowerLeg" || name == "RightFoot";
    }

    inline void DrawOutlinedText(ImDrawList* drawList, const ImVec2& pos, const std::string& text, ImU32 textColor) {
        drawList->AddText(ImVec2(pos.x - 1, pos.y), IM_COL32(0, 0, 0, 255), text.c_str());
        drawList->AddText(ImVec2(pos.x + 1, pos.y), IM_COL32(0, 0, 0, 255), text.c_str());
        drawList->AddText(ImVec2(pos.x, pos.y - 1), IM_COL32(0, 0, 0, 255), text.c_str());
        drawList->AddText(ImVec2(pos.x, pos.y + 1), IM_COL32(0, 0, 0, 255), text.c_str());
        drawList->AddText(pos, textColor, text.c_str());
    }

    inline void DrawLine(ImDrawList* drawList, const ImVec2& start, const ImVec2& end, ImU32 color, float thickness, bool outline) {
        if (outline) {
            drawList->AddLine(start, end, IM_COL32(0, 0, 0, 255), thickness + 2.0f);
        }
        drawList->AddLine(start, end, color, thickness);
    }

    inline void DrawSkeletonBone(ImDrawList* drawList, const RBX::Vec3& pos1, const RBX::Vec3& pos2,
        const RBX::Mat4& viewMatrix, const RBX::Vec2& dimensions,
        ImU32 color, float thickness, bool outline) {
        RBX::Vec2 screenPos1{}, screenPos2{};
        bool visible1 = W2S::WorldToScreen(pos1, viewMatrix, dimensions, screenPos1);
        bool visible2 = W2S::WorldToScreen(pos2, viewMatrix, dimensions, screenPos2);

        if (visible1 && visible2) {
            ImVec2 start(screenPos1.X, screenPos1.Y);
            ImVec2 end(screenPos2.X, screenPos2.Y);
            DrawLine(drawList, start, end, color, thickness, outline);
        }
    }

    inline void DrawSkeletonConnection(ImDrawList* drawList, RBX::RbxInstance character,
        std::string_view fromName, std::string_view toName, bool isR6,
        const RBX::Mat4& viewMatrix, const RBX::Vec2& dimensions,
        ImU32 color, float thickness, bool outline) {
        const auto isBodyPart = [isR6](std::string_view name) {
            return isR6 ? is_r6_body(name) : is_r15_body(name);
        };

        if (!isBodyPart(fromName) || !isBodyPart(toName)) return;

        auto from = character.FindChild(std::string(fromName));
        auto to = character.FindChild(std::string(toName));
        if (from.Addr == 0 || to.Addr == 0) return;

        const auto fromPos = from.GetPos();
        const auto toPos = to.GetPos();
        if (!std::isfinite(fromPos.X) || !std::isfinite(fromPos.Y) || !std::isfinite(fromPos.Z)
            || !std::isfinite(toPos.X) || !std::isfinite(toPos.Y) || !std::isfinite(toPos.Z)) return;

        DrawSkeletonBone(drawList, fromPos, toPos, viewMatrix, dimensions, color, thickness, outline);
    }

    inline void RenderSkeleton(ImDrawList* drawList, RBX::RbxInstance character,
        const RBX::Mat4& viewMatrix, const RBX::Vec2& dimensions, bool isR6) {
        const ImU32 boneColor = IM_COL32(255, 255, 255, 255);
        const float thickness = variables::ESP::skeletonThickness;
        const bool outline = variables::ESP::skeletonOutline;

        if (isR6) {
            DrawSkeletonConnection(drawList, character, "Head", "Torso", true, viewMatrix, dimensions, boneColor, thickness, outline);
            DrawSkeletonConnection(drawList, character, "Torso", "Left Arm", true, viewMatrix, dimensions, boneColor, thickness, outline);
            DrawSkeletonConnection(drawList, character, "Torso", "Right Arm", true, viewMatrix, dimensions, boneColor, thickness, outline);
            DrawSkeletonConnection(drawList, character, "Torso", "Left Leg", true, viewMatrix, dimensions, boneColor, thickness, outline);
            DrawSkeletonConnection(drawList, character, "Torso", "Right Leg", true, viewMatrix, dimensions, boneColor, thickness, outline);
            return;
        }

        DrawSkeletonConnection(drawList, character, "Head", "UpperTorso", false, viewMatrix, dimensions, boneColor, thickness, outline);
        DrawSkeletonConnection(drawList, character, "UpperTorso", "LowerTorso", false, viewMatrix, dimensions, boneColor, thickness, outline);

        DrawSkeletonConnection(drawList, character, "UpperTorso", "LeftUpperArm", false, viewMatrix, dimensions, boneColor, thickness, outline);
        DrawSkeletonConnection(drawList, character, "LeftUpperArm", "LeftLowerArm", false, viewMatrix, dimensions, boneColor, thickness, outline);
        DrawSkeletonConnection(drawList, character, "LeftLowerArm", "LeftHand", false, viewMatrix, dimensions, boneColor, thickness, outline);

        DrawSkeletonConnection(drawList, character, "UpperTorso", "RightUpperArm", false, viewMatrix, dimensions, boneColor, thickness, outline);
        DrawSkeletonConnection(drawList, character, "RightUpperArm", "RightLowerArm", false, viewMatrix, dimensions, boneColor, thickness, outline);
        DrawSkeletonConnection(drawList, character, "RightLowerArm", "RightHand", false, viewMatrix, dimensions, boneColor, thickness, outline);

        const auto legAnchor = character.FindChild("LowerTorso").Addr != 0 ? "LowerTorso" : "UpperTorso";
        DrawSkeletonConnection(drawList, character, legAnchor, "LeftUpperLeg", false, viewMatrix, dimensions, boneColor, thickness, outline);
        DrawSkeletonConnection(drawList, character, "LeftUpperLeg", "LeftLowerLeg", false, viewMatrix, dimensions, boneColor, thickness, outline);
        DrawSkeletonConnection(drawList, character, "LeftLowerLeg", "LeftFoot", false, viewMatrix, dimensions, boneColor, thickness, outline);

        DrawSkeletonConnection(drawList, character, legAnchor, "RightUpperLeg", false, viewMatrix, dimensions, boneColor, thickness, outline);
        DrawSkeletonConnection(drawList, character, "RightUpperLeg", "RightLowerLeg", false, viewMatrix, dimensions, boneColor, thickness, outline);
        DrawSkeletonConnection(drawList, character, "RightLowerLeg", "RightFoot", false, viewMatrix, dimensions, boneColor, thickness, outline);
    }

    inline void RenderESP(ImDrawList* drawList, const RBX::Mat4& viewMatrix)
    {
        if (!variables::ESP::enabled) return;

        RBX::Vec2 dimensions = Globals::renderEngine.GetDimensions();
        if (dimensions.X < 2.0f || dimensions.Y < 2.0f) {
            const auto display = ImGui::GetIO().DisplaySize;
            dimensions = { display.x, display.y };
        }
        if (dimensions.X < 2.0f || dimensions.Y < 2.0f) return;

        for (auto& plr : PlayerCache::players) {
            if (!plr.isValid) continue;

            auto character = RBX::RbxInstance(plr.characterAddr);
            auto head = character.FindChild("Head");
            auto torso = character.FindChild("Torso");
            bool isR6 = (torso.Addr != 0);

            RBX::RbxInstance hrp = character.FindChild("HumanoidRootPart");

            RBX::Vec3 hrpPos = plr.position;
            if (hrp.Addr != 0) {
                const auto liveRootPos = hrp.GetPos();
                if (std::isfinite(liveRootPos.X) && std::isfinite(liveRootPos.Y) && std::isfinite(liveRootPos.Z))
                    hrpPos = liveRootPos;
            }

            RBX::Vec3 headPos = { hrpPos.X, hrpPos.Y + 2.5f, hrpPos.Z };
            if (head.Addr != 0) {
                const auto liveHeadPos = head.GetPos();
                if (std::isfinite(liveHeadPos.X) && std::isfinite(liveHeadPos.Y) && std::isfinite(liveHeadPos.Z))
                    headPos = liveHeadPos;
            }

            RBX::Vec2 headScreen{}, hrpScreen{};
            bool headVisible = W2S::WorldToScreen(headPos, viewMatrix, dimensions, headScreen);
            bool hrpVisible = W2S::WorldToScreen(hrpPos, viewMatrix, dimensions, hrpScreen);

            if (!headVisible || !hrpVisible) continue;

            if (variables::ESP::skeleton) {
                RenderSkeleton(drawList, character, viewMatrix, dimensions, isR6);
            }

            float headYOffset = isR6 ? 0.5f : 0.5f;
            float feetYOffset = isR6 ? 3.0f : 2.5f;

            RBX::Vec3 topPos = { headPos.X, headPos.Y + headYOffset, headPos.Z };
            RBX::Vec3 bottomPos = { hrpPos.X, hrpPos.Y - feetYOffset, hrpPos.Z };

            RBX::Vec2 topScreen{}, bottomScreen{};
            bool topVisible = W2S::WorldToScreen(topPos, viewMatrix, dimensions, topScreen);
            bool bottomVisible = W2S::WorldToScreen(bottomPos, viewMatrix, dimensions, bottomScreen);

            if (!topVisible || !bottomVisible) continue;

            float height = bottomScreen.Y - topScreen.Y;
            if (!std::isfinite(height) || height <= 0.0f) continue;
            float width = height * 0.4f;

            float minX = topScreen.X - width / 2.0f;
            float minY = topScreen.Y;
            float maxX = topScreen.X + width / 2.0f;
            float maxY = bottomScreen.Y;

            ImVec2 screenSize = ImGui::GetIO().DisplaySize;
            if (minX < -500 || minY < -500 || maxX > screenSize.x + 500 || maxY > screenSize.y + 500) continue;

            if (variables::ESP::boxes) {
                drawList->AddRect(
                    ImVec2(minX, minY),
                    ImVec2(maxX, maxY),
                    IM_COL32(0, 0, 0, 255),
                    0.0f, 0, 3.0f
                );

                drawList->AddRect(
                    ImVec2(minX, minY),
                    ImVec2(maxX, maxY),
                    IM_COL32(255, 255, 255, 255),
                    0.0f, 0, 1.0f
                );

                drawList->AddRect(
                    ImVec2(minX + 1, minY + 1),
                    ImVec2(maxX - 1, maxY - 1),
                    IM_COL32(0, 0, 0, 255),
                    0.0f, 0, 1.0f
                );
            }

            if (variables::ESP::healthBar && plr.maxHealth > 0) {
                float healthPercent = static_cast<float>(plr.health) / static_cast<float>(plr.maxHealth);
                ImU32 healthColor = IM_COL32(255 * (1 - healthPercent), 255 * healthPercent, 0, 255);

                float barHeight = (maxY - minY) * healthPercent;

                drawList->AddRectFilled(
                    ImVec2(minX - 6, minY),
                    ImVec2(minX - 2, maxY),
                    IM_COL32(0, 0, 0, 200)
                );

                drawList->AddRectFilled(
                    ImVec2(minX - 5, maxY - barHeight),
                    ImVec2(minX - 3, maxY),
                    healthColor
                );
            }

            if (variables::ESP::names) {
                ImVec2 textSize = ImGui::CalcTextSize(plr.name.c_str());
                float textX = (minX + maxX) / 2.0f - textSize.x / 2.0f;
                float textY = minY - textSize.y - 2;
                DrawOutlinedText(drawList, ImVec2(textX, textY), plr.name, IM_COL32(255, 255, 255, 255));
            }

            if (variables::ESP::distance) {
                std::string distText = std::to_string(static_cast<int>(plr.distance)) + "m";
                ImVec2 textSize = ImGui::CalcTextSize(distText.c_str());
                float textX = (minX + maxX) / 2.0f - textSize.x / 2.0f;
                float textY = maxY + 2;
                DrawOutlinedText(drawList, ImVec2(textX, textY), distText, IM_COL32(255, 255, 255, 255));
            }

            if (variables::ESP::snaplines) {
                ImU32 color = IM_COL32(255, 255, 255, 255);
                ImVec2 screenSize = ImGui::GetIO().DisplaySize;
                ImVec2 origin_pos, destination_pos;

                switch (variables::ESP::snaplinesOrigin) {
                case 0: {
                    POINT cursorPos;
                    if (GetCursorPos(&cursorPos)) {
                        HWND hwnd = GetForegroundWindow();
                        if (ScreenToClient(hwnd, &cursorPos)) {
                            origin_pos = ImVec2((float)cursorPos.x, (float)cursorPos.y);
                        }
                        else {
                            origin_pos = ImVec2(screenSize.x * 0.5f, screenSize.y);
                        }
                    }
                    else {
                        origin_pos = ImVec2(screenSize.x * 0.5f, screenSize.y);
                    }
                    break;
                }
                case 1: origin_pos = ImVec2(screenSize.x * 0.5f, screenSize.y * 0.5f); break;
                case 2: origin_pos = ImVec2(screenSize.x * 0.5f, 0.f); break;
                case 3: origin_pos = ImVec2(screenSize.x * 0.5f, screenSize.y); break;
                case 4: {
                    auto localChar = RBX::RbxInstance(Globals::localPlayer.GetModelRef());
                    auto localHead = localChar.FindChild("Head");
                    if (localHead.Addr != 0) {
                        auto headPos = W2S::WorldToScreen(localHead.GetPos(), viewMatrix);
                        origin_pos = ImVec2(headPos.X, headPos.Y);
                    }
                    else {
                        origin_pos = ImVec2(screenSize.x * 0.5f, screenSize.y);
                    }
                    break;
                }
                case 5: {
                    auto localChar = RBX::RbxInstance(Globals::localPlayer.GetModelRef());
                    auto localHRP = localChar.FindChild("HumanoidRootPart");
                    if (localHRP.Addr != 0) {
                        auto hrpPos = W2S::WorldToScreen(localHRP.GetPos(), viewMatrix);
                        origin_pos = ImVec2(hrpPos.X, hrpPos.Y);
                    }
                    else {
                        origin_pos = ImVec2(screenSize.x * 0.5f, screenSize.y);
                    }
                    break;
                }
                default: origin_pos = ImVec2(screenSize.x * 0.5f, screenSize.y); break;
                }

                switch (variables::ESP::snaplinesDestination) {
                case 0: {
                    RBX::Vec2 headScreenPos = W2S::WorldToScreen(headPos, viewMatrix);
                    destination_pos = ImVec2(headScreenPos.X, headScreenPos.Y);
                    break;
                }
                case 1: {
                    RBX::Vec2 hrpScreenPos = W2S::WorldToScreen(hrpPos, viewMatrix);
                    destination_pos = ImVec2(hrpScreenPos.X, hrpScreenPos.Y);
                    break;
                }
                case 2: {
                    ImVec2 best = ImVec2((minX + maxX) * 0.5f, (minY + maxY) * 0.5f);
                    float closest = FLT_MAX;

                    RBX::Vec2 headScreenPos = W2S::WorldToScreen(headPos, viewMatrix);
                    ImVec2 headScreen = ImVec2(headScreenPos.X, headScreenPos.Y);
                    auto delta = ImVec2(origin_pos.x - headScreen.x, origin_pos.y - headScreen.y);
                    float distance = delta.x * delta.x + delta.y * delta.y;
                    if (distance < closest) {
                        closest = distance;
                        best = headScreen;
                    }

                    RBX::Vec2 hrpScreenPos2 = W2S::WorldToScreen(hrpPos, viewMatrix);
                    ImVec2 hrpScreen = ImVec2(hrpScreenPos2.X, hrpScreenPos2.Y);
                    delta = ImVec2(origin_pos.x - hrpScreen.x, origin_pos.y - hrpScreen.y);
                    distance = delta.x * delta.x + delta.y * delta.y;
                    if (distance < closest) {
                        closest = distance;
                        best = hrpScreen;
                    }

                    destination_pos = best;
                    break;
                }
                default: {
                    RBX::Vec2 headScreenPos = W2S::WorldToScreen(headPos, viewMatrix);
                    destination_pos = ImVec2(headScreenPos.X, headScreenPos.Y);
                    break;
                }
                }

                switch (variables::ESP::snaplinesStyle) {
                case 0: {
                    DrawLine(drawList, origin_pos, destination_pos, color, variables::ESP::snaplinesThickness, variables::ESP::snaplinesOutline);
                    break;
                }
                case 1: {
                    auto drop = 180.f;
                    auto segments = 24;
                    ImVec2 prev = origin_pos;
                    auto control = ImVec2(
                        (origin_pos.x + destination_pos.x) * 0.5f,
                        (origin_pos.y + destination_pos.y) * 0.5f + drop
                    );
                    for (auto i = 1; i <= segments; ++i) {
                        auto t = (float)(i) / segments;
                        auto it = 1.0f - t;
                        ImVec2 point;
                        point.x = it * it * origin_pos.x + 2 * it * t * control.x + t * t * destination_pos.x;
                        point.y = it * it * origin_pos.y + 2 * it * t * control.y + t * t * destination_pos.y;
                        DrawLine(drawList, prev, point, color, variables::ESP::snaplinesThickness, variables::ESP::snaplinesOutline);
                        prev = point;
                    }
                    break;
                }
                case 2: {
                    auto delta = ImVec2(destination_pos.x - origin_pos.x, destination_pos.y - origin_pos.y);
                    auto length = std::sqrt(delta.x * delta.x + delta.y * delta.y);
                    auto dir = ImVec2(delta.x / length, delta.y / length);
                    auto dash_len = length / (10 * 2.f);
                    for (auto i = 0; i < 10; ++i) {
                        auto start = i * 2.f * dash_len;
                        auto end = start + dash_len;
                        auto s = ImVec2(origin_pos.x + dir.x * start, origin_pos.y + dir.y * start);
                        auto e = ImVec2(origin_pos.x + dir.x * end, origin_pos.y + dir.y * end);
                        DrawLine(drawList, s, e, color, variables::ESP::snaplinesThickness, variables::ESP::snaplinesOutline);
                    }
                    break;
                }
                }
            }
        }
    }
}
