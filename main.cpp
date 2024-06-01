#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <string>
// ������ ũ�� ����
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int PADDLE_WIDTH = 100;
const int PADDLE_HEIGHT = 20;
const int BALL_SIZE = 10;
const int BLOCK_WIDTH = 60;
int BLOCK_HEIGHT = 20; // �� �ӵ� ����
const int NUM_BLOCKS_X = 11;
const int NUM_BLOCKS_Y = 4;
int startstate = 0; // ���� �޴� ����
int gameoverstate = 0;
int levelstate = 1; // ���̵� ����
int highscore = 0;  // �ְ� ����

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
TTF_Font* font = nullptr;

// �ؽ�Ʈ�� �������ϴ� �Լ�
void renderText(const std::string& message, int x, int y, SDL_Color color) {
    SDL_Surface* surface = TTF_RenderUTF8_Blended(font, message.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect dstrect = { x, y, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}
void renderText(const std::string& text, int y, SDL_Color color, int fontSize) {
    // ��Ʈ ũ�� ����
    TTF_Font* font = TTF_OpenFont("arial.ttf", fontSize);
    if (!font) {
        // ��Ʈ �ε� ���� ó��
        // ��: std::cerr << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << std::endl;
        return;
    }

    // �ؽ�Ʈ ǥ�ø� ���� ǥ��(surface) ����
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (!surface) {
        // ǥ�� ���� ���� ó��
        // ��: std::cerr << "Unable to render text surface! SDL_ttf Error: " << TTF_GetError() << std::endl;
        TTF_CloseFont(font); // ��Ʈ �ݱ�
        return;
    }

    // ǥ���� �ؽ�ó�� ��ȯ
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        // �ؽ�ó ���� ���� ó��
        // ��: std::cerr << "Unable to create texture from rendered text! SDL Error: " << SDL_GetError() << std::endl;
    }
    else {
        // �ؽ�Ʈ�� ���� ���� ���� ���
        int textWidth = surface->w;
        int textHeight = surface->h;

        // �ؽ�Ʈ�� ȭ�� �߾ӿ� ��ġ�ϱ� ���� x ��ǥ ���
        int x = (SCREEN_WIDTH - textWidth) / 2;

        // �ؽ�Ʈ ǥ��
        SDL_Rect renderQuad = { x, y, textWidth, textHeight };
        SDL_RenderCopy(renderer, texture, NULL, &renderQuad);
        // �ؽ�ó ����
        SDL_DestroyTexture(texture);
    }

    // ǥ�� ����
    SDL_FreeSurface(surface);
    // ��Ʈ �ݱ�
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

    // �ؽ�Ʈ ǥ���� �ʺ�� ���� ��������
    int textWidth = textSurface->w;
    int textHeight = textSurface->h;

    // �ؽ�Ʈ�� ��� �����ϱ� ���� x ��ǥ ���
    int x = (SCREEN_WIDTH - textWidth) / 2;

    // �ؽ�Ʈ ǥ��� �ؽ�Ʈ �ؽ�ó ����
    SDL_FreeSurface(textSurface);

    // �ؽ�Ʈ �ؽ�ó ������
    SDL_Rect renderRect = { x, y, textWidth, textHeight };
    SDL_RenderCopy(renderer, textTexture, nullptr, &renderRect);

    // �ؽ�Ʈ �ؽ�ó ����
    SDL_DestroyTexture(textTexture);
}

// ��� �̹����� �������ϴ� �Լ�
void renderBackground(const char* filePath) {
    SDL_Surface* surface = IMG_Load(filePath);
    if (!surface) {
        std::cerr << "�̹��� �ε� ����! IMG_Error: " << IMG_GetError() << std::endl;
        return;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    SDL_Rect dstrect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
    SDL_RenderCopy(renderer, texture, NULL, &dstrect);

    SDL_DestroyTexture(texture);
}

// �е� Ŭ���� ����
class Paddle {
public:
    SDL_Rect rect; // �е��� �簢��
    int speed; // �е��� �̵� �ӵ�

    // ������: �е��� �ʱ� ��ġ�� ũ�� ����
    Paddle(int x, int y) : speed(25) {
        rect = { x, y, PADDLE_WIDTH, PADDLE_HEIGHT };
    }

    // �е��� �������� �̵�
    void moveLeft() {
        if (rect.x > 0) {
            rect.x -= speed;
        }
    }

    // �е��� ���������� �̵�
    void moveRight() {
        if (rect.x + rect.w < SCREEN_WIDTH) {
            rect.x += speed;
        }
    }

    // �е��� �������� �׸���
    void render(SDL_Renderer* renderer) const {
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // �е� ���� �Ķ���
        SDL_RenderFillRect(renderer, &rect); // �е� �׸���
    }
};

// �� Ŭ���� ����
class Ball {
public:
    SDL_Rect rect; // ���� ��ġ�� ũ�⸦ �����ϴ� SDL_Rect ����ü
    int xVel, yVel; // ���� �ӵ�

    // �� ������, ���� �ʱ� ��ġ�� �ӵ� ����
    Ball(int x, int y) : xVel(3), yVel(-3) {
        rect = { x, y, BALL_SIZE, BALL_SIZE };
    }

    // ���� �̵���Ű�� �Լ�
    void move() {
        
        rect.x += xVel;
        rect.y += yVel;

        // ���� �¿� ���� ����� �� �ӵ� ����
        if (rect.x <= 0 || rect.x + rect.w >= SCREEN_WIDTH) {
            xVel = -xVel;
        }
        // ���� ȭ���� õ�忡 ����� �� �ӵ� ����
        if (rect.y <= 0) {
            yVel = -yVel;
        }
    }

    // ���� �������� �׸��� �Լ�
    void render(SDL_Renderer* renderer) const {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // �� ���� �����
        SDL_RenderFillRect(renderer, &rect);
    }

    // ���� ��ġ�� �ӵ��� �ʱ�ȭ�ϴ� �Լ�
    void reset(int x, int y) {
        rect.x = x;
        rect.y = y;
        xVel = 3;
        yVel = -3;
    }
};
void setBallSpeed(Ball& ball, int level) {//�� ���̵��� �°� �ӵ�����
    switch (level) {
    case 1: // ������
        ball.xVel = 3;
        ball.yVel = -3;
        break;
    case 2: // ������
        ball.xVel = 4;
        ball.yVel = -4;
        break;
    case 3: // ���� ������
        ball.xVel = 6;
        ball.yVel = -6;
        break;
    default:
        ball.xVel = 3; // �⺻��
        ball.yVel = -3;
        break;
    }
}


// ��� Ŭ���� ����
class Block {
public:
    SDL_Rect rect;  // ����� ��ġ�� ũ�⸦ �����ϴ� SDL_Rect ����ü
    bool destroyed; // ����� �ı��Ǿ����� ����
    bool special;   // Ư�� ��� ����
    int strength;   // ���� ����� ���� (�� �� �¾ƾ� ��������)
    SDL_Color color; // ����� ����

    // ��� ������, �ʱ� ��ġ ���� �� �ı� ���� �ʱ�ȭ
    Block(int x, int y, bool isSpecial, int blockStrength = 1) : destroyed(false), special(isSpecial), strength(blockStrength) {
        rect = { x, y, BLOCK_WIDTH, BLOCK_HEIGHT };
        // ��� ���� ����
        if (special) {
            color = { 255, 0, 255, 255 }; // Ư�� ����� ���� (�����)
        }
        else if (blockStrength > 1) {
            color = { 0, 0, 255, 255 }; // ���� ����� ���� (�Ķ���)
        }
        else {
            color = { 0, 255, 0, 255 }; // �Ϲ� ����� ���� (���)
        }
    }

    // ����� �������� �׸��� �Լ�
    void render(SDL_Renderer* renderer) const {
        if (!destroyed) {  // ����� �ı����� �ʾ�����
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a); // ����� ���� ����
            SDL_RenderFillRect(renderer, &rect); // ��� �׸���
        }
    }
    // ���� ����� ������ ���ҽ�Ű�� ������ ���ϰ� ����� �Լ�
    void decreaseStrength() {
        if (strength > 1) {
            strength--;
            // ������ ���� ������ ���ϰ� ����ϴ�.
            color.r /= 2;
            color.g /= 2;
            color.b /= 2;
        }
    }
};

// �� SDL_Rect ��ü�� �浹�ϴ��� Ȯ���ϴ� �Լ�
bool checkCollision(const SDL_Rect& a, const SDL_Rect& b) {
    return SDL_HasIntersection(&a, &b);
}

// Ư�� ��� ����
void createSpecialBlock(std::vector<Block>& blocks, int x, int y) {
    blocks.emplace_back(x, y, true);
}

// ���� ��� ����
void createStrongBlock(std::vector<Block>& blocks, int x, int y) {
    blocks.emplace_back(x, y, false, 3); // ������ 3���� �����Ͽ� 3�� �¾ƾ� �������� ��
}

// �ֺ� ��� �ı�
void destroySurroundingBlocks(std::vector<Block>& blocks, int x, int y) {
    const int range = 40;
    SDL_Rect surroundingArea = { x - range, y - range, BLOCK_WIDTH + range * 2, BLOCK_HEIGHT + range * 2 };

    for (auto& block : blocks) {
        if (!block.destroyed && checkCollision(block.rect, surroundingArea)) {
            block.destroyed = true;
        }
    }
}

// ���� �ʱ�ȭ
void initializeGame(std::vector<Block>& blocks) {
    blocks.clear(); // ���� ��� �ʱ�ȭ

    // ���� ������ �ʱ�ȭ
    std::srand(std::time(nullptr));

    // ��� ����
    for (int i = 0; i < NUM_BLOCKS_X; ++i) {
        for (int j = 0; j < NUM_BLOCKS_Y; ++j) {
            // ����� ���� Ȯ�� ���� (70%�� �Ϲ�, 20%�� ����, 10%�� Ư��)
            int blockType = std::rand() % 100 + 1; // 1���� 100������ ���� ����
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
    int selectedOption = 0; // ���õ� �ɼ� (0: Hard, 1: Normal, 2: Easy, 3: Exit)

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
                startstate = -1; // ���α׷� ����
            }
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_UP:
                    selectedOption = (selectedOption - 1 + 4) % 4; // �� ����Ű�� ������ ����
                    break;
                case SDLK_DOWN:
                    selectedOption = (selectedOption + 1) % 4; // �Ʒ� ����Ű�� ������ ����
                    break;
                case SDLK_RETURN:
                    if (selectedOption == 3) {
                        quit = true; // ���� �ɼ� ���� �� �޴� ����
                    }
                    else {
                        // ���õ� ���̵��� ���� levelstate ����
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

        SDL_SetRenderDrawColor(renderer, 128, 0, 128, 255); // ���� �����
        SDL_RenderClear(renderer);

        // ���� ���̵� ǥ��
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

        // �� ���̵� �ɼ� ǥ��
        renderText("Hard", SCREEN_WIDTH / 2 - 30, 250, selectedOption == 0 ? SDL_Color{ 255, 0, 0, 255 } : SDL_Color{ 255, 255, 255, 255 });
        renderText("Normal", SCREEN_WIDTH / 2 - 30, 300, selectedOption == 1 ? SDL_Color{ 255, 0, 0, 255 } : SDL_Color{ 255, 255, 255, 255 });
        renderText("Easy", SCREEN_WIDTH / 2 - 30, 350, selectedOption == 2 ? SDL_Color{ 255, 0, 0, 255 } : SDL_Color{ 255, 255, 255, 255 });
        renderText("Exit", SCREEN_WIDTH / 2 - 30, 400, selectedOption == 3 ? SDL_Color{ 255, 0, 0, 255 } : SDL_Color{ 255, 255, 255, 255 });

        SDL_RenderPresent(renderer);
    }

    startstate = 0; // levelstateMenu ���� �� �޴��� ���ư���
}

// ���� ���� ȭ�� �Լ�
void gameover(int score) {
    bool quit = false;
    SDL_Event e;
    int selectedOption = 0; // ���õ� �ɼ� (0: �ٽ� �õ�, 1: ����)

    // ��� �̹��� ǥ�ø� ���� ǥ��(surface)
    SDL_Surface* backgroundSurface = nullptr;

    if (score >= 40) {
        backgroundSurface = IMG_Load("gameclear_bg.png");

    }
    else {
        backgroundSurface = IMG_Load("gameover_bg.jpg");
    }
   
    if (!backgroundSurface) {
        // �̹��� �ε� ���� ó��
        // ��: std::cerr << "Unable to load background image! SDL_image Error: " << IMG_GetError() << std::endl;
    }
    else {
        // ��� �̹��� ǥ�ø� ���� �ؽ�ó ����
        SDL_Texture* backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
        if (!backgroundTexture) {
            // �ؽ�ó ���� ���� ó��
            // ��: std::cerr << "Unable to create texture from surface! SDL Error: " << SDL_GetError() << std::endl;
        }
        else {
            // ��� �̹��� ǥ��
            SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
            // �ؽ�ó ����
            SDL_DestroyTexture(backgroundTexture);
        }
        // ǥ�� ����
        SDL_FreeSurface(backgroundSurface);
    }
    if (score > highscore) {
        highscore = score;
    }


    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
                startstate = -1; // ���α׷� ����
            }
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_UP:
                    selectedOption = (selectedOption - 1 + 2) % 2; // �� ����Ű�� ������ ����
                    break;
                case SDLK_DOWN:
                    selectedOption = (selectedOption + 1) % 2; // �Ʒ� ����Ű�� ������ ����
                    break;
                case SDLK_RETURN:
                    if (selectedOption == 0) {
                        void gameStart(); // �ٽ� �õ�
                        quit = true;
                    }
                    else if (selectedOption == 1) {
                        startstate = -1; // ���α׷� ����
                        quit = true;
                    }
                    break;
                }
            }
        }

        // ���� �ؽ�Ʈ
        renderTextCentered("High Score: " + std::to_string(highscore), 100, { 255, 255, 255, 255 });
        renderTextCentered("Score: " + std::to_string(score), 200, { 255, 255, 255, 255 });

        // �ٽ� �õ� �ؽ�Ʈ
        SDL_Color retryColor = { 255, 255, 255, 255 }; // �⺻ ���� ���
        if (selectedOption == 0) {
            retryColor = { 255, 0, 0, 255 }; // ���õ� �׸� ������
        }
        renderTextCentered("Retry", 400, retryColor);

        // ���� �ؽ�Ʈ
        SDL_Color quitColor = { 255, 255, 255, 255 }; // �⺻ ���� ���
        if (selectedOption == 1) {
            quitColor = { 255, 0, 0, 255 }; // ���õ� �׸� ������
        }
        renderTextCentered("Quit", 500, quitColor);

        SDL_RenderPresent(renderer);
    }
}
// ���� ���� ȭ�� �Լ�
void gameStart() {
    Paddle paddle(SCREEN_WIDTH / 2 - PADDLE_WIDTH / 2, SCREEN_HEIGHT - 50);
    Ball ball(SCREEN_WIDTH / 2 - BALL_SIZE / 2, SCREEN_HEIGHT / 2 - BALL_SIZE / 2);
    setBallSpeed(ball, levelstate);
    bool paused = false;

    // ��� ��ü ����
    std::vector<Block> blocks;
    initializeGame(blocks);

    int lives = 5  ; // ���� ���
    int score = 0; // ����
    bool quit = false; // ���� ���� ���� ����
    SDL_Event e; // �̺�Ʈ ó�� ����

    // ���� ���� ����
    while (!quit && lives > 0) { // ����� 0�� �Ǹ� ���� ����
        // �̺�Ʈ ó��
        while (SDL_PollEvent(&e) != 0) {
            // ��� ����� �ı��Ǿ����� Ȯ��
            bool allBlocksDestroyed = true;
            for (const auto& block : blocks) {
                if (!block.destroyed) {

                    allBlocksDestroyed = false;
                    break;
                }
            }
           
            // ��� ����� �ı��Ǿ��� �� ���� Ŭ���� ó��
            if (allBlocksDestroyed) {
                std::cout << allBlocksDestroyed;
                gameover(score);
                quit = true; // ���� Ŭ���� ���·� �����Ͽ� ���� ������ ����
                
            }
            if (e.type == SDL_QUIT) { // â �ݱ� ��ư�� ������ ��
                quit = true;
            }
            else if (e.type == SDL_KEYDOWN) { // Ű�� ������ ��
                switch (e.key.keysym.sym) {
                case SDLK_LEFT: // ���� ȭ��ǥ Ű
                    paddle.moveLeft();
                    break;
                case SDLK_RIGHT: // ������ ȭ��ǥ Ű
                    paddle.moveRight();
                    break;
                }
            }
        }

        // �� �̵�
        
        ball.move();
        

        // ���� �е��� �浹 Ȯ��
        if (checkCollision(ball.rect, paddle.rect)) {
            ball.yVel = -ball.yVel;
        }

        // ���� ����� �浹 Ȯ��
        for (auto& block : blocks) {
            if (!block.destroyed && checkCollision(ball.rect, block.rect)) {
                if (block.strength == 1) { // ������ 1�� ��� (�Ϲ� ��� �� Ư�� ���)
                    block.destroyed = true;
                    if (block.special) { // Ư�� ����� ���, �ֺ� ��ϵ� �ı�
                        destroySurroundingBlocks(blocks, block.rect.x, block.rect.y);
                    }
                }
                else { // ������ 2 �̻��� ��� (���� ���)
                    block.decreaseStrength(); // ���� ����
                    // ���� ���ϰ� ����
                    SDL_SetRenderDrawColor(renderer, block.color.r, block.color.g, block.color.b, 255);
                }
                ball.yVel = -ball.yVel;
                score++; // ����� ������ �� ���� ����
                break;
            }
        }

        // ���� ȭ�� �ϴܿ� ����� �� ó��
        if (ball.rect.y + ball.rect.h >= SCREEN_HEIGHT) {
           


            setBallSpeed(ball, levelstate);
            lives--;
            if (lives <= 0) {
                quit = true; // ����� ������ ���� ����
            }
        }

        // ȭ���� ���������� ä��
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // �е�, ��, ��� ������
        paddle.render(renderer);
        ball.render(renderer);
        for (const auto& block : blocks) {
            block.render(renderer);
        }

        // ���� ǥ��
        renderText("Score: " + std::to_string(score), 10, { 255, 255, 255, 255 }, 24);

        // ������ ����
        SDL_RenderPresent(renderer);

        SDL_Delay(16); // ������ ���� (�� 60fps)
    }
    
    if (lives <= 0) {
        gameover(score);
    }
}




// ���� ���� �޴� ȭ���� �����ִ� �Լ�
void showMenu() {
    bool quit = false;
    SDL_Event e;
    int selectedOption = 0; // ���õ� �ɼ� (0: ���� ����, 1: ���̵� ����)

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
                startstate = -1; // ���α׷� ����
            }
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                case SDLK_UP:
                    selectedOption = (selectedOption - 1 + 2) % 2; // �� ����Ű�� ������ ����
                    break;
                case SDLK_DOWN:
                    selectedOption = (selectedOption + 1) % 2; // �Ʒ� ����Ű�� ������ ����
                    break;
                case SDLK_RETURN:
                    if (selectedOption == 0) {
                        startstate = 1; // ���� ����
                        quit = true;
                    }
                    else if (selectedOption == 1) {
                        startstate = 2;
                        quit = true;
                        // ���̵� ������ ���⿡ �߰��� �� �ֽ��ϴ�.
                        // ��: levelstate�� �����ϴ� �ڵ�
                    }
                    break;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 128, 0, 128, 255); // ���� �����
        SDL_RenderClear(renderer);

        // ��� �̹��� ������
        renderBackground("background_image.jpg");

        // Ÿ��Ʋ �ؽ�Ʈ
        renderText("Block-breaking game", SCREEN_WIDTH / 2 - 100, 100, { 255, 255, 255, 255 });

        // ���� ���� �ؽ�Ʈ
        SDL_Color startColor = { 255, 255, 255, 255 }; // �⺻ ���� ���
        if (selectedOption == 0) {
            startColor = { 255, 0, 0, 255 }; // ���õ� �׸� ������
        }
        renderText("Game start", SCREEN_WIDTH / 2 - 50, 300, startColor);

        // ���̵� ���� �ؽ�Ʈ
        SDL_Color settingsColor = { 255, 255, 255, 255 }; // �⺻ ���� ���
        if (selectedOption == 1) {
            settingsColor = { 255, 0, 0, 255 }; // ���õ� �׸� ������
        }
        renderText("Level Settings", SCREEN_WIDTH / 2 - 50, 400, settingsColor);

        SDL_RenderPresent(renderer);
    }
}





int main(int argc, char* args[]) {
    // SDL �ʱ�ȭ
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL �ʱ�ȭ ����! SDL_Error: " << SDL_GetError() << std::endl;
        return -1;
    }

    // TTF �ʱ�ȭ
    if (TTF_Init() == -1) {
        std::cerr << "TTF �ʱ�ȭ ����! TTF_Error: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    // ��Ʈ �ε�
    font = TTF_OpenFont(u8"Roboto/Roboto-Black.ttf", 24);
    if (font == NULL) {
        std::cerr << "��Ʈ �ε� ����! TTF_Error: " << TTF_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    // ������ ����
    window = SDL_CreateWindow("SDL ��� ���� ����", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        std::cerr << "������ ���� ����! SDL_Error: " << SDL_GetError() << std::endl;
        TTF_CloseFont(font);
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    // ������ ����
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        std::cerr << "������ ���� ����! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        TTF_CloseFont(font);
        TTF_Quit();
        SDL_Quit();
        return -1;
    }

    // ���� ����: ���� ���� ȭ��� ���� ȭ���� ��ȯ
    while (startstate != -1) {
        if (startstate == 0) {
            showMenu();
            if (startstate == 2) {
                levelstateMenu();
            }
        }
        else if (startstate == 1) {
            gameStart();
            
           
            startstate = 0; // ���� ���� �� �ٽ� �޴��� ���ư�
        }
    }
    // SDL ����
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
