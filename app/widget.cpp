#include"widget.h"

#include<SDL2/SDL2_gfxPrimitives.h>

void Button::render(){
	
//渲染背景
	int now=0;
	if(isPressed)now=1;
	else if(isHovered)now=2;
	
	SDL_Rect crect{this->rect.x,this->rect.y,this->rect.w,this->rect.h};
	SDL_Color cbgcol={bgcolor[now].r,bgcolor[now].g,bgcolor[now].b,bgcolor[now].a};
	SDL_Color cbrcol={bordercolor[now].r,bordercolor[now].g,bordercolor[now].b,bordercolor[now].a};
	
	screen->window->app->post([this,crect,cbgcol,cbrcol](){
		int delta=crect.h*0.3,delta2=crect.h*0.07,delta3=crect.h*0.25;
		roundedBoxRGBA(screen->window->renderer,crect.x, crect.y, crect.x + crect.w, crect.y + crect.h,delta,
			cbrcol.r, cbrcol.g, cbrcol.b, cbrcol.a);
		roundedBoxRGBA(screen->window->renderer,crect.x+delta2, crect.y+delta2, crect.x + crect.w-delta2, crect.y + crect.h-delta2,delta3,
			cbgcol.r, cbgcol.g, cbgcol.b, cbgcol.a);
	});
}
void Button::handleEvent(SDL_Event e){
	
}
void Text::render(){
	
}
void Text::handleEvent(SDL_Event e){
	
}
void Image::render(){
	
}
void Image::handleEvent(SDL_Event e){
	
}