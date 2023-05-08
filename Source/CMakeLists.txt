# version 3.11 or later of CMake or needed later for installing GoogleTest
# so let's require it now.
cmake_minimum_required(VERSION 3.11)

project(space)
add_executable(space 
    main.cpp
    App.cpp
    AppDef.cpp 
    Camera.cpp
    EntityFactory.cpp 
    Game.cpp 
    GameDef.cpp 
    GameState.cpp 
    IntroState.cpp 
    MainMenuState.cpp 
    ParticleSystem.cpp 
    pch.cpp 
    Starfield.cpp 
    World.cpp 
    WorldDef.cpp 
    WorldDraw.cpp 
    WorldQuery.cpp 
    WorldUpdate.cpp 
)

target_precompile_headers(space PRIVATE pch.h)
target_link_libraries(space PRIVATE d2d)

# Tell C++ compiler to use C++20 features. We don't actually use any of them.
target_compile_features(space PUBLIC cxx_std_20)