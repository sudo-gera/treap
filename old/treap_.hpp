using ll=long long int;

#include <inttypes.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <type_traits>
#ifndef assert
#include <assert.h>
#endif
#include <algorithm>
#include <bits/stdc++.h>

// #ifdef assert_m
// #undef assert
// #define assert assert_m
// #endif

#define get(a) auto a##_s=a?a->size:0;auto a##_z=a?a->get_child(0):0;auto a##_x=a?a->get_child(1):0;auto a##_z_s=a##_z?a##_z->size:0;auto a##_x_s=a##_x?a##_x->size:0;

struct FastRand{
    size_t state=1;
    constexpr FastRand(size_t state=1):
        state(state){}
    constexpr auto operator()(){
        state ^= state << 21;
        state ^= state >> 35;
        state ^= state << 4;
        return state&0xffffffff;
    }
    constexpr operator size_t(){
        return (*this)();
    }
};

template<typename T>
struct BasicTreap{
    struct Node{
        size_t priority=0;
        std::array<Node*, 2> children = {nullptr,nullptr};
        Node *parent = nullptr;
        size_t size=1;
        size_t height=1;
        union OptValue{
            char empty;
            T value;
            constexpr ~OptValue(){}
        } value;

        template<typename...CON_ARGS>
        constexpr Node(FastRand&rand,size_t,CON_ARGS&&...args):
            priority(rand),
            value({0})
        {
            value=OptValue{.value=T(std::forward<CON_ARGS>(args)...)};
            update(this);
        }

        constexpr Node* get_child(bool num){
            make(children[num]);
            return children[num];
        }

        constexpr static void make(Node*root){
            if (root){
                T::make(root);
            }
        }

        static void constexpr update(Node*root){
            if (root){
                if (root->children[0]){
                    root->children[0]->parent=root;
                }
                if (root->children[1]){
                    root->children[1]->parent=root;
                }
                if (root->children[0] and root->children[1]){
                    root->size=root->children[0]->size+1+root->children[1]->size;
                    root->height=std::max(root->children[0]->height,root->children[1]->height)+1;
                }else
                if (root->children[0]){
                    root->size=root->children[0]->size+1;
                    root->height=root->children[0]->height+1;
                }else
                if (root->children[1]){
                    root->size=root->children[1]->size+1;
                    root->height=root->children[1]->height+1;
                }else{
                    root->size=1;
                    root->height=1;
                }
                if (root->height>16){
                    std::cerr<<"bamboo!! "<<root->height<<std::endl;
                    exit(0);
                }
                T::update(root);
            }
        }

        constexpr static void check(Node*root){
            if (!root){
                return;
            }
            check(root->children[0]);
            check(root->children[1]);
            if (root->children[0]){
                assert(root->children[0]->parent==root);
                assert(root->children[0]->priority<=root->priority);
            }
            if (root->children[1]){
                assert(root->children[1]->parent==root);
                assert(root->children[1]->priority<=root->priority);
            }
            if (root->children[0] and root->children[1]){
                assert(root->children[0]->size+root->children[1]->size+1==root->size);
                assert(root->height==1+std::max(root->children[0]->height,root->children[1]->height));
                assert(root->value.value.sum==root->value.value.v + root->children[0]->value.value.sum*root->children[0]->value.value.to_mul+root->children[0]->value.value.to_add*root->children[0]->size + root->children[1]->value.value.sum*root->children[1]->value.value.to_mul+root->children[1]->value.value.to_add*root->children[1]->size);
            }else
            if (root->children[0]){
                assert(root->children[0]->size+1==root->size);
                assert(root->children[0]->height+1==root->height);
            }else
            if (root->children[1]){
                assert(root->children[1]->size+1==root->size);
                assert(root->children[1]->height+1==root->height);
            }else{
                assert(1==root->size);
                assert(1==root->height);
            }
            T::check(root);
        }

        struct PrintNode{
            size_t value = 0;
            size_t save = 0;
            PrintNode* next = nullptr;
        };

        static void print(Node* root,auto& ss,PrintNode* start=nullptr, PrintNode*prev_prev_node=nullptr){
            if (not root){
                return;
            }
            if (not start){
                PrintNode prev1{3};
                PrintNode prev2{0, 0, &prev1};
                print(root, ss, &prev1, &prev2);
                return;
            }
            prev_prev_node->save = prev_prev_node->value;
            if (prev_prev_node->value == prev_prev_node->next->value){
                prev_prev_node->value = 0;
            }
            PrintNode node;
            prev_prev_node->next->next = &node;
            node.value = 2;
            print(root->children[0], ss, start, prev_prev_node->next);
            for (PrintNode* n = start; n != &node; n = n->next){
                char const* strs[]={" ", " ", "┃", "┗", "┃", "┏", "", ""};
                ss << strs[n->value * 2 + (n == prev_prev_node->next)];
            }
            char const* strs[] = {"━", "┳", "┻", "╋"};
            ss << strs[bool(root->children[0]) * 2 + bool(root->children[1])] << "► " << T::to_string(root,ss) << "\n";
            node.value = 1;
            print(root->children[1], ss, start, prev_prev_node->next);
            prev_prev_node->value = prev_prev_node->save;
        }

        static constexpr void del(Node* q){
            if (!q){
                return;
            }
            del(q->children[0]);
            del(q->children[1]);
            delete q;
        }

        static constexpr Node* merge(std::array<Node*, 2> roots, bool args_order_is_not_reversed){
            if (not roots[0]) {return roots[1];}
            if (not roots[1]) {return roots[0];}
            auto new_root = roots[0]->priority < roots[1]->priority;
            auto child = new_root ^ args_order_is_not_reversed;
            roots[new_root]->children[child] = merge({roots[new_root]->get_child(child), roots[new_root ^ 1]}, child);
            update(roots[new_root]);
            return roots[new_root];
        }

        static constexpr Node* add(Node* current, std::ptrdiff_t add){
            if (not current){return nullptr;}
            auto left = current->get_child(0);
            auto right = current->get_child(1);
            size_t left_size = left ? left->size : 0;
            size_t right_size = right ? right->size : 0;
            while (add and current){
                if (add > (std::ptrdiff_t)right_size or -add > (std::ptrdiff_t)left_size){
                    if (not current->parent){
                        current = nullptr;
                        break;
                    }else if (current == current->parent->children[0]){
                        add -= right_size + 1;
                        left = current;
                        current = current->parent;
                        right = current -> get_child(1);
                        left_size = left->size;
                        right_size = right ? right->size : 0;
                    }else if (current == current->parent->children[1]){
                        add += left_size + 1;
                        right = current;
                        current = current->parent;
                        left = current -> get_child(0);
                        left_size = left ? left->size : 0;
                        right_size = right->size;
                    }
                }else if (add < 0){
                    current = left;
                    right = current->get_child(1);
                    right_size = right ? right->size : 0;
                    left = current -> get_child(0);
                    left_size = left ? left->size : 0;
                    add += right_size + 1;
                }else{
                    current = right;
                    left = current->get_child(0);
                    left_size = left ? left->size : 0;
                    right = current -> get_child(1);
                    right_size = right ? right ->size : 0;
                    add -= left_size + 1;
                }
            }
            return current;
        }

        static constexpr ssize_t find_offset(Node* current){
            if (not current){
                return 0;
            }
            ssize_t ans = 0;
            while (current->parent){
                if (current->parent->children[0] == current){
                    auto right = current -> get_child(1);
                    auto right_size = right ? right ->size : 0;
                    ans -= right_size + 1;
                }else
                if (current->parent->children[1] == current){
                    auto left = current->get_child(0);
                    auto left_size = left ? left->size : 0;
                    ans += left_size + 1;
                }
                current=current->parent;
            }
            return ans;
        }

        template<bool condition_tells_if_is_right=0,typename COND>
        static constexpr std::array<Node*, 2> split(Node* root, COND&condition){
            if (not root){
                return {nullptr,nullptr};
            }
            bool is_left = condition_tells_if_is_right^bool(condition(root));
            auto child = root->get_child(is_left);
            auto tmp = split<condition_tells_if_is_right>(child,condition);
            root->children[is_left] = tmp[is_left^1];
            update(root);
            tmp[is_left^1]=root;
            if (tmp[is_left]){
                tmp[is_left]->parent = nullptr;
            }
            return tmp;
        }


    };
    Node* root = nullptr;
    FastRand rand;
    // Node*& current=root;
    #define current root
    constexpr BasicTreap(Node*current=nullptr):root(current){}
    constexpr BasicTreap(BasicTreap&&other)noexcept{
        std::swap(root, other.root);
    }
    constexpr auto&operator=(BasicTreap&&other)noexcept{
        std::swap(root, other.root);
        return *this;
    }
    constexpr ~BasicTreap(){
        Node::del(root);
    }


    constexpr size_t size(){
        return root?root->size:0;
    }
    constexpr T get_one(){
        assert(size()==1);
        get(current)
        assert(!current_z and !current_x);
        return current->value.value;
    }

    struct iter{
        Node* current = 0;
        bool is_end = 0;
        constexpr auto& operator+=(std::ptrdiff_t add){
            if (is_end){
                add +=1;
                is_end = 0;
            }
            if (Node::add(current, add)){
                current = Node::add(current, add);
            }else{
                current = Node::add(current, add-1);
                is_end = 1;
            }
            return *this;
        }
        constexpr auto&operator-=(std::ptrdiff_t add){
            return (*this)+=-add;
        }
        constexpr auto&operator++(){
            return (*this)+=1;
        }
        constexpr auto&operator--(){
            return (*this)+=-1;
        }
        constexpr auto get_offset(){
            return Node::find_offset(root)+is_end;
        }
    };

    constexpr iter begin(){
        if (root){
            auto left = root->get_child(0);
            iter t{root};
            t-= left ? left->size : 0;
            return t;
        }else{
            return {0,1};
        }
    }
    constexpr iter end(){
        return begin()+size();
    }
};

constexpr auto operator-(auto q, auto e)->decltype(q.get_offset()-e.get_offset()){
    return q.get_offset()-e.get_offset();
}

constexpr auto operator==(auto q, auto e)->decltype(q.current == e.current){
    return q.current == e.current and q.is_end == e.is_end;
}

constexpr auto operator+(auto q, auto e)->std::decay_t<decltype(q+=e)>{
    return q+=e;
}


constexpr auto operator-(auto q, auto e)->std::decay_t<decltype(q-=e)>{
    return q-=e;
}

template<typename T>
constexpr auto& operator<<(BasicTreap<T>&left, BasicTreap<T>&right){
    left.root = BasicTreap<T>::Node::merge(left.root, right.root);
    right.root = nullptr;
    return left;
}

constexpr auto operator<=>(auto q,auto e)->decltype(q.get_offset() - e.get_offset() <=> 0){
    return q.get_offset() - e.get_offset() <=> 0;
}

template<typename T>
constexpr auto& operator>>(BasicTreap<T>&left, BasicTreap<T>&right){
    right.root = BasicTreap<T>::Node::merge({left.root, right.root}, 1);
    left.root = nullptr;
    return right;
}

template<typename T,typename UNARY_PREDICATE>
constexpr BasicTreap<T> operator|(BasicTreap<T>&left, UNARY_PREDICATE&& is_right){
    auto pair = BasicTreap<T>::Node::template split<1>(left.current, std::forward<UNARY_PREDICATE>(is_right));
    left.current = pair[0];
    return BasicTreap<T>(pair[1]);
}

template<typename T,typename UNARY_PREDICATE>
constexpr BasicTreap<T> operator|(UNARY_PREDICATE&& is_left, BasicTreap<T>&right){
    auto pair = BasicTreap<T>::Node::split(right.current,std::forward<UNARY_PREDICATE>(is_left));
    right.current = pair[1];
    return BasicTreap<T>(pair[0]);
}

struct item{
    ll v=0;
    ll rev=0;
    ll first=0;
    ll last=0;
    ll sum=0;
    ll to_add=0;
    ll to_mul=1;
    ll is_increasing=1;
    ll is_decreasing=1;
    constexpr static auto to_string(auto root,auto&&cout){
    #define _put(size) cout<<#size<<" = "<<root->size<<' ';

            _put(value.value.v)
            _put(priority)
            _put(value.value.first)
            _put(value.value.last)
            _put(value.value.to_mul)
            _put(value.value.to_add)
        return "";
    }
    constexpr static void make(auto t){
        if (t->value.value.to_mul!=1){
            (t->value.value.sum)*=t->value.value.to_mul;
            (t->value.value.first)*=t->value.value.to_mul;
            (t->value.value.last)*=t->value.value.to_mul;
            (t->value.value.v)*=t->value.value.to_mul;
            if (t->children[0]){
                (t->children[0]->value.value.to_mul)*=t->value.value.to_mul;
                (t->children[0]->value.value.to_add)*=t->value.value.to_mul;
            }
            if (t->children[1]){
                (t->children[1]->value.value.to_mul)*=t->value.value.to_mul;
                (t->children[1]->value.value.to_add)*=t->value.value.to_mul;
            }
            if (t->value.value.to_mul==0){
                (t->value.value.is_increasing)=1;
                (t->value.value.is_decreasing)=1;
            }
            (t->value.value.to_mul)=1;
        }

        if (t->value.value.to_add!=0){
            (t->value.value.sum)+=t->value.value.to_add*t->size;
            (t->value.value.first)+=t->value.value.to_add;
            (t->value.value.last)+=t->value.value.to_add;
            (t->value.value.v)+=t->value.value.to_add;
            if (t->children[0]){
                (t->children[0]->value.value.to_add)+=t->value.value.to_add;
            }
            if (t->children[1]){
                (t->children[1]->value.value.to_add)+=t->value.value.to_add;
            }
            (t->value.value.to_add)=0;
        }

        if (t->value.value.rev){
            auto te=(t->children[0]);
            (t->children[0])=(t->children[1]);
            (t->children[1])=te;
            ll tl=(t->value.value.is_increasing);
            (t->value.value.is_increasing)=(t->value.value.is_decreasing);
            (t->value.value.is_decreasing)=tl;
            tl=(t->value.value.first);
            (t->value.value.first)=(t->value.value.last);
            (t->value.value.last)=tl;
            if (t->children[0]){
                (t->children[0]->value.value.rev)^=1;
            }
            if (t->children[1]){
                (t->children[1]->value.value.rev)^=1;
            }
            (t->value.value.rev)=0;
        }
    }
    constexpr static void update(auto t){
        if (t->children[0]){
            if (t->children[0]->value.value.rev){
                (t->value.value.first)=t->children[0]->value.value.last*t->children[0]->value.value.to_mul+t->children[0]->value.value.to_add;
            }else{
                (t->value.value.first)=t->children[0]->value.value.first*t->children[0]->value.value.to_mul+t->children[0]->value.value.to_add;
            }
        }else{
            (t->value.value.first)=t->value.value.v;
        }
        if (t->children[1]){
            if (t->children[1]->value.value.rev){
                (t->value.value.last)=t->children[1]->value.value.first*t->children[1]->value.value.to_mul+t->children[1]->value.value.to_add;
            }else{
                (t->value.value.last)=t->children[1]->value.value.last*t->children[1]->value.value.to_mul+t->children[1]->value.value.to_add;
            }
        }else{
            (t->value.value.last)=t->value.value.v;
        }
        if (t->children[0] and t->children[1]){
            ll s_z_increasing=(t->children[0]->value.value.to_mul==0 or (t->children[0]->value.value.rev?t->children[0]->value.value.is_decreasing:t->children[0]->value.value.is_increasing));
            ll s_z_decreasing=(t->children[0]->value.value.to_mul==0 or (t->children[0]->value.value.rev?t->children[0]->value.value.is_increasing:t->children[0]->value.value.is_decreasing));
            ll s_x_increasing=(t->children[1]->value.value.to_mul==0 or (t->children[1]->value.value.rev?t->children[1]->value.value.is_decreasing:t->children[1]->value.value.is_increasing));
            ll s_x_decreasing=(t->children[1]->value.value.to_mul==0 or (t->children[1]->value.value.rev?t->children[1]->value.value.is_increasing:t->children[1]->value.value.is_decreasing));
            ll s_x_first=(t->children[1]->value.value.rev?t->children[1]->value.value.last:t->children[1]->value.value.first)*t->children[1]->value.value.to_mul+t->children[1]->value.value.to_add;
            ll s_z_last=(t->children[0]->value.value.rev?t->children[0]->value.value.first:t->children[0]->value.value.last)*t->children[0]->value.value.to_mul+t->children[0]->value.value.to_add;
            ll s_z_s=t->children[0]->size;
            ll s_x_s=t->children[1]->size;
            (t->value.value.sum)=t->value.value.v + t->children[0]->value.value.sum*t->children[0]->value.value.to_mul+t->children[0]->value.value.to_add*t->children[0]->size + t->children[1]->value.value.sum*t->children[1]->value.value.to_mul+t->children[1]->value.value.to_add*t->children[1]->size;
            (t->value.value.is_increasing)=(s_z_increasing and s_x_increasing and s_z_last<=t->value.value.v and s_x_first>=t->value.value.v);
            (t->value.value.is_decreasing)=(s_z_decreasing and s_x_decreasing and s_z_last>=t->value.value.v and s_x_first<=t->value.value.v);
        }else
        if (t->children[0]){
            ll s_x_increasing=1;
            ll s_x_decreasing=1;
            ll s_z_increasing=(t->children[0]->value.value.to_mul==0 or (t->children[0]->value.value.rev?t->children[0]->value.value.is_decreasing:t->children[0]->value.value.is_increasing));
            ll s_z_decreasing=(t->children[0]->value.value.to_mul==0 or (t->children[0]->value.value.rev?t->children[0]->value.value.is_increasing:t->children[0]->value.value.is_decreasing));
            ll s_z_last=(t->children[0]->value.value.rev?t->children[0]->value.value.first:t->children[0]->value.value.last)*t->children[0]->value.value.to_mul+t->children[0]->value.value.to_add;
            ll s_x_first=0;
            ll s_z_s=t->children[0]->size;
            ll s_x_s=0;
            (t->value.value.sum)=t->value.value.v + t->children[0]->value.value.sum*t->children[0]->value.value.to_mul+t->children[0]->value.value.to_add*t->children[0]->size;
            (t->value.value.is_increasing)=(s_z_increasing and s_z_last<=t->value.value.v);
            (t->value.value.is_decreasing)=(s_z_decreasing and s_z_last>=t->value.value.v);
        }else
        if (t->children[1]){
            ll s_z_increasing=1;
            ll s_z_decreasing=1;
            ll s_x_increasing=(t->children[1]->value.value.to_mul==0 or (t->children[1]->value.value.rev?t->children[1]->value.value.is_decreasing:t->children[1]->value.value.is_increasing));
            ll s_x_decreasing=(t->children[1]->value.value.to_mul==0 or (t->children[1]->value.value.rev?t->children[1]->value.value.is_increasing:t->children[1]->value.value.is_decreasing));
            ll s_x_first=(t->children[1]->value.value.rev?t->children[1]->value.value.last:t->children[1]->value.value.first)*t->children[1]->value.value.to_mul+t->children[1]->value.value.to_add;
            ll s_z_last=0;
            ll s_z_s=0;
            ll s_x_s=t->children[1]->size;
            (t->value.value.sum)=t->value.value.v + t->children[1]->value.value.sum*t->children[1]->value.value.to_mul+t->children[1]->value.value.to_add*t->children[1]->size;
            (t->value.value.is_increasing)=(s_x_increasing and s_x_first>=t->value.value.v);
            (t->value.value.is_decreasing)=(s_x_decreasing and s_x_first<=t->value.value.v);
        }else{
            (t->value.value.sum)=t->value.value.v;
            (t->value.value.is_increasing)=1;
            (t->value.value.is_decreasing)=1;
        }
    }
    constexpr static void check(auto size){
        assert(size->value.value.rev<2);
        if (size->children[0]){
            if (size->children[0]->value.value.rev){
                assert(size->value.value.first==size->children[0]->value.value.last*size->children[0]->value.value.to_mul+size->children[0]->value.value.to_add);
            }else{
                assert(size->value.value.first==size->children[0]->value.value.first*size->children[0]->value.value.to_mul+size->children[0]->value.value.to_add);
            }
        }else{
            assert(size->value.value.first==size->value.value.v);
        }
        if (size->children[1]){
            if (size->children[1]->value.value.rev){
                assert(size->value.value.last==size->children[1]->value.value.first*size->children[1]->value.value.to_mul+size->children[1]->value.value.to_add);
            }else{
                assert(size->value.value.last==size->children[1]->value.value.last*size->children[1]->value.value.to_mul+size->children[1]->value.value.to_add);
            }
        }else{
            assert(size->value.value.last==size->value.value.v);
        }
        if (size->children[0] and size->children[1]){
            assert((size->value.value.is_increasing)==((size->children[0]->value.value.to_mul==0 or (size->children[0]->value.value.rev?size->children[0]->value.value.is_decreasing:size->children[0]->value.value.is_increasing)) and (size->children[1]->value.value.to_mul==0 or (size->children[1]->value.value.rev?size->children[1]->value.value.is_decreasing:size->children[1]->value.value.is_increasing)) and (size->children[0]->value.value.rev?size->children[0]->value.value.first:size->children[0]->value.value.last)*size->children[0]->value.value.to_mul+size->children[0]->value.value.to_add<=size->value.value.v and (size->children[1]->value.value.rev?size->children[1]->value.value.last:size->children[1]->value.value.first)*size->children[1]->value.value.to_mul+size->children[1]->value.value.to_add>=size->value.value.v));
            assert((size->value.value.is_decreasing)==((size->children[0]->value.value.to_mul==0 or (size->children[0]->value.value.rev?size->children[0]->value.value.is_increasing:size->children[0]->value.value.is_decreasing)) and (size->children[1]->value.value.to_mul==0 or (size->children[1]->value.value.rev?size->children[1]->value.value.is_increasing:size->children[1]->value.value.is_decreasing)) and (size->children[0]->value.value.rev?size->children[0]->value.value.first:size->children[0]->value.value.last)*size->children[0]->value.value.to_mul+size->children[0]->value.value.to_add>=size->value.value.v and (size->children[1]->value.value.rev?size->children[1]->value.value.last:size->children[1]->value.value.first)*size->children[1]->value.value.to_mul+size->children[1]->value.value.to_add<=size->value.value.v));
        }else
        if (size->children[0]){
            assert(size->value.value.sum==size->value.value.v + size->children[0]->value.value.sum*size->children[0]->value.value.to_mul+size->children[0]->value.value.to_add*size->children[0]->size);
            assert((size->value.value.is_increasing)==((size->children[0]->value.value.to_mul==0 or (size->children[0]->value.value.rev?size->children[0]->value.value.is_decreasing:size->children[0]->value.value.is_increasing)) and (size->children[0]->value.value.rev?size->children[0]->value.value.first:size->children[0]->value.value.last)*size->children[0]->value.value.to_mul+size->children[0]->value.value.to_add<=size->value.value.v));
            assert((size->value.value.is_decreasing)==((size->children[0]->value.value.to_mul==0 or (size->children[0]->value.value.rev?size->children[0]->value.value.is_increasing:size->children[0]->value.value.is_decreasing)) and (size->children[0]->value.value.rev?size->children[0]->value.value.first:size->children[0]->value.value.last)*size->children[0]->value.value.to_mul+size->children[0]->value.value.to_add>=size->value.value.v));
        }else
        if (size->children[1]){
            assert(size->value.value.sum==size->value.value.v + size->children[1]->value.value.sum*size->children[1]->value.value.to_mul+size->children[1]->value.value.to_add*size->children[1]->size);
            assert((size->value.value.is_increasing)==((size->children[1]->value.value.to_mul==0 or (size->children[1]->value.value.rev?size->children[1]->value.value.is_decreasing:size->children[1]->value.value.is_increasing)) and (size->children[1]->value.value.rev?size->children[1]->value.value.last:size->children[1]->value.value.first)*size->children[1]->value.value.to_mul+size->children[1]->value.value.to_add>=size->value.value.v));
            assert((size->value.value.is_decreasing)==((size->children[1]->value.value.to_mul==0 or (size->children[1]->value.value.rev?size->children[1]->value.value.is_increasing:size->children[1]->value.value.is_decreasing)) and (size->children[1]->value.value.rev?size->children[1]->value.value.last:size->children[1]->value.value.first)*size->children[1]->value.value.to_mul+size->children[1]->value.value.to_add<=size->value.value.v));
        }else{
            assert(size->value.value.sum==size->value.value.v);
            assert(size->value.value.is_increasing==1);
            assert(size->value.value.is_decreasing==1);
        }
    }
};

struct treap:BasicTreap<item>{
    constexpr BasicTreap cut_left(ll n){
        auto tmp=[n](Node* t)mutable{
            get(t);
            if (n>=t_z_s+1){
                n-=t_z_s+1;
                return true;
            }
            return false;
        };
        return tmp | *this;
    }
    constexpr BasicTreap cut_right(ll n){
        auto tmp=[n](Node* t)mutable{
            get(t);
            if (n>=t_x_s+1){
                n-=t_x_s+1;
                return true;
            }
            return false;
        };
        return *this | tmp;
    }
    constexpr auto update(ll n,BasicTreap& _q){
        assert(_q.size()==1);
        auto priority=cut_left(n);
        auto r=cut_left(1);
        _q >> *this;
        // add_left(_q);
        priority >> *this;
        // add_left(priority);
        return r;
    }
    void constexpr push_back(const ll&a){
        current=(Node::merge({current,new auto(Node(rand,0,item{a}))},1));
    }
    constexpr void push_front(const ll&a){
        current=(Node::merge({new auto(Node(rand,0,item{a})),current},1));
    }
    ll constexpr pop_back(){
        auto q=cut_right(1);
        return q.get_one().v;
    }
    ll constexpr pop_front(){
        auto q=cut_left(1);
        return q.get_one().v;
    }
    constexpr treap cut_decreasing_suffix(){
        Node::check(current);
        auto f = [prev=0?current:nullptr](Node* t)mutable{
            get(t);
            if ((not t_x or t_x->value.value.is_decreasing and t->value.value.v>=t_x->value.value.first) and (not prev or (t_x?t_x->value.value.last:t->value.value.v)>=prev->value.value.v)){
                prev=t;
                return false;
            }
            return true;
        };
        auto tmp = Node::split(current, f);
        current=tmp[1];
        return {tmp[0]};
    }
    constexpr treap cut_increasing_suffix(){
        Node::check(current);
        auto f = [prev=0?current:nullptr](Node* t)mutable{
            get(t);
            if ((not t_x or t_x->value.value.is_increasing and t->value.value.v<=t_x->value.value.first) and (not prev or (t_x?t_x->value.value.last:t->value.value.v)<=prev->value.value.v)){
                prev=t;
                return false;
            }
            return true;
        };
        auto tmp = Node::split(current, f);
        current=tmp[1];
        return {tmp[0]};
    }
    treap cut_upper_bound(ll r){
        auto f=[&](Node* t){
            return t->value.value.v<=r;
        };
        auto tmp = Node::split(current, f);
        current=tmp[1];
        return {tmp[0]};
    }
    constexpr treap cut_less_than_in_increasing(ll r){
        auto f=[&](Node* t){
            return t->value.value.v<=r;
        };
        auto tmp = Node::split(current, f);
        current=tmp[1];
        return {tmp[0]};
    }
    constexpr treap cut_greater_than_in_decreasing(ll r){
        auto f=[&](Node* t){
            return t->value.value.v>=r;
        };
        auto tmp = Node::split(current, f);
        current=tmp[1];
        return {tmp[0]};
    }
    constexpr void next_permutation(){
        auto t=cut_decreasing_suffix();
        if (t.size()){
            auto r=t.cut_right(1);
            rev();
            auto tmp=cut_less_than_in_increasing(r.current->value.value.v);
            r=update(0,r);
            r >> t >> tmp >> *this;
        }else{
            rev();
        }
    }
    constexpr void prev_permutation(){
        auto t=cut_increasing_suffix();
        if (t.size()){
            auto r=t.cut_right(1);
            rev();
            auto tmp=cut_greater_than_in_decreasing(r.current->value.value.v);
            r=update(0,r);
            r >> t >> tmp >> *this;
        }else{
            rev();
        }
    }
    constexpr ll sum(){
        if (current){
            return current->value.value.sum;
        }else{
            return 0;
        }
    }
    constexpr void add(ll v){
        if (current){
            (current->value.value.to_add)+=v;
            Node::make(current);
        }
    }
    constexpr void mul(ll v){
        if (current){
            (current->value.value.to_mul)*=v;
            Node::make(current);
        }
    }
    constexpr void rev(){
        if (current){
            (current->value.value.rev)^=1;
            Node::make(current);
        }
    }

};







// struct treap{
//     constexpr treap(){}
//     void push_back(ll){}
//     void push_front(ll){}
//     void next_permutation(){}
//     void prev_permutation(){}
//     void add(ll){}
//     ll sum(){}
//     ll size(){}
//     treap update(ll,treap){}
//     // BasicTreap<item>::Node*current=nullptr;
// };



