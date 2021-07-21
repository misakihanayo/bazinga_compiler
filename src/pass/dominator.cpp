#include "pass/dominator.h"
#include "queue"
#include "iostream"
#include "iostream"
// reference : A Simple, Fast Dominance Algorithm
void print_block(Function* f){
    for(auto bb: f->get_basic_blocks()){
        std::cout << "**********" << std::endl;
        std::cout << bb->get_name() << " is a " << (bb->is_fake_block() ? "fake block" : "true block") << std::endl;
        if(!bb->is_fake_block()){
            std::cout << bb->get_name() << "has " << bb->get_pre_basic_blocks().size() << " pre block " << std::endl;
            for(auto succ: bb->get_pre_basic_blocks()){
                std::cout<< succ->get_name() << std::endl;
            }
        }
        std::cout << "**********" << std::endl;
    }
}

void dominator::run(){
    for (auto f : m_->get_functions()) {
        if (f->get_basic_blocks().size() == 0)
            continue;
//        print_block(f);
        for (auto bb : f->get_basic_blocks() ){
            if(bb->is_fake_block()) continue;
//            std::cout << bb->get_name() << std::endl;
            immediate_dominance.insert({bb ,{}});
            dominannce_frontier.insert({bb ,{}});
            dom_tree_succ_blocks.insert({bb ,{}});
        }
//        std::cout << "dom debug 1" << std::endl;
        create_reverse_post_order(f);
//        std::cout << "dom debug 2" << std::endl;
        create_immediate_dominance(f);
//        print_dom_tree();
//        std::cout << "dom debug 3" << std::endl;
        create_dominance_frontier(f);
//        std::cout << "dom debug 4" << std::endl;
        create_dom_tree_succ(f);
//        std::cout << "dom debug 5" << std::endl;
    }
}

void dominator::create_reverse_post_order(Function *f){
    reverse_post_order_.clear();
    post_order_id_.clear();
    std::set<BasicBlock *> visited;
    post_order_visit(f->get_entry_block(), visited);
    reverse_post_order_.reverse();
}

void dominator::post_order_visit(BasicBlock *bb, std::set<BasicBlock *> &visited){
    visited.insert(bb);
    for (auto b : bb->get_succ_basic_blocks()) {
//        std::cout << "++++++++++" << std::endl;
//        std::cout << bb->get_name() << std::endl;
//        std::cout<< bb->get_succ_basic_blocks().size() << std::endl;
//        std::cout<< (b->is_fake_block() ? "fake block" : " true block") << std::endl;
//        std::cout << "++++++++++" << std::endl;
        if(b->is_fake_block()) continue;
//        std::cout << bb->get_name() << "--->" << b->get_name() << std::endl;
        if (visited.find(b) == visited.end())
            post_order_visit(b, visited);
    }
    post_order_id_[bb] = reverse_post_order_.size();
    reverse_post_order_.push_back(bb);
}

void dominator::create_immediate_dominance(Function *f){
    for (auto bb : f->get_basic_blocks()){
        if(bb->is_fake_block()){
            continue;
        }
        set_immediate_dominance(bb, nullptr);
    }
    auto root = f->get_entry_block();
    set_immediate_dominance(root, root);

    bool changed = true;
    while (changed) {
        changed = false;
        for (auto bb : this->reverse_post_order_) {
//            std::cout << "-----------"<<std::endl;
//            for(auto bb: this->reverse_post_order_){
//                std::cout<<bb->get_name()<<std::endl;
//            }
//            std::cout << this->reverse_post_order_.size() << std::endl;
//            std::cout << "-----------"<<std::endl;
            if(bb->is_fake_block()) continue;
            if (bb == root) {
                continue;
            }
            // find one pred which has idom
            BasicBlock *pred = nullptr;
            for (auto p : bb->get_pre_basic_blocks()) {
                if(p->is_fake_block()) continue;
//                std::cout << "-----------"<<std::endl;
//                std::cout<<bb->get_name()<<std::endl;
//                for(auto pre: bb->get_pre_basic_blocks()){
//                    std::cout<<pre->get_name()<<std::endl;
//                }
//                std::cout << "-----------"<<std::endl;
                if (get_immediate_dominance(p)) {
                    pred = p;
                    break;
                }
            }
            assert(pred);

            BasicBlock *new_idom = pred;
            for (auto p : bb->get_pre_basic_blocks()) {
                if(p->is_fake_block()) continue;
                if (p == pred)
                    continue;
                if (get_immediate_dominance(p)) {
                    new_idom = intersect(p, new_idom);
                }
            }
            if (get_immediate_dominance(bb) != new_idom) {
                set_immediate_dominance(bb, new_idom);
                changed = true;
            }
        }
    }
}

// find closest parent of b1 and b2
BasicBlock *dominator::intersect(BasicBlock *b1, BasicBlock *b2){
    while (b1 != b2) {
        while (post_order_id_[b1] < post_order_id_[b2]) {
            assert(get_immediate_dominance(b1));
            b1 = get_immediate_dominance(b1);
        }
        while (post_order_id_[b2] < post_order_id_[b1]) {
            assert(get_immediate_dominance(b2));
            b2 = get_immediate_dominance(b2);
        }
    }
    return b1;
}

void dominator::create_dominance_frontier(Function *f){
    for (auto bb : f->get_basic_blocks()) {
        if(bb->is_fake_block()) continue;
        if (bb->get_pre_basic_blocks().size() >= 2) {
            for (auto p : bb->get_pre_basic_blocks()) {
                auto runner = p;
                while (runner != get_immediate_dominance(bb)) {
                    add_dominance_frontier(runner, bb);
                    runner = get_immediate_dominance(runner);
                }
            }
        }
    }
}

void dominator::create_dom_tree_succ(Function *f){
    for (auto bb : f->get_basic_blocks()) {
        if(bb->is_fake_block()) continue;
        auto idom = get_immediate_dominance(bb);
        if(!idom || idom->is_fake_block()) continue;
        // e.g, entry bb
        if (idom != bb) {
            add_dom_tree_succ_block(idom, bb);
//            std::cout<<"in add succ bb"<<std::endl;
//            std::cout<< bb->get_name()<<std::endl;
//            std::cout << idom->get_name() << std::endl;
//            std::cout << "" << std::endl;
        }
    }
}

void dominator::print_dom_tree() {
//    std::cout <<"======="<<std::endl;
//    std::cout << dom_tree_succ_blocks.size() << std::endl;
    for(auto x: dom_tree_succ_blocks){
        if(x.first->is_fake_block()) continue;
//        std::cout << x.first->get_name() << std::endl;
//        for(auto s: x.second){
//            std::cout << x.first->get_name() << " -->  " << s->get_name() << std::endl;
//        }
    }
//    std::cout <<"======="<<std::endl;
}