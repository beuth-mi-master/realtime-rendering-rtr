#pragma once

enum GameState {START, RUNNING, ENDED};

class Game {
    public:
        Game();
        Game(float offset);
        double getElapsedTime();
        std::basic_string<char> getTime();
        void updateTime(double now);
    
        float getGameboardLength();

        int getCurrentLevel();
        void nextLevel();
        std::vector<std::vector<int>> loadLevels();
        void buildWorld(ci::vec2 offsets);
        std::vector<ci::vec3> generateGameItems(ci::vec2 offset);
    
        bool isAtStart();
        bool isRunning();
        bool hasEnded();
    
        void start(double now);
        void end();
        void reset();

        std::vector<std::vector<int>> levels;
        std::vector<ci::vec3> obstacles;
        std::vector<ci::vec3> boosters;
        std::vector<ci::vec3> detractors;
        ci::vec2 gameboardSize;
    
        float gameboardColumns;
        float gameboardRows;

    private:
        float timeStart = 0.0f;
        float timeCurrent = 0.0f;
        float gameboardOffset = 0.0f;
        int level = 0;
        GameState running = GameState::START;
    
};
