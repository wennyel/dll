cmake_minimum_required(VERSION 3.10)
project(ClienteCabal)

# Definir padrão C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Incluir diretórios
include_directories(include)
link_directories(lib)

# Encontrar arquivos fonte
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c")
file(GLOB_RECURSE HEADERS "include/*.h" "include/*.hpp")

# Criar DLL
add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS})

# Definir propriedades da DLL
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "gamehook"
    PREFIX ""
    SUFFIX ".dll"
)

# Linkar bibliotecas necessárias
target_link_libraries(${PROJECT_NAME}
    # Adicione suas bibliotecas aqui
    # fmodex
    # keystone
)

# Definir macros para exportação
target_compile_definitions(${PROJECT_NAME} PRIVATE
    BUILDING_DLL
    _CRT_SECURE_NO_WARNINGS
)
