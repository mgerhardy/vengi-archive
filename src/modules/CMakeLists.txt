# The dependency order here is relevant if you e.g. have modules with files attached
# If you don't include the module with files before its dependent modules, then the
# files will not get copied

check_cxx_compiler_flag("-Wdocumentation" COMPILER_SUPPORTS_WDOCUMENTATION)
if (COMPILER_SUPPORTS_WDOCUMENTATION)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wdocumentation -Wno-documentation-unknown-command")
endif()

add_subdirectory(core)
add_subdirectory(io)
add_subdirectory(command)
add_subdirectory(metric)
add_subdirectory(app)
add_subdirectory(uuidutil)
add_subdirectory(console)
add_subdirectory(http)
add_subdirectory(commonlua)
add_subdirectory(math)
add_subdirectory(util)
add_subdirectory(image)
add_subdirectory(video)
add_subdirectory(compute)
add_subdirectory(computevideo)
add_subdirectory(noise)
add_subdirectory(render)
add_subdirectory(ui)
add_subdirectory(network)
add_subdirectory(audio)
add_subdirectory(voxel)
add_subdirectory(voxelfont)
add_subdirectory(voxelutil)
add_subdirectory(voxelformat)
add_subdirectory(voxelgenerator)
add_subdirectory(voxelworld)
add_subdirectory(attrib)
add_subdirectory(persistence)
add_subdirectory(eventmgr)
add_subdirectory(cooldown)
add_subdirectory(ai-shared)
add_subdirectory(stock)
add_subdirectory(poi)
add_subdirectory(shared)
add_subdirectory(animation)
add_subdirectory(voxelrender)
add_subdirectory(rma)
add_subdirectory(testcore)
if (GAMES OR MAPVIEW)
  add_subdirectory(frontend)
  add_subdirectory(voxelworldrender)
endif()
if (GAMES)
  add_subdirectory(backend)
endif()
