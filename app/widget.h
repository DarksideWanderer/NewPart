#pragma once
#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>
#include<bits/stdc++.h>
#include"screen.h"

struct Screen;
struct Widget{
	Screen *screen;
	Widget(Screen*screen):screen(screen){}
	virtual void render()=0;
	virtual void handleEvent(SDL_Event e)=0;
};

struct Button:public Widget{
	SDL_Color bgcolor[3]={
		{215,216,218,255},
		{215-10,216-10,218-10,255},
		{215-20,216-20,218-20,255},
	};//blue
	SDL_Color bordercolor[3]={
		{52,80,101,255},
		{52,80,101,255},
		{52,80,101,255},
	};//red
	SDL_Color textcolor{255,255,255,255};//black
	SDL_Rect rect;
	
	
	bool isPressed=false;
	bool isHovered=false;
	
	Button(Screen*screen,SDL_Rect rect):Widget(screen),rect(rect){}
	void render()override;
	void handleEvent(SDL_Event e)override;
};

struct Text:public Widget{
	void render()override;
	void handleEvent(SDL_Event e)override;
};

struct Image:public Widget{
	void render()override;
	void handleEvent(SDL_Event e)override;
};