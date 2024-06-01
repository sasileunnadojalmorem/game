#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <string>
// 윈도우 크기 정의
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int PADDLE_WIDTH = 100;
const int PADDLE_HEIGHT = 20;
const int BALL_SIZE = 10;
const int BLOCK_WIDTH = 60;
int BLOCK_HEIGHT = 20; // 공 속도 조절
const int NUM_BLOCKS_X = 11;
const int NUM_BLOCKS_Y = 4;
int startstate = 0; // 시작 메뉴 설정
int gameoverstate = 0;
int levelstate = 1; // 난이도 설정
int highscore = 0;  // 최고 점수

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
TTF_Font* font = nullptr;

// 텍스트를 렌더링하는 함수
void renderText(const std::string& message, int x, int y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, message.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect dstrect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}
void renderText(const std::string& text, int y, SDL_Color color, int fontSize) {
    // 폰트 크기 지정
    TTF_Font* font = TTF_OpenFont("arial.ttf", fontSize);
    if (!font) {
        // 폰트 로드 실패 처리
        // 예: std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return;
    }

    // 텍스트 표시를 위한 표면(surface) 생성
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!surface) {
        // 표면 생성 실패 처리
        // 예: std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
        TTF_CloseFont(font); // 폰트 닫기
        return;
    }

    // 표면을 텍스처로 변환
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        // 텍스처 생성 실패 처리
        // 예: std::cerr << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << std::endl;
    }
    else {
        // 텍스트의 가로 폭과 높이 계산
        int textWidth = surface->w;
        int textHeight = surface->h;

        // 텍스트를 화면 중앙에 배치하기 위한 x 좌표 계산
        int x = (SCREEN_WIDTH - textWidth) / 2;

        // 텍스트 표시
        SDL_Rect renderQuad = { x, y, textWidth, textHeight };
        SDL_RenderCopy(renderer, texture, NULL, &renderQuad);
        // 텍스처 해제
        SDL_DestroyTexture(texture);
    }

    // 표면 해제
    SDL_FreeSurface(surface);
    // 폰트 닫기
    TTF_CloseFont(font);
}

void renderTextCentered(const std::string& text, int y, SDL_Color color, int fontSize = 24) {
    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (textSurface == nullptr) {
        return;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (textTexture == nullptr) {
        SDL_FreeSurface(textSurface);
        return;
    }

    // 텍스트 표면의 너비와 높이 가져오기
    int textWidth = textSurface->w;
    int textHeight = textSurface->h;

    // 텍스트를 가운데 정렬하기 위한 x 좌표 계산
    int x = (SCREEN_WIDTH - textWidth) / 2;

    // 텍스트 표면과 텍스트 텍스처 해제
    SDL_FreeSurface(textSurface);

    // 텍스트 텍스처 렌더링
    SDL_Rect renderRect = { x, y, textWidth, textHeight };
    SDL_RenderCopy(renderer, textTexture, nullptr, &renderRect);

    // 텍스트 텍스처 해제
    SDL_DestroyTexture(textTexture);
}

// 배경 이미지를 렌더링하는 함수
void renderBackground(const char* filePath) {
    SDL_Surface* surface = IMG_Load(filePath);
    if (!surface) {
        std::cerr << "이미지 로드 실패! IMG_Error: " << IMG_GetError() << std::endl;
        return;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    SDL_Rect dstrect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);

    SDL_DestroyTexture(texture);
}

// 패들 클래스 정의
class Paddle {
public:
    SDL_Rect rect; // 패들의 사각형
    int speed; // 패들의 이동 속도

    // 생성자: 패들의 초기 위치와 크기 설정
    Paddle(int x, int y) : speed(25) {
        rect = { x, y, PADDLE_WIDTH, PADDLE_HEIGHT };
    }

    // 패들을 왼쪽으로 이동
    void moveLeft() {
        if (rect.x > 0) {
            rect.x -= speed;
        }
    }

    // 패들을 오른쪽으로 이동
    void moveRight() {
        if (rect.x + rect.w < SCREEN_WIDTH) {
            rect.x += speed;
        }
    }

    // 패들을 렌더러에 그리기
    void render(SDL_Renderer* renderer) const {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // 패들 색상 파란색
        SDL_RenderFillRect(renderer, &rect); // 패들 그리기
    }
};

// 공 클래스 정의
class Ball {
public:
    SDL_Rect rect; // 공의 위치와 크기를 저장하는 SDL_Rect 구조체
    int xVel, yVel; // 공의 속도

    // 공 생성자, 공의 초기 위치와 속도 설정
    Ball(int x, int y) : xVel(3), yVel(-3) {
        rect = { x, y, BALL_SIZE, BALL_SIZE };
    }

    // 공을 이동시키는 함수
    void move() {
        
        rect.x += xVel;
        rect.y += yVel;

        // 공이 좌우 벽에 닿았을 때 속도 반전
        if (rect.x <= 0 || rect.x + rect.w >= SCREEN_WIDTH) {
            xVel = -xVel;
        }
        // 공이 화면의 천장에 닿았을 때 속도 반전
        if (rect.y <= 0) {
            yVel = -yVel;
        }
    }

    // 공을 렌더러에 그리는 함수
    void render(SDL_Renderer* renderer) const {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // 공 색상 노란색
        SDL_RenderFillRect(renderer, &rect);
    }

    // 공의 위치와 속도를 초기화하는 함수
    void reset(int x, int y) {
        rect.x = x;
        rect.y = y;
        xVel = 3;
        yVel = -3;
    }
};
void setBallSpeed(Ball& ball, int level) {//공 난이도에 맞게 속도조절
    switch (level) {
    case 1: // 느리게
        ball.xVel = 3;
        ball.yVel = -3;
        break;
    case 2: // 빠르게
        ball.xVel = 4;
        ball.yVel = -4;
        break;
    case 3: // 아주 빠르게
        ball.xVel = 6;
        ball.yVel = -6;
        break;
    default:
        ball.xVel = 3; // 기본값
        ball.yVel = -3;
        break;
    }
}


// 블록 클래스 정의
class Block {
public:
    SDL_Rect rect;  // 블록의 위치와 크기를 저장하는 SDL_Rect 구조체
    bool destroyed; // 블록이 파괴되었는지 여부
    bool special;   // 특수 블록 여부
    int strength;   // 강한 블록의 강도 (몇 번 맞아야 깨지는지)
    SDL_Color color; // 블록의 색상

    // 블록 생성자, 초기 위치 설정 및 파괴 여부 초기화
    Block(int x, int y, bool isSpecial, int blockStrength = 1) : destroyed(false), special(isSpecial), strength(blockStrength) {
        rect = { x, y, BLOCK_WIDTH, BLOCK_HEIGHT };
        // 블록 색상 설정
        if (special) {
            color = { 255, 0, 255, 255 }; // 특수 블록의 색상 (보라색)
        }
        else if (blockStrength > 1) {
            color = { 0, 0, 255, 255 }; // 강한 블록의 색상 (파란색)
        }
        else {
            color = { 0, 255, 0, 255 }; // 일반 블록의 색상 (녹색)
        }
    }

    // 블록을 렌더러에 그리는 함수
    void render(SDL_Renderer* renderer) const {
        if (!destroyed) {  // 블록이 파괴되지 않았으면
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a); // 블록의 색상 설정
            SDL_RenderFillRect(renderer, &rect); // 블록 그리기
        }
    }
    // 강한 블록의 강도를 감소시키고 색상을 연하게 만드는 함수
    void decreaseStrength() {
        if (strength > 1) {
            strength--;
            // 강도에 따라 색상을 연하게 만듭니다.
            color.r /= 2;
            color.g /= 2;
            color.b /= 2;
        }
    }
};

// 두 SDL_Rect 객체가 충돌하는지 확인하는 함수
bool checkCollision(const SDL_Rect& a, const SDL_Rect& b) {
    return SDL_HasIntersection(&a, &b);
}

// 특수 블록 생성
void createSpecialBlock(std::vector<Block>& blocks, int x, int y) {
    blocks.emplace_back(x, y, true);
}

// 강한 블록 생성
void createStrongBlock(std::vector<Block>& blocks, int x, int y) {
    blocks.emplace_back(x, y, false, 3); // 강도를 3으로 설정하여 3번 맞아야 깨지도록 함
}

// 주변 블록 파괴
void destroySurroundingBlocks(std::vector<Block>& blocks, int x, int y) {
    const int range = 40;
    SDL_Rect surroundingArea = { x - range, y - range, BLOCK_WIDTH + range * 2, BLOCK_HEIGHT + range * 2 };

    for (auto& block : blocks) {
        if (!block.destroyed && checkCollision(block.rect, surroundingArea)) {
            block.destroyed = true;
        }
    }
}

// 게임 초기화
void initializeGame(std::vector<Block>& blocks) {
    blocks.clear(); // 기존 블록 초기화

    // 난수 생성기 초기화
    std::srand(std::time(nullptr));

    // 블록 생성
    for (int i = 0; i < NUM_BLOCKS_X; ++i) {
        for (int j = 0; j < NUM_BLOCKS_Y; ++j) {
            // 블록의 생성 확률 설정 (70%는 일반, 20%는 강한, 10%는 특수)
            int blockType = std::rand() % 100 + 1; // 1부터 100까지의 난수 생성
            if (blockType <= 70) {
                blocks.emplace_back(i * (BLOCK_WIDTH + 5) + 50, j * (BLOCK_HEIGHT + 5) + 50, false);
            }
            else if (blockType <= 90) {
                createStrongBlock(blocks, i * (BLOCK_WIDTH + 5) + 50, j * (BLOCK_HEIGHT + 5) + 50);
            }
            else {
                createSpecialBlock(blocks, i * (BLOCK_WIDTH + 5) + 50, j * (BLOCK_HEIGHT + 5) + 50);
            }
        }
    }
}
void levelstateMenu() {
    bool quit = false;
    SDL_Event e;
    int selectedOption = 0; // 선택된 옵션 (0: Hard, 1: Normal, 2: Easy, 3: Exit)

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
                startstate = -1; // 프로그램 종료
            }
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_UP:
                    selectedOption = (selectedOption - 1 + 4) % 4; // 위 방향키로 선택지 변경
                    break;
                case SDLK_DOWN:
                    selectedOption = (selectedOption + 1) % 4; // 아래 방향키로 선택지 변경
                    break;
                case SDLK_RETURN:
                    if (selectedOption == 3) {
                        quit = true; // 종료 옵션 선택 시 메뉴 종료
                    }
                    else {
                        // 선택된 난이도에 따라 levelstate 변경
                        switch (selectedOption) {
                        case 0:
                            levelstate = 3; // Hard
                            break;
                        case 1:
                            levelstate = 2; // Normal
                            break;
                        case 2:
                            levelstate = 1; // Easy
                            break;
                        }
                    }
                    break;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 128, 0, 128, 255); // 배경색 보라색
        SDL_RenderClear(renderer);

        // 현재 난이도 표시
        renderText("Current Difficulty:", SCREEN_WIDTH / 2 - 100, 100, { 255, 255, 255, 255 });
        std::string currentDifficulty;
        switch (levelstate) {
        case 3:
            currentDifficulty = "Hard";
            break;
        case 2:
            currentDifficulty = "Normal";
            break;
        case 1:
            currentDifficulty = "Easy";
            break;
        }
        renderText(currentDifficulty.c_str(), SCREEN_WIDTH / 2 - 30, 150, { 255, 255, 255, 255 });

        // 각 난이도 옵션 표시
        renderText("Hard", SCREEN_WIDTH / 2 - 30, 250, selectedOption == 0 ? SDL_Color{ 255, 0, 0, 255 } : SDL_Color{ 255, 255, 255, 255 });
        renderText("Normal", SCREEN_WIDTH / 2 - 30, 300, selectedOption == 1 ? SDL_Color{ 255, 0, 0, 255 } : SDL_Color{ 255, 255, 255, 255 });
        renderText("Easy", SCREEN_WIDTH / 2 - 30, 350, selectedOption == 2 ? SDL_Color{ 255, 0, 0, 255 } : SDL_Color{ 255, 255, 255, 255 });
        renderText("Exit", SCREEN_WIDTH / 2 - 30, 400, selectedOption == 3 ? SDL_Color{ 255, 0, 0, 255 } : SDL_Color{ 255, 255, 255, 255 });

        SDL_RenderPresent(renderer);
    }

    startstate = 0; // levelstateMenu 종료 후 메뉴로 돌아가기
}

// 게임 오버 화면 함수
void gameover(int score) {
    bool quit = false;
    SDL_Event e;
    int selectedOption = 0; // 선택된 옵션 (0: 다시 시도, 1: 종료)

    // 배경 이미지 표시를 위한 표면(surface)
    SDL_Surface* backgroundSurface = nullptr;

    if (score >= 40) {
        backgroundSurface = IMG_Load("gameclear_bg.png");

    }
    else {
        backgroundSurface = IMG_Load("gameover_bg.jpg");
    }
   
    if (!backgroundSurface) {
        // 이미지 로드 실패 처리
        // 예: std::cerr << "Unable to load background image! SDL_image Error: " << IMG_GetError() << std::endl;
    }
    else {
        // 배경 이미지 표시를 위한 텍스처 생성
        SDL_Texture* backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
        if (!backgroundTexture) {
            // 텍스처 생성 실패 처리
            // 예: std::cerr << "Unable to create texture from surface! SDL Error: " << SDL_GetError() << std::endl;
        }
        else {
            // 배경 이미지 표시
            SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
            // 텍스처 해제
            SDL_DestroyTexture(backgroundTexture);
        }
        // 표면 해제
        SDL_FreeSurface(backgroundSurface);
    }
    if (score > highscore) {
        highscore = score;
    }


    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
                startstate = -1; // 프로그램 종료
            }
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_UP:
                    selectedOption = (selectedOption - 1 + 2) % 2; // 위 방향키로 선택지 변경
                    break;
                case SDLK_DOWN:
                    selectedOption = (selectedOption + 1) % 2; // 아래 방향키로 선택지 변경
                    break;
                case SDLK_RETURN:
                    if (selectedOption == 0) {
                        void gameStart(); // 다시 시도
                        quit = true;
                    }
                    else if (selectedOption == 1) {
                        startstate = -1; // 프로그램 종료
                        quit = true;
                    }
                    break;
                }
            }
        }

        // 점수 텍스트
        renderTextCentered("High Score: " + std::to_string(highscore), 100, { 255, 255, 255, 255 });
        renderTextCentered("Score: " + std::to_string(score), 200, { 255, 255, 255, 255 });

        // 다시 시도 텍스트
        SDL_Color retryColor = { 255, 255, 255, 255 }; // 기본 색상 흰색
        if (selectedOption == 0) {
            retryColor = { 255, 0, 0, 255 }; // 선택된 항목 빨간색
        }
        renderTextCentered("Retry", 400, retryColor);

        // 종료 텍스트
        SDL_Color quitColor = { 255, 255, 255, 255 }; // 기본 색상 흰색
        if (selectedOption == 1) {
            quitColor = { 255, 0, 0, 255 }; // 선택된 항목 빨간색
        }
        renderTextCentered("Quit", 500, quitColor);

        SDL_RenderPresent(renderer);
    }
}
// 게임 시작 화면 함수
void gameStart() {
    Paddle paddle(SCREEN_WIDTH / 2 - PADDLE_WIDTH / 2, SCREEN_HEIGHT - 50);
    Ball ball(SCREEN_WIDTH / 2 - BALL_SIZE / 2, SCREEN_HEIGHT / 2 - BALL_SIZE / 2);
    setBallSpeed(ball, levelstate);
    bool paused = false;

    // 블록 객체 생성
    std::vector<Block> blocks;
    initializeGame(blocks);

    int lives = 5  ; // 남은 목숨
    int score = 0; // 점수
    bool quit = false; // 게임 루프 제어 변수
    SDL_Event e; // 이벤트 처리 변수

    // 게임 루프 시작
    while (!quit && lives > 0) { // 목숨이 0이 되면 게임 종료
        // 이벤트 처리
        while (SDL_PollEvent(&e) != 0) {
            // 모든 블록이 파괴되었는지 확인
            bool allBlocksDestroyed = true;
            for (const auto& block : blocks) {
                if (!block.destroyed) {

                    allBlocksDestroyed = false;
                    break;
                }
            }
           
            // 모든 블록이 파괴되었을 때 게임 클리어 처리
            if (allBlocksDestroyed) {
                std::cout << allBlocksDestroyed;
                gameover(score);
                quit = true; // 게임 클리어 상태로 설정하여 게임 루프를 종료
                
            }
            if (e.type == SDL_QUIT) { // 창 닫기 버튼을 눌렀을 때
                quit = true;
            }
            else if (e.type == SDL_KEYDOWN) { // 키가 눌렸을 때
                switch (e.key.keysym.sym) {
                case SDLK_LEFT: // 왼쪽 화살표 키
                    paddle.moveLeft();
                    break;
                case SDLK_RIGHT: // 오른쪽 화살표 키
                    paddle.moveRight();
                    break;
                }
            }
        }

        // 공 이동
        
        ball.move();
        

        // 공과 패들의 충돌 확인
        if (checkCollision(ball.rect, paddle.rect)) {
            ball.yVel = -ball.yVel;
        }

        // 공과 블록의 충돌 확인
        for (auto& block : blocks) {
            if (!block.destroyed && checkCollision(ball.rect, block.rect)) {
                if (block.strength == 1) { // 강도가 1인 블록 (일반 블록 및 특수 블록)
                    block.destroyed = true;
                    if (block.special) { // 특수 블록인 경우, 주변 블록도 파괴
                        destroySurroundingBlocks(blocks, block.rect.x, block.rect.y);
                    }
                }
                else { // 강도가 2 이상인 블록 (강한 블록)
                    block.decreaseStrength(); // 강도 감소
                    // 색깔 연하게 변경
                    SDL_SetRenderDrawColor(renderer, block.color.r, block.color.g, block.color.b, 255);
                }
                ball.yVel = -ball.yVel;
                score++; // 블록이 깨졌을 때 점수 증가
                break;
            }
        }

        // 공이 화면 하단에 닿았을 때 처리
        if (ball.rect.y + ball.rect.h >= SCREEN_HEIGHT) {
           


            setBallSpeed(ball, levelstate);
            lives--;
            if (lives <= 0) {
                quit = true; // 목숨이 없으면 게임 종료
            }
        }

        // 화면을 검은색으로 채움
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // 패들, 공, 블록 렌더링
        paddle.render(renderer);
        ball.render(renderer);
        for (const auto& block : blocks) {
            block.render(renderer);
        }

        // 점수 표시
        renderText("Score: " + std::to_string(score), 10, { 255, 255, 255, 255 }, 24);

        // 렌더링 갱신
        SDL_RenderPresent(renderer);

        SDL_Delay(16); // 프레임 제한 (약 60fps)
    }
    
    if (lives <= 0) {
        gameover(score);
    }
}




// 게임 시작 메뉴 화면을 보여주는 함수
void showMenu() {
    bool quit = false;
    SDL_Event e;
    int selectedOption = 0; // 선택된 옵션 (0: 게임 시작, 1: 난이도 설정)

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
                startstate = -1; // 프로그램 종료
            }
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_UP:
                    selectedOption = (selectedOption - 1 + 2) % 2; // 위 방향키로 선택지 변경
                    break;
                case SDLK_DOWN:
                    selectedOption = (selectedOption + 1) % 2; // 아래 방향키로 선택지 변경
                    break;
                case SDLK_RETURN:
                    if (selectedOption == 0) {
                        startstate = 1; // 게임 시작
                        quit = true;
                    }
                    else if (selectedOption == 1) {
                        startstate = 2;
                        quit = true;
                        // 난이도 설정을 여기에 추가할 수 있습니다.
                        // 예: levelstate를 조절하는 코드
                    }
                    break;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 128, 0, 128, 255); // 배경색 보라색
        SDL_RenderClear(renderer);

        // 배경 이미지 렌더링
        renderBackground("background_image.jpg");

        // 타이틀 텍스트
        renderText("Block-breaking game", SCREEN_WIDTH / 2 - 100, 100, { 255, 255, 255, 255 });

        // 게임 시작 텍스트
        SDL_Color startColor = { 255, 255, 255, 255 }; // 기본 색상 흰색
        if (selectedOption == 0) {
            startColor = { 255, 0, 0, 255 }; // 선택된 항목 빨간색
        }
        renderText("Game start", SCREEN_WIDTH / 2 - 50, 300, startColor);

        // 난이도 설정 텍스트
        SDL_Color settingsColor = { 255, 255, 255, 255 }; // 기본 색상 흰색
        if (selectedOption == 1) {
            settingsColor = { 255, 0, 0, 255 }; // 선택된 항목 빨간색
        }
        renderText("Level Settings", SCREEN_WIDTH / 2 - 50, 400, settingsColor);

        SDL_RenderPresent(renderer);
    }
}





int main(int argc, char* args[]) {
    // SDL 초기화
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL 초기화 실패! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    // TTF 초기화
    if (TTF_Init() == -1) {
        std::cerr << "TTF 초기화 실패! TTF_Error: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // 폰트 로드
    font = TTF_OpenFont(u8"Roboto/Roboto-Black.ttf", 24);
    if (font == NULL) {
        std::cerr << "폰트 로드 실패! TTF_Error: " << TTF_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    // 윈도우 생성
    window = SDL_CreateWindow("SDL 블록 깨기 게임", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        std::cerr << "윈도우 생성 실패! SDL_Error: " << SDL_GetError() << std::endl;
        TTF_CloseFont(font);
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    // 렌더러 생성
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        std::cerr << "렌더러 생성 실패! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        TTF_CloseFont(font);
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    // 메인 루프: 게임 시작 화면과 게임 화면을 전환
    while (startstate != -1) {
        if (startstate == 0) {
            showMenu();
            if (startstate == 2) {
                levelstateMenu();
            }
        }
        else if (startstate == 1) {
            gameStart();
            
           
            startstate = 0; // 게임 종료 후 다시 메뉴로 돌아감
        }
    }
    // SDL 정리
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
