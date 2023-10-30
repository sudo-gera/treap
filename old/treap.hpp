#include <array>


#  ifndef TREAP_STD_VER
#    if __cplusplus <= 201103L
#      define TREAP_STD_VER 11
#    elif __cplusplus <= 201402L
#      define TREAP_STD_VER 14
#    elif __cplusplus <= 201703L
#      define TREAP_STD_VER 17
#    elif __cplusplus <= 202002L
#      define TREAP_STD_VER 20
#    else
#      define TREAP_STD_VER 23
#    endif
#  endif // TREAP_STD_VER

#  if TREAP_STD_VER > 11
#    define TREAP_CONSTEXPR_AFTER_CXX11 constexpr
#  else
#    define TREAP_CONSTEXPR_AFTER_CXX11
#  endif

#  if TREAP_STD_VER > 14
#    define TREAP_CONSTEXPR_AFTER_CXX14 constexpr
#  else
#    define TREAP_CONSTEXPR_AFTER_CXX14
#  endif

#  if TREAP_STD_VER > 17
#    define TREAP_CONSTEXPR_AFTER_CXX17 constexpr
#  else
#    define TREAP_CONSTEXPR_AFTER_CXX17
#  endif

struct FastRand{
    std::size_t state = 0x123456;
    TREAP_CONSTEXPR_AFTER_CXX17 std::size_t operator()(){
        state = state << 3 ^ state >> 11;
        state = state >> 5 ^ state << 13;
        state = state << 7 ^ state >> 17;
        return state;
    }
};
template<typename T>
struct BasicTreap{
private:
    struct Node{
        std::size_t priority_=0;
        std::array<Node*, 2> children_ = {nullptr,nullptr};
        Node *parent_ = nullptr;
        std::size_t size_ = 1;
        std::size_t height_ = 1;
        T value_;

        template<typename...CON_ARGS>
        TREAP_CONSTEXPR_AFTER_CXX17 Node(FastRand& rand, CON_ARGS&&...args):
            priority_(rand()),
            value_(std::forward<CON_ARGS&&>(args)...)
        {update(this);}

        TREAP_CONSTEXPR_AFTER_CXX17 Node* get_child(bool num){
            make(children_[num]);
            return children_[num];
        }

        TREAP_CONSTEXPR_AFTER_CXX17 static void make(Node*root){
            if (root){
                T::make(root);
            }
        }

        static void TREAP_CONSTEXPR_AFTER_CXX17 update(Node*root){
            if (root){
                std::size_t tmp[4] = {0,0,0,0};
                for (std::size_t q=0;q<2;++q){
                    if (root->children_[q]){
                        auto& child = root->children_[q];
                        child->parent_ = root;
                        tmp[q] = child->height_;
                        tmp[q+2] = child->size_;
                    }
                }
                root->height_ = tmp[0] > tmp[1] ? tmp[0] : tmp[1];
                root->size_ = tmp[2] + 1 + tmp[3];
                T::update(root);
            }
        }

        static TREAP_CONSTEXPR_AFTER_CXX17 void del(Node* q){
            if (not q){
                return;
            }
            del(q->children_[0]);
            del(q->children_[1]);
            delete q;
        }

        static TREAP_CONSTEXPR_AFTER_CXX17 Node* merge(std::array<Node*, 2> roots, bool args_order_is_not_reversed){
            if (not roots[0]) {return roots[1];}
            if (not roots[1]) {return roots[0];}
            auto new_root = roots[0]->priority_ < roots[1]->priority_;
            auto child = new_root ^ args_order_is_not_reversed;
            roots[new_root]->children_[child] = merge({roots[new_root]->get_child(child), roots[new_root ^ 1]}, child);
            update(roots[new_root]);
            return roots[new_root];
        }

        template<bool condition_tells_if_is_right=0,typename COND>
        static TREAP_CONSTEXPR_AFTER_CXX17 std::array<Node*, 2> split(Node* root, COND&&condition){
            if (not root){return {nullptr,nullptr};}
            bool is_left = condition_tells_if_is_right ^ bool(condition(root));
            auto child = root->get_child(is_left);
            auto tmp = split<condition_tells_if_is_right>(child, std::forward<COND>(condition));
            root->children_[is_left] = tmp[is_left^1];
            update(root);
            tmp[is_left^1] = root;
            if (tmp[is_left]){
                tmp[is_left]->parent_ = nullptr;
            }
            return tmp;
        }

        struct PrintNode{
            std::size_t value_ = 0;
            std::size_t save = 0;
            PrintNode* next = nullptr;
        };

        template<typename OSTREAM>
        static void print(Node* root, OSTREAM& ss, PrintNode* start = nullptr, PrintNode* prev_prev_node = nullptr){
            if (not root){
                return;
            }
            if (not start){
                PrintNode prev1{3};
                PrintNode prev2{0, 0, &prev1};
                print(root, ss, &prev1, &prev2);
                return;
            }
            prev_prev_node->save = prev_prev_node->value_;
            if (prev_prev_node->value_ == prev_prev_node->next->value_){
                prev_prev_node->value_ = 0;
            }
            PrintNode node;
            prev_prev_node->next->next = &node;
            node.value_ = 2;
            print(root->children_[0], ss, start, prev_prev_node->next);
            for (PrintNode* n = start; n != &node; n = n->next){
                char const* strs[] = {" ", " ", "┃", "┗", "┃", "┏", "", ""};
                ss << strs[n->value_ * 2 + (n == prev_prev_node->next)];
            }
            char const* strs[] = {"━", "┳", "┻", "╋"};
            ss << strs[bool(root->children_[0]) * 2 + bool(root->children_[1])] << "► ";
            T::to_string(root, ss);
            ss << "\n";
            node.value_ = 1;
            print(root->children_[1], ss, start, prev_prev_node->next);
            prev_prev_node->value_ = prev_prev_node->save;
        }

        static TREAP_CONSTEXPR_AFTER_CXX17 std::pair<Node*, bool> add(Node* current, std::ptrdiff_t offset){
            if (not current){return {nullptr, 0};}
            auto left = current->get_child(0);
            auto right = current->get_child(1);
            std::size_t left_size = left ? left->size_ : 0;
            std::size_t right_size = right ? right->size_ : 0;
            while (offset and current){
                if (offset > (std::ptrdiff_t)right_size or -offset > (std::ptrdiff_t)left_size){
                    if (not current->parent_){
                        return {current, 1};
                        // current = nullptr;
                        // break;
                    }else if (current == current->parent_->children_[0]){
                        offset -= right_size + 1;
                        left = current;
                        current = current->parent_;
                        right = current -> get_child(1);
                        left_size = left->size_;
                        right_size = right ? right->size_ : 0;
                    }else if (current == current->parent_->children_[1]){
                        offset += left_size + 1;
                        right = current;
                        current = current->parent_;
                        left = current -> get_child(0);
                        left_size = left ? left->size_ : 0;
                        right_size = right->size_;
                    }
                }else if (offset < 0){
                    current = left;
                    right = current->get_child(1);
                    right_size = right ? right->size_ : 0;
                    left = current -> get_child(0);
                    left_size = left ? left->size_ : 0;
                    offset += right_size + 1;
                }else{
                    current = right;
                    left = current->get_child(0);
                    left_size = left ? left->size_ : 0;
                    right = current -> get_child(1);
                    right_size = right ? right ->size_ : 0;
                    offset -= left_size + 1;
                }
            }
            return {current, 0};
        }

        static TREAP_CONSTEXPR_AFTER_CXX17 std::ptrdiff_t root_offset(Node* current){
            if (not current){
                return 0;
            }
            std::ptrdiff_t ans = 0;
            while (current->parent_){
                if (current->parent_->children_[0] == current){
                    auto right = current -> get_child(1);
                    auto right_size = right ? right ->size_ : 0;
                    ans -= right_size + 1;
                }else
                if (current->parent_->children_[1] == current){
                    auto left = current->get_child(0);
                    auto left_size = left ? left->size_ : 0;
                    ans += left_size + 1;
                }
                current=current->parent_;
            }
            return ans;
        }
    };



    Node* root_ = nullptr;
    FastRand rand_;
public:
    TREAP_CONSTEXPR_AFTER_CXX17 BasicTreap(Node* root = nullptr):
        root_(root)
    {}

    TREAP_CONSTEXPR_AFTER_CXX17 BasicTreap(BasicTreap&& other)noexcept{
        std::swap(root_, other.root_);
    }

    TREAP_CONSTEXPR_AFTER_CXX17 auto&operator=(BasicTreap&& other)noexcept{
        std::swap(root_, other.root_);
        return *this;
    }

    TREAP_CONSTEXPR_AFTER_CXX17 void clear(){
        Node::del(root_);
        root_ = nullptr;
    }

    TREAP_CONSTEXPR_AFTER_CXX17 ~BasicTreap(){
        clear();
    }

    TREAP_CONSTEXPR_AFTER_CXX17 std::size_t size()const{
        Node::make(root_);
        return root_ ? root_->size_ : 0;
    }

    TREAP_CONSTEXPR_AFTER_CXX17 bool empty()const{
        return not root_;
    }

    auto& print(std::ostream& out) const{
        Node::print(root_, out);
        return out;
    }
private:
    struct Iter{
        using difference_type = std::ptrdiff_t;
        using value_type = const T;
        using pointer = const T*;
        using reference = const T&;
        using iterator_category = std::random_access_iterator_tag;
        using param = T;
        Node* current_ = 0;
        bool is_end_ = 0;
    public:
        TREAP_CONSTEXPR_AFTER_CXX17 auto tuple()const{
            return make_tuple(current_, is_end_);
        }
        TREAP_CONSTEXPR_AFTER_CXX17 const T& operator*()const{
            return current_->value_;
        }
        TREAP_CONSTEXPR_AFTER_CXX17 auto operator->()const{
            return &**this;
        }
        TREAP_CONSTEXPR_AFTER_CXX17 const T& operator[](std::ptrdiff_t offset)const{
            return *(*this+offset);
        }
        TREAP_CONSTEXPR_AFTER_CXX17 auto& operator+=(std::ptrdiff_t offset){
            if (not current_){
                return *this;
            }
            if (is_end_){
                offset +=1;
                is_end_ = 0;
            }
            auto tmp = Node::add(current_, offset);
            if (not tmp.second){
                current_ = tmp.first;
            }else{
                current_ = tmp.first;
                auto child = current_->get_child(1);
                auto child_size = child ? child->size_ : 0;
                current_ = Node::add(current_, child_size).first;
                is_end_ = 1;
            }
            return *this;
        }
        TREAP_CONSTEXPR_AFTER_CXX17 auto&operator-=(std::ptrdiff_t add){
            return this[0]+=-add;
        }
        TREAP_CONSTEXPR_AFTER_CXX17 auto&operator++(){
            return this[0]+=1;
        }
        TREAP_CONSTEXPR_AFTER_CXX17 auto&operator--(){
            return this[0]+=-1;
        }
        TREAP_CONSTEXPR_AFTER_CXX17 auto&operator++(int){
            auto tmp = *this;
            this[0]++;
            return tmp;
        }
        TREAP_CONSTEXPR_AFTER_CXX17 auto&operator--(int){
            auto tmp = *this;
            this[0]--;
            return tmp;
        }
        TREAP_CONSTEXPR_AFTER_CXX17 auto root_offset()const{
            return Node::root_offset(current_)+is_end_;
        }
        TREAP_CONSTEXPR_AFTER_CXX17 static auto make_iter(Node* current, bool is_end){
            return Iter{current, is_end};
        }

        friend TREAP_CONSTEXPR_AFTER_CXX17 auto operator+(Iter left, std::ptrdiff_t right){
            return left+=right;
        }

        friend TREAP_CONSTEXPR_AFTER_CXX17 auto operator+(std::ptrdiff_t left, Iter right){
            return right+=left;
        }

        friend TREAP_CONSTEXPR_AFTER_CXX17 auto operator-(Iter left, std::ptrdiff_t right){
            return left-=right;
        }

        friend TREAP_CONSTEXPR_AFTER_CXX17 auto operator-(Iter left, Iter right){
            return left.root_offset() - right.root_offset();
        }

        friend TREAP_CONSTEXPR_AFTER_CXX17 bool operator==(Iter left, Iter right){
            return left.current_ == right.current_ and left.is_end_ == right.is_end_;
        }

        friend TREAP_CONSTEXPR_AFTER_CXX17 bool operator!=(Iter left, Iter right){
            return not (left == right);
        }

        friend TREAP_CONSTEXPR_AFTER_CXX17 auto operator<(Iter left, Iter right){
            return left - right < 0;
        }

        friend TREAP_CONSTEXPR_AFTER_CXX17 auto operator>(Iter left, Iter right){
            return left - right > 0;
        }

        friend TREAP_CONSTEXPR_AFTER_CXX17 auto operator<=(Iter left, Iter right){
            return left - right <= 0;
        }

        friend TREAP_CONSTEXPR_AFTER_CXX17 auto operator>=(Iter left, Iter right){
            return left - right >= 0;
        }
    };
public:

    TREAP_CONSTEXPR_AFTER_CXX17 Iter root()const{
        return Iter::make_iter(root_, empty());
        // return {root_, empty()};
    }

private:
    template<bool end>
    TREAP_CONSTEXPR_AFTER_CXX17 auto get_iter()const{
        auto it = root();
        if (not empty()){
            Node::make(root_);
            auto child = root_->get_child(end);
            if TREAP_CONSTEXPR_AFTER_CXX17(end){
                it += child ? child->size_ : 0;
            }else{
                it -= child ? child->size_ : 0;
            }
            it += end;
        }
        return it;
    }
public:
    TREAP_CONSTEXPR_AFTER_CXX17 auto cbegin()const{
        return get_iter<0>();
    }
    TREAP_CONSTEXPR_AFTER_CXX17 auto cend()const{
        return get_iter<1>();
    }
    TREAP_CONSTEXPR_AFTER_CXX17 auto begin()const{
        return get_iter<0>();
    }
    TREAP_CONSTEXPR_AFTER_CXX17 auto end()const{
        return get_iter<1>();
    }
    TREAP_CONSTEXPR_AFTER_CXX17 auto rcbegin()const{
        return std::make_reverse_iterator(get_iter<1>());
    }
    TREAP_CONSTEXPR_AFTER_CXX17 auto rcend()const{
        return std::make_reverse_iterator(get_iter<0>());
    }
    TREAP_CONSTEXPR_AFTER_CXX17 auto rbegin()const{
        return std::make_reverse_iterator(get_iter<1>());
    }
    TREAP_CONSTEXPR_AFTER_CXX17 auto rend()const{
        return std::make_reverse_iterator(get_iter<0>());
    }
    template<typename...CON_ARGS>
    TREAP_CONSTEXPR_AFTER_CXX17 void emplace_root(CON_ARGS&&...args){
        clear();
        root_ = new Node(rand_, std::forward<CON_ARGS>(args)...);
    }

    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = T&;
    using const_reference = const T&;
    using pointer = T*;
    using const_pointer = const T*;
    using iterator = Iter;
    using const_iterator = Iter;
    using reverse_iterator = std::reverse_iterator<Iter>;
    using const_reverse_iterator = const std::reverse_iterator<Iter>;

    template<typename UNARY_PREDICATE>
    friend TREAP_CONSTEXPR_AFTER_CXX17 BasicTreap operator|(BasicTreap&left, UNARY_PREDICATE&& is_right){
        auto pair = BasicTreap::Node::template split<1>(left.root_, std::forward<UNARY_PREDICATE>(is_right));
        left.root_ = pair[0];
        return BasicTreap<T>(pair[1]);
    }

    template<typename UNARY_PREDICATE>
    friend TREAP_CONSTEXPR_AFTER_CXX17 BasicTreap operator|(UNARY_PREDICATE&& is_left, BasicTreap&right){
        auto pair = BasicTreap::Node::template split<0>(right.root_, std::forward<UNARY_PREDICATE>(is_left));
        right.root_ = pair[1];
        return BasicTreap<T>(pair[0]);
    }

    friend auto& operator<<(std::ostream&out, BasicTreap&treap){
        return treap.print(out);
    }

    friend TREAP_CONSTEXPR_AFTER_CXX17 auto& operator<<(BasicTreap&left, BasicTreap&right){
        left.root_ = Treap::Node::merge({left.root_, right.root_}, true);
        right.root_ = nullptr;
        return left;
    }

    friend TREAP_CONSTEXPR_AFTER_CXX17 auto& operator<<(BasicTreap&left, BasicTreap&&right){
        left.root_ = Treap::Node::merge({left.root_, right.root_}, true);
        right.root_ = nullptr;
        return left;
    }

    friend TREAP_CONSTEXPR_AFTER_CXX17 auto& operator>>(BasicTreap&left, BasicTreap&right){
        right.root_ = Treap::Node::merge({left.root_, right.root_}, true);
        left.root_ = nullptr;
        return right;
    }

    friend TREAP_CONSTEXPR_AFTER_CXX17 auto& operator>>(BasicTreap&&left, BasicTreap&right){
        right.root_ = Treap::Node::merge({left.root_, right.root_}, true);
        left.root_ = nullptr;
        return right;
    }

    const auto& back()const{
        return *rbegin();
    }

    const auto& front()const{
        return *begin();
    }

private:
    friend TREAP_CONSTEXPR_AFTER_CXX17 auto operator==(const BasicTreap& left, const BasicTreap& right){
        return left.size() == right.size() and std::equal(left.begin(), left.end(), right.begin());
    }

    friend TREAP_CONSTEXPR_AFTER_CXX17 auto operator<(const BasicTreap& left, const BasicTreap& right){
        return std::lexicographical_compare(left.begin(), left.end(), right.begin(), right.end());
    }

    friend TREAP_CONSTEXPR_AFTER_CXX17 auto operator>(const BasicTreap& left, const BasicTreap& right){
        return right < left;
    }

    friend TREAP_CONSTEXPR_AFTER_CXX17 auto operator!=(const BasicTreap& left, const BasicTreap& right){
        return not (left == right);
    }

    friend TREAP_CONSTEXPR_AFTER_CXX17 auto operator<=(const BasicTreap& left, const BasicTreap& right){
        return not (left > right);
    }

    friend TREAP_CONSTEXPR_AFTER_CXX17 auto operator>=(const BasicTreap& left, const BasicTreap& right){
        return not (left < right);
    }
};

