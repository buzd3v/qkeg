# For 2D games, use:
#
# get_qkeg_common_2d_shaders(QKEG_2D_SHADERS)
# target_shaders(platformer ${QKEG_2D_SHADERS})
#
# For 3D games:
#
# get_qkeg_common_3d_shaders(QKEG_3D_SHADERS)
# target_shaders(platformer ${QKEG_3D_SHADERS})
#
# If the game has custom shaders, just append them to the list
# before passing it info target_shaders

# makes return(PROPAGATE ...) work
cmake_policy(SET CMP0140 NEW)

function(get_qkeg_common_2d_shaders outVar)
  set(QKEG_2D_SHADERS

    fullScreenQuad.vert

    # fullscreen_triangle.vert
    # sprite.vert
    # sprite.frag
    # crt_lottes.frag

    # imgui.vert
    # imgui.frag
  )

  get_target_property(QKEG_SOURCE_DIR qkeg SOURCE_DIR)
  set(QKEG_SHADERS_DIR "${QKEG_SOURCE_DIR}/src/Shaders/")
  list(TRANSFORM QKEG_2D_SHADERS PREPEND "${QKEG_SHADERS_DIR}")

  set(${outVar} ${QKEG_2D_SHADERS})
  return(PROPAGATE ${outVar})
endfunction()

# im3d has quite a few shaders...
function(get_im3d_shaders outVar)
  set(IM3D_SHADERS

    # im3d_points.vert
    # im3d_points.frag
    # im3d_lines.vert
    # im3d_lines.geom
    # im3d_lines.frag
    # im3d_triangles.vert
    # im3d_triangles.frag
  )

  get_target_property(QKEG_SOURCE_DIR qkeg SOURCE_DIR)
  set(IM3D_SHADERS_DIR "${QKEG_SOURCE_DIR}/src/Shaders/im3d/")
  list(TRANSFORM IM3D_SHADERS PREPEND "${IM3D_SHADERS_DIR}")

  set(${outVar} ${IM3D_SHADERS})
  return(PROPAGATE ${outVar})
endfunction()

function(get_qkeg_common_3d_shaders outVar)
  set(QKEG_3D_SHADERS
    fullScreenQuad.vert

    # fullscreen_triangle.vert
    # skybox.frag
    # skinning.comp
    # mesh.vert
    # mesh_depth_only.vert
    # mesh_depth.frag
    # mesh.frag
    # postfx.frag
    # depth_resolve.frag
    # sprite.vert
    # sprite.frag
    # shadow_map_point.vert
    # shadow_map_point.frag
    # imgui.vert
    # imgui.frag
    gradient.frag
    mesh.vert
    mesh.frag
    skeleton.comp
    cubemap.frag
    default.vert
  )

  get_target_property(QKEG_SOURCE_DIR qkeg SOURCE_DIR)
  set(QKEG_SHADERS_DIR "${QKEG_SOURCE_DIR}/src/Shaders/")
  list(TRANSFORM QKEG_3D_SHADERS PREPEND "${QKEG_SHADERS_DIR}")

  get_im3d_shaders(IM3D_SHADERS)
  list(APPEND QKEG_3D_SHADERS ${IM3D_SHADERS})

  set(${outVar} ${QKEG_3D_SHADERS})
  return(PROPAGATE ${outVar})
endfunction()
