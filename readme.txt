Adresářová struktura:
    BE/
        CMakeLists.txt           # build script pro backend
        game.cpp                 # implementace herní logiky (backend)
        game.h                   # deklarace tříd a funkcí backendu
        gametypes.h              # definice typů (gameboard, sides, nodetype, nodeshape, apod.)
    zarovka.cpp                  # implementace grafického rozhraní a ovládání hry (frontend)
    zarovka.h                    # deklarace tříd a funkcí frontendové části
    CMakeLists.txt               # build script pro frontend
    resources/
        mainlevels/              # složka s předdefinovanými úrovněmi
        textures/                # složka s texturami herních prvků
    readme.txt                   # tento soubor

Autorství:
    Matyáš Hebert       herní pole, BE
    Jan Ostatnický      editor, výběr levelů
    Klára Čoupková      nastavení, statistiky, výpis uživatelských levelů
