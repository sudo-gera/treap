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
    constexpr static void to_string(auto node,auto&ss){
        ss << node->value_.value;
    }
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
    size_t nums[] = {
        4, 14418056304197111324,
        6, 5811580654458012234,
        4, 718808072108537838,
        4, 10566337935053845080,
        4, 8006852448307657392,
        3, 4367132734686408082,
        0, 15322892075650789689,
        2, 17660987595728913762,
        2, 18142770897153192430,
        4, 8107925458599154748,
        1, 6933125715451374827,
        6, 910399713590780398,
        6, 11415177753321554256,
        2, 10634269614053354643,
        0, 16626348121602761208,
        7, 17519407205871855116,
        5, 13655061312738843411,
        7, 3894730679575826935,
        6, 11531075928346213613,
        5, 17494754833904919234,
        3, 5747878201528166666,
        6, 4407851752358848778,
        5, 1494248366951844402,
        0, 16084385263072113060,
        3, 3196461694209715185,
        2, 6336665629887030473,
        1, 7502415298447005816,
        4, 1055266556064249564,
        2, 12961596365313303524,
        5, 15748419271946797331,
        5, 11843912937072622554,
        5, 1577906459019975327,
        6, 14563649749031165464,
        5, 635179492536476997,
        3, 11532844193924414808,
        1, 10183461298410523241,
        3, 16631929415355925035,
        5, 5749833091598515285,
        0, 16792384127347817424,
        7, 10648242994638814847,
        2, 5359551148373195250,
        0, 11749915945192153589,
        0, 2242869250502902681,
        0, 17728715127741252560,
        7, 17926326562860499005,
        5, 839733995863213542,
        5, 12590088271425941725,
        4, 9868121936950509646,
        4, 17705850552886695069,
        4, 17026399601266333324,
        3, 14481806511776294417,
        4, 3146649696958159072,
        4, 9352703707008762698,
        7, 18035802267406135415,
        1, 18441135232600252038,
        5, 12123589178719024735,
    };
    auto rand = [&,n=0]()mutable{
        return nums[n++];
    };
    // fast_rand rand{seed};
    size_t op, value, current=0;
    auto operations = make_tuple(
        // [&]{
        //     treaps.emplace_back();
        //     auto& treap = treaps.back();
        //     treap.second.push_back(treap.first.begin());
        //     check(treap.first.begin() == treap.first.end());
        //     current = treaps.size() - 1;
        //     vectors.emplace_back();
        // },
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
            copy(treap.second.end() - n, treap.second.end(), back_inserter(treaps.back().second));
            treap.second.resize(treap.second.size() - n);
            treap.second.push_back(treap.first.end());
            current = treaps.size() - 1;
        },
        [&]{
            if (treaps.size() > 1){
                auto index = value % treaps.size();
                index ^= index == current;
                index %= treaps.size();
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
            }
        },
        [&]{
            if (treaps.size() > 1){
                auto index = value % treaps.size();
                index ^= index == current;
                index %= treaps.size();
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
            }
        },
        [&]{
            if (treaps.size() > 1){
                auto index = value % treaps.size();
                index ^= index == current;
                index %= treaps.size();
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
            }
        },
        [&]{
            if (treaps.size() > 1){
                auto index = value % treaps.size();
                index ^= index == current;
                index %= treaps.size();
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
            }
        },
        [&]{
            if (treaps.size() > 1){
                auto index = value % treaps.size();
                index ^= index == current;
                index %= treaps.size();
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
            }
        }
        // [&]{
        //     current = value % treaps.size();
        // }
    );
    for (size_t q = 0; q < 64; ++q){
        op = rand() % tuple_size_v<decltype(operations)>;
        value = rand();
        // out << treaps[current].first;
        out << current << " " << op << " " << value << "\n";
        // out << op << ", " << value << ",\n";
        make_size_t_constexpr([&](auto op){
            if constexpr (op < tuple_size_v<decltype(operations)>){
                get<op>(operations)();
            }
        }, op);
        // out << treaps[current].first;
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

// static_assert(test(1, null_out{}, constexpr_seed()));

int main(){
    size_t seed = 2114134053;
    // seed = constexpr_seed();
    // seed = random_device()();
    cout << seed << endl;
    test(1, cout, seed);
    // test(1, null_out{}, seed);
}
