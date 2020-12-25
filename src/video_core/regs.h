// Copyright 2017 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include <array>
#include <cstddef>
#include <string>

#include "common/common_funcs.h"
#include "common/common_types.h"
#include "video_core/regs_framebuffer.h"
#include "video_core/regs_lighting.h"
#include "video_core/regs_pipeline.h"
#include "video_core/regs_rasterizer.h"
#include "video_core/regs_shader.h"
#include "video_core/regs_texturing.h"

namespace Pica {

#define PICA_REG_INDEX(field_name) (offsetof(Pica::Regs, field_name) / sizeof(u32))

struct Regs {
    static constexpr std::size_t NUM_REGS = 0x300;

    union {
        struct {
            INSERT_PADDING_WORDS(0x10);
            u32 trigger_irq;
            INSERT_PADDING_WORDS(0x2f);
            RasterizerRegs rasterizer;
            TexturingRegs texturing;
            FramebufferRegs framebuffer;
            LightingRegs lighting;
            PipelineRegs pipeline;
            ShaderRegs gs;
            ShaderRegs vs;
            INSERT_PADDING_WORDS(0x20);
        };
        std::array<u32, NUM_REGS> reg_array;
    };

    /// Map register indices to names readable by humans
    static const char* GetRegisterName(u16 index);

    void Write(u32 id, u32 value, u32 mask) {
        // Expand a 4-bit mask to 4-byte mask, e.g. 0b0101 -> 0x00FF00FF
        static const u32 expand_bits_to_bytes[] = {
                0x00000000, 0x000000ff, 0x0000ff00, 0x0000ffff, 0x00ff0000, 0x00ff00ff, 0x00ffff00, 0x00ffffff,
                0xff000000, 0xff0000ff, 0xff00ff00, 0xff00ffff, 0xffff0000, 0xffff00ff, 0xffffff00, 0xffffffff,
        };
        // TODO: Figure out how register masking acts on e.g. vs.uniform_setup.set_value
        u32 old_value = reg_array[id];
        u32 write_mask = expand_bits_to_bytes[mask];
        reg_array[id] = (old_value & ~write_mask) | (value & write_mask);
    }
};

static_assert(sizeof(Regs) == Regs::NUM_REGS * sizeof(u32), "Regs struct has wrong size");

#define ASSERT_REG_POSITION(field_name, position)                                                  \
    static_assert(offsetof(Regs, field_name) == position * 4,                                      \
                  "Field " #field_name " has invalid position")

ASSERT_REG_POSITION(trigger_irq, 0x10);

ASSERT_REG_POSITION(rasterizer, 0x40);
ASSERT_REG_POSITION(rasterizer.cull_mode, 0x40);
ASSERT_REG_POSITION(rasterizer.viewport_size_x, 0x41);
ASSERT_REG_POSITION(rasterizer.viewport_size_y, 0x43);
ASSERT_REG_POSITION(rasterizer.viewport_depth_range, 0x4d);
ASSERT_REG_POSITION(rasterizer.viewport_depth_near_plane, 0x4e);
ASSERT_REG_POSITION(rasterizer.vs_output_attributes[0], 0x50);
ASSERT_REG_POSITION(rasterizer.vs_output_attributes[1], 0x51);
ASSERT_REG_POSITION(rasterizer.scissor_test, 0x65);
ASSERT_REG_POSITION(rasterizer.viewport_corner, 0x68);
ASSERT_REG_POSITION(rasterizer.depthmap_enable, 0x6D);

ASSERT_REG_POSITION(texturing, 0x80);
ASSERT_REG_POSITION(texturing.main_config, 0x80);
ASSERT_REG_POSITION(texturing.texture0, 0x81);
ASSERT_REG_POSITION(texturing.texture0_format, 0x8e);
ASSERT_REG_POSITION(texturing.fragment_lighting_enable, 0x8f);
ASSERT_REG_POSITION(texturing.texture1, 0x91);
ASSERT_REG_POSITION(texturing.texture1_format, 0x96);
ASSERT_REG_POSITION(texturing.texture2, 0x99);
ASSERT_REG_POSITION(texturing.texture2_format, 0x9e);
ASSERT_REG_POSITION(texturing.proctex, 0xa8);
ASSERT_REG_POSITION(texturing.proctex_noise_u, 0xa9);
ASSERT_REG_POSITION(texturing.proctex_noise_v, 0xaa);
ASSERT_REG_POSITION(texturing.proctex_noise_frequency, 0xab);
ASSERT_REG_POSITION(texturing.proctex_lut, 0xac);
ASSERT_REG_POSITION(texturing.proctex_lut_offset, 0xad);
ASSERT_REG_POSITION(texturing.proctex_lut_config, 0xaf);
ASSERT_REG_POSITION(texturing.tev_stage0, 0xc0);
ASSERT_REG_POSITION(texturing.tev_stage1, 0xc8);
ASSERT_REG_POSITION(texturing.tev_stage2, 0xd0);
ASSERT_REG_POSITION(texturing.tev_stage3, 0xd8);
ASSERT_REG_POSITION(texturing.tev_combiner_buffer_input, 0xe0);
ASSERT_REG_POSITION(texturing.fog_mode, 0xe0);
ASSERT_REG_POSITION(texturing.fog_color, 0xe1);
ASSERT_REG_POSITION(texturing.fog_lut_offset, 0xe6);
ASSERT_REG_POSITION(texturing.fog_lut_data, 0xe8);
ASSERT_REG_POSITION(texturing.tev_stage4, 0xf0);
ASSERT_REG_POSITION(texturing.tev_stage5, 0xf8);
ASSERT_REG_POSITION(texturing.tev_combiner_buffer_color, 0xfd);

ASSERT_REG_POSITION(framebuffer, 0x100);
ASSERT_REG_POSITION(framebuffer.output_merger, 0x100);
ASSERT_REG_POSITION(framebuffer.framebuffer, 0x110);

ASSERT_REG_POSITION(lighting, 0x140);

ASSERT_REG_POSITION(pipeline, 0x200);
ASSERT_REG_POSITION(pipeline.vertex_attributes, 0x200);
ASSERT_REG_POSITION(pipeline.index_array, 0x227);
ASSERT_REG_POSITION(pipeline.num_vertices, 0x228);
ASSERT_REG_POSITION(pipeline.vertex_offset, 0x22a);
ASSERT_REG_POSITION(pipeline.trigger_draw, 0x22e);
ASSERT_REG_POSITION(pipeline.trigger_draw_indexed, 0x22f);
ASSERT_REG_POSITION(pipeline.vs_default_attributes_setup, 0x232);
ASSERT_REG_POSITION(pipeline.command_buffer, 0x238);
ASSERT_REG_POSITION(pipeline.gpu_mode, 0x245);
ASSERT_REG_POSITION(pipeline.triangle_topology, 0x25e);
ASSERT_REG_POSITION(pipeline.restart_primitive, 0x25f);

ASSERT_REG_POSITION(gs, 0x280);
ASSERT_REG_POSITION(vs, 0x2b0);

#undef ASSERT_REG_POSITION
} // namespace Pica
