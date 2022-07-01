link_directories(${CMAKE_SOURCE_DIR}/lib)

find_package(GLM REQUIRED)
message(STATUS "GLM included at ${GLM_INCLUDE_DIR}")

set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
find_package(GLFW3 REQUIRED)
message(STATUS "Found GLFW3 in ${GLFW3_INCLUDE_DIR}")

add_library(GLAD "glad.c")

set(LIBS glfw3 opengl32 GLAD)
# set(DEBUG_LIBS glfw3_mt opengl32 GLAD)

add_definitions(-D_CRT_SECURE_NO_WARNINGS)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB SHADERS
	"shaders/*.vert"
	"shaders/*.frag"
	"shaders/*.geom"

	"shaders/**/*.vert"
	"shaders/**/*.frag"
	"shaders/**/*.geom"
)

file(GLOB IMGUI
	"imgui/*"
)

file(GLOB TOOLS
	".clang-format"
	".clang-tidy"
)

file(GLOB TEXTURAS
	"texturas/*"
)

source_group("Shaders" FILES ${SHADERS})
source_group("Imgui" FILES ${IMGUI})
source_group("Tools" FILES ${TOOLS})
source_group("Texturas" FILES ${TEXTURAS})

# set(CMAKE_CXX_CLANG_TIDY clang-tidy -checks=-*,readability-identifier-naming)

add_executable(tfg main.cpp
	render_info.h
	runtime_config.h
	utils.h
	utils.cpp
	camara.h
	camara.cpp
	shader.h
	shader.cpp
	texture.h
	texture.cpp
	vertice.h
	mesh.h
	mesh.cpp
	modelo.h
	modelo.cpp
	luz.h
	luz.cpp
	generacion_base.h
	generacion_base.cpp
	diamond_square.h
	diamond_square.cpp
	render.cpp
	render.h
	ruidos.cpp
	ruidos.h
	${IMGUI}
	${SHADERS}
	${TOOLS}
	${TEXTURAS}
)

add_custom_target(shaders
	COMMAND ${CMAKE_COMMAND} -E remove_directory "$<TARGET_FILE_DIR:tfg>/shaders"
	COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:tfg>/shaders"
	COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/shaders" "$<TARGET_FILE_DIR:tfg>/shaders"

	COMMAND ${CMAKE_COMMAND} -E remove_directory "$<TARGET_FILE_DIR:tfg>/texturas"
	COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:tfg>/texturas"
	COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/texturas" "$<TARGET_FILE_DIR:tfg>/texturas"
)

add_dependencies(tfg shaders)

target_link_libraries(tfg ${LIBS})
# target_link_libraries(tfg debug ${DEBUG_LIBS})


target_compile_options(tfg PRIVATE /std:c++20 /MP)
target_link_options(tfg PUBLIC /ignore:4099)

set_target_properties(tfg PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set_target_properties(tfg PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/Debug")


# foreach(SHADER ${SHADERS})
# 	add_custom_command(TARGET tfg PRE_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_FILE_DIR:tfg>/shaders")
# 	add_custom_command(TARGET tfg PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SHADER} "$<TARGET_FILE_DIR:tfg>/shaders")
# endforeach(SHADER)

include_directories(${CMAKE_SOURCE_DIR}/includes)
