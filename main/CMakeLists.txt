add_executable(${projectName} main.cpp)
set(CADMIUM_DIR $ENV{CADMIUM})
message(STATUS "✅ JSON (from ENV) = $ENV{JSON}")

target_include_directories(${projectName} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"              
    "${CMAKE_CURRENT_SOURCE_DIR}/include"      
    ${CADMIUM_DIR}                              
    "${CADMIUM_DIR}/third_party/cadmium_v2/include"   
    "${CMAKE_SOURCE_DIR}/third_party"     
)


target_compile_features(${projectName} PRIVATE cxx_std_20)
get_target_property(INC_DIRS ${projectName} INCLUDE_DIRECTORIES)
message(STATUS "🔍 Actual include dirs: ${INC_DIRS}")
