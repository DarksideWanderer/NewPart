#pragma once
#include<SDL2/SDL.h>
#include<SDL2/SDL_ttf.h>
#include<bits/stdc++.h>

struct Widget{
	virtual void render()=0;
	virtual void handleEvent()=0;
};

struct Button:public Widget{
	void render()override;
	void handleEvent()override;
};

struct Text:public Widget{
	void render()override;
	void handleEvent()override;
};

struct Image:public Widget{
	void render()override;
	void handleEvent()override;
};