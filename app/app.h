#pragma once
#include<bits/stdc++.h>
#include"window.h"

struct Window;
struct Appilication{
	bool running=true;
	std::queue<std::function<void()>>taskQueue;
	std::mutex taskMutex;
	std::condition_variable taskCondition;
	std::map<int,Window*>windowMap;
	
	void post(std::function<void()>task);
	void execute();
	void clean();
	void init();
};