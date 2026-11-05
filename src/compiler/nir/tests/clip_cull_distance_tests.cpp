/*
 * Copyright © 2026
 * SPDX-License-Identifier: MIT
 */

#include "nir_test.h"

class nir_clip_cull_distance_test : public nir_test {
public:
   nir_clip_cull_distance_test()
      : nir_test("clip_cull_distance", MESA_SHADER_VERTEX)
   {
   }

protected:
   nir_variable *
   create_distance_var(nir_variable_mode mode, const char *name,
                       unsigned location, unsigned size)
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

   nir_variable *
   find_output_var(const char *name)
   {
      nir_foreach_variable_with_modes(var, b->shader, nir_var_shader_out) {
         if (var->name && strcmp(var->name, name) == 0)
            return var;
      }
      return nullptr;
   }
};

TEST_F(nir_clip_cull_distance_test, vertex_clip_only_keeps_clip_location)
{
   nir_variable *clip =
      create_distance_var(nir_var_shader_out, "gl_ClipDistance",
                          VARYING_SLOT_CLIP_DIST0, 4);

   nir_gather_clip_cull_distance_sizes_from_vars(b->shader);
   EXPECT_EQ(b->shader->info.clip_distance_array_size, 4);
   EXPECT_EQ(b->shader->info.cull_distance_array_size, 0);

   EXPECT_TRUE(nir_merge_clip_cull_distance_vars(b->shader));
   EXPECT_EQ(clip->data.location, VARYING_SLOT_CLIP_DIST0);
   EXPECT_EQ(clip->data.location_frac, 0);
}

TEST_F(nir_clip_cull_distance_test, vertex_cull_only_starts_at_clip_location)
{
   nir_variable *cull =
      create_distance_var(nir_var_shader_out, "gl_CullDistance",
                          VARYING_SLOT_CULL_DIST0, 4);

   nir_gather_clip_cull_distance_sizes_from_vars(b->shader);
   EXPECT_EQ(b->shader->info.clip_distance_array_size, 0);
   EXPECT_EQ(b->shader->info.cull_distance_array_size, 4);

   EXPECT_TRUE(nir_merge_clip_cull_distance_vars(b->shader));
   EXPECT_EQ(cull->data.location, VARYING_SLOT_CLIP_DIST0);
   EXPECT_EQ(cull->data.location_frac, 0);
}

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


TEST_F(nir_clip_cull_distance_test,
       vertex_mixed_arrays_vec4_lowering_packs_clip_and_cull)
{
   create_distance_var(nir_var_shader_out, "gl_ClipDistance",
                       VARYING_SLOT_CLIP_DIST0, 3);
   create_distance_var(nir_var_shader_out, "gl_CullDistance",
                       VARYING_SLOT_CULL_DIST0, 2);

   EXPECT_TRUE(nir_lower_clip_cull_distance_to_vec4s(b->shader));

   nir_variable *packed = find_output_var("gl_ClipDistanceMESA");
   ASSERT_NE(packed, nullptr);
   ASSERT_TRUE(glsl_type_is_array(packed->type));
   EXPECT_EQ(glsl_get_length(packed->type), 2);
   EXPECT_EQ(glsl_get_vector_elements(glsl_get_array_element(packed->type)), 4);
}

TEST_F(nir_clip_cull_distance_test,
       vertex_cull_only_vec4_lowering_preserves_cull_count)
{
   create_distance_var(nir_var_shader_out, "gl_CullDistance",
                       VARYING_SLOT_CULL_DIST0, 5);

   EXPECT_TRUE(nir_lower_clip_cull_distance_to_vec4s(b->shader));

   nir_variable *packed = find_output_var("gl_ClipDistanceMESA");
   ASSERT_NE(packed, nullptr);
   ASSERT_TRUE(glsl_type_is_array(packed->type));
   EXPECT_EQ(glsl_get_length(packed->type), 2);
   EXPECT_EQ(glsl_get_vector_elements(glsl_get_array_element(packed->type)), 4);
}

TEST_F(nir_clip_cull_distance_test,
       vertex_mixed_arrays_vec4_lowering_crosses_vec4_boundary)
{
   create_distance_var(nir_var_shader_out, "gl_ClipDistance",
                       VARYING_SLOT_CLIP_DIST0, 5);
   create_distance_var(nir_var_shader_out, "gl_CullDistance",
                       VARYING_SLOT_CULL_DIST0, 3);

   EXPECT_TRUE(nir_lower_clip_cull_distance_to_vec4s(b->shader));

   nir_variable *packed = find_output_var("gl_ClipDistanceMESA");
   ASSERT_NE(packed, nullptr);
   ASSERT_TRUE(glsl_type_is_array(packed->type));
   EXPECT_EQ(glsl_get_length(packed->type), 2);
   EXPECT_EQ(glsl_get_vector_elements(glsl_get_array_element(packed->type)), 4);
}

class nir_clip_cull_fragment_test : public nir_test {
public:
   nir_clip_cull_fragment_test()
      : nir_test("clip_cull_fragment", MESA_SHADER_FRAGMENT)
   {
   }

   nir_variable *create_distance_var(nir_variable_mode mode, const char *name,
                                     unsigned location, unsigned size)
   {
      nir_variable *var = nir_variable_create(
         b->shader, mode, glsl_array_type(glsl_float_type(), size, 0), name);
      var->data.location = location;
      var->data.location_frac = 0;
      var->data.assigned = true;
      return var;
   }
};

TEST_F(nir_clip_cull_fragment_test, fragment_mixed_input_arrays_share_location)
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


static void
count_packed_distance_accesses(nir_shader *shader, nir_variable *packed,
                               unsigned *loads, unsigned *stores)
{
   *loads = 0;
   *stores = 0;

   nir_foreach_function_impl(impl, shader) {
      nir_foreach_block(block, impl) {
         nir_foreach_instr(instr, block) {
            if (instr->type != nir_instr_type_intrinsic)
               continue;

            nir_intrinsic_instr *intr = nir_instr_as_intrinsic(instr);
            if (intr->intrinsic != nir_intrinsic_load_deref &&
                intr->intrinsic != nir_intrinsic_store_deref)
               continue;

            nir_deref_instr *deref = nir_src_as_deref(intr->src[0]);
            if (nir_deref_instr_get_variable(deref) != packed)
               continue;

            if (intr->intrinsic == nir_intrinsic_load_deref)
               (*loads)++;
            else
               (*stores)++;
         }
      }
   }
}

TEST_F(nir_clip_cull_distance_test,
       vertex_constant_and_dynamic_scalar_accesses_retarget_vec4)
{
   nir_variable *clip =
      create_distance_var(nir_var_shader_out, "gl_ClipDistance",
                          VARYING_SLOT_CLIP_DIST0, 8);

   nir_deref_instr *clip_deref = nir_build_deref_var(b, clip);
   nir_deref_instr *constant_deref =
      nir_build_deref_array_imm(b, clip_deref, 5);
   nir_load_deref(b, constant_deref);

   nir_variable *index_var =
      nir_variable_create(b->shader, nir_var_shader_in, glsl_int_type(),
                          "dynamic_index");
   index_var->data.location = VARYING_SLOT_VAR0;
   index_var->data.assigned = true;
   nir_def *dynamic_index =
      nir_i2iN(b, nir_load_var(b, index_var),
               nir_get_ptr_bitsize(b->shader));
   nir_deref_instr *dynamic_deref =
      nir_build_deref_array(b, clip_deref, dynamic_index);
   nir_load_deref(b, dynamic_deref);

   nir_store_deref(b, constant_deref, nir_imm_float(b, 1.0), 1);
   nir_store_deref(b, dynamic_deref, nir_imm_float(b, 2.0), 1);

   EXPECT_TRUE(nir_lower_clip_cull_distance_to_vec4s(b->shader));

   nir_variable *packed = find_output_var("gl_ClipDistanceMESA");
   ASSERT_NE(packed, nullptr);

   unsigned loads, stores;
   count_packed_distance_accesses(b->shader, packed, &loads, &stores);
   EXPECT_EQ(loads, 2);
   /* The dynamic store is expanded into four guarded component stores. */
   EXPECT_EQ(stores, 5);

#ifndef NDEBUG
   nir_validate_shader(b->shader, "clip/cull constant and dynamic access test");
#endif
}


static nir_variable *
create_arrayed_distance_var(nir_shader *shader, nir_variable_mode mode,
                            const char *name, unsigned vertices,
                            unsigned distances)
{
   const struct glsl_type *distance_array =
      glsl_array_type(glsl_float_type(), distances, 0);
   nir_variable *var =
      nir_variable_create(shader, mode,
                          glsl_array_type(distance_array, vertices, 0),
                          name);
   var->data.location = VARYING_SLOT_CLIP_DIST0;
   var->data.assigned = true;
   return var;
}

static nir_variable *
find_packed_distance_var(nir_shader *shader, nir_variable_mode mode)
{
   nir_foreach_variable_with_modes(var, shader, mode) {
      if (var->name && strcmp(var->name, "gl_ClipDistanceMESA") == 0)
         return var;
   }
   return nullptr;
}

class nir_clip_cull_geometry_test : public nir_test {
public:
   nir_clip_cull_geometry_test()
      : nir_test("clip_cull_geometry", MESA_SHADER_GEOMETRY)
   {
   }
};

TEST_F(nir_clip_cull_geometry_test,
       geometry_arrayed_input_preserves_vertex_deref)
{
   nir_variable *clip =
      create_arrayed_distance_var(b->shader, nir_var_shader_in,
                                  "gl_ClipDistance", 3, 8);

   nir_deref_instr *deref = nir_build_deref_var(b, clip);
   /* Build the original 2D deref: per-vertex array, then distance array. */
   deref = nir_build_deref_array_imm(b, deref, 1);
   deref = nir_build_deref_array_imm(b, deref, 5);
   nir_load_deref(b, deref);

   EXPECT_TRUE(nir_lower_clip_cull_distance_to_vec4s(b->shader));

   nir_variable *packed =
      find_packed_distance_var(b->shader, nir_var_shader_in);
   ASSERT_NE(packed, nullptr);

   unsigned loads, stores;
   count_packed_distance_accesses(b->shader, packed, &loads, &stores);
   EXPECT_EQ(loads, 1);
   EXPECT_EQ(stores, 0);

#ifndef NDEBUG
   nir_validate_shader(b->shader, "geometry arrayed clip input test");
#endif
}

class nir_clip_cull_tess_control_test : public nir_test {
public:
   nir_clip_cull_tess_control_test()
      : nir_test("clip_cull_tess_control", MESA_SHADER_TESS_CTRL)
   {
   }
};

TEST_F(nir_clip_cull_tess_control_test,
       tess_control_arrayed_output_preserves_vertex_deref)
{
   nir_variable *clip =
      create_arrayed_distance_var(b->shader, nir_var_shader_out,
                                  "gl_ClipDistance", 3, 8);

   nir_deref_instr *deref = nir_build_deref_var(b, clip);
   deref = nir_build_deref_array_imm(b, deref, 1);
   deref = nir_build_deref_array_imm(b, deref, 5);
   nir_store_deref(b, deref, nir_imm_float(b, 1.0), 1);

   EXPECT_TRUE(nir_lower_clip_cull_distance_to_vec4s(b->shader));

   nir_variable *packed =
      find_packed_distance_var(b->shader, nir_var_shader_out);
   ASSERT_NE(packed, nullptr);

   unsigned loads, stores;
   count_packed_distance_accesses(b->shader, packed, &loads, &stores);
   EXPECT_EQ(loads, 0);
   EXPECT_EQ(stores, 1);

#ifndef NDEBUG
   nir_validate_shader(b->shader, "tess-control arrayed clip output test");
#endif
}

class nir_clip_cull_tess_eval_test : public nir_test {
public:
   nir_clip_cull_tess_eval_test()
      : nir_test("clip_cull_tess_eval", MESA_SHADER_TESS_EVAL)
   {
   }
};

TEST_F(nir_clip_cull_tess_eval_test,
       tess_eval_arrayed_input_preserves_vertex_deref)
{
   nir_variable *clip =
      create_arrayed_distance_var(b->shader, nir_var_shader_in,
                                  "gl_ClipDistance", 3, 8);

   nir_deref_instr *deref = nir_build_deref_var(b, clip);
   deref = nir_build_deref_array_imm(b, deref, 1);
   deref = nir_build_deref_array_imm(b, deref, 5);
   nir_load_deref(b, deref);

   EXPECT_TRUE(nir_lower_clip_cull_distance_to_vec4s(b->shader));

   nir_variable *packed =
      find_packed_distance_var(b->shader, nir_var_shader_in);
   ASSERT_NE(packed, nullptr);

   unsigned loads, stores;
   count_packed_distance_accesses(b->shader, packed, &loads, &stores);
   EXPECT_EQ(loads, 1);
   EXPECT_EQ(stores, 0);

#ifndef NDEBUG
   nir_validate_shader(b->shader, "tess-eval arrayed clip input test");
#endif
}


static void
count_packed_distance_accesses_at_index(nir_shader *shader,
                                        nir_variable *packed,
                                        unsigned array_index,
                                        unsigned *loads, unsigned *stores)
{
   *loads = 0;
   *stores = 0;

   nir_foreach_function_impl(impl, shader) {
      nir_foreach_block(block, impl) {
         nir_foreach_instr(instr, block) {
            if (instr->type != nir_instr_type_intrinsic)
               continue;

            nir_intrinsic_instr *intr = nir_instr_as_intrinsic(instr);
            if (intr->intrinsic != nir_intrinsic_load_deref &&
                intr->intrinsic != nir_intrinsic_store_deref)
               continue;

            nir_deref_instr *deref = nir_src_as_deref(intr->src[0]);
            if (nir_deref_instr_get_variable(deref) != packed ||
                deref->deref_type != nir_deref_type_array ||
                !nir_src_is_const(deref->arr.index) ||
                nir_src_as_uint(deref->arr.index) != array_index)
               continue;

            if (intr->intrinsic == nir_intrinsic_load_deref)
               (*loads)++;
            else
               (*stores)++;
         }
      }
   }
}

TEST_F(nir_clip_cull_distance_test,
       vertex_cull_accesses_use_clip_offset_in_shared_vec4_array)
{
   create_distance_var(nir_var_shader_out, "gl_ClipDistance",
                       VARYING_SLOT_CLIP_DIST0, 3);
   nir_variable *cull =
      create_distance_var(nir_var_shader_out, "gl_CullDistance",
                          VARYING_SLOT_CULL_DIST0, 2);

   nir_deref_instr *cull_deref = nir_build_deref_var(b, cull);
   cull_deref = nir_build_deref_array_imm(b, cull_deref, 1);
   nir_load_deref(b, cull_deref);
   nir_store_deref(b, cull_deref, nir_imm_float(b, 3.0), 1);

   EXPECT_TRUE(nir_lower_clip_cull_distance_to_vec4s(b->shader));

   nir_variable *packed = find_output_var("gl_ClipDistanceMESA");
   ASSERT_NE(packed, nullptr);

   /* clip[0..2] occupy lanes 0..2; cull[1] maps to packed vec4[1].x. */
   unsigned loads, stores;
   count_packed_distance_accesses_at_index(b->shader, packed, 1,
                                           &loads, &stores);
   EXPECT_EQ(loads, 1);
   EXPECT_EQ(stores, 1);

#ifndef NDEBUG
   nir_validate_shader(b->shader, "clip/cull shared vec4 offset test");
#endif
}
