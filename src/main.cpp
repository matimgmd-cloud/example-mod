#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>

using namespace geode::prelude();

// Rozmiar okienka w tickach (0-5 ticków przed akcją z makra)
static int g_windowSizeTicks = 5;

static bool g_hasBufferedClick = false;
static int g_targetMacroTick = -1;

class $modify(CBFPlayLayer, PlayLayer) {
    void update(float dt) {
        PlayLayer::update(dt);

        // Pobieramy aktualny tick gry
        int currentTick = static_cast<int>(m_gameState.m_currentLevelTime * 60.0f); 

        // Jeśli gracz kliknął w okienku i nadszedł docelowy tick z makra:
        if (g_hasBufferedClick && currentTick >= g_targetMacroTick) {
            // Wymuszamy perfekcyjny skok w grze
            this->handleButton(true, static_cast<int>(PlayerButton::Jump), true);
            
            // Resetujemy bufor
            g_hasBufferedClick = false;
            g_targetMacroTick = -1;
        }
    }
};

class $modify(CBFBaseGameLayer, GJBaseGameLayer) {
    void handleButton(bool down, int button, bool isPlayer1) {
        // Sprawdzamy czy wciśnięto przycisk skoku
        if (down && button == static_cast<int>(PlayerButton::Jump)) {
            
            int macroNextTick = 100; // Przykładowy tick z makra
            int currentTick = static_cast<int>(m_gameState.m_currentLevelTime * 60.0f);

            // Sprawdzamy czy kliknięcie mieści się w wyprzedzającym okienku
            if (currentTick >= (macroNextTick - g_windowSizeTicks) && currentTick <= macroNextTick) {
                log::info("Click Between Frames++: Trafiono w okienko! (Tick {})", currentTick);
                
                g_hasBufferedClick = true;
                g_targetMacroTick = macroNextTick;
                
                // Blokujemy natychmiastowy surowy skok, mod wykona go w odpowiednim ticku
                return; 
            }
        }

        GJBaseGameLayer::handleButton(down, button, isPlayer1);
    }
};
