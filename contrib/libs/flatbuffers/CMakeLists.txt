set(SRCS
	dummy.cpp
	flatbuffers/flatbuffers.h
	flatbuffers/flexbuffers.h
	flatbuffers/hash.h
	flatbuffers/idl.h
	flatbuffers/util.h
)

engine_add_library(
	LIB flatbuffers
	SRCS ${SRCS}
)
if (NOT FLATBUFFERS_FOUND)
	target_include_directories(flatbuffers PUBLIC .)
endif()

find_program(FLATC_EXECUTABLE NAMES flatc)
if (FLATC_EXECUTABLE)
	add_executable(flatc IMPORTED GLOBAL)
	set_property(TARGET flatc PROPERTY IMPORTED_LOCATION ${FLATC_EXECUTABLE})
else()
	set(COMPILER_SRCS
		compiler/src/compiler/cpp_generator.cc
		compiler/src/compiler/cpp_generator.h
		compiler/src/compiler/go_generator.cc
		compiler/src/compiler/go_generator.h
		compiler/src/compiler/java_generator.cc
		compiler/src/compiler/java_generator.h
		compiler/src/compiler/swift_generator.cc
		compiler/src/compiler/swift_generator.h
		compiler/src/compiler/python_generator.cc
		compiler/src/compiler/python_generator.h
		compiler/src/compiler/ts_generator.cc
		compiler/src/compiler/ts_generator.h
		compiler/bfbs_gen_lua.cpp
		compiler/code_generators.cpp
		compiler/flatc.cpp
		compiler/flatc_main.cpp
		compiler/idl_gen_cpp.cpp
		compiler/idl_gen_dart.cpp
		compiler/idl_gen_fbs.cpp
		compiler/idl_gen_csharp.cpp
		compiler/idl_gen_java.cpp
		compiler/idl_gen_go.cpp
		compiler/idl_gen_grpc.cpp
		compiler/idl_gen_json_schema.cpp
		compiler/idl_gen_kotlin.cpp
		compiler/idl_gen_lobster.cpp
		compiler/idl_gen_lua.cpp
		compiler/idl_gen_php.cpp
		compiler/idl_gen_python.cpp
		compiler/idl_gen_rust.cpp
		compiler/idl_gen_swift.cpp
		compiler/idl_gen_text.cpp
		compiler/idl_gen_ts.cpp
		compiler/idl_parser.cpp
		compiler/reflection.cpp
		compiler/util.cpp
	)
	engine_add_build_executable(TARGET flatc SRCS ${COMPILER_SRCS} NOINSTALL)
	set_target_properties(flatc PROPERTIES UNITY_BUILD FALSE)
	target_link_libraries(flatc flatbuffers)
	target_include_directories(flatc PRIVATE compiler/)
endif()
