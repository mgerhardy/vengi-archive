set(LIB voxelworld)
set(SRCS
	BiomeLUAFunctions.h BiomeLUAFunctions.cpp
	Biome.h Biome.cpp
	BiomeManager.h BiomeManager.cpp
	CachedFloorResolver.h CachedFloorResolver.cpp
	ChunkPersister.h ChunkPersister.cpp
	FilePersister.h FilePersister.cpp
	TreeVolumeCache.h TreeVolumeCache.cpp
	WorldContext.h WorldContext.cpp
	WorldEvents.h
	WorldMgr.cpp WorldMgr.h
	WorldPager.h WorldPager.cpp
)

set(FILES
	voxel/models/trees/bush/1.qb
	voxel/models/trees/deciduous/1.qb
	voxel/models/trees/deciduous/2.qb
	voxel/models/trees/deciduous/3.qb
	voxel/models/trees/deciduous/4.qb
	voxel/models/trees/deciduous/5.qb
	voxel/models/trees/fir/1.qb
	voxel/models/trees/palm/1.qb
	voxel/models/trees/pine/1.qb
	voxel/models/trees/trunk.qb
)

engine_add_module(TARGET ${LIB} SRCS ${SRCS} FILES ${FILES} DEPENDENCIES voxelformat noise)

set(TEST_SRCS
	tests/AbstractVoxelWorldTest.h
	tests/FilePersisterTest.cpp
	tests/BiomeManagerTest.cpp
)

set(TEST_FILES
	shared/worldparams.lua
	shared/biomes.lua
)

gtest_suite_sources(tests ${TEST_SRCS})
gtest_suite_deps(tests ${LIB} test-app)
gtest_suite_files(tests ${TEST_FILES})

gtest_suite_begin(tests-${LIB} TEMPLATE ${ROOT_DIR}/src/modules/core/tests/main.cpp.in)
gtest_suite_sources(tests-${LIB} ${TEST_SRCS})
gtest_suite_files(tests-${LIB} ${TEST_FILES})
gtest_suite_deps(tests-${LIB} ${LIB} test-app)
gtest_suite_end(tests-${LIB})

set(BENCHMARK_SRCS
	benchmarks/VoxelBenchmark.cpp
)
engine_add_executable(TARGET benchmarks-${LIB} SRCS ${BENCHMARK_SRCS} FILES ${FILES} shared/worldparams.lua shared/biomes.lua NOINSTALL)
engine_target_link_libraries(TARGET benchmarks-${LIB} DEPENDENCIES benchmark-app ${LIB})
