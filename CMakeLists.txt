cmake_minimum_required(VERSION 3.10)
project(window)

set(CMAKE_CXX_STANDARD 17)

# Encontrar pacotes do OpenGL e GLUT
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

# Incluir os diretórios de cabeçalho, se necessário
include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})

# Definir o executável
add_executable(window src/main.cpp)

# Vincular as bibliotecas (Isso resolve os erros do glutSolidCube e glutSwapBuffers)
target_link_libraries(window ${OPENGL_LIBRARIES} ${GLUT_LIBRARIES})