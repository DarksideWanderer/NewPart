#include<bits/stdc++.h>

using std::vector;


int main(){
	vector<int>a;a.push_back(1);
	
	vector<int>b;b.push_back(2);
	
	
	b=std::move(a);
	
	std::cout<<a.size()<<std::endl;
	
	return 0;
}