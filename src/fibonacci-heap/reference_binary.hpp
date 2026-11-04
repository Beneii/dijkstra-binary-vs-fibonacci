#pragma once
#include "common.hpp"
class Binary  {
    public:
    using Handle=std::size_t;
    std::vector<std::pair<Key,Handle>> items;
    std::vector<std::size_t> pos;
    static constexpr std::size_t DEAD=std::numeric_limits<std::size_t>::max();
    explicit operator bool()const {
        return !items.empty();
    }
    void swap_at(std::size_t a,std::size_t b) {
        std::swap(items[a],items[b]);
        pos[items[a].second]=a;
        pos[items[b].second]=b;
    }
    void up(std::size_t i) {
        while(i&&items[i].first<items[(i-1)/2].first) {
            auto p=(i-1)/2;
            swap_at(i,p);
            i=p;
        }
    }
    Handle insert(Key k) {
        auto h=pos.size();
        pos.push_back(items.size());
        items.push_back( {
            k,h
        }
        );
        up(items.size()-1);
        return h;
    }
    void decrease_key(Handle h,Key k) {
        require(h<pos.size()&&pos[h]!=DEAD,"invalid handle");
        auto i=pos[h];
        require(k<=items[i].first,"key increase");
        items[i].first=k;
        up(i);
    }
    Key extract_min() {
        require(!items.empty(),"empty heap");
        auto answer=items.front();
        auto last=items.back();
        items.pop_back();
        pos[answer.second]=DEAD;
        if(!items.empty()) {
            items[0]=last;
            pos[last.second]=0;
            std::size_t i=0;
            while(2*i+1<items.size()) {
                auto c=2*i+1;
                if(c+1<items.size()&&items[c+1].first<items[c].first)++c;
                if(items[i].first<=items[c].first)break;
                swap_at(i,c);
                i=c;
            }
        }
        return answer.first;
    }
    void validate()const {
        for(std::size_t i=0;i<items.size();++i) {
            require(pos[items[i].second]==i,"position mismatch");
            require(!i||items[(i-1)/2].first<=items[i].first,"binary order");
        }
    }
}
;
