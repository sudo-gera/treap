#include "treap.hpp"
#include <bits/stdc++.h>
using namespace std;

constexpr size_t constexpr_seed(){
    size_t seed = 0;
    const char* str = __TIME__ __DATE__ __TIMESTAMP__;
    while (str[0]){
        seed *= 67;
        seed += str++[0];
    }
    return seed;
}

struct item{
    size_t value=0;
    constexpr item(size_t val=0):
        value(val){}
    constexpr item(item&&other)noexcept{
        swap(value, other.value);
    }
    constexpr item(const item&&other)noexcept{
        value = other.value;
    }
    constexpr auto&operator=(item&&other){
        swap(value, other.value);
        return *this;
    }
    constexpr auto&operator=(const item&&other){
        value = other.value;
        return *this;
    }
    constexpr static void make(void*){}
    constexpr static void update(void*){}
    template<typename NODE, typename OSTREAM>
    constexpr static void to_string(NODE* node,OSTREAM& ss){
        ss << node->value_.value;
    }
    constexpr operator size_t()const{
        return value;
    }
};

template<size_t b = 0, size_t e = 16, typename FUNC>
constexpr decltype(auto) make_size_t_constexpr(FUNC&& f, size_t n){
    assert(b <= n and n < e);
    if constexpr(b + 1 == e){
        return f(std::integral_constant<size_t, b>());
    }else if (n < (b + e) / 2){
        return make_size_t_constexpr<b, (b + e) / 2>(f, n);
    }else{
        return make_size_t_constexpr<(b + e) / 2, e>(f, n);
    }
}

struct null_out{};

template<typename T>
constexpr null_out operator<<(null_out,T&&){
    return {};
}

constexpr void check(bool b){
    if (not b){
        throw ""[b^1];
        // throw std::exception();
    }
}

template<typename T>
constexpr auto cmp(T&& left, T&& right){
    return (left > right) - (left < right);
}

template<typename OSTREAM>
TREAP_CONSTEXPR_AFTER_CXX17 bool test(bool run, OSTREAM&&out, size_t seed){
    if (not run){return true;}
    vector<
        pair<
            BasicTreap<item>, 
            vector<
                BasicTreap<item>::iterator
            >
        >
    > treaps(1);
    treaps[0].second.push_back(treaps[0].first.begin());
    decltype(treaps)::value_type tp;
    vector<vector<size_t>> vectors(1);
    size_t nums[] = {
    };
    // auto rand = [&,n=0]()mutable{return nums[n++];};
    FastRand rand{seed};
    size_t op, value, current=0;
    auto operations = make_tuple(
        [&]{
            treaps.emplace_back();
            vectors.emplace_back();
            auto& treap = treaps.back();
            auto& vector = vectors.back();
            for (size_t q=0;q<9;++q){
                auto tmp = std::move(treap.first);
                treap.first.emplace_root(value);
                if (value % 2){
                    treap.first << tmp;
                }else{
                    tmp >> treap.first;
                }
                value >>= 1;
            }
            for (auto it = treap.first.begin(); it != treap.first.end(); ++it){
                treap.second.push_back(it);
                vector.push_back(it->value);
            }
            treap.second.push_back(treap.first.end());
            check(treap.first.size() == 9);
        },
        [&]{
            treaps.emplace_back();
            auto& treap = treaps[current];
            vectors.emplace_back();
            auto& vector = vectors[current];
            check(treap.first.size() == vector.size());
            size_t n = value % (treap.first.size() + 1);
            treaps.back().first = [n](auto t)mutable{
                auto child = t->get_child(0);
                auto child_size = child ? child->size_ : 0;
                if (n >= child_size + 1){
                    n -= child_size + 1;
                    return true;
                }
                return false;
            } | treap.first;
            copy(vector.begin(), vector.begin() + n, back_insert_iterator(vectors.back()));
            rotate(vector.begin(), vector.begin() + n, vector.end());
            vector.resize(vector.size() - n);
            n += n + 1 == treap.second.size();
            check(n <= treap.second.size());
            copy(treap.second.begin(), treap.second.begin() + n, back_inserter(treaps.back().second));
            rotate(treap.second.begin(), treap.second.begin() + n, treap.second.end());
            treap.second.resize(treap.second.size() - n);
            if (treap.second.empty()){
                treap.second.push_back(treap.first.end());
            }else{
                treaps.back().second.push_back(treaps.back().first.end());
            }
            current = treaps.size() - 1;
        },
        [&]{
            treaps.emplace_back();
            auto& treap = treaps[current];
            vectors.emplace_back();
            auto& vector = vectors[current];
            check(treap.first.size() == vector.size());
            size_t n = value % (treap.first.size() + 1);
            treaps.back().first = treap.first | [n](auto t)mutable{
                auto child = t->get_child(1);
                auto child_size = child ? child->size_ : 0;
                if (n >= child_size + 1){
                    n -= child_size + 1;
                    return true;
                }
                return false;
            };
            copy(vector.end() - n, vector.end(), back_inserter(vectors.back()));
            vector.resize(vector.size() - n);
            n += 1;
            check(n <= treap.second.size());
            if (n == 1){
                treaps.back().second.push_back(treaps.back().first.begin());
            }else{
                copy(treap.second.end() - n, treap.second.end(), back_inserter(treaps.back().second));
                treap.second.resize(treap.second.size() - n);
                treap.second.push_back(treap.first.end());
            }
            current = treaps.size() - 1;
        },
        [&]{
            current = value % treaps.size();
        },
        [&]{
            auto index = value % treaps.size();
            check(
                cmp(vectors[current], vectors[index])
             == 
                cmp(treaps[current].first, treaps[index].first)
            );
        },
        [&]{
            if (treaps.size() > 1){
                auto index = value % treaps.size();
                index ^= index == current;
                index %= treaps.size();
                check(current < treaps.size());
                check(index < treaps.size());
                treaps[index].first >> treaps[current].first;
                if (treaps[current].second.size() == 1){
                    treaps[current].second = std::move(treaps[index].second);
                }else{
                    treaps[index].second.pop_back();
                    treaps[current].second.insert(
                        treaps[current].second.begin(),
                        treaps[index].second.begin(),
                        treaps[index].second.end()
                    );
                }
                treaps[index].second.clear();
                treaps[index].second.push_back(treaps[index].first.begin());
                vectors[current].insert(
                    vectors[current].begin(),
                    vectors[index].begin(),
                    vectors[index].end()
                );
                vectors[index].clear();
                check(treaps[index].first.size() + 1 == treaps[index].second.size());
                check(treaps[index].first.size() == vectors[index].size());
                check(treaps[current].first.size() + 1 == treaps[current].second.size());
                check(treaps[current].first.size() == vectors[current].size());
                treaps.erase(treaps.begin() + index);
                vectors.erase(vectors.begin() + index);
                current -= current == treaps.size();
            }
        },
        [&]{
            if (treaps.size() > 1){
                auto index = value % treaps.size();
                index ^= index == current;
                index %= treaps.size();
                check(current < treaps.size());
                check(index < treaps.size());
                treaps[current].first << treaps[index].first;
                if (treaps[current].second.size() == 1){
                    treaps[current].second = std::move(treaps[index].second);
                }else if (treaps[index].second.size() != 1){
                    treaps[current].second.pop_back();
                    treaps[current].second.insert(
                        treaps[current].second.end(),
                        treaps[index].second.begin(),
                        treaps[index].second.end()
                    );
                }
                treaps[index].second.clear();
                treaps[index].second.push_back(treaps[index].first.begin());
                vectors[current].insert(
                    vectors[current].end(),
                    vectors[index].begin(),
                    vectors[index].end()
                );
                vectors[index].clear();
                check(treaps[index].first.size() + 1 == treaps[index].second.size());
                check(treaps[index].first.size() == vectors[index].size());
                check(treaps[current].first.size() + 1 == treaps[current].second.size());
                check(treaps[current].first.size() == vectors[current].size());
                treaps.erase(treaps.begin() + index);
                vectors.erase(vectors.begin() + index);
                current -= current == treaps.size();
            }
        }
        // [&]{
        //     current = value % treaps.size();
        // }
    );
    for (size_t q = 0; q < 32; ++q){
        op = rand() % (tuple_size_v<decltype(operations)> + 2);
        value = rand();
        // out << treaps[current].first;
        out << q << " " << current << " " << op << " " << value << " " << treaps.size() << " " << treaps[current].first.size() << "\n";
        // out << op << ", " << value << ",\n";
        make_size_t_constexpr([&](auto op){
            if constexpr (op < tuple_size_v<decltype(operations)>){
                get<op>(operations)();
            }else{
                get<tuple_size_v<decltype(operations)>-1-op%2>(operations)();
            }
        }, op);
        // out << treaps[current].first;
        check(vectors.size() == treaps.size());
        for (size_t current = 0; current < treaps.size(); ++current){
            auto& treap = treaps[current];
            auto& vector = vectors[current];
            check(current < vectors.size());
            check(treap.first.size() == vector.size());
            check(treap.first.size() + 1 == treap.second.size());
            for (size_t q = 0; q < treap.second.size(); ++q){
                for (size_t w = 0; w < treap.second.size(); ++w){
                    // ic(current, treap.second.size(), q, w)
                    // ic(treap.first.size())
                    auto d = q - w;
                    check(treap.second[q] - treap.second[w] == d);
                    check(q >= w or treap.second[q]  < treap.second[w]);
                    check(q <= w or treap.second[q]  > treap.second[w]);
                    check(q == w or treap.second[q] != treap.second[w]);
                    check(q >  w or treap.second[q] <= treap.second[w]);
                    check(q <  w or treap.second[q] >= treap.second[w]);
                    check(q != w or treap.second[q] == treap.second[w]);
                    // ic(treap.second[q], treap.second[q]+-d)
                    // ic(treap.second[w])
                    check(treap.second[q] + -d == treap.second[w]);
                    check(treap.second[q] -  d == treap.second[w]);
                }
            }
            auto it = treap.first.begin();
            for (size_t q = 0; q < vector.size(); ++q){
                check(it->value == vector[q]);
                check(it == treap.second[q]);
                check(&*it == &*treap.second[q]);
                for (size_t w = 0; w < treap.second.size(); ++w){
                    check(&*it == &treap.second[w][q-w]);
                }
                ++it;
            }
            check(it == treap.first.end());
            check(it == treap.second.back());
        }
    }
    return true;
}
#  if TREAP_STD_VER > 17
    static_assert(test(1, null_out{}, constexpr_seed()));
#  endif

int main(){
    size_t seed = 1678390810;
    seed = constexpr_seed();
    cout << seed << endl;
    seed = random_device()();
    cout << seed << endl;
    test(1, cout, seed);
    // test(1, null_out{}, seed);
}
