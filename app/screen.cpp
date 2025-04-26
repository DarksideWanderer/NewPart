#include"screen.h"

void MainMenu::init(){
	running.store(true);
}
void MainMenu::render(){
	std::unique_lock<std::mutex>lock(eventMutex);
	eventCondition.wait_for(lock, std::chrono::milliseconds(1), [&](){
		return !eventQueue.empty();
	});
	while(!eventQueue.empty()){
		auto task=std::move(eventQueue.front());
		eventQueue.pop();
		lock.unlock();
		task();
		lock.lock();
	}
	
	window->app->post([this](){
		SDL_SetRenderDrawColor(window->renderer,color.r,color.g,color.b,color.a);
		SDL_RenderClear(window->renderer);
	});
	
	for(auto &i:wid){i->render();}
	
	window->app->post([this](){
		SDL_RenderPresent(window->renderer);
	});
}
void MainMenu::onEvent(SDL_Event e){
	//SDL官方不建议这样写,事实上有非常奇怪的bug
	if(e.window.event == SDL_WINDOWEVENT_CLOSE ||
		e.window.event == SDL_WINDOWEVENT_HIDDEN){
		running.store(false);
		window->running.store(false);
		return ;
	}
	
}
void MainMenu::clean(){
	
}
void MainMenu::execute(){
	init();
	
	wid.push_back(new Button(this,SDL_Rect{100,100,500,100}));
	while(running.load()){
		render();
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}
	clean();
}

void DesignScreen::init(){
	running.store(true);
}
void DesignScreen::render(){
	std::unique_lock<std::mutex>lock(eventMutex);
	eventCondition.wait_for(lock, std::chrono::milliseconds(1), [&](){
		return !eventQueue.empty();
	});
	
	while(!eventQueue.empty()){
		auto task=std::move(eventQueue.front());
		eventQueue.pop();
		lock.unlock();
		task();
		lock.lock();
	}
	
	window->app->post([this](){
		SDL_SetRenderDrawColor(window->renderer,0,0,0,255);
		SDL_RenderClear(window->renderer);
	});
	
	for(auto &i:wid){i->render();}
	
	window->app->post([this](){
		SDL_RenderPresent(window->renderer);
	});
}
void DesignScreen::onEvent(SDL_Event e){
	if(e.window.event == SDL_WINDOWEVENT_CLOSE ||
		e.window.event == SDL_WINDOWEVENT_HIDDEN){
		running.store(false);
		window->running.store(false);
		return;
	}
	else if(e.type==SDL_MOUSEBUTTONDOWN){
		if(e.button.button==SDL_BUTTON_LEFT){
			std::cerr<<"DesignScreen Pos:"<<'('<<e.button.x<<' '<<e.button.y<<')'<<std::endl;
		}
	}
}
void DesignScreen::clean(){
	
}
void DesignScreen::execute(){
	init();
	while(running.load()){
		render();
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	}
	clean();
}