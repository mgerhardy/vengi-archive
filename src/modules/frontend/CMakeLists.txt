set(SRCS
	ClientEntity.h ClientEntity.cpp
	ClientEntityId.h
	ClientEntityRenderer.h ClientEntityRenderer.cpp
	Colors.h
	EntityMgr.cpp EntityMgr.h
	PlayerAction.h PlayerAction.cpp
	PlayerMovement.h PlayerMovement.cpp
	PlayerCamera.h PlayerCamera.cpp
)
set(FILES
	shared/sound/ambience_wind.wav

	shared/sound/footstep_generic.wav

	shared/sound/water_enter.wav
	shared/sound/water_leave.wav
)

set(LIB frontend)
engine_add_module(TARGET ${LIB} SRCS ${SRCS} FILES ${FILES} DEPENDENCIES attrib animation shared audio)
if (HOT_RELOAD_ANIM)
	add_dependencies(${LIB} anim)
endif()
