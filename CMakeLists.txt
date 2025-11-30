cmake_minimum_required(VERSION 3.26)
# Configurando o padrão c++
set(CMAKE_CXX_STANDARD 17)

# Configurando o nome e executável do projeto
set(PROJECT_NAME "bug_walking")
project(${PROJECT_NAME})

# set(GLEW_ROOT "C:/Program Files/glew-2.1.0")

find_package(GLEW REQUIRED)
find_package(SDL2 2.32 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)
#include_directories(${GLEW_ROOT}/include)
#link_directories(${GLEW_ROOT}/lib/Release/x64)
find_package(OpenGL REQUIRED)

add_executable(${PROJECT_NAME}
        Source/Renderer/Shader.cpp
        Source/Renderer/Shader.h
        Source/Main.cpp
        Source/Math.cpp
        Source/Math.h
        Source/Random.cpp
        Source/Random.h
        Source/Game.cpp
        Source/Game.h
        Source/Actors/Actor.cpp
        Source/Actors/Actor.h
        Source/Components/Component.cpp
        Source/Components/Component.h
        Source/Components/Drawing/DrawComponent.cpp
        Source/Components/Drawing/DrawComponent.h
        Source/Components/Physics/RigidBodyComponent.cpp
        Source/Components/Physics/RigidBodyComponent.h
        Source/Components/Physics/AABBColliderComponent.cpp
        Source/Components/Physics/AABBColliderComponent.h
        Source/Components/ParticleSystemComponent.cpp
        Source/Components/ParticleSystemComponent.h
        Source/AudioSystem.cpp
        Source/AudioSystem.h
        Source/Renderer/Font.h
        Source/Renderer/Font.cpp
        Source/UI/UIButton.cpp
        Source/UI/UIButton.h
        Source/UI/UIElement.cpp
        Source/UI/UIElement.h
        Source/UI/UIImage.cpp
        Source/UI/UIImage.h
        Source/UI/Screens/UIScreen.cpp
        Source/UI/Screens/UIScreen.h
        Source/UI/UIText.cpp
        Source/UI/UIText.h
        Source/UI/Screens/HUD.cpp
        Source/UI/UIRect.cpp
        Source/UI/UIRect.h
        Source/UI/Screens/MainMenu.cpp
        Source/UI/Screens/MainMenu.h
        Source/UI/Screens/GameOver.cpp
        Source/UI/Screens/GameOver.h
        Source/UI/Screens/PauseMenu.cpp
        Source/UI/Screens/PauseMenu.h
        Source/UI/Screens/CrossFadeScreen.cpp
        Source/UI/Screens/CrossFadeScreen.h
        Source/Renderer/Renderer.cpp
        Source/Renderer/Renderer.h
        Source/Renderer/VertexArray.h
        Source/Renderer/VertexArray.cpp
        Source/Renderer/Texture.cpp
        Source/Renderer/Texture.h
        Source/Actors/Block.cpp
        Source/Actors/Block.h
        Source/Actors/Goomba.cpp
        Source/Actors/Goomba.h
        Source/Actors/Spawner.cpp
        Source/Actors/Spawner.h
        Source/Actors/Mario.cpp
        Source/Actors/Mario.h
        Source/Components/Drawing/AnimatorComponent.cpp
        Source/Components/Drawing/AnimatorComponent.h
        Source/Components/Drawing/RectComponent.cpp
        Source/Components/Drawing/RectComponent.h
        Source/Actors/Mushroom.cpp
        Source/Actors/Mushroom.h
        Source/Actors/Bullet.cpp
        Source/Actors/Bullet.h
        Source/Actors/Chaser.cpp
        Source/Actors/Chaser.h
        Source/Actors/ChaserSpawner.cpp
        Source/Actors/ChaserSpawner.h
        Source/Actors/Missile.cpp
        Source/Actors/Missile.h
        Source/Actors/Walker.cpp
        Source/Actors/Walker.h
        Source/Actors/WalkerSpawner.cpp
        Source/Actors/WalkerSpawner.h
        Source/Actors/Robot.cpp
)

# Incluir diretórios de terceiros (GLEW/SDL2)
# Se o pacote GLEW fornecer um alvo IMPORTED use-o; caso contrário use as variáveis definidas por find_package
if(TARGET GLEW::GLEW)
    set(GLEW_TARGET GLEW::GLEW)
else()
    # fallback para variáveis clássicas do find_package(GLEW)
    if(NOT DEFINED GLEW_INCLUDE_DIRS)
        message(FATAL_ERROR "GLEW encontrado, mas GLEW_INCLUDE_DIRS não definido. Verifique a instalação do GLEW.")
    endif()
    target_include_directories(${PROJECT_NAME} PRIVATE ${GLEW_INCLUDE_DIRS})
    if(DEFINED GLEW_LIBRARIES)
        set(GLEW_LIBS ${GLEW_LIBRARIES})
    endif()
endif()

# Inclui SDL2
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})

# Liga bibliotecas
if(DEFINED GLEW_TARGET)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${GLEW_TARGET})
elseif(DEFINED GLEW_LIBS)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${GLEW_LIBS})
endif()

# Usar OpenGL::GL (cross-platform) em vez de opengl32 (Windows-only)
if(TARGET OpenGL::GL)
    target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL)
elseif(APPLE)
    find_library(COCOA_FRAMEWORK Cocoa)
    find_library(OPENGL_FRAMEWORK OpenGL)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENGL_FRAMEWORK} ${COCOA_FRAMEWORK})
else()
    # fallback genérico
    target_link_libraries(${PROJECT_NAME} PRIVATE ${OPENGL_LIBRARIES})
endif()

# Linkando targets SDL2
target_link_libraries(${PROJECT_NAME} PRIVATE SDL2::SDL2 SDL2_image::SDL2_image SDL2_mixer::SDL2_mixer SDL2_ttf::SDL2_ttf)
