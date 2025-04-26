#include"window.h"
#include"logger.h"

void Window::init(){
	
}

void Window::execute(){
	
}

void Window::clean(){
	
}

void MainWindow::init(){
	running.store(true);
	window=SDL_CreateWindow(
		"MainWindow",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		800,600,SDL_WINDOW_SHOWN
	);
	window==nullptr&&(logDebug("SDL_CreateWindow failed"),exit(0),0);
	renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
	renderer==nullptr&&(logDebug("SDL_CreateRenderer failed"),exit(0),0);
	font=TTF_OpenFont("../resource/Resource.ttf",24);
	font==nullptr&&(logDebug("TTF_OpenFont failed"),exit(0),0);
	SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1")<0&&(logDebug("SDL_SetHint failed"),exit(0),0);
	SDL_StartTextInput();
}
void MainWindow::execute(){
	scr.push_back(new MainMenu(this));
	which=0;
	while(running.load()){
		scr[which]->execute();
	}
	delete scr[0];
	app->post([this](){
		app->windowMap.erase(SDL_GetWindowID(window));
		this->clean();
		std::cerr<<"MainWindow clean"<<std::endl;
	});
}
void MainWindow::clean(){
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	TTF_CloseFont(font);
	SDL_StopTextInput();
}
void DesignWindow::init(){
	running.store(true);
	window=SDL_CreateWindow(
		"DesignWindow",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		800,600,SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
	);
	window==nullptr&&(logDebug("SDL_CreateWindow failed"),exit(0),0);
	renderer=SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
	renderer==nullptr&&(logDebug("SDL_CreateRenderer failed"),exit(0),0);
	font=TTF_OpenFont("../resource/Resource.ttf",24);
	font==nullptr&&(logDebug("TTF_OpenFont failed"),exit(0),0);
	SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1")<0&&(logDebug("SDL_SetHint failed"),exit(0),0);
	SDL_StartTextInput();
}
void DesignWindow::execute(){
	scr.push_back(new DesignScreen(this));
	which=0;
	while(running.load()){
		scr[which]->execute();
	}
	
	delete scr[0];
	app->post([this](){
		app->windowMap.erase(SDL_GetWindowID(window));
		this->clean();
	});
}
void DesignWindow::clean(){
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	TTF_CloseFont(font);
	SDL_StopTextInput();
}