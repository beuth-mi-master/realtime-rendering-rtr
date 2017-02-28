#include "Game.hpp"

Game::Game() {

}

Game::Game(float offset) {
    gameboardOffset = offset * 2.0f;
    level = 1;
}

void Game::start(double now) {
    running = GameState::RUNNING;
    timeStart = now;
}

bool Game::isRunning() {
    return running == GameState::RUNNING;
}

bool Game::isAtStart() {
    return running == GameState::START;
}

bool Game::hasEnded() {
    return running == GameState::ENDED;
}

void Game::end() {
    running = GameState::ENDED;
}

void Game::reset() {
    timeStart = 0.0;
    timeCurrent = 0.0;
    running = GameState::START;
}

double Game::getElapsedTime() {
    return timeCurrent - timeStart;
}

void Game::updateTime(double now) {
    timeCurrent = now;
}

float Game::getGameboardLength() {
    return gameboardSize.y - gameboardOffset;
}


std::basic_string<char> Game::getTime() {
    std::basic_string<char> time = std::to_string((int)getElapsedTime());
    time.insert(time.begin(), 5 - time.length(), '0');
    return time;
}

int Game::getCurrentLevel() {
    return level;
}

void Game::nextLevel() {
    level++;
}

std::vector<int> split(const std::string &s, char delim) {
    std::vector<int> elems;
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(std::stoi(item));
    }
    return elems;
}

std::vector<std::vector<int>> Game::loadLevels() {
    
    std::vector<std::vector<int>> level;
    
    ci::DataSourceRef loadedMatrix = ci::loadFile(cinder::app::getAssetPath("levels/level-1.txt"));
    ci::IStreamRef loadedMatrixStream = loadedMatrix->createStream();
    
    while (!loadedMatrixStream->isEof()) {
        std::string line = loadedMatrixStream->readLine();
        std::vector<int> row = split(line, '\t');
        level.push_back(row);
    }
    
    return level;

};

void Game::buildWorld(ci::vec2 offset) {
    levels = loadLevels();
    generateGameItems(offset);
    gameboardColumns = levels[0].size();
    gameboardRows = levels.size();
    gameboardSize = ci::vec2(gameboardColumns * offset.x, gameboardRows * offset.y + gameboardOffset);
}

std::vector<ci::vec3> Game::generateGameItems(ci::vec2 offset) {
    for (int y = 0; y < levels.size(); y++) {
        std::vector<int> row = levels[y];
        for (int x = 0; x < row.size(); x++) {
            int cell = row[x];
            float halfSize = row.size() / 2;
            float xOff = (x - halfSize) * offset.x;
            ci::vec3 pos = ci::vec3(xOff, 0, offset.y * y);
            if (cell == 1) {
                obstacles.push_back(pos);
            } else if (cell == 2) {
                boosters.push_back(pos);
            } else if (cell == 3) {
                detractors.push_back(pos);
            }
        }
    }
    return obstacles;
}
