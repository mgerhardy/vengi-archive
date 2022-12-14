include_directories(modules)
add_subdirectory(modules)

project(voxedit)
set(SRCS
	VoxEdit.h VoxEdit.cpp
)

set(FILES
	voxedit/chr_knight.qb
	voxedit/robo.qb
	voxedit/robo.vox
	voxedit/voxedit-keybindings.cfg
	voxedit/scripts/test-import-plane.cfg
	voxedit/scripts/test-layers.cfg
	voxedit/scripts/test-animate.cfg
)

set(VOXEDIT_ANIMATION True CACHE BOOL "Activate or deactivate the animation system for voxedit")

engine_add_executable(TARGET ${PROJECT_NAME} SRCS ${SRCS} FILES ${FILES} WINDOWED)
engine_target_link_libraries(TARGET ${PROJECT_NAME} DEPENDENCIES voxedit-ui voxedit-util)
if (HOT_RELOAD_ANIM AND VOXEDIT_ANIMATION)
	add_dependencies(${PROJECT_NAME} anim)
endif()
