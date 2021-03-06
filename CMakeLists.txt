cmake_minimum_required(VERSION 2.6)

project(ClassTrace)
enable_testing()

if(WIN32)
	set(CXX_OPT /EHsc)
elseif(CLANG)
	set(CMAKE_C_COMPILER clang)
	set(CMAKE_CXX_COMPILER clang++)
	set(STL -stdlib=libc++)
else()
	set(CMAKE_C_COMPILER gcc)
	set(CMAKE_CXX_COMPILER g++)
	if(OLDGCC)
		set(CXX_OPT -std=c++0x)
	else()
		set(CXX_OPT -std=c++11)
	endif()

	if(${CMAKE_SYSTEM_NAME} STREQUAL FreeBSD)
		set(CMAKE_C_COMPILER "gcc48")
		set(CMAKE_CXX_COMPILER "g++48")
		set(LD_CXX_PATH "/usr/local/lib/gcc48")
	endif()
endif()

if(WITH_BOOST)
	include(/usr/local/include) # path to boost
else()
	set(CMAKE_CXX_FLAGS "${CXX_OPT} ${STL}")
	set(CMAKE_EXE_LINKER_FLAGS "${CXX_OPT} ${STL}")
	set(CMAKE_SHARED_LINKER_FLAGS "${STL}")
endif()

add_definitions(
#	-DDISABLE_CLASS_TRACE
#	-DCLASS_TRACE_LEVEL=4
)

set(LIB_TRACE classtrace)

add_library(classtrace STATIC trace_config.cpp)
set_target_properties(classtrace PROPERTIES COMPILE_FLAGS -DCLASS_TRACE_CONFIG)

# samples

add_executable(s1 sample1.cpp)
set_target_properties(s1 PROPERTIES COMPILE_FLAGS -DCLASS_TRACE_OUT=sample1.trace)

add_executable(s2 sample2.cpp)
set_target_properties(s2 PROPERTIES COMPILE_FLAGS -DCLASS_TRACE_CONFIG)
target_link_libraries(s2 ${LIB_TRACE})

add_executable(s3 sample3.cpp)
set_target_properties(s3 PROPERTIES COMPILE_FLAGS -DCLASS_TRACE_OUT=sample3.trace)

add_executable(sx sample_x.cpp)

add_executable(s1_cfg sample1.cpp)
set_target_properties(s1_cfg PROPERTIES COMPILE_FLAGS -DCLASS_TRACE_CONFIG)
target_link_libraries(s1_cfg ${LIB_TRACE})

# tests

add_test(test1 s1)
add_test(test1_cfg s1_cfg)
add_test(test2 s2)
add_test(test3 s3)

set_property(TEST test1 test3
	PROPERTY ENVIRONMENT
		LD_LIBRARY_PATH=${LD_CXX_PATH}
)

set_property(TEST test1_cfg test2
	PROPERTY ENVIRONMENT
		PATH=${PROJECT_SOURCE_DIR}
		LD_LIBRARY_PATH=${LD_CXX_PATH}
		CLASS_TRACE_CONFIG=${PROJECT_SOURCE_DIR}/trace.init
)
