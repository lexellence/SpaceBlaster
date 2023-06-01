target_sources(${PROJECT_NAME} PRIVATE
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

target_sources(${PROJECT_NAME} PRIVATE
    App.h
    AppDef.h
    Camera.h
    EntityFactory.h
    Game.h
    GameDef.h
    GameState.h
    IntroState.h
    MainMenuState.h
    ParticleSystem.h
    pch.h
    Starfield.h
    World.h
    WorldDef.h
)
