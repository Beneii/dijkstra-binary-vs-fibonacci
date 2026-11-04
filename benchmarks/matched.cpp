#define Graph BenGraph
#include "../src/fibonacci-heap/common.hpp"
#include "../src/fibonacci-heap/dijkstra.hpp"
#undef Graph
#include "../src/binary-heap/dijkstra_binary.hpp"
#include "../src/binary-heap/binary_heap.hpp"
#include <iomanip>

volatile U matched_sink=0;
Graph convert(const BenGraph& g){
    Graph result(g.size());
    for(std::size_t u=0;u<g.size();++u) for(auto[v,w]:g[u]){
        require(w<=U(std::numeric_limits<int>::max()),"James edge-weight domain exceeded");
        require(w<=U(std::numeric_limits<long long>::max())/std::max<std::size_t>(1,g.size()),"signed path overflow domain");
        result[u].push_back({v,static_cast<int>(w)});
    }
    return result;
}
void compare(const BenGraph& g,int source){
    auto jg=convert(g);
    auto f=dijkstra<Fibonacci,true>(g,source);
    DijkstraStats stats;
    auto j=dijkstraBinary(jg,source,&stats);
    auto ref=oracle(g,source);
    require(f.dist==ref,"Fibonacci/oracle distance mismatch");
    for(std::size_t i=0;i<j.size();++i){
        const U value=j[i]==std::numeric_limits<long long>::max()?INF:static_cast<U>(j[i]);
        require(value==ref[i],"James/oracle distance mismatch");
    }
    paths(g,source,f);
}
void randomized_heap_checks(){
    std::mt19937 rng(20261104);
    constexpr int N=256;
    BinaryMinHeap heap(N);
    std::vector<long long> keys(N,0);
    std::vector<bool> active(N,false);
    for(int t=0;t<18000;++t){
        int v=int(rng()%N);
        if(!active[v]){ keys[v]=long long(rng()%1000); heap.insert(v,keys[v]);active[v]=true; }
        else if(rng()%2){keys[v]-=long long(rng()%20);heap.decreaseKey(v,keys[v]);}
        else {
            long long best=std::numeric_limits<long long>::max();
            for(int k=0;k<N;++k)if(active[k])best=std::min(best,keys[k]);
            auto got=heap.extractMin();
            require(active[got.vertex]&&got.distance==best,"James randomized heap mismatch");
            active[got.vertex]=false;
        }
        for(int k=0;k<N;++k)require(heap.contains(k)==active[k],"James position membership mismatch");
    }
    Fibonacci fib;
    std::vector<Fibonacci::Handle> handles(N,nullptr);
    std::vector<Key> fk(N);std::fill(active.begin(),active.end(),false);
    for(int t=0;t<18000;++t){int v=int(rng()%N);
        if(!active[v]){fk[v]={U(rng()%100000),v};handles[v]=fib.insert(fk[v]);active[v]=true;}
        else if(rng()%2){fk[v].first=fk[v].first>20?fk[v].first-20:0;fib.decrease_key(handles[v],fk[v]);}
        else {Key best={INF,N};for(int k=0;k<N;++k)if(active[k])best=std::min(best,fk[k]);auto got=fib.extract_min();require(got==best,"Fibonacci randomized heap mismatch");active[got.second]=false;}
        fib.validate();
    }
}
void selftest(){
    randomized_heap_checks();
    std::mt19937 rng(144528);
    for(int t=0;t<80;++t){int n=2+int(rng()%48);BenGraph g(n);
        for(int u=0;u<n;++u)for(int v=0;v<n;++v)if(u!=v&&rng()%5==0)g[u].push_back({v,U(rng()%21)});
        compare(g,int(rng()%n));
    }
    std::cout<<"PASS: James and Fibonacci each 18000 randomized heap operations; 80 random graphs with zero/tie/unreachable cases; both full distance vectors vs array oracle\n";
}
template<bool Fib>double batch(const BenGraph& fg,const Graph& jg,int source,int count){
    auto begin=std::chrono::steady_clock::now();
    for(int k=0;k<count;++k){
        if constexpr(Fib){auto r=dijkstra<Fibonacci>(fg,source);matched_sink^=r.dist[(k+source)%fg.size()];}
        else {auto r=dijkstraBinary(jg,source,nullptr);matched_sink^=U(r[(k+source)%jg.size()]);}
    }
    return double(std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now()-begin).count());
}
int main(int argc,char**argv){try{
    if(argc==2&&std::string(argv[1])=="selftest"){selftest();return 0;}
    if(argc==2&&std::string(argv[1])=="compiler"){std::cout<<"MSVC "<<_MSC_FULL_VER<<" C++ "<<_MSVC_LANG<<" pointer_bits "<<8*sizeof(void*)<<'\n';return 0;}
    require(argc==5,"mode CSV n source");
    auto fg=load(argv[2],std::stoi(argv[3]));auto jg=convert(fg);int source=std::stoi(argv[4]);
    if(std::string(argv[1])=="distances"){
        compare(fg,source);auto f=dijkstra<Fibonacci>(fg,source);std::cout<<'[';
        for(std::size_t i=0;i<f.dist.size();++i){if(i)std::cout<<',';if(f.dist[i]==INF)std::cout<<"null";else std::cout<<f.dist[i];}std::cout<<"]\n";return 0;
    }
    if(std::string(argv[1])=="verify"){
        compare(fg,source);auto f=dijkstra<Fibonacci,true>(fg,source);DijkstraStats j;auto d=dijkstraBinary(jg,source,&j);
        std::cout<<"PASS,"<<fg.size()<<','<<source<<','<<f.counts.inserts<<','<<f.counts.extracts<<','<<f.counts.decreases<<','<<f.counts.arcs<<','<<j.insertions<<','<<j.extractions<<','<<j.decreaseKeys<<','<<j.edgesExamined<<','<<j.successfulRelaxations<<'\n';return 0;
    }
    require(std::string(argv[1])=="time","unknown mode");
    for(int w=0;w<2;++w){batch<true>(fg,jg,source,1);batch<false>(fg,jg,source,1);}
    const int count=fg.size()<=250?32:fg.size()<=1000?8:2;
    std::cout<<"trial,order,algorithm,calls_per_batch,batch_ns,ns_per_call\n"<<std::fixed<<std::setprecision(6);
    for(int trial=0;trial<8;++trial)for(int order=0;order<2;++order){bool fib=(trial%2==0)==(order==0);double ns=fib?batch<true>(fg,jg,source,count):batch<false>(fg,jg,source,count);std::cout<<trial<<','<<order<<','<<(fib?"ben-fibonacci":"james-binary")<<','<<count<<','<<ns<<','<<ns/count<<'\n';}
    return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
