#pragma once
// Assistant-authored provisional C++ implementation. Binary is NOT James's code.
#include <algorithm>
#include <cassert>
#include <charconv>
#include <chrono>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <random>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <unordered_set>
#include <vector>
#define NOMINMAX
#include <windows.h>
#include <psapi.h>
using U=std::uint64_t;
constexpr U INF=std::numeric_limits<U>::max();
using Key=std::pair<U,int>;
using Graph=std::vector<std::vector<std::pair<int,U>>>;
void require(bool ok,const char* message) {
    if(!ok)throw std::runtime_error(message);
}
U integer(const std::string& text) {
    U value=0;
    auto r=std::from_chars(text.data(),text.data()+text.size(),value);
    require(r.ec==std::errc()&&r.ptr==text.data()+text.size()&&!text.empty(),"invalid unsigned integer");
    return value;
}
Graph load(const std::string& path,int n) {
    require(n>0,"n must be positive");
    std::ifstream in(path);
    require(bool(in),"cannot open CSV");
    std::string line;
    std::getline(in,line);
    if(!line.empty()&&line.back()=='\r')line.pop_back();
    require(line=="source,target,weight","invalid CSV header");
    Graph g(n);
    std::set<std::pair<int,int>> arcs;
    while(std::getline(in,line)) {
        if(!line.empty()&&line.back()=='\r')line.pop_back();
        auto a=line.find(','),b=a==std::string::npos?a:line.find(',',a+1);
        require(a!=std::string::npos&&b!=std::string::npos&&line.find(',',b+1)==std::string::npos,"CSV needs exactly three fields");
        U u=integer(line.substr(0,a)),v=integer(line.substr(a+1,b-a-1)),w=integer(line.substr(b+1));
        require(u<U(n)&&v<U(n),"vertex out of range");
        require(w<=(INF-1)/U(std::max(1,n-1)),"weight exceeds safe simple-path bound");
        require(arcs.emplace(int(u),int(v)).second,"duplicate arc");
        g[int(u)].push_back( {
            int(v),w
        }
        );
    }
    require(!in.bad(),"CSV read failure");
    return g;
}
