#include"app.h"
#include"logger.h"
#include<bits/stdc++.h>

Appilication app;

int main(){
	Logger::getInstance()->open("/Users/piggy/Documents/GitHub/NewPart/src/mainlog.txt");
	app.execute();
	return 0;
}