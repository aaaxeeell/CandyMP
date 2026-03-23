cmake_minimum_required(VERSION 3.10)
project(p2_project)

set(CMAKE_CXX_STANDARD 20)

# Detectar ruta de Homebrew
execute_process(COMMAND brew --prefix OUTPUT_VARIABLE BREW_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)

# 1. Rutas de búsqueda para el Linker
link_directories("${BREW_PREFIX}/lib")

# 2. Buscar paquetes SDL
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_ttf REQUIRED)
find_package(SDL2_mixer REQUIRED)

# 3. Buscar la librería PNG de forma manual para asegurar el tiro
find_library(PNG_LIB NAMES png png16 HINTS "${BREW_PREFIX}/lib")

add_executable(p2_project "")

file(GLOB_RECURSE SOURCES "src/*.cpp" "test/*.cpp")
target_sources(p2_project PRIVATE ${SOURCES})

# 4. Includes
target_include_directories(p2_project PRIVATE 
    "src"
    "src/private/sdl_wrapper"
    "src/private/extlibs"
    "${BREW_PREFIX}/include"
    "${BREW_PREFIX}/include/SDL2"
)

# 5. Enlace final (Usando la variable de la librería encontrada)
target_link_libraries(p2_project PRIVATE 
    SDL2::SDL2 
    SDL2_image::SDL2_image 
    SDL2_ttf::SDL2_ttf 
    SDL2_mixer::SDL2_mixer
    ${PNG_LIB}
)