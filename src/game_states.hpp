// game_states.hpp
#pragma once

enum class TurnState {
    PLAYER_TURN,
    ENEMY_TURN,
};

enum class GameState {
    PLAYING,
    VICTORY,
    MAP,
    TUTORIAL,
    LOOT,
    MENU,
    GAMEOVER
};