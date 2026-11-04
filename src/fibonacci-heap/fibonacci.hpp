#pragma once
#include "common.hpp"
class Fibonacci  {
    // storage owns nodes; parent/child/ring links are non-owning. Handles remain
    // allocated for this heap's lifetime, and active rejects extracted handles.
    public:
    struct Node  {
        Key key;
        Node *left,*right,*parent=nullptr,*child=nullptr;
        int degree=0;
        bool mark=false,active=true;
        const Fibonacci* owner;
        Node(Key k,const Fibonacci* h):key(k),left(this),right(this),owner(h) {
        }
    }
    ;
    using Handle=Node*;
    std::vector<std::unique_ptr<Node>> storage;
    Node* minimum=nullptr;
    std::size_t size=0;
    Fibonacci()=default;
    Fibonacci(const Fibonacci&)=delete;
    Fibonacci& operator=(const Fibonacci&)=delete;
    static std::vector<Node*> ring(Node* start) {
        std::vector<Node*> out;
        if(start) {
            auto p=start;
            do {
                out.push_back(p);
                p=p->right;
            }
            while(p!=start);
        }
        return out;
    }
    static void detach(Node* x) {
        x->left->right=x->right;
        x->right->left=x->left;
        x->left=x->right=x;
    }
    static void splice(Node* a,Node* x) {
        x->left=a;
        x->right=a->right;
        a->right->left=x;
        a->right=x;
    }
    void root(Node* x) {
        x->parent=nullptr;
        x->mark=false;
        if(!minimum)minimum=x;
        else {
            splice(minimum,x);
            if(x->key<minimum->key)minimum=x;
        }
    }
    Handle insert(Key k) {
        storage.push_back(std::make_unique<Node>(k,this));
        auto x=storage.back().get();
        root(x);
        ++size;
        return x;
    }
    explicit operator bool()const {
        return size>0;
    }
    void consolidate() {
        // Merge equal-degree roots, always linking the larger key below the smaller.
        auto roots=ring(minimum);
        minimum=nullptr;
        for(auto x:roots)detach(x);
        std::vector<Node*> table;
        for(auto x:roots) {
            while(true) {
                if(table.size()<=std::size_t(x->degree))table.resize(x->degree+1,nullptr);
                auto y=table[x->degree];
                if(!y)break;
                table[x->degree]=nullptr;
                if(y->key<x->key)std::swap(x,y);
                y->parent=x;
                y->mark=false;
                if(!x->child)x->child=y;
                else splice(x->child,y);
                ++x->degree;
            }
            table[x->degree]=x;
        }
        for(auto x:table)if(x)root(x);
    }
    Key extract_min() {
        require(minimum!=nullptr,"empty heap");
        auto z=minimum;
        for(auto x:ring(z->child)) {
            detach(x);
            root(x);
        }
        z->child=nullptr;
        z->degree=0;
        auto next=z->right;
        bool alone=next==z;
        detach(z);
        minimum=alone?nullptr:next;
        --size;
        if(minimum)consolidate();
        z->active=false;
        z->left=z->right=nullptr;
        return z->key;
    }
    void cut(Node* x,Node* p) {
        if(p->child==x)p->child=x->right==x?nullptr:x->right;
        detach(x);
        --p->degree;
        root(x);
    }
    void decrease_key(Handle x,Key k) {
        // A first child loss marks a non-root; a second loss cuts it recursively.
        require(x&&x->owner==this&&x->active,"invalid handle");
        require(k<=x->key,"key increase");
        x->key=k;
        auto p=x->parent;
        if(p&&x->key<p->key) {
            cut(x,p);
            while(p->parent) {
                auto grand=p->parent;
                if(!p->mark) {
                    p->mark=true;
                    break;
                }
                cut(p,grand);
                p=grand;
            }
        }
        if(x->key<minimum->key)minimum=x;
    }
    void visit(Node* start,Node* parent,std::unordered_set<Node*>& seen)const {
        for(auto x:ring(start)) {
            require(seen.insert(x).second,"duplicate/cyclic membership");
            require(x->left->right==x&&x->right->left==x,"ring reciprocity");
            require(x->parent==parent&&x->active&&x->owner==this,"ownership/parent");
            require(!parent||parent->key<=x->key,"heap order");
            require(parent||!x->mark,"marked root");
            require(x->degree==int(ring(x->child).size()),"degree mismatch");
            visit(x->child,x,seen);
        }
    }
    void validate()const {
        std::unordered_set<Node*> seen;
        visit(minimum,nullptr,seen);
        require(seen.size()==size,"size mismatch");
        require(bool(minimum)==bool(size),"empty mismatch");
        for(auto x:ring(minimum))require(minimum->key<=x->key,"minimum mismatch");
        for(auto& x:storage)require(x->active==(seen.count(x.get())!=0),"active membership");
    }
}
;
