// Copyright 2018 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#include <algorithm>
#include <cstring>
#include <android/log.h>
#include "citra_qt/camera/camera_util.h"
#include "core/frontend/camera/factory.h"
#include "core/frontend/camera/interface.h"

namespace CameraUtil {

// The following are data tables for RGB -> YUV conversions.
namespace YuvTable {

constexpr std::array<int, 256> Y_R = {
    53,  53,  53,  54,  54,  54,  55,  55,  55,  56,  56,  56,  56,  57,  57,  57,  58,  58,  58,
    59,  59,  59,  59,  60,  60,  60,  61,  61,  61,  62,  62,  62,  62,  63,  63,  63,  64,  64,
    64,  65,  65,  65,  65,  66,  66,  66,  67,  67,  67,  67,  68,  68,  68,  69,  69,  69,  70,
    70,  70,  70,  71,  71,  71,  72,  72,  72,  73,  73,  73,  73,  74,  74,  74,  75,  75,  75,
    76,  76,  76,  76,  77,  77,  77,  78,  78,  78,  79,  79,  79,  79,  80,  80,  80,  81,  81,
    81,  82,  82,  82,  82,  83,  83,  83,  84,  84,  84,  85,  85,  85,  85,  86,  86,  86,  87,
    87,  87,  87,  88,  88,  88,  89,  89,  89,  90,  90,  90,  90,  91,  91,  91,  92,  92,  92,
    93,  93,  93,  93,  94,  94,  94,  95,  95,  95,  96,  96,  96,  96,  97,  97,  97,  98,  98,
    98,  99,  99,  99,  99,  100, 100, 100, 101, 101, 101, 102, 102, 102, 102, 103, 103, 103, 104,
    104, 104, 105, 105, 105, 105, 106, 106, 106, 107, 107, 107, 108, 108, 108, 108, 109, 109, 109,
    110, 110, 110, 110, 111, 111, 111, 112, 112, 112, 113, 113, 113, 113, 114, 114, 114, 115, 115,
    115, 116, 116, 116, 116, 117, 117, 117, 118, 118, 118, 119, 119, 119, 119, 120, 120, 120, 121,
    121, 121, 122, 122, 122, 122, 123, 123, 123, 124, 124, 124, 125, 125, 125, 125, 126, 126, 126,
    127, 127, 127, 128, 128, 128, 128, 129, 129,
};

constexpr std::array<int, 256> Y_G = {
    -79, -79, -78, -78, -77, -77, -76, -75, -75, -74, -74, -73, -72, -72, -71, -71, -70, -70, -69,
    -68, -68, -67, -67, -66, -65, -65, -64, -64, -63, -62, -62, -61, -61, -60, -60, -59, -58, -58,
    -57, -57, -56, -55, -55, -54, -54, -53, -52, -52, -51, -51, -50, -50, -49, -48, -48, -47, -47,
    -46, -45, -45, -44, -44, -43, -42, -42, -41, -41, -40, -40, -39, -38, -38, -37, -37, -36, -35,
    -35, -34, -34, -33, -33, -32, -31, -31, -30, -30, -29, -28, -28, -27, -27, -26, -25, -25, -24,
    -24, -23, -23, -22, -21, -21, -20, -20, -19, -18, -18, -17, -17, -16, -15, -15, -14, -14, -13,
    -13, -12, -11, -11, -10, -10, -9,  -8,  -8,  -7,  -7,  -6,  -5,  -5,  -4,  -4,  -3,  -3,  -2,
    -1,  -1,  0,   0,   0,   1,   1,   2,   2,   3,   4,   4,   5,   5,   6,   6,   7,   8,   8,
    9,   9,   10,  11,  11,  12,  12,  13,  13,  14,  15,  15,  16,  16,  17,  18,  18,  19,  19,
    20,  21,  21,  22,  22,  23,  23,  24,  25,  25,  26,  26,  27,  28,  28,  29,  29,  30,  31,
    31,  32,  32,  33,  33,  34,  35,  35,  36,  36,  37,  38,  38,  39,  39,  40,  41,  41,  42,
    42,  43,  43,  44,  45,  45,  46,  46,  47,  48,  48,  49,  49,  50,  50,  51,  52,  52,  53,
    53,  54,  55,  55,  56,  56,  57,  58,  58,  59,  59,  60,  60,  61,  62,  62,  63,  63,  64,
    65,  65,  66,  66,  67,  68,  68,  69,  69,
};

constexpr std::array<int, 256> Y_B = {
    25, 25, 26, 26, 26, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28,
    28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 29, 29, 30, 30, 30, 30, 30, 30, 30, 30, 30, 31, 31,
    31, 31, 31, 31, 31, 31, 31, 32, 32, 32, 32, 32, 32, 32, 32, 32, 33, 33, 33, 33, 33, 33, 33, 33,
    34, 34, 34, 34, 34, 34, 34, 34, 34, 35, 35, 35, 35, 35, 35, 35, 35, 35, 36, 36, 36, 36, 36, 36,
    36, 36, 36, 37, 37, 37, 37, 37, 37, 37, 37, 38, 38, 38, 38, 38, 38, 38, 38, 38, 39, 39, 39, 39,
    39, 39, 39, 39, 39, 40, 40, 40, 40, 40, 40, 40, 40, 40, 41, 41, 41, 41, 41, 41, 41, 41, 41, 42,
    42, 42, 42, 42, 42, 42, 42, 43, 43, 43, 43, 43, 43, 43, 43, 43, 44, 44, 44, 44, 44, 44, 44, 44,
    44, 45, 45, 45, 45, 45, 45, 45, 45, 45, 46, 46, 46, 46, 46, 46, 46, 46, 47, 47, 47, 47, 47, 47,
    47, 47, 47, 48, 48, 48, 48, 48, 48, 48, 48, 48, 49, 49, 49, 49, 49, 49, 49, 49, 49, 50, 50, 50,
    50, 50, 50, 50, 50, 51, 51, 51, 51, 51, 51, 51, 51, 51, 52, 52, 52, 52, 52, 52, 52, 52, 52, 53,
    53, 53, 53, 53, 53, 53, 53, 53, 54, 54, 54, 54, 54, 54, 54, 54,
};

static constexpr int Y(int r, int g, int b) {
    return Y_R[r] + Y_G[g] + Y_B[b];
}

constexpr std::array<int, 256> U_R = {
    30, 30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 32, 32, 32, 32, 32, 32, 33, 33, 33, 33, 33, 33, 34,
    34, 34, 34, 34, 34, 35, 35, 35, 35, 35, 35, 36, 36, 36, 36, 36, 36, 37, 37, 37, 37, 37, 37, 38,
    38, 38, 38, 38, 38, 39, 39, 39, 39, 39, 39, 40, 40, 40, 40, 40, 40, 41, 41, 41, 41, 41, 41, 42,
    42, 42, 42, 42, 42, 43, 43, 43, 43, 43, 43, 44, 44, 44, 44, 44, 45, 45, 45, 45, 45, 45, 46, 46,
    46, 46, 46, 46, 47, 47, 47, 47, 47, 47, 48, 48, 48, 48, 48, 48, 49, 49, 49, 49, 49, 49, 50, 50,
    50, 50, 50, 50, 51, 51, 51, 51, 51, 51, 52, 52, 52, 52, 52, 52, 53, 53, 53, 53, 53, 53, 54, 54,
    54, 54, 54, 54, 55, 55, 55, 55, 55, 55, 56, 56, 56, 56, 56, 56, 57, 57, 57, 57, 57, 57, 58, 58,
    58, 58, 58, 59, 59, 59, 59, 59, 59, 60, 60, 60, 60, 60, 60, 61, 61, 61, 61, 61, 61, 62, 62, 62,
    62, 62, 62, 63, 63, 63, 63, 63, 63, 64, 64, 64, 64, 64, 64, 65, 65, 65, 65, 65, 65, 66, 66, 66,
    66, 66, 66, 67, 67, 67, 67, 67, 67, 68, 68, 68, 68, 68, 68, 69, 69, 69, 69, 69, 69, 70, 70, 70,
    70, 70, 70, 71, 71, 71, 71, 71, 72, 72, 72, 72, 72, 72, 73, 73,
};

constexpr std::array<int, 256> U_G = {
    -45, -44, -44, -44, -43, -43, -43, -42, -42, -42, -41, -41, -41, -40, -40, -40, -39, -39, -39,
    -38, -38, -38, -37, -37, -37, -36, -36, -36, -35, -35, -35, -34, -34, -34, -33, -33, -33, -32,
    -32, -32, -31, -31, -31, -30, -30, -30, -29, -29, -29, -28, -28, -28, -27, -27, -27, -26, -26,
    -26, -25, -25, -25, -24, -24, -24, -23, -23, -23, -22, -22, -22, -21, -21, -21, -20, -20, -20,
    -19, -19, -19, -18, -18, -18, -17, -17, -17, -16, -16, -16, -15, -15, -15, -14, -14, -14, -14,
    -13, -13, -13, -12, -12, -12, -11, -11, -11, -10, -10, -10, -9,  -9,  -9,  -8,  -8,  -8,  -7,
    -7,  -7,  -6,  -6,  -6,  -5,  -5,  -5,  -4,  -4,  -4,  -3,  -3,  -3,  -2,  -2,  -2,  -1,  -1,
    -1,  0,   0,   0,   0,   0,   0,   1,   1,   1,   2,   2,   2,   3,   3,   3,   4,   4,   4,
    5,   5,   5,   6,   6,   6,   7,   7,   7,   8,   8,   8,   9,   9,   9,   10,  10,  10,  11,
    11,  11,  12,  12,  12,  13,  13,  13,  14,  14,  14,  15,  15,  15,  16,  16,  16,  17,  17,
    17,  18,  18,  18,  19,  19,  19,  20,  20,  20,  21,  21,  21,  22,  22,  22,  23,  23,  23,
    24,  24,  24,  25,  25,  25,  26,  26,  26,  27,  27,  27,  28,  28,  28,  29,  29,  29,  30,
    30,  30,  31,  31,  31,  32,  32,  32,  33,  33,  33,  34,  34,  34,  35,  35,  35,  36,  36,
    36,  37,  37,  37,  38,  38,  38,  39,  39,
};

constexpr std::array<int, 256> U_B = {
    113, 113, 114, 114, 115, 115, 116, 116, 117, 117, 118, 118, 119, 119, 120, 120, 121, 121, 122,
    122, 123, 123, 124, 124, 125, 125, 126, 126, 127, 127, 128, 128, 129, 129, 130, 130, 131, 131,
    132, 132, 133, 133, 134, 134, 135, 135, 136, 136, 137, 137, 138, 138, 139, 139, 140, 140, 141,
    141, 142, 142, 143, 143, 144, 144, 145, 145, 146, 146, 147, 147, 148, 148, 149, 149, 150, 150,
    151, 151, 152, 152, 153, 153, 154, 154, 155, 155, 156, 156, 157, 157, 158, 158, 159, 159, 160,
    160, 161, 161, 162, 162, 163, 163, 164, 164, 165, 165, 166, 166, 167, 167, 168, 168, 169, 169,
    170, 170, 171, 171, 172, 172, 173, 173, 174, 174, 175, 175, 176, 176, 177, 177, 178, 178, 179,
    179, 180, 180, 181, 181, 182, 182, 183, 183, 184, 184, 185, 185, 186, 186, 187, 187, 188, 188,
    189, 189, 190, 190, 191, 191, 192, 192, 193, 193, 194, 194, 195, 195, 196, 196, 197, 197, 198,
    198, 199, 199, 200, 200, 201, 201, 202, 202, 203, 203, 204, 204, 205, 205, 206, 206, 207, 207,
    208, 208, 209, 209, 210, 210, 211, 211, 212, 212, 213, 213, 214, 214, 215, 215, 216, 216, 217,
    217, 218, 218, 219, 219, 220, 220, 221, 221, 222, 222, 223, 223, 224, 224, 225, 225, 226, 226,
    227, 227, 228, 228, 229, 229, 230, 230, 231, 231, 232, 232, 233, 233, 234, 234, 235, 235, 236,
    236, 237, 237, 238, 238, 239, 239, 240, 240,
};

static constexpr int U(int r, int g, int b) {
    return -U_R[r] - U_G[g] + U_B[b];
}

constexpr std::array<int, 256> V_R = {
    89,  90,  90,  91,  91,  92,  92,  93,  93,  94,  94,  95,  95,  96,  96,  97,  97,  98,  98,
    99,  99,  100, 100, 101, 101, 102, 102, 103, 103, 104, 104, 105, 105, 106, 106, 107, 107, 108,
    108, 109, 109, 110, 110, 111, 111, 112, 112, 113, 113, 114, 114, 115, 115, 116, 116, 117, 117,
    118, 118, 119, 119, 120, 120, 121, 121, 122, 122, 123, 123, 124, 124, 125, 125, 126, 126, 127,
    127, 128, 128, 129, 129, 130, 130, 131, 131, 132, 132, 133, 133, 134, 134, 135, 135, 136, 136,
    137, 137, 138, 138, 139, 139, 140, 140, 141, 141, 142, 142, 143, 143, 144, 144, 145, 145, 146,
    146, 147, 147, 148, 148, 149, 149, 150, 150, 151, 151, 152, 152, 153, 153, 154, 154, 155, 155,
    156, 156, 157, 157, 158, 158, 159, 159, 160, 160, 161, 161, 162, 162, 163, 163, 164, 164, 165,
    165, 166, 166, 167, 167, 168, 168, 169, 169, 170, 170, 171, 171, 172, 172, 173, 173, 174, 174,
    175, 175, 176, 176, 177, 177, 178, 178, 179, 179, 180, 180, 181, 181, 182, 182, 183, 183, 184,
    184, 185, 185, 186, 186, 187, 187, 188, 188, 189, 189, 190, 190, 191, 191, 192, 192, 193, 193,
    194, 194, 195, 195, 196, 196, 197, 197, 198, 198, 199, 199, 200, 200, 201, 201, 202, 202, 203,
    203, 204, 205, 205, 206, 206, 207, 207, 208, 208, 209, 209, 210, 210, 211, 211, 212, 212, 213,
    213, 214, 214, 215, 215, 216, 216, 217, 217,
};

constexpr std::array<int, 256> V_G = {
    -57, -56, -56, -55, -55, -55, -54, -54, -53, -53, -52, -52, -52, -51, -51, -50, -50, -50, -49,
    -49, -48, -48, -47, -47, -47, -46, -46, -45, -45, -45, -44, -44, -43, -43, -42, -42, -42, -41,
    -41, -40, -40, -39, -39, -39, -38, -38, -37, -37, -37, -36, -36, -35, -35, -34, -34, -34, -33,
    -33, -32, -32, -31, -31, -31, -30, -30, -29, -29, -29, -28, -28, -27, -27, -26, -26, -26, -25,
    -25, -24, -24, -24, -23, -23, -22, -22, -21, -21, -21, -20, -20, -19, -19, -18, -18, -18, -17,
    -17, -16, -16, -16, -15, -15, -14, -14, -13, -13, -13, -12, -12, -11, -11, -10, -10, -10, -9,
    -9,  -8,  -8,  -8,  -7,  -7,  -6,  -6,  -5,  -5,  -5,  -4,  -4,  -3,  -3,  -3,  -2,  -2,  -1,
    -1,  0,   0,   0,   0,   0,   1,   1,   2,   2,   2,   3,   3,   4,   4,   4,   5,   5,   6,
    6,   7,   7,   7,   8,   8,   9,   9,   10,  10,  10,  11,  11,  12,  12,  12,  13,  13,  14,
    14,  15,  15,  15,  16,  16,  17,  17,  17,  18,  18,  19,  19,  20,  20,  20,  21,  21,  22,
    22,  23,  23,  23,  24,  24,  25,  25,  25,  26,  26,  27,  27,  28,  28,  28,  29,  29,  30,
    30,  31,  31,  31,  32,  32,  33,  33,  33,  34,  34,  35,  35,  36,  36,  36,  37,  37,  38,
    38,  38,  39,  39,  40,  40,  41,  41,  41,  42,  42,  43,  43,  44,  44,  44,  45,  45,  46,
    46,  46,  47,  47,  48,  48,  49,  49,  49,
};

constexpr std::array<int, 256> V_B = {
    18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20, 20,
    20, 20, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22,
    22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24,
    24, 24, 24, 24, 24, 24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26,
    26, 26, 26, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 28, 28,
    28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 30, 30,
    30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 32,
    32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 34,
    34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
    36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
    38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 39, 39, 39, 39,
};

static constexpr int V(int r, int g, int b) {
    return V_R[r] - V_G[g] - V_B[b];
}
} // namespace YuvTable

void Rgb2Yuv(std::vector<u16>& buffer, const std::vector<u32>& image) {
    auto dst = buffer.begin();
    bool write = false;
    int py, pu, pv;

    for (u32 i = 0; i < image.size(); ++i) {
        u32 rgba = image[i];
        int r = (rgba >> 16) & 0xFF;
        int g = (rgba >> 8) & 0xFF;
        int b = (rgba >> 0) & 0xFF;
        // The following transformation is a reverse of the one in Y2R using ITU_Rec601
        int y = YuvTable::Y(r, g, b);
        int u = YuvTable::U(r, g, b);
        int v = YuvTable::V(r, g, b);

        if (write) {
            pu = (pu + u) / 2;
            pv = (pv + v) / 2;
            *(dst++) =
                    static_cast<u16>(std::clamp(py, 0, 0xFF) | (std::clamp(pu, 0, 0xFF) << 8));
            *(dst++) =
                    static_cast<u16>(std::clamp(y, 0, 0xFF) | (std::clamp(pv, 0, 0xFF) << 8));
        } else {
            py = y;
            pu = u;
            pv = v;
        }
        write = !write;
    }
}

inline u16 RGBA8888toRGB565(u32 px) {
    return ((px >> 3) & 0x001F) | ((px >> 5) & 0x07E0) | ((px >> 8) & 0xF800);
}

void ConvertRGBA8888ToRGB565(std::vector<u16>& dst, const std::vector<u32>& image) {
    for (u32 i = 0; i < dst.size(); ++i) {
        dst[i] = RGBA8888toRGB565(image[i]);
    }
}

std::vector<u16> ProcessImage(const std::vector<u32>& image, int width, int height,
                              bool output_rgb = false, bool flip_horizontal = false,
                              bool flip_vertical = false) {
    std::vector<u16> buffer(width * height);
    if (image.empty()) {
        return buffer;
    }

    std::vector<u32> data(width * height);
    for (u32 y = 0; y < height; ++y) {
        for (u32 x = 0; x < width; ++x) {
            u32 idx = (flip_vertical ? (height - y) : y) * width + (flip_horizontal ? width - x : x);
            data[y * width + x] = image[idx];
        }
    }

    if (output_rgb) {
        ConvertRGBA8888ToRGB565(buffer, data);
    } else {
        Rgb2Yuv(buffer, data);
    }
    return buffer;
}

} // namespace CameraUtil