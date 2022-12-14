set(LIB voxelrender)
set(SRCS
	CachedMeshRenderer.cpp CachedMeshRenderer.h
	MeshRenderer.cpp MeshRenderer.h
	SceneGraphRenderer.cpp SceneGraphRenderer.h
	RawVolumeRenderer.cpp RawVolumeRenderer.h
	ShaderAttribute.h
	ImageGenerator.h ImageGenerator.cpp
	VoxelFontRenderer.h VoxelFontRenderer.cpp
)
set(SHADERS voxel voxel_indirect voxel_instanced)
set(SRCS_SHADERS
	shaders/_material.vert
	shaders/_voxelflags.glsl
)
foreach (SHADER ${SHADERS})
	list(APPEND SRCS_SHADERS "shaders/${SHADER}.vert")
	list(APPEND SRCS_SHADERS "shaders/${SHADER}.frag")
endforeach()

engine_add_module(TARGET ${LIB} SRCS ${SRCS} ${SRCS_SHADERS} DEPENDENCIES render voxelfont voxelformat)
generate_shaders(${LIB} ${SHADERS})

set(TEST_SRCS
	tests/RawVolumeRendererTest.cpp
	tests/VoxelRenderShaderTest.cpp
)

gtest_suite_sources(tests ${TEST_SRCS})
gtest_suite_deps(tests ${LIB} test-app)
gtest_suite_files(tests ${TEST_FILES})

gtest_suite_begin(tests-${LIB} TEMPLATE ${ROOT_DIR}/src/modules/core/tests/main.cpp.in)
gtest_suite_sources(tests-${LIB} ${TEST_SRCS})
gtest_suite_files(tests-${LIB} ${TEST_FILES})
gtest_suite_deps(tests-${LIB} ${LIB} test-app)
gtest_suite_end(tests-${LIB})
