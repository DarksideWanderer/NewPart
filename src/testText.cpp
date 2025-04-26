#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_clipboard.h>
#include <iostream>
#include <string>
#include <vector>

// 设置常量
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int SCROLL_SPEED = 30;
const int MIN_FONT_SIZE = 8;
const int MAX_FONT_SIZE = 48;
const int DEFAULT_FONT_SIZE = 20;

struct Editor {
    std::vector<std::string> lines{""}; // 文本按行存储
    int cursor_line = 0;
    int cursor_col = 0;
    int selection_line = -1;
    int selection_col = -1;
    int font_size = DEFAULT_FONT_SIZE;
    int scroll_offset = 0;
    bool selecting = false;
    bool editing_text = false; // 输入法是否激活
    std::string composing_text; // 输入法临时文字
    TTF_Font* font = nullptr;
};

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
Editor editor;

// 初始化
bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL Init Error: " << SDL_GetError() << std::endl;
        return false;
    }
    if (TTF_Init() != 0) {
        std::cerr << "TTF Init Error: " << TTF_GetError() << std::endl;
        return false;
    }

    window = SDL_CreateWindow("SDL2 UTF-8 Editor", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Create Window Error: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Create Renderer Error: " << SDL_GetError() << std::endl;
        return false;
    }

    editor.font = TTF_OpenFont("../resource/Resource.ttf", editor.font_size);
    if (!editor.font) {
        std::cerr << "Load Font Error: " << TTF_GetError() << std::endl;
        return false;
    }
    return true;
}

// 清理
void cleanUp() {
    TTF_CloseFont(editor.font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

// 插入文本
void insertText(const std::string& text) {
    auto& line = editor.lines[editor.cursor_line];
    line.insert(editor.cursor_col, text);
    editor.cursor_col += text.length();
}

// 删除字符
void deleteChar() {
    if (editor.cursor_col > 0) {
        auto& line = editor.lines[editor.cursor_line];
        line.erase(editor.cursor_col - 1, 1);
        editor.cursor_col--;
    } else if (editor.cursor_line > 0) {
        editor.cursor_col = editor.lines[editor.cursor_line - 1].length();
        editor.lines[editor.cursor_line - 1] += editor.lines[editor.cursor_line];
        editor.lines.erase(editor.lines.begin() + editor.cursor_line);
        editor.cursor_line--;
    }
}

// 渲染函数
void render() {
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255); // 背景色
    SDL_RenderClear(renderer);

    int y = 10 - editor.scroll_offset;
    for (int i = 0; i < editor.lines.size(); ++i) {
        std::string text = editor.lines[i];
        if (i == editor.cursor_line && !editor.composing_text.empty()) {
            text.insert(editor.cursor_col, editor.composing_text);
        }

        SDL_Color color = {255, 255, 255, 255};
        SDL_Surface* surface = TTF_RenderUTF8_Blended(editor.font, text.c_str(), color);
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            SDL_Rect dest = {10, y, surface->w, surface->h};
            SDL_RenderCopy(renderer, texture, NULL, &dest);

            SDL_FreeSurface(surface);
            SDL_DestroyTexture(texture);
        }
        y += editor.font_size + 4;
    }

    // 绘制光标
    int cursor_x = 10;
    for (int i = 0; i < editor.cursor_col; ++i) {
        int w;
        TTF_SizeUTF8(editor.font, editor.lines[editor.cursor_line].substr(0, i+1).c_str(), &w, nullptr);
        cursor_x = 10 + w;
    }
    int cursor_y = 10 + editor.cursor_line * (editor.font_size + 4) - editor.scroll_offset;

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_Rect cursor_rect = {cursor_x, cursor_y, 2, editor.font_size};
    SDL_RenderFillRect(renderer, &cursor_rect);

    SDL_RenderPresent(renderer);
}

// 处理事件
void handleEvent(SDL_Event& e) {
    if (e.type == SDL_QUIT) {
        cleanUp();
        exit(0);
    } else if (e.type == SDL_MOUSEWHEEL) {
        editor.scroll_offset -= e.wheel.y * SCROLL_SPEED;
        if (editor.scroll_offset < 0) editor.scroll_offset = 0;
    } else if (e.type == SDL_KEYDOWN) {
        if (editor.editing_text) {
            if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_BACKSPACE ||
                e.key.keysym.sym == SDLK_LEFT || e.key.keysym.sym == SDLK_RIGHT ||
                e.key.keysym.sym == SDLK_UP || e.key.keysym.sym == SDLK_DOWN) {
                return; // 输入法中这些按键无效
            }
        }

        switch (e.key.keysym.sym) {
            case SDLK_RETURN:
                {
                    auto& line = editor.lines[editor.cursor_line];
                    std::string new_line = line.substr(editor.cursor_col);
                    line = line.substr(0, editor.cursor_col);
                    editor.lines.insert(editor.lines.begin() + editor.cursor_line + 1, new_line);
                    editor.cursor_line++;
                    editor.cursor_col = 0;
                }
                break;
            case SDLK_BACKSPACE:
                deleteChar();
                break;
            case SDLK_LEFT:
                if (editor.cursor_col > 0) editor.cursor_col--;
                else if (editor.cursor_line > 0) {
                    editor.cursor_line--;
                    editor.cursor_col = editor.lines[editor.cursor_line].length();
                }
                break;
            case SDLK_RIGHT:
                if (editor.cursor_col < editor.lines[editor.cursor_line].length()) editor.cursor_col++;
                else if (editor.cursor_line + 1 < editor.lines.size()) {
                    editor.cursor_line++;
                    editor.cursor_col = 0;
                }
                break;
            case SDLK_UP:
                if (editor.cursor_line > 0) editor.cursor_line--;
                if (editor.cursor_col > editor.lines[editor.cursor_line].length())
                    editor.cursor_col = editor.lines[editor.cursor_line].length();
                break;
            case SDLK_DOWN:
                if (editor.cursor_line + 1 < editor.lines.size()) editor.cursor_line++;
                if (editor.cursor_col > editor.lines[editor.cursor_line].length())
                    editor.cursor_col = editor.lines[editor.cursor_line].length();
                break;
            case SDLK_c:
                if (SDL_GetModState() & KMOD_CTRL) {
                    SDL_SetClipboardText(editor.lines[editor.cursor_line].c_str());
                }
                break;
            case SDLK_v:
                if (SDL_GetModState() & KMOD_CTRL) {
                    char* clip = SDL_GetClipboardText();
                    if (clip) {
                        insertText(clip);
                        SDL_free(clip);
                    }
                }
                break;
            case SDLK_EQUALS:
                if (SDL_GetModState() & KMOD_CTRL) {
                    editor.font_size = std::min(MAX_FONT_SIZE, editor.font_size + 2);
                    TTF_CloseFont(editor.font);
                    editor.font = TTF_OpenFont("../resource/Resource.ttf", editor.font_size);
                }
                break;
            case SDLK_MINUS:
                if (SDL_GetModState() & KMOD_CTRL) {
                    editor.font_size = std::max(MIN_FONT_SIZE, editor.font_size - 2);
                    TTF_CloseFont(editor.font);
                    editor.font = TTF_OpenFont("../resource/Resource.ttf", editor.font_size);
                }
                break;
        }
    } else if (e.type == SDL_TEXTINPUT) {
        if (!editor.editing_text) {
            insertText(e.text.text);
        }
    } else if (e.type == SDL_TEXTEDITING) {
        editor.editing_text = true;
        editor.composing_text = e.edit.text;
    }
}

int main() {
    if (!init()) {
        return -1;
    }
    SDL_StartTextInput();

    SDL_Event e;
    while (true) {
        while (SDL_PollEvent(&e)) {
            handleEvent(e);
        }
        render();
        SDL_Delay(16); // 稍微休息一下，限帧
    }

    SDL_StopTextInput();
    cleanUp();
    return 0;
}
