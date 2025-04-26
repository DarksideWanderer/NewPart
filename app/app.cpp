#include"app.h"
#include"screen.h"
#include"logger.h"

void Appilication::post(std::function<void()>task){
	std::lock_guard<std::mutex>lock(taskMutex);
	taskQueue.push(std::move(task));
	taskCondition.notify_one(); 
}

void Appilication::execute(){
	init();
	SDL_Event e;
	while(running){
		std::unique_lock<std::mutex> lock(taskMutex);
		taskCondition.wait_for(lock, std::chrono::milliseconds(1), [&]() {
			return !taskQueue.empty();
		});
		while (!taskQueue.empty()) {
			auto task = std::move(taskQueue.front());
			taskQueue.pop();
			lock.unlock();
			task();
			lock.lock();
		}
		
		if (SDL_PollEvent(&e)) {
			if(e.type==SDL_WINDOWEVENT_HIDDEN)
				std::cerr<<"Window close event"<<std::endl;
			int id = e.window.windowID;
			if (windowMap.find(id) != windowMap.end()) {
				Screen* target = windowMap[id]->scr[windowMap[id]->which];
				while (target->eventMutex.try_lock()) {
					std::thread([target, e]() {
						target->onEvent(e);
						target->eventMutex.unlock();
					}).detach();
					break;
				}
			}
		}
		if(e.type == SDL_QUIT){
			running=false;
		}
		SDL_Delay(16);
	}
}

void Appilication::init(){
	TTF_Init();
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_SetHint(SDL_HINT_RENDER_VSYNC, "1");
	
	post([this](){
		Window *window=new MainWindow(this);
		window->init();
		windowMap[SDL_GetWindowID(window->window)]=window;
		std::thread([window](){window->execute();}).detach();
		std::cerr<<"MainWindow created"<<std::endl;
	});
	post([this](){
		Window *window=new DesignWindow(this);
		window->init();
		windowMap[SDL_GetWindowID(window->window)]=window;
		std::thread([window](){window->execute();}).detach();
		std::cerr<<"DesignWindow created"<<std::endl;
	});
}
void Appilication::clean(){
	SDL_Quit();
	TTF_Quit();
}