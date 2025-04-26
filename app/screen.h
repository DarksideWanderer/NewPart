#pragma once
#include<SDL2/SDL.h>

#include"widget.h"
#include"window.h"

struct Window;
struct Widget;
struct Screen{
	std::atomic<bool> running;
	Window *window;
	std::mutex eventMutex;
	std::queue<std::function<void()>> eventQueue;
	std::vector<Widget*>wid;
	std::condition_variable eventCondition;
	
	Screen(Window *wind):window(wind){}
	
	virtual void post(std::function<void()>task){
		eventQueue.push(std::move(task));
		eventCondition.notify_one(); 
	}
	virtual void init()=0;
	virtual void render()=0;
	virtual void onEvent(SDL_Event e)=0;
	virtual void clean()=0;
	virtual void execute()=0;
};

struct MainMenu:public Screen{
	
	MainMenu(Window *wind):Screen(wind){}
	SDL_Color color{255,255,255,255};
	void init()override;
	void render()override;
	void onEvent(SDL_Event e)override;
	void clean()override;
	void execute()override;
};

struct DesignScreen:public Screen{
	
	DesignScreen(Window *wind):Screen(wind){}
	
	void init()override;
	void render()override;
	void onEvent(SDL_Event e)override;
	void clean()override;
	void execute()override;
};