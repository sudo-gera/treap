#include <array>

struct fast_rand{
    std::size_t state = 0x123456;
    constexpr std::size_t operator()(){
        state = state << 3 ^ state >> 11;
        state = state >> 5 ^ state << 13;
        state = state << 7 ^ state >> 17;
        return state;
    }
};
template<typename T>
struct Treap{
private:

    struct Node{
        std::size_t priority_=0;
        std::array<Node*, 2> children_ = {nullptr,nullptr};
        Node *parent_ = nullptr;
        std::size_t size_ = 1;
        std::size_t height_ = 1;
        T value_;

        template<typename...CON_ARGS>
        constexpr Node(fast_rand& rand, CON_ARGS&&...args):
            priority_(rand()),
            value_(std::forward<CON_ARGS&&>(args)...)
        {update(this);}

        constexpr Node* get_child(bool num){
            make(children_[num]);
            return children_[num];
        }

        constexpr static void make(Node*root){
            if (root){
                T::make(root);
            }
        }

        static void constexpr update(Node*root){
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

        static constexpr void del(Node* q){
            if (not q){
                return;
            }
            del(q->children_[0]);
            del(q->children_[1]);
            delete q;
        }

        static constexpr Node* merge(std::array<Node*, 2> roots, bool args_order_is_not_reversed){
            if (not roots[0]) {return roots[1];}
            if (not roots[1]) {return roots[0];}
            auto new_root = roots[0]->priority_ < roots[1]->priority_;
            auto child = new_root ^ args_order_is_not_reversed;
            roots[new_root]->children_[child] = merge({roots[new_root]->get_child(child), roots[new_root ^ 1]}, child);
            update(roots[new_root]);
            return roots[new_root];
        }

        template<bool condition_tells_if_is_right=0,typename COND>
        static constexpr std::array<Node*, 2> split(Node* root, COND&&condition){
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

        struct print_node{
            std::size_t value_ = 0;
            std::size_t save = 0;
            print_node* next = nullptr;
        };

        template<typename OSTREAM>
        static void print(Node* root, OSTREAM& ss, print_node* start = nullptr, print_node* prev_prev_node = nullptr){
            if (not root){
                return;
            }
            if (not start){
                print_node prev1{3};
                print_node prev2{0, 0, &prev1};
                print(root, ss, &prev1, &prev2);
                return;
            }
            prev_prev_node->save = prev_prev_node->value_;
            if (prev_prev_node->value_ == prev_prev_node->next->value_){
                prev_prev_node->value_ = 0;
            }
            print_node node;
            prev_prev_node->next->next = &node;
            node.value_ = 2;
            print(root->children_[0], ss, start, prev_prev_node->next);
            for (print_node* n = start; n != &node; n = n->next){
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

        static constexpr Node* add(Node* current, std::ptrdiff_t offset){
            if (not current){return nullptr;}
            auto left = current->get_child(0);
            auto right = current->get_child(1);
            std::size_t left_size = left ? left->size_ : 0;
            std::size_t right_size = right ? right->size_ : 0;
            while (offset and current){
                if (offset > (std::ptrdiff_t)right_size or -offset > (std::ptrdiff_t)left_size){
                    if (not current->parent_){
                        current = nullptr;
                        break;
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
            return current;
        }

        static constexpr std::ptrdiff_t root_offset(Node* current){
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
    fast_rand rand_;
public:
    constexpr Treap(Node* root = nullptr):
        root_(root)
    {}

    constexpr Treap(Treap&& other)noexcept{
        std::swap(root_, other.root_);
    }

    constexpr auto&operator=(Treap&& other)noexcept{
        std::swap(root_, other.root_);
        return *this;
    }

    constexpr void clear(){
        Node::del(root_);
        root_ = nullptr;
    }

    constexpr ~Treap(){
        clear();
    }

    constexpr std::size_t size()const{
        Node::make(root_);
        return root_ ? root_->size_ : 0;
    }

    constexpr bool empty()const{
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
        constexpr const T& operator*()const{
            return current_->value_;
        }
        constexpr auto operator->()const{
            return &**this;
        }
        constexpr const T& operator[](std::ptrdiff_t offset)const{
            return *(*this+offset);
        }
        constexpr auto& operator+=(std::ptrdiff_t offset){
            if (is_end_){
                offset +=1;
                is_end_ = 0;
            }
            if (Node::add(current_, offset)){
                current_ = Node::add(current_, offset);
            }else{
                current_ = Node::add(current_, offset-1);
                is_end_ = 1;
            }
            return *this;
        }
        constexpr auto&operator-=(std::ptrdiff_t add){
            return this[0]+=-add;
        }
        constexpr auto&operator++(){
            return this[0]+=1;
        }
        constexpr auto&operator--(){
            return this[0]+=-1;
        }
        constexpr auto&operator++(int){
            auto tmp = *this;
            this[0]++;
            return tmp;
        }
        constexpr auto&operator--(int){
            auto tmp = *this;
            this[0]--;
            return tmp;
        }
        constexpr auto root_offset()const{
            return Node::root_offset(current_)+is_end_;
        }
        constexpr static auto make_iter(Node* current, bool is_end){
            return Iter{current, is_end};
        }

        friend constexpr auto operator+(Iter left, std::ptrdiff_t right){
            return left+=right;
        }

        friend constexpr auto operator+(std::ptrdiff_t left, Iter right){
            return right+=left;
        }

        friend constexpr auto operator-(Iter left, std::ptrdiff_t right){
            return left-=right;
        }

        friend constexpr auto operator-(Iter left, Iter right){
            return left.root_offset() - right.root_offset();
        }

        friend constexpr bool operator==(Iter left, Iter right){
            return left.current_ == right.current_ and left.is_end_ == right.is_end_;
        }

        friend constexpr auto operator<=>(Iter left, Iter right){
            return left - right <=> 0;
        }

    };
public:

    template<typename ITER>
    struct IsIteratorImpl{
        static constexpr bool value_ = std::is_same_v<
            std::decay_t<Iter>, ITER
        >;
    };

    template<typename ITER>
    static constexpr bool IsIterator = IsIteratorImpl<ITER>::value_;

    constexpr Iter root()const{
        return Iter::make_iter(root_, empty());
        // return {root_, empty()};
    }

private:
    template<bool end>
    constexpr auto get_iter()const{
        auto it = root();
        if (not empty()){
            Node::make(root_);
            auto child = root_->get_child(end);
            if constexpr(end){
                it += child ? child->size_ : 0;
            }else{
                it -= child ? child->size_ : 0;
            }
            it += end;
        }
        return it;
    }
public:
    constexpr auto cbegin()const{
        return get_iter<0>();
    }
    constexpr auto cend()const{
        return get_iter<1>();
    }
    constexpr auto begin()const{
        return get_iter<0>();
    }
    constexpr auto end()const{
        return get_iter<1>();
    }
    constexpr auto rcbegin()const{
        return std::make_reverse_iterator(get_iter<1>());
    }
    constexpr auto rcend()const{
        return std::make_reverse_iterator(get_iter<0>());
    }
    constexpr auto rbegin()const{
        return std::make_reverse_iterator(get_iter<1>());
    }
    constexpr auto rend()const{
        return std::make_reverse_iterator(get_iter<0>());
    }
    template<typename...CON_ARGS>
    constexpr void emplace_root(CON_ARGS&&...args){
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

    template<typename UnaryPredicate>
    friend constexpr Treap operator|(Treap&left, UnaryPredicate&& is_right){
        auto pair = Treap::Node::template split<1>(left.root_, std::forward<UnaryPredicate>(is_right));
        left.root_ = pair[0];
        return Treap<T>(pair[1]);
    }

    template<typename UnaryPredicate>
    friend constexpr Treap operator|(UnaryPredicate&& is_left, Treap&right){
        auto pair = Treap::Node::template split<0>(right.root_, std::forward<UnaryPredicate>(is_left));
        right.root_ = pair[1];
        return Treap<T>(pair[0]);
    }

    friend auto& operator<<(std::ostream&out, Treap&treap){
        return treap.print(out);
    }

    friend constexpr auto& operator<<(Treap&left, Treap&right){
        left.root_ = Treap::Node::merge({left.root_, right.root_}, true);
        right.root_ = nullptr;
        return left;
    }

    friend constexpr auto& operator<<(Treap&left, Treap&&right){
        left.root_ = Treap::Node::merge({left.root_, right.root_}, true);
        right.root_ = nullptr;
        return left;
    }

    friend constexpr auto& operator>>(Treap&left, Treap&right){
        right.root_ = Treap::Node::merge({left.root_, right.root_}, true);
        left.root_ = nullptr;
        return right;
    }

    friend constexpr auto& operator>>(Treap&&left, Treap&right){
        right.root_ = Treap::Node::merge({left.root_, right.root_}, true);
        left.root_ = nullptr;
        return right;
    }

    const auto& last()const{
        return *rbegin();
    }

    const auto& first()const{
        return *begin();
    }

    friend constexpr auto operator<=>(const Treap&left, const Treap& right){
        for (iterator left_it = left.begin(), right_it = right.begin();
            left_it != left.end() and right_it != right.end();
            ++left_it , ++right_it
        ){
            auto diff = *left_it <=> *right_it;
            if (diff != 0){
                return diff;
            }
        }
        return left.size() <=> right.size();
    }
};


