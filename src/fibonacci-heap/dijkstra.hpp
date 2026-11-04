#pragma once
#include "fibonacci.hpp"
#include "reference_binary.hpp"
struct Counts  {
    U inserts=0,extracts=0,decreases=0,arcs=0;
    bool operator==(const Counts& b)const {
        return std::tie(inserts,extracts,decreases,arcs)==std::tie(b.inserts,b.extracts,b.decreases,b.arcs);
    }
}
;
struct Result  {
    std::vector<U> dist;
    std::vector<int> previous;
    Counts counts;
}
;
template<class Heap,bool Count=false> Result dijkstra(const Graph& g,int source) {
    require(source>=0&&source<int(g.size()),"source out of range");
    Result r;
    r.dist.assign(g.size(),INF);
    r.previous.assign(g.size(),-1);
    r.dist[source]=0;
    Heap heap;
    std::vector<typename Heap::Handle> handles;
    handles.reserve(g.size());
    for(int v=0;v<int(g.size());++v)handles.push_back(heap.insert( {
        r.dist[v],v
    }
    ));
    if constexpr(Count)r.counts.inserts=g.size();
    while(heap) {
        auto [distance,u]=heap.extract_min();
        if constexpr(Count)++r.counts.extracts;
        if(distance==INF)break;
        for(auto [v,w]:g[u]) {
            if constexpr(Count)++r.counts.arcs;
            if(w>INF-1-distance)continue;
            U candidate=distance+w;
            if(candidate<r.dist[v]) {
                r.dist[v]=candidate;
                r.previous[v]=u;
                heap.decrease_key(handles[v], {
                    candidate,v
                }
                );
                if constexpr(Count)++r.counts.decreases;
            }
        }
    }
    return r;
    // heap and handles destruct before returning: cleanup is inside timing
}
std::vector<U> oracle(const Graph& g,int source) {
    std::vector<U>d(g.size(),INF);
    std::vector<bool>done(g.size());
    d[source]=0;
    for(std::size_t step=0;step<g.size();++step) {
        int u=-1;
        for(int v=0;v<int(g.size());++v)if(!done[v]&&(u<0||Key(d[v],v)<Key(d[u],u)))u=v;
        if(u<0||d[u]==INF)break;
        done[u]=true;
        for(auto[v,w]:g[u])if(w<=INF-1-d[u])d[v]=std::min(d[v],d[u]+w);
    }
    return d;
}
void paths(const Graph&g,int source,const Result&r) {
    require(r.dist[source]==0&&r.previous[source]==-1,"source path");
    for(int v=0;v<int(g.size());++v) {
        if(v==source||r.dist[v]==INF)continue;
        std::set<int>seen;
        int current=v;
        while(current!=source) {
            require(seen.insert(current).second,"predecessor cycle");
            int p=r.previous[current];
            require(p>=0,"missing predecessor");
            bool found=false;
            for(auto[to,w]:g[p])if(to==current&&w<=INF-1-r.dist[p]&&r.dist[p]+w==r.dist[current])found=true;
            require(found,"invalid predecessor edge");
            current=p;
        }
    }
}
