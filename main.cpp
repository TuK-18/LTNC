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
SDL_Texture* diceTextures[6] = { nullptr }; //  ảnh xúc xắc

TTF_Font* font = nullptr;
TTF_Font* smallFont = nullptr;
Mix_Music* backgroundMusic = nullptr;  

enum GameState { CHOOSE, RESULT };

GameState currentState = CHOOSE;

int maxTurns = 10;
int currentTurn = 3;
bool gameOver = false;
int remainingTurns = 5;
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

    // Load background
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

    //  "PLAY"
    SDL_Surface* buttonSurface = TTF_RenderText_Blended(font, "PLAY", white);
    SDL_Texture* buttonTexture = SDL_CreateTextureFromSurface(renderer, buttonSurface);
    SDL_FreeSurface(buttonSurface);

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

        // nền
        SDL_RenderCopy(renderer, bgTexture, nullptr, nullptr);

        // luật chơi
        SDL_RenderCopy(renderer, ruleTexture, nullptr, &ruleRect);

        // nút
        SDL_SetRenderDrawColor(renderer, isHover ? hoverColor.r : buttonColor.r,
            isHover ? hoverColor.g : buttonColor.g,
            isHover ? hoverColor.b : buttonColor.b,
            255);
        SDL_RenderFillRect(renderer, &buttonRect);

        // viền nút
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Viền đen
        SDL_RenderDrawRect(renderer, &buttonRect);

        // chữ
        SDL_RenderCopy(renderer, buttonTexture, nullptr, &textRect);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(bgTexture);
    SDL_DestroyTexture(ruleTexture);
    SDL_DestroyTexture(buttonTexture);
}




// load ảnh
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



//  chọn Big/Small
void renderChooseScreen() {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);

    // Hiển thị ảnh nền
    SDL_RenderCopy(renderer, background1, nullptr, nullptr);

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

    // Vẽ viền nút 
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // viền đen
    SDL_RenderDrawRect(renderer, &bigButton);
    SDL_RenderDrawRect(renderer, &smallButton);

    // Căn giữa chữ 
    int bigTextW = 0, bigTextH = 0;
    SDL_QueryTexture(bigText, nullptr, nullptr, &bigTextW, &bigTextH);
    SDL_Rect bigTextRect = {
        bigButton.x + (bigButton.w - bigTextW) / 2,
        bigButton.y + (bigButton.h - bigTextH) / 2,
        bigTextW, bigTextH
    };
    SDL_RenderCopy(renderer, bigText, nullptr, &bigTextRect);

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



// ảnh xúc xắc 
void renderResultScreen(int dice1, int dice2, int dice3, int sum, bool playerChoice) {
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, background2, nullptr, nullptr);

    SDL_Color white = { 255, 255, 255 };
    SDL_Rect diceRect = { 220, 100, 100, 100 };

    // Hiệu ứng xúc xắc xoay 
    for (int i = 0; i < 10; ++i) {
        int temp1 = (rand() % 6);
        int temp2 = (rand() % 6);
        int temp3 = (rand() % 6);

        // 3 xúc xắc
        diceRect.x = 220;
        SDL_RenderCopy(renderer, diceTextures[temp1], nullptr, &diceRect);

        diceRect.x += 120;
        SDL_RenderCopy(renderer, diceTextures[temp2], nullptr, &diceRect);

        diceRect.x += 120;
        SDL_RenderCopy(renderer, diceTextures[temp3], nullptr, &diceRect);

        SDL_RenderPresent(renderer);
        SDL_Delay(50);  // dừng 5s
    }

    // 3 xuc xac
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

    
    bool actualResult = sum > 10;
    string resultText = "Total: " + to_string(sum) + " (" + (actualResult ? "Big" : "Small") + ")";
    resultText += (playerChoice == actualResult) ? " - You WIN!" : " - You LOSE!";
    SDL_Texture* resultTexture = renderText(resultText, white, font);

    SDL_Rect resultRect = { 250, 400, 300, 50 };
    SDL_RenderCopy(renderer, resultTexture, nullptr, &resultRect);
    SDL_DestroyTexture(resultTexture);

    SDL_RenderPresent(renderer);
    this_thread::sleep_for(chrono::seconds(2));

	currentState = CHOOSE;  // về trang chọn
}

int score = 0;
int winCount = 0;
int loseCount = 0;
int bigCount = 0;
int smallCount = 0;

// bảng điểm
void renderScoreboard() {
    SDL_Color white = { 255, 255, 255 };
    SDL_Color bgColor = { 0, 0, 0, 150 };

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

	// scoreboard background
    SDL_Rect bgRect = { x - 10, y - 10, width, height };
    SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderFillRect(renderer, &bgRect);

    // Vẽ text
    for (size_t i = 0; i < lines.size(); ++i) {
        SDL_Texture* lineTexture = renderText(lines[i], white, font);
        int textW, textH;
        SDL_QueryTexture(lineTexture, nullptr, nullptr, &textW, &textH);
        SDL_Rect textRect = { x, y + static_cast<int>(i) * lineHeight, textW, textH };
        SDL_RenderCopy(renderer, lineTexture, nullptr, &textRect);
        SDL_DestroyTexture(lineTexture);
    }
}



// update điểm
void updateScore(bool playerChoice, bool actualResult) {
        if (playerChoice == actualResult) {
            score += 10; 
            winCount++;
            currentTurn++;
        }
        else {
            score -= 5;    
            loseCount++;
            currentTurn--;
        }

        if (playerChoice) bigCount++;
        else smallCount++;

        // check end game
        if (!currentTurn) {
            gameOver = true;
        }
    }

void renderGameOverScreen() {
    SDL_Event e;
    bool proceed = false;
    bool quit = false;

    SDL_Color white = { 255, 255, 255 };
    SDL_Color buttonColor = { 0, 200, 0 };

    const char* lines[] = {
        "Game Over",
        "Nguoi khong choi la nguoi thang",
        "Nguoi choi khong bao gio thang"
    };
    int numLines = 3;
    int lineSpacing = 10;
    int startY = 150;

    // "Play Again"
    SDL_Rect buttonRect = { SCREEN_WIDTH / 2 - 75, 350, 150, 60 };
    SDL_Surface* buttonSurface = TTF_RenderText_Solid(font, "Play Again", white);
    SDL_Texture* buttonTexture = SDL_CreateTextureFromSurface(renderer, buttonSurface);
    SDL_FreeSurface(buttonSurface);

    SDL_Rect textRect = { buttonRect.x + 10, buttonRect.y + 10, 130, 40 };

    while (!proceed && !quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x = e.button.x, y = e.button.y;
                if (x >= buttonRect.x && x <= buttonRect.x + buttonRect.w &&
                    y >= buttonRect.y && y <= buttonRect.y + buttonRect.h) {
                    proceed = true;
                    currentTurn = 0;
                    score = winCount = loseCount = bigCount = smallCount = 0;
                    gameOver = false;
                    currentState = CHOOSE;
                }
            }
        }

        //background
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // over
        for (int i = 0; i < numLines; i++) {
            SDL_Surface* lineSurface = TTF_RenderText_Solid(font, lines[i], white);
            SDL_Texture* lineTexture = SDL_CreateTextureFromSurface(renderer, lineSurface);

            SDL_Rect lineRect;
            lineRect.w = lineSurface->w;
            lineRect.h = lineSurface->h;
            lineRect.x = SCREEN_WIDTH / 2 - lineRect.w / 2;
            lineRect.y = startY + i * (lineRect.h + lineSpacing);

            SDL_RenderCopy(renderer, lineTexture, nullptr, &lineRect);

            SDL_FreeSurface(lineSurface);
            SDL_DestroyTexture(lineTexture);
        }

        // Vẽ nút "Play Again"
        SDL_SetRenderDrawColor(renderer, buttonColor.r, buttonColor.g, buttonColor.b, 255);
        SDL_RenderFillRect(renderer, &buttonRect);
        SDL_RenderCopy(renderer, buttonTexture, nullptr, &textRect);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(buttonTexture);
}


// loop
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

        SDL_RenderPresent(renderer);  
    }
}


// clear
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
    // 6 cai xuc xac

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
