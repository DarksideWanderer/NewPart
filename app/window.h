#pragma once
#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>
#include<bits/stdc++.h>
#include"screen.h"
#include"app.h"
struct Screen;
struct Appilication;
struct Window{
	std::atomic<bool> running;
	SDL_Window *window;
	SDL_Renderer *renderer;
	TTF_Font *font;
	int which;
	std::vector<Screen*>scr;
	Appilication *app;
	
	
	Window(Appilication *app):app(app){}
	virtual void init()=0;
	virtual void execute()=0;
	virtual void clean()=0;
};
struct MainWindow:public Window{
	MainWindow(Appilication *app):Window(app){}
	void init()override;
	void execute()override;
	void clean()override;
	
};
struct DesignWindow:public Window{
	DesignWindow(Appilication *app):Window(app){}
	void init()override;
	void execute()override;
	void clean()override;
};