/*
 * Copyright © 2026
 * SPDX-License-Identifier: MIT
 */

#include "nir_test.h"

class nir_clip_cull_distance_test : public nir_test {
public:
   nir_clip_cull_distance_test(const char *name, mesa_shader_stage stage)
      : nir_test(name, stage)
   {
   }

protected:
   nir_variable *
   create_distance_var(nir_variable_mode mode, const char *name,
                       gl_varying_slot location, unsigned size)
   {
      nir_variable *var =
         nir_variable_create(b->shader, mode,
                             glsl_array_type(glsl_float_type(), size, 0),
                             name);
      var->data.location = location;
      var->data.location_frac = 0;
      var->data.assigned = true;
      return var;
   }
};

TEST_F(nir_clip_cull_distance_test, vertex_mixed_arrays_share_location)
{
   nir_variable *clip =
      create_distance_var(nir_var_shader_out, "gl_ClipDistance",
                          VARYING_SLOT_CLIP_DIST0, 3);
   nir_variable *cull =
      create_distance_var(nir_var_shader_out, "gl_CullDistance",
                          VARYING_SLOT_CULL_DIST0, 2);

   nir_gather_clip_cull_distance_sizes_from_vars(b->shader);

   EXPECT_EQ(b->shader->info.clip_distance_array_size, 3);
   EXPECT_EQ(b->shader->info.cull_distance_array_size, 2);

   EXPECT_TRUE(nir_merge_clip_cull_distance_vars(b->shader));

   EXPECT_EQ(clip->data.location, VARYING_SLOT_CLIP_DIST0);
   EXPECT_EQ(cull->data.location, VARYING_SLOT_CLIP_DIST0);
   EXPECT_EQ(cull->data.location_frac, 3);
   EXPECT_EQ(clip->data.how_declared, nir_var_hidden);
   EXPECT_EQ(cull->data.how_declared, nir_var_hidden);
}

TEST_F(nir_clip_cull_distance_test, vertex_mixed_arrays_cross_vec4_boundary)
{
   nir_variable *clip =
      create_distance_var(nir_var_shader_out, "gl_ClipDistance",
                          VARYING_SLOT_CLIP_DIST0, 5);
   nir_variable *cull =
      create_distance_var(nir_var_shader_out, "gl_CullDistance",
                          VARYING_SLOT_CULL_DIST0, 3);

   nir_gather_clip_cull_distance_sizes_from_vars(b->shader);
   EXPECT_TRUE(nir_merge_clip_cull_distance_vars(b->shader));

   EXPECT_EQ(b->shader->info.clip_distance_array_size, 5);
   EXPECT_EQ(b->shader->info.cull_distance_array_size, 3);
   EXPECT_EQ(cull->data.location, VARYING_SLOT_CLIP_DIST1);
   EXPECT_EQ(cull->data.location_frac, 1);
   EXPECT_EQ(clip->data.location, VARYING_SLOT_CLIP_DIST0);
}

TEST_F(nir_clip_cull_distance_test, fragment_mixed_input_arrays_share_location)
{
   nir_variable *clip =
      create_distance_var(nir_var_shader_in, "gl_ClipDistance",
                          VARYING_SLOT_CLIP_DIST0, 6);
   nir_variable *cull =
      create_distance_var(nir_var_shader_in, "gl_CullDistance",
                          VARYING_SLOT_CULL_DIST0, 2);

   nir_gather_clip_cull_distance_sizes_from_vars(b->shader);
   EXPECT_EQ(b->shader->info.clip_distance_array_size, 6);
   EXPECT_EQ(b->shader->info.cull_distance_array_size, 2);

   EXPECT_TRUE(nir_merge_clip_cull_distance_vars(b->shader));
   EXPECT_EQ(cull->data.location, VARYING_SLOT_CLIP_DIST1);
   EXPECT_EQ(cull->data.location_frac, 2);
   EXPECT_EQ(clip->data.location, VARYING_SLOT_CLIP_DIST0);
}
