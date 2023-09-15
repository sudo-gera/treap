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
    constexpr static void make(auto){}
    constexpr static void update(auto){}
    constexpr static void to_sting(auto,auto&){}
};

template<size_t b = 0,size_t e = 16>
constexpr decltype(auto) make_size_t_constexpr(auto&& f, size_t n){
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

constexpr bool test(bool run, auto&&out, size_t seed){
    if (not run){return true;}
    vector<
        pair<
            Treap<item>, 
            vector<
                Treap<item>::iterator
            >
        >
    > treaps(1);
    treaps[0].second.push_back(treaps[0].first.begin());
    decltype(treaps)::value_type tp;
    vector<vector<size_t>> vectors(1);
    fast_rand rand{seed};
    size_t op, value, current=0;
    auto operations = make_tuple(
        [&]{
            treaps.emplace_back();
            auto& treap = treaps.back();
            treap.second.push_back(treap.first.begin());
            check(treap.first.begin() == treap.first.end());
            current = treaps.size() - 1;
            vectors.emplace_back();
        },
        [&]{
            treaps.emplace_back();
            auto& treap = treaps.back();
            treap.first.emplace_root(value);
            treap.second.push_back(treap.first.begin());
            treap.second.push_back(treap.first.end());
            check(treap.first.begin() != treap.first.end());
            check(treap.first.begin() + 1 == treap.first.end());
            current = treaps.size() - 1;
            vectors.push_back({value});
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
            n += n == treap.first.size();
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
            copy(treap.second.end() - n, treap.second.end(), back_inserter(treaps.back().second));
            treap.second.resize(treap.second.size() - n);
            treap.second.push_back(treap.first.end());
            current = treaps.size() - 1;
        },
        [&]{
            
        },
        [&]{
            current = value % treaps.size();
        }
    );
    for (size_t q = 0; q < 512; ++q){
        op = rand() % tuple_size_v<decltype(operations)>;
        value = rand();
        out << current << " " << op << " " << value << "\n";
        make_size_t_constexpr([&](auto op){
            if constexpr (op < tuple_size_v<decltype(operations)>){
                get<op>(operations)();
            }
        }, op);
        auto& treap = treaps[current];
        auto& vector = vectors[current];
        check(vectors.size() == treaps.size());
        check(current < vectors.size());
        check(treap.first.size() == vector.size());
        check(treap.first.size() + 1 == treap.second.size());
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
        for (size_t q = 0; q < treap.second.size(); ++q){
            for (size_t w = 0; w < treap.second.size(); ++w){
                auto d = q - w;
                check(treap.second[q] - treap.second[w] == d);
                check(q >= w or treap.second[q]  < treap.second[w]);
                check(q <= w or treap.second[q]  > treap.second[w]);
                check(q == w or treap.second[q] != treap.second[w]);
                check(q >  w or treap.second[q] <= treap.second[w]);
                check(q <  w or treap.second[q] >= treap.second[w]);
                check(q != w or treap.second[q] == treap.second[w]);
                check(treap.second[q] + -d == treap.second[w]);
                check(treap.second[q] -  d == treap.second[w]);
            }
        }
        // Treap<item> another;
        // check (treap.first >= another);
        // check (another <= treap.first);
        // for (size_t q = 0; q < vector.size(); ++q){
        //     another <<= 
        // }
    }
    return true;
}

static_assert(test(1, null_out{}, constexpr_seed()));

int main(){
    size_t seed = 484357768;
    seed = constexpr_seed();
    // seed = random_device()();
    cout << seed << endl;
    // test(1, cout, seed);
    test(1, null_out{}, seed);
}
