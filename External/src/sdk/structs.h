#include <cstdint>
#include <string>
#include "math.h"

using Vector2 = rbx::vector2_t;
using Vector3 = rbx::vector3_t;
using Matrix3x3 = rbx::matrix3_t;
using UDim2 = rbx::udim2_t;
using ViewMatrix_t = rbx::matrix4_t;

// trust ud!
namespace Structs {
    inline std::string ClientVersion = "version-d584fb6c717a43d9";

    struct Humanoid {
        char pad_0[0x20];
        int HumanoidStateID; // 0x20
        char pad_1[0x94];
        char pad_2[0xd];
        bool BreakJointsOnDeath; // 0xc5
        char pad_3[0x42];
        uintptr_t SeatPart; // 0x108
        char pad_4[0x8];
        uintptr_t MoveToPart; // 0x118
        char pad_5[0x8];
        Vector3 CameraOffset; // 0x128
        char pad_6[0xc];
        Vector3 MoveDirection; // 0x140
        Vector3 TargetPoint; // 0x14c
        char pad_7[0xc];
        Vector3 MoveToPoint; // 0x164
        char pad_8[0x10];
        int DisplayDistanceType; // 0x180
        int FloorMaterial; // 0x184
        float HealthDisplayDistance; // 0x188
        int HealthDisplayType; // 0x18c
        float Health; // 0x190
        float HipHeight; // 0x194
        char pad_9[0x8];
        float JumpHeight; // 0x1a0
        float JumpPower; // 0x1a4
        float MaxHealth; // 0x1a8
        float MaxSlopeAngle; // 0x1ac
        float NameDisplayDistance; // 0x1b0
        int NameOcclusion; // 0x1b4
        char pad_10[0x8];
        int RigType; // 0x1c0
        char pad_11[0xc];
        float Walkspeed; // 0x1d0
        bool AutoJumpEnabled; // 0x1d4
        bool AutoRotate; // 0x1d5
        bool AutomaticScalingEnabled; // 0x1d6
        char pad_12[0x1];
        bool EvaluateStateMachine; // 0x1d8
        char pad_13[0x1];
        bool Jump; // 0x1da
        char pad_14[0x1];
        bool PlatformStand; // 0x1dc
        bool Sit; // 0x1dd
        bool RequiresNeck; // 0x1dd
        char pad_15[0x1];
        bool UseJumpPower; // 0x1e0
        char pad_16[0x1d3];
        float WalkspeedCheck; // 0x3b4
        char pad_17[0x48];
        double WalkTimer; // 0x400
        char pad_18[0x68];
        uintptr_t HumanoidRootPart; // 0x470
        char pad_19[0x440];
        int HumanoidState; // 0x8b8
        char pad_20[0xa3];
        bool IsWalking; // 0x95f
    }; // sizeof = 960

}
