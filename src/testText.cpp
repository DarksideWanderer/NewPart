#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <SDL2/SDL_clipboard.h>

// 屏幕宽度和高度
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;

// 字体设置
const int DEFAULT_FONT_SIZE = 16;
const int MIN_FONT_SIZE = 8;
const int MAX_FONT_SIZE = 32;

// 编辑器状态
struct Editor {
    std::string text;           // 文本内容
    int cursor_pos;             // 光标位置
    int scroll_offset;          // 滚动位置
    bool is_selecting;          // 是否在选中文本
    int selection_start;        // 选中文本的起始位置
    int selection_end;          // 选中文本的结束位置
    TTF_Font* font;             // 字体
    int font_size;              // 字体大小
    bool is_editing;            // 是否正在编辑（输入法状态）
};

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
Editor editor;

// 初始化 SDL 和字体库
bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL2 初始化失败: " << SDL_GetError() << std::endl;
        return false;
    }
    window = SDL_CreateWindow("SDL2 UTF-8 文本编辑器", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "创建窗口失败: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "创建渲染器失败: " << SDL_GetError() << std::endl;
        return false;
    }

    if (TTF_Init() == -1) {
        std::cerr << "TTF 库初始化失败: " << TTF_GetError() << std::endl;
        return false;
    }

    editor.font = TTF_OpenFont("../resource/Resource.ttf", DEFAULT_FONT_SIZE);  // 替换为合适的字体路径
    if (!editor.font) {
        std::cerr << "加载字体失败: " << TTF_GetError() << std::endl;
        return false;
    }

    editor.font_size = DEFAULT_FONT_SIZE;
    editor.cursor_pos = 0;
    editor.scroll_offset = 0;
    editor.is_selecting = false;
    editor.is_editing = false; // 初始化时，输入法未激活
    return true;
}

// 清理资源
void cleanUp() {
    TTF_CloseFont(editor.font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

// 渲染文本
void renderText() {
    SDL_RenderClear(renderer);

    int x = 10;
    int y = 10 - editor.scroll_offset;
    SDL_Color color = { 255, 255, 255, 255 };

    std::string line = editor.text;
    for (size_t i = 0; i < line.size(); i++) {
        if (editor.is_selecting && i >= editor.selection_start && i <= editor.selection_end) {
            // 选中文本高亮
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 128);  // 蓝色背景
            SDL_RenderFillRect(renderer, &(SDL_Rect{x, y, 10, 20}));  // 高亮区域
        }

        SDL_Surface* surface = TTF_RenderUTF8_Solid(editor.font, std::string(1, line[i]).c_str(), color);  // 使用 UTF-8 渲染
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect dest_rect = { x, y, surface->w, surface->h };
        SDL_RenderCopy(renderer, texture, nullptr, &dest_rect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);

        x += surface->w;  // 移动光标位置
    }

    SDL_RenderPresent(renderer);
}

// 处理事件
void handleEvent(SDL_Event& e) {
    if (e.type == SDL_QUIT) {
        cleanUp();
        exit(0);
    } else if (e.type == SDL_KEYDOWN) {
        if (editor.is_editing) {
            // 如果正在输入法状态，避免常规键对文本的修改
            if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_BACKSPACE || 
                e.key.keysym.sym == SDLK_LEFT || e.key.keysym.sym == SDLK_RIGHT || 
                e.key.keysym.sym == SDLK_UP || e.key.keysym.sym == SDLK_DOWN) {
                return;  // 输入法输入时，不处理这些键
            }
        }

        if (e.key.keysym.sym == SDLK_RETURN && !editor.is_editing) {
            editor.text.insert(editor.cursor_pos, "\n");
            editor.cursor_pos++;
        } else if (e.key.keysym.sym == SDLK_LEFT && editor.cursor_pos > 0) {
            editor.cursor_pos--;
        } else if (e.key.keysym.sym == SDLK_RIGHT && editor.cursor_pos < editor.text.size()) {
            editor.cursor_pos++;
        } else if (e.key.keysym.sym == SDLK_UP) {
            // 向上滚动
            editor.scroll_offset -= 10;
            if (editor.scroll_offset < 0) editor.scroll_offset = 0;
        } else if (e.key.keysym.sym == SDLK_DOWN) {
            // 向下滚动
            editor.scroll_offset += 10;
        } else if (e.key.keysym.sym == SDLK_C && SDL_GetModState() & KMOD_CTRL) {
            // 复制
            if (editor.is_selecting) {
                std::string selected_text = editor.text.substr(editor.selection_start, editor.selection_end - editor.selection_start + 1);
                SDL_SetClipboardText(selected_text.c_str());
            }
        } else if (e.key.keysym.sym == SDLK_V && SDL_GetModState() & KMOD_CTRL) {
            // 粘贴
            char* clipboard_text = SDL_GetClipboardText();
            if (clipboard_text) {
                editor.text.insert(editor.cursor_pos, clipboard_text);
                SDL_free(clipboard_text);
            }
        } else if (e.key.keysym.sym == SDLK_X && SDL_GetModState() & KMOD_CTRL) {
            // 剪切
            if (editor.is_selecting) {
                std::string selected_text = editor.text.substr(editor.selection_start, editor.selection_end - editor.selection_start + 1);
                SDL_SetClipboardText(selected_text.c_str());
                editor.text.erase(editor.selection_start, editor.selection_end - editor.selection_start + 1);
                editor.cursor_pos = editor.selection_start;
                editor.is_selecting = false;
            }
        } else if (e.key.keysym.sym == SDLK_BACKSPACE && editor.cursor_pos > 0) {
            editor.text.erase(editor.cursor_pos - 1, 1);
            editor.cursor_pos--;
        }
    } else if (e.type == SDL_TEXTINPUT) {
        // 处理正常的文本输入（不会在输入法状态下触发）
        if (!editor.is_editing) {
            editor.text.insert(editor.cursor_pos, e.text.text);
            editor.cursor_pos += strlen(e.text.text);
        }
    } else if (e.type == SDL_TEXTEDITING) {
        // 处理输入法状态的字符
        editor.is_editing = true;  // 激活输入法状态
    } else if (e.type == SDL_MOUSEBUTTONDOWN) {
        if (e.button.button == SDL_BUTTON_LEFT) {
            editor.selection_start = editor.cursor_pos;
            editor.is_selecting = true;
        }
    } else if (e.type == SDL_MOUSEMOTION) {
        if (editor.is_selecting) {
            // Update selection end based on mouse position
            editor.selection_end = editor.cursor_pos;
        }
    }
}

// 主函数
int main() {
    if (!init()) {
        return -1;
    }

    SDL_StartTextInput();  // 启用文本输入事件
    SDL_Event e;
    bool running = true;

    while (running) {
        while (SDL_PollEvent(&e)) {
            handleEvent(e);
        }

        renderText();
    }

    SDL_StopTextInput();  // 停止文本输入事件
    cleanUp();
    return 0;
}
