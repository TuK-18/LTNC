#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <SDL_mixer.h>
#include <vector>

using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* background1 = nullptr;
SDL_Texture* background2 = nullptr;
SDL_Texture* bigText = nullptr;
SDL_Texture* smallText = nullptr;

SDL_Rect bigButton = { 150, 300, 200, 100 };
SDL_Rect smallButton = { 450, 300, 200, 100 };
SDL_Texture* diceTextures[6] = { nullptr }; // Mảng lưu ảnh xúc xắc

TTF_Font* font = nullptr;
TTF_Font* smallFont = nullptr;
Mix_Music* backgroundMusic = nullptr;  // Biến để lưu trữ nhạc nền

enum GameState { CHOOSE, RESULT };

GameState currentState = CHOOSE;

int maxTurns = 10;
int currentTurn = 5;
bool gameOver = false;
int remainingTurns = 5;
// Hàm khởi tạo SDL và SDL_mixer
bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;
    if (TTF_Init() == -1) return false;
    if (IMG_Init(IMG_INIT_PNG) == 0) return false;

    window = SDL_CreateWindow("Tai Xiu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) return false;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return false;

    font = TTF_OpenFont("calibri-bold-italic.ttf", 28);
    smallFont = TTF_OpenFont("calibri-bold-italic.ttf", 10);
    if (!font || !smallFont) return false;

    //// Khởi tạo SDL_mixer
    /*if (Mix_Init(MIX_INIT_MP3) == 0) {
        cerr << "Mix_Init failed: " << Mix_GetError() << endl;
        return false;
    }

    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
        cerr << "Mix_OpenAudio failed: " << Mix_GetError() << endl;
        return false;
    }*/

    //// Tải và phát nhạc nền
    /*backgroundMusic = Mix_LoadMUS("nhac-xo-so.mp3");
    if (backgroundMusic == nullptr) {
        cerr << "Failed to load background music! SDL_mixer Error: " << Mix_GetError() << endl;
        return false;
    }*/

    /*if (Mix_PlayMusic(backgroundMusic, -1) == -1) {
        cerr << "Mix_PlayMusic failed: " << Mix_GetError() << endl;
        return false;
    }*/

    SDL_Color white = { 255, 255, 255 };
    SDL_Surface* bigSurface = TTF_RenderText_Solid(font, "BIG", white);
    SDL_Surface* smallSurface = TTF_RenderText_Solid(font, "SMALL", white);

    bigText = SDL_CreateTextureFromSurface(renderer, bigSurface);
    smallText = SDL_CreateTextureFromSurface(renderer, smallSurface);

    SDL_FreeSurface(bigSurface);
    SDL_FreeSurface(smallSurface);

    return true;
}

void showGameRules() {
    SDL_Event e;
    bool proceed = false;
    SDL_Color white = { 0, 0, 0 };
    SDL_Color buttonColor = { 0, 200, 0 };
    SDL_Color hoverColor = { 0, 255, 0 };

    // Load background image
    SDL_Surface* bgSurface = IMG_Load("cobac.jpg");
    SDL_Texture* bgTexture = SDL_CreateTextureFromSurface(renderer, bgSurface);
    SDL_FreeSurface(bgSurface);

    std::string rules =
        "Game Rules:\n"
        "- You have a certain number of guesses (5 in the begin) \n"
        "- Guess if the sum of three dice is BIG (>10) or SMALL (<=10).\n"
        "- If you guess right, you win a turn and can guess more. Otherwise, you lose one.";

    SDL_Surface* ruleSurface = TTF_RenderText_Blended_Wrapped(font, rules.c_str(), white, SCREEN_WIDTH - 100);
    SDL_Texture* ruleTexture = SDL_CreateTextureFromSurface(renderer, ruleSurface);
    SDL_FreeSurface(ruleSurface);

    SDL_Rect ruleRect = { 50, 50, SCREEN_WIDTH - 100, 300 };

    SDL_Rect buttonRect = { SCREEN_WIDTH / 2 - 100, 400, 200, 60 };

    // Tạo texture cho chữ "PLAY"
    SDL_Surface* buttonSurface = TTF_RenderText_Blended(font, "PLAY", white);
    SDL_Texture* buttonTexture = SDL_CreateTextureFromSurface(renderer, buttonSurface);
    SDL_FreeSurface(buttonSurface);

    // Đo kích thước chữ
    int textW, textH;
    TTF_SizeText(font, "PLAY", &textW, &textH);
    SDL_Rect textRect = {
        buttonRect.x + (buttonRect.w - textW) / 2,
        buttonRect.y + (buttonRect.h - textH) / 2,
        textW,
        textH
    };

    while (!proceed) {
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        bool isHover = (mouseX >= buttonRect.x && mouseX <= buttonRect.x + buttonRect.w &&
            mouseY >= buttonRect.y && mouseY <= buttonRect.y + buttonRect.h);

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                exit(0);
            }
            if (e.type == SDL_MOUSEBUTTONDOWN && isHover) {
                proceed = true;
            }
        }

        // Vẽ nền
        SDL_RenderCopy(renderer, bgTexture, nullptr, nullptr);

        // Vẽ luật chơi
        SDL_RenderCopy(renderer, ruleTexture, nullptr, &ruleRect);

        // Vẽ nút
        SDL_SetRenderDrawColor(renderer, isHover ? hoverColor.r : buttonColor.r,
            isHover ? hoverColor.g : buttonColor.g,
            isHover ? hoverColor.b : buttonColor.b,
            255);
        SDL_RenderFillRect(renderer, &buttonRect);

        // Vẽ viền nút
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Viền đen
        SDL_RenderDrawRect(renderer, &buttonRect);

        // Vẽ chữ
        SDL_RenderCopy(renderer, buttonTexture, nullptr, &textRect);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(bgTexture);
    SDL_DestroyTexture(ruleTexture);
    SDL_DestroyTexture(buttonTexture);
}




// Hàm load ảnh
SDL_Texture* loadTexture(const string& path) {
    return IMG_LoadTexture(renderer, path.c_str());
}

// Hàm render text
SDL_Texture* renderText(const string& message, SDL_Color color, TTF_Font* usedFont) {
    SDL_Surface* surface = TTF_RenderText_Solid(usedFont, message.c_str(), color);
    if (!surface) return nullptr;
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}



// Hiển thị màn hình chọn Big/Small
// Hiển thị màn hình chọn Big/Small
void renderChooseScreen() {
    // Lấy vị trí chuột
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    // Hiển thị ảnh nền
    SDL_RenderCopy(renderer, background1, nullptr, nullptr);

    // Kiểm tra hover từng nút
    bool hoverBig = (mouseX >= bigButton.x && mouseX <= bigButton.x + bigButton.w &&
        mouseY >= bigButton.y && mouseY <= bigButton.y + bigButton.h);
    bool hoverSmall = (mouseX >= smallButton.x && mouseX <= smallButton.x + smallButton.w &&
        mouseY >= smallButton.y && mouseY <= smallButton.y + smallButton.h);

    // Tô màu nút BIG (xanh đậm -> xanh sáng khi hover)
    SDL_Color bigColor = hoverBig ? SDL_Color{ 0, 200, 0, 255 } : SDL_Color{ 0, 128, 0, 255 };
    SDL_SetRenderDrawColor(renderer, bigColor.r, bigColor.g, bigColor.b, bigColor.a);
    SDL_RenderFillRect(renderer, &bigButton);

    // Tô màu nút SMALL (đỏ đậm -> đỏ sáng khi hover)
    SDL_Color smallColor = hoverSmall ? SDL_Color{ 200, 0, 0, 255 } : SDL_Color{ 128, 0, 0, 255 };
    SDL_SetRenderDrawColor(renderer, smallColor.r, smallColor.g, smallColor.b, smallColor.a);
    SDL_RenderFillRect(renderer, &smallButton);

    // Vẽ viền nút cho đẹp
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // viền đen
    SDL_RenderDrawRect(renderer, &bigButton);
    SDL_RenderDrawRect(renderer, &smallButton);

    // Căn giữa chữ trong nút BIG
    int bigTextW = 0, bigTextH = 0;
    SDL_QueryTexture(bigText, nullptr, nullptr, &bigTextW, &bigTextH);
    SDL_Rect bigTextRect = {
        bigButton.x + (bigButton.w - bigTextW) / 2,
        bigButton.y + (bigButton.h - bigTextH) / 2,
        bigTextW, bigTextH
    };
    SDL_RenderCopy(renderer, bigText, nullptr, &bigTextRect);

    // Căn giữa chữ trong nút SMALL
    int smallTextW = 0, smallTextH = 0;
    SDL_QueryTexture(smallText, nullptr, nullptr, &smallTextW, &smallTextH);
    SDL_Rect smallTextRect = {
        smallButton.x + (smallButton.w - smallTextW) / 2,
        smallButton.y + (smallButton.h - smallTextH) / 2,
        smallTextW, smallTextH
    };
    SDL_RenderCopy(renderer, smallText, nullptr, &smallTextRect);
    // Vẽ số lượt còn lại
    SDL_Color yellow = { 255, 255, 0 };
    string turnsText = "Remaining Turns: " + to_string(currentTurn);
    SDL_Texture* turnsTexture = renderText(turnsText, yellow, font);

    int textW = 0, textH = 0;
    SDL_QueryTexture(turnsTexture, nullptr, nullptr, &textW, &textH);

    // Hiển thị ở góc trên bên phải
    SDL_Rect turnsRect = { SCREEN_WIDTH - textW - 20, 20, textW, textH };
    SDL_RenderCopy(renderer, turnsTexture, nullptr, &turnsRect);
    SDL_DestroyTexture(turnsTexture);

    SDL_RenderPresent(renderer);
}



// Hiển thị kết quả bằng ảnh xúc xắc
void renderResultScreen(int dice1, int dice2, int dice3, int sum, bool playerChoice) {
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, background2, nullptr, nullptr);

    SDL_Color white = { 255, 255, 255 };
    SDL_Rect diceRect = { 220, 100, 100, 100 };

    // Hiệu ứng xúc xắc xoay nhanh
    for (int i = 0; i < 10; ++i) {
        int temp1 = (rand() % 6);
        int temp2 = (rand() % 6);
        int temp3 = (rand() % 6);

        // Vẽ 3 viên xúc xắc tạm
        diceRect.x = 220;
        SDL_RenderCopy(renderer, diceTextures[temp1], nullptr, &diceRect);

        diceRect.x += 120;
        SDL_RenderCopy(renderer, diceTextures[temp2], nullptr, &diceRect);

        diceRect.x += 120;
        SDL_RenderCopy(renderer, diceTextures[temp3], nullptr, &diceRect);

        SDL_RenderPresent(renderer);
        SDL_Delay(50);  // Tạm dừng 70ms cho hiệu ứng
        //SDL_RenderClear(renderer); // Xóa màn hình trước khi vẽ lại
    }

    // Hiển thị kết quả thật
    diceRect = { 220, 100, 100, 100 };
    SDL_RenderCopy(renderer, diceTextures[dice1 - 1], nullptr, &diceRect);
    SDL_RenderPresent(renderer);
    this_thread::sleep_for(chrono::milliseconds(500));

    diceRect.x += 120;
    SDL_RenderCopy(renderer, diceTextures[dice2 - 1], nullptr, &diceRect);
    SDL_RenderPresent(renderer);
    this_thread::sleep_for(chrono::milliseconds(500));

    diceRect.x += 120;
    SDL_RenderCopy(renderer, diceTextures[dice3 - 1], nullptr, &diceRect);
    SDL_RenderPresent(renderer);
    this_thread::sleep_for(chrono::milliseconds(500));

    // Hiển thị kết quả
    bool actualResult = sum > 10;
    string resultText = "Total: " + to_string(sum) + " (" + (actualResult ? "Tai" : "Xiu") + ")";
    resultText += (playerChoice == actualResult) ? " - You WIN!" : " - You LOSE!";
    SDL_Texture* resultTexture = renderText(resultText, white, font);

    SDL_Rect resultRect = { 250, 400, 300, 50 };
    SDL_RenderCopy(renderer, resultTexture, nullptr, &resultRect);
    SDL_DestroyTexture(resultTexture);

    SDL_RenderPresent(renderer);
    this_thread::sleep_for(chrono::seconds(2));

    currentState = CHOOSE;  // Quay lại màn hình chọn
}

int score = 0;
int winCount = 0;
int loseCount = 0;
int bigCount = 0;
int smallCount = 0;

// Hàm render bảng điểm
void renderScoreboard() {
    SDL_Color white = { 255, 255, 255 };
    SDL_Color bgColor = { 0, 0, 0, 150 }; // Màu nền mờ

    std::vector<std::string> lines = {
        "Score: " + std::to_string(score),
        "Wins: " + std::to_string(winCount),
        "Losses: " + std::to_string(loseCount),
        "Big: " + std::to_string(bigCount),
        "Small: " + std::to_string(smallCount)
    };

    int x = 20, y = 20;
    int lineHeight = 30;
    int width = 200;
    int height = lineHeight * lines.size() + 20;

    // Vẽ nền scoreboard
    SDL_Rect bgRect = { x - 10, y - 10, width, height };
    SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderFillRect(renderer, &bgRect);

    // Vẽ từng dòng text
    for (size_t i = 0; i < lines.size(); ++i) {
        SDL_Texture* lineTexture = renderText(lines[i], white, font);
        int textW, textH;
        SDL_QueryTexture(lineTexture, nullptr, nullptr, &textW, &textH);
        SDL_Rect textRect = { x, y + static_cast<int>(i) * lineHeight, textW, textH };
        SDL_RenderCopy(renderer, lineTexture, nullptr, &textRect);
        SDL_DestroyTexture(lineTexture);
    }
}



// Cập nhật điểm sau khi kết quả được tính
void updateScore(bool playerChoice, bool actualResult) {
        if (playerChoice == actualResult) {
            score += 10;      // Thắng +10 điểm
            winCount++;
            currentTurn++;
        }
        else {
            score -= 5;       // Thua -5 điểm
            loseCount++;
            currentTurn--;
        }

        if (playerChoice) bigCount++;
        else smallCount++;

        // Kiểm tra kết thúc game
        if (!currentTurn) {
            gameOver = true;
        }
    }

void renderGameOverScreen() {
    SDL_Event e;
    bool proceed = false;
    bool quit = false;
    SDL_Color white = { 255, 255, 255 };  // Màu chữ trắng
    SDL_Color buttonColor = { 0, 200, 0 };  // Màu xanh lá cho nút Play Again

    // Tạo chữ "Game Over" với phông chữ và màu sắc
    SDL_Surface* overSurface = TTF_RenderText_Solid(font, "Game Over", white);
    SDL_Texture* overTexture = SDL_CreateTextureFromSurface(renderer, overSurface);
    SDL_FreeSurface(overSurface);

    SDL_Rect overRect = { SCREEN_WIDTH / 2 - 150, 150, 300, 60 };

    // Tạo nút "Play Again"
    SDL_Rect buttonRect = { SCREEN_WIDTH / 2 - 75, 300, 150, 60 };
    SDL_Surface* buttonSurface = TTF_RenderText_Solid(font, "Play Again", white);
    SDL_Texture* buttonTexture = SDL_CreateTextureFromSurface(renderer, buttonSurface);
    SDL_FreeSurface(buttonSurface);

    SDL_Rect textRect = { buttonRect.x + 10, buttonRect.y + 10, 130, 40 };

    while (!proceed) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;  // Đảm bảo thoát nếu người dùng muốn đóng game
            }

            // Kiểm tra nếu người chơi nhấn nút "Play Again"
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x = e.button.x, y = e.button.y;
                if (x >= buttonRect.x && x <= buttonRect.x + buttonRect.w &&
                    y >= buttonRect.y && y <= buttonRect.y + buttonRect.h) {
                    proceed = true;  // Người chơi muốn chơi lại
                    // Reset lại trạng thái của trò chơi
                    currentTurn = 0;
                    score = winCount = loseCount = bigCount = smallCount = 0;
                    gameOver = false;
                    currentState = CHOOSE;
                }
            }
        }

        // Vẽ nền đen cho màn hình game over
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Màu nền đen
        SDL_RenderClear(renderer);  // Làm mới màn hình với nền đen

        // Vẽ chữ "Game Over"
        SDL_RenderCopy(renderer, overTexture, nullptr, &overRect);

        // Vẽ nút "Play Again"
        SDL_SetRenderDrawColor(renderer, buttonColor.r, buttonColor.g, buttonColor.b, 255);  // Nút màu xanh lá
        SDL_RenderFillRect(renderer, &buttonRect);  // Vẽ hình chữ nhật nút
        SDL_RenderCopy(renderer, buttonTexture, nullptr, &textRect);  // Vẽ chữ trên nút

        SDL_RenderPresent(renderer);  // Cập nhật màn hình
    }

    // Dọn dẹp bộ nhớ
    SDL_DestroyTexture(overTexture);
    SDL_DestroyTexture(buttonTexture);
}

// Vòng lặp game
void gameLoop() {
    bool quit = false;
    SDL_Event e;

    srand(time(nullptr));
    int dice1 = 0, dice2 = 0, dice3 = 0, sum = 0;
    bool playerChoice = false;

    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }

            if (e.type == SDL_MOUSEBUTTONDOWN && currentState == CHOOSE) {
                int x = e.button.x;
                int y = e.button.y;

                bool clickedBig = (x >= bigButton.x && x <= bigButton.x + bigButton.w &&
                    y >= bigButton.y && y <= bigButton.y + bigButton.h);

                bool clickedSmall = (x >= smallButton.x && x <= smallButton.x + smallButton.w &&
                    y >= smallButton.y && y <= smallButton.y + smallButton.h);

                if (clickedBig || clickedSmall) {
                    playerChoice = clickedBig;

                    // Quay xúc xắc
                    dice1 = (rand() % 6) + 1;
                    dice2 = (rand() % 6) + 1;
                    dice3 = (rand() % 6) + 1;
                    sum = dice1 + dice2 + dice3;

                    // Cập nhật điểm
                    bool actualResult = sum > 10;
                    updateScore(playerChoice, actualResult);

                    // Chuyển sang màn kết quả
                    currentState = RESULT;
                }
            }
        }

        // Hiển thị tùy theo trạng thái
        if (currentState == CHOOSE) {
            renderChooseScreen();
            renderScoreboard();
        }
        else if (currentState == RESULT) {
            renderResultScreen(dice1, dice2, dice3, sum, playerChoice);

            // Tự động chuyển về CHOOSE sau khi hiện kết quả một lúc
            SDL_Delay(2000);  // Hiện kết quả 2 giây
            currentState = CHOOSE;
        }

        if (gameOver) {
            renderGameOverScreen();
        }

        SDL_RenderPresent(renderer);  // Vẽ mọi thứ
    }
}


// Dọn dẹp bộ nhớ
void close() {
    //Mix_HaltMusic();  // Dừng nhạc nền khi kết thúc game
    //Mix_FreeMusic(backgroundMusic);  // Giải phóng nhạc nền
    //backgroundMusic = nullptr;

    SDL_DestroyTexture(background1);
    SDL_DestroyTexture(background2);
    for (int i = 0; i < 6; i++) {
        SDL_DestroyTexture(diceTextures[i]);
    }
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    //Mix_Quit();
    SDL_Quit();
    SDL_DestroyTexture(bigText);
    SDL_DestroyTexture(smallText);
    TTF_CloseFont(smallFont);
}

int main(int argc, char* args[]) {
    if (!init()) return -1;

    // Load ảnh nền
    background1 = loadTexture("back.jpg");
    background2 = loadTexture("thantai.jpg");

    if (!background1 || !background2) {
        cerr << "Failed to load background images!" << endl;
        return -1;
    }           

    // Load ảnh xúc xắc
    for (int i = 0; i < 6; i++) {
        diceTextures[i] = loadTexture("dice" + to_string(i + 1) + ".png");
        if (!diceTextures[i]) {
            cerr << "Failed to load dice" << i + 1 << ".png" << endl;
            return -1;
        }
    }
    showGameRules();
    gameLoop();
    close();
    return 0;
}
