#undef assert
#define assert assert_f
#include "treap_.hpp"
#include <bits/stdc++.h>
#undef assert
#define assert assert_f

constexpr size_t make_seed(){
    char const* str = __DATE__ __TIME__ __TIMESTAMP__;
    size_t hash=0;
    while (str[0]){
        hash *= 257;
        hash += str[0];
        ++str;
    }
    return hash;
}

using namespace std;

constexpr void assert_f(bool c){
    char f=""[1-c];
}


template<typename T>
constexpr void check(T q,T e,size_t len){
    vector<T> a;
    a.resize(len);
    // ic("00000000")
    size_t la=0;
    for (auto w=q;w!=e;++w){
        // ic(w.root)
        a[la++]=w;
        assert(la<len);
        // a.push_back(w);
    }
    a[la++]=e;
    // a.push_back(e);
    for (size_t w=0;w<a.size();++w){
        for (size_t e=0;e<a.size();++e){
            long d=long(w)-long(e);
            assert(a[w]-a[e]==d);
            assert(w>=e or  a[w]<a[e]);
            assert(w<=e or  a[w]>a[e]);
            assert(w==e or a[w]!=a[e]);
            assert(w>e  or a[w]<=a[e]);
            assert(w<e  or a[w]>=a[e]);
            assert(w!=e or a[w]==a[e]);
            assert(a[w]+(-d)==a[e]);
            assert(a[w]-(d)==a[e]);
        }
    }
    auto s=a;
    s.clear();
    s.resize(a.size());
    size_t ls=0;
    for (auto w=e;w!=q;--w){
        s[ls++]=w;
        assert(ls<len);
        // s.push_back(w);
    }
    s[ls++]=q;
    // s.push_back(q);
    reverse(s.begin(),s.end());
    // cout << a.size() << " " << s.size() << endl;
    assert(a==s);
}

constexpr auto cut_left(auto&d,size_t n){
    decay_t<decltype(d)> t;
    for (size_t q=0;q<n;++q){
        t.push_back(move(d.front()));
        d.pop_front();
    }
    return t;
}

constexpr auto cut_right(auto&d,size_t n){
    decay_t<decltype(d)> t;
    for (size_t q=0;q<n;++q){
        t.push_front(move(d.back()));
        d.pop_back();
    }
    return t;
}

#define av_len 256
// #define av deque
template<typename T>
struct av{
    // union Opt{
    //     constexpr Opt(){}
    //     constexpr Opt(T&&y):
    //         value(move(y)){}
    //     constexpr Opt(Opt&&o){

    //     }
    //     char empty;
    //     T value;
    //     constexpr ~Opt(){}
    // };
    // Opt a[av_len];
    array<optional<T>,av_len> a;
    size_t size_=0;
    constexpr void emplace_back(auto&&...args){
        a[size_++].emplace(std::forward<decltype(args)>(args)...);
    }
    constexpr void push_back(auto&&args){
        a[size_++].emplace(std::forward<decltype(args)>(args));
    }
    constexpr void push_front(auto&&args){
        push_back(std::forward<decltype(args)>(args));
        for (size_t q=size_-1;q;--q){
            swap(a[q-1],a[q]);
        }
    }
    // constexpr T* data(){
    //     return (T*)a;
    // }
    struct iter{
        using iterator_category = random_access_iterator_tag;
        using difference_type = ptrdiff_t;
        using value_type = T;
        using reference = T&;
        using pointer = T*;
        optional<T>* current;
        constexpr auto&operator* (){
            return **current;
        }
        constexpr auto&operator+=(ssize_t len){
            current+=len;
            return *this;
        }
        constexpr auto operator-(iter oth){
            return current - oth.current;
        }
        constexpr auto operator==(iter oth){
            return current == oth.current;
        }
        constexpr auto operator<=>(iter oth){
            return current <=> oth.current;
        }
        constexpr auto operator--(){
            return (*this)+=-1;
        }
        constexpr auto operator++(){
            return (*this)+=1;
        }
        
    };
    constexpr auto begin(){
        return iter{a.data()};
    }
    constexpr auto size(){
        return size_;
    }
    constexpr auto end(){
        auto tmp = begin();
        tmp+=size();
        return tmp;
    }
    constexpr auto&operator[](size_t n){
        return *(a[n]);
    }
};

using llu=long long unsigned;

template<llu b=0,llu e=1024>
constexpr decltype(auto) call(auto&&f, llu n){
    assert(b<=n and n<e);
    if constexpr(b+1==e){
        return f(std::integral_constant<llu,b>());
    }else if (n<(b+e)/2){
        return call<b,(b+e)/2>(f,n);
    }else{
        return call<(b+e)/2,e>(f,n);
    }
}

struct con_stream{
    constexpr auto&operator<<(auto&&arg){
        if (not is_constant_evaluated()){
            std::cout<<arg;
        }
        return *this;
    }
};

// #define av deque

constexpr bool test(bool _r,auto seed){
    if (not _r){
        return true;
    }
    vector<treap> a_vec;
    a_vec.emplace_back();
    vector<av<long>> s_vec;
    s_vec.emplace_back();
    auto a=&a_vec[0];
    auto s=&s_vec[0];
    // size_t seed=139361832;
    auto cout = con_stream();
    // seed=random_device()();
    // seed = make_seed();
    // auto rand=mt19937(seed);
    auto rand=FastRand(seed);
    cout<<seed<<"\n";
    auto g=[&](size_t q,size_t w,size_t e,size_t r,size_t t,size_t y,bool h=0)constexpr{
        auto f = make_tuple(
            [&]{
                cout<<"// "<<__FILE__<<":"<<__LINE__<<"\n";
                a->push_back(e);
                s->push_back(e);
            },
            // [&]{
            //     cout<<"// "<<__FILE__<<":"<<__LINE__<<"\n";
            //     if (a->size()){
            //         a->pop_back();
            //         s->pop_back();
            //     }
            // },
            [&]{
                cout<<"// "<<__FILE__<<":"<<__LINE__<<"\n";
                a->push_front(e);
                s->push_front(e);
            },
            // [&]{
            //     cout<<"// "<<__FILE__<<":"<<__LINE__<<"\n";
            //     if (a->size()){
            //         a->pop_front();
            //         s->pop_front();
            //     }
            // },
            // [&]{
            //     cout<<"// "<<__FILE__<<":"<<__LINE__<<"\n";
            //     if (a->size()){
            //         // assert(a[0][e%a->size()]==s[0][e%s->size()]);
            //     }
            // },
            // [&]{
            //     cout<<"// "<<__FILE__<<":"<<__LINE__<<"\n";
            //     y=a-a_vec.data();
            //     a_vec.emplace_back();
            //     s_vec.emplace_back();
            //     a=a_vec.data()+y;
            //     s=s_vec.data()+y;
            // },
            // [&]{
            //     cout<<"// "<<__FILE__<<":"<<__LINE__<<"\n";
            //     a_vec.push_back(move(*a));
            //     s_vec.push_back(move(*s));
            //     a=a_vec.data()+a_vec.size()-1;
            //     s=s_vec.data()+s_vec.size()-1;
            // },
            // [&]{
            //     cout<<"// "<<__FILE__<<":"<<__LINE__<<"\n";
            //     a_vec.push_back(a->cut_left(e%(a->size()+1)));
            //     s_vec.push_back(cut_left(*s,e%(s->size()+1)));
            //     a=a_vec.data()+a_vec.size()-1;
            //     s=s_vec.data()+s_vec.size()-1;
            // },
            // [&]{
            //     cout<<"// "<<__FILE__<<":"<<__LINE__<<"\n";
            //     a_vec.push_back(a->cut_right(e%(a->size()+1)));
            //     s_vec.push_back(cut_right(*s,e%(s->size()+1)));
            //     a=a_vec.data()+a_vec.size()-1;
            //     s=s_vec.data()+s_vec.size()-1;
            // },
            // [&]{
            //     cout<<"// "<<__FILE__<<":"<<__LINE__<<"\n";
            //     a=a_vec.data()+e%a_vec.size();
            //     s=s_vec.data()+e%s_vec.size();
            // },
            [&]{
                cout<<"// "<<__FILE__<<":"<<__LINE__<<"\n";
                a->next_permutation();
                next_permutation(s->begin(), s->end());
            },
            [&]{
                cout<<"// "<<__FILE__<<":"<<__LINE__<<"\n";
                a->prev_permutation();
                prev_permutation(s->begin(), s->end());
            },
            [&]{
                cout<<"// "<<__FILE__<<":"<<__LINE__<<"\n";
                for (auto&d:*s){
                    d+=e;
                }
                a->add(e);
            },
            [&]{
                cout<<"// "<<__FILE__<<":"<<__LINE__<<"\n";
                // ic(*a)
                // a->root->print(a->root,cout);
                // ic(s->a)
                size_t z=0;
                for (auto&d:*s){
                    z+=d;
                }
                assert(z==a->sum());
            }
        );
        cout<<"g("<<q<<"LLU, "<<w%tuple_size_v<decltype(f)><<"LLU, "<<e<<"LLU, "<<r<<"LLU, "<<t<<"LLU, "<<y<<"LLU);"<<"\n";
        // a->out();
        call([&](auto n){
            if constexpr(n<tuple_size_v<decltype(f)>){
                get<n>(f)();
            }
        },w%tuple_size_v<decltype(f)>);

        // f[w%f.size()]();
        // ic(*s)
        assert(a->size()==s->size());
        for (size_t q=0;q<a->size();++q){
            treap tmp;
            tmp.push_back(0);
            auto t=a[0].update(q,tmp);
            // assert(t.current->value.value.v==s[0][q]);
            a[0].update(q,t);
        }
        // treap::Node::check(a->current);
        // ic()
        // if (q==4){
            // ic("+++++++++++++++++++++++==")
            // cout << a->size() << endl;
            // decay_t<decltype(*a)>::Node::print(a->root, cout);
            // ic()
            // auto _t = a->begin();
            // ic(_t.is_end)
            // _t += a->size();
            // ic(_t.is_end)
            // cout << a->begin().root << " " << a->end().root << endl;
            // cout << a->begin().is_end << " " << a->end().is_end << endl;
            // ic(21212121212121212)
            check(a->begin(),a->end(), s->size()+1);
        // }
    };
    for (size_t q=0;q<48;++q){
        size_t w=rand(),e=rand(),r=rand(),t=rand(),y=rand();
        g(q,w,e,r,t,y,1);
    }
    // for (size_t q=0;q<64;++q){
    //     a->push_back(rand());
    // }
    // stringstream ss;
    // a->current->print(a->current,ss);
    // cout<<ss.str();
    return true;
}


constexpr bool test1(bool k){
    if (k){
        treap r;
        r.push_back(0);
        r.cut_left(0);
        // treap::Node r(1,2);
    }
    return true;
}

template<size_t seed>
struct print_seed{
    tuple_element_t<
        seed&1|1,
        tuple<void,int>
    > f(){}
};

print_seed<make_seed()> g;

static_assert(test(1,make_seed()));
auto h=g.f();

int main(){
    // test(1,random_device()());
    // test(1,123);
}


void j(){







// #define prime 509

// #define co 40



// struct B {int d=0; B(B&&){} explicit B(int f):d(f){} };

// int main_(){
// 	long sr=1653272014;
// 	sr=time(0);
// 	std::cout<<"seed = "<<sr<<std::"\n";
// 	srand(sr);
// 	treap<long> e({1,2,3,4,5});
// 	auto y=e;

// 	auto q=treap<long>({1,2,3});
// 	auto w=q;
// 	assert(q==w);
// 	assert(vector<long>(q)==vector<long>({1,2,3}));
// 	assert(q.pop_back()==3);
// 	assert(vector<long>(q)==vector<long>({1,2}));
// 	assert(q.pop_front()==1);
// 	assert(vector<long>(q)==vector<long>({2}));
// 	assert(q.pop_front()==2);
// 	assert(vector<long>(q)==vector<long>({}));
// 	q.push_back(2);
// 	assert(vector<long>(q)==vector<long>({2}));
// 	w.push_back(4);
// 	assert(vector<long>(w)==vector<long>({1,2,3,4}));
// 	w.push_front(0);
// 	assert(vector<long>(w)==vector<long>({0,1,2,3,4}));
// 	assert(w<q);
// 	assert(treap<long>(long(12))>treap<long>(long(1)));
// 	w.insert(0,-1);
// 	assert(vector<long>(w)==vector<long>({-1,0,1,2,3,4}));
// 	w.insert(w.size(),6);
// 	assert(vector<long>(w)==vector<long>({-1,0,1,2,3,4,6}));
// 	w.insert(w.size()-1,5);
// 	assert(vector<long>(w)==vector<long>({-1,0,1,2,3,4,5,6}));
// 	assert(vector<long>(w)==vector<long>({-1,0,1,2,3,4,5,6}));
// 	q.erase(0);
// 	assert(q.empty());
// 	swap(q,w);
// 	assert(w.empty());
// 	assert(vector<long>(q)==vector<long>({-1,0,1,2,3,4,5,6}));
// 	swap(q,w);
// 	q.insert(0,1);
// 	q.insert(0,0);
// 	q.insert(2,2);
// 	swap(q,w);
// 	assert(vector<long>(q)==vector<long>({-1,0,1,2,3,4,5,6}));
// 	assert(vector<long>(w)==vector<long>({0,1,2}));

// 	check(e.begin(),e.end());
// 	assert(vector<long>(e)==vector<long>({1,2,3,4,5}));
// 	e.rev();
// 	assert(vector<long>(e)==vector<long>({5,4,3,2,1}));
// 	e.rev();
// 	assert(vector<long>(e)==vector<long>({1,2,3,4,5}));
// 	check(e.begin()-20,e.end()+20);
// 	check(e.rbegin()-20,e.rend()+20);
// 	e.rev();
// 	assert(vector<long>(e)==vector<long>({5,4,3,2,1}));
// 	e.rev();
// 	assert(vector<long>(e)==vector<long>({1,2,3,4,5}));
// 	check(e.rbegin()-20,e.rend()+20);
// 	y=e;
// 	for (size_t w=0;w<e.size();w++){
// 		auto t=e.cut_left(1);
// 		e.add_left(t);
// 		assert(e.size()==y.size());
// 	}
// 	assert(y==e);
// 	for (size_t w=0;w<e.size();w++){
// 		auto t=e.cut_right(1);
// 		e.add_right(t);
// 		assert(e.size()==y.size());
// 	}
// 	assert(y==e);
// 	for (size_t w=0;w<e.size();w++){
// 		auto t=e.cut_right(1);
// 		e.add_left(t);
// 		assert(e.size()==y.size());
// 	}
// 	assert(y==e);
// 	for (size_t w=0;w<e.size();w++){
// 		auto t=e.cut_left(1);
// 		e.add_right(t);
// 		assert(e.size()==y.size());
// 	}
// 	assert(y==e);
// 	e.clear();
// 	y=e;
// 	assert(e==y);
// 	auto z=e.begin();
// 	auto x=e.end();
// 	assert(z==x);


// 	for (size_t w=1;w<prime;++w){
// 		size_t r=1;
// 		size_t t=w; 
// 		for (size_t y=1;y<prime;y*=2){
// 			if (y&(prime-2)){
// 				r*=t;
// 				r%=prime;
// 			}
// 			t*=t;
// 			t%=prime;
// 		}
// 		e.push_back(r);
// 	}
// 	for (uint64_t w=0;w<e.size();++w){
// 		assert(e[w]*(w+1)%prime==1);
// 	}

// 	for (uint64_t w=0;w<e.size();++w){
// 		auto p=lower_bound(e.begin(),e.begin()+w,e[w])-e.begin();
// 		auto z=e.cut_left(w+1);
// 		auto x=z.cut_left(p);
// 		z.rev();
// 		x.add_right(z);
// 		z=x.cut_left(p+1);
// 		x.rev();
// 		x.add_left(z);
// 		e.add_left(x);
// 	}
// 	e.push_front(0);
// 	for (uint64_t w=0;w<e.size();++w){
// 		assert(e[w]==w);
// 	}

// 	e.clear();
// 	for (size_t w=1;w<prime;++w){
// 		size_t r=1;
// 		size_t t=w; 
// 		for (size_t y=1;y<prime;y*=2){
// 			if (y&(prime-2)){
// 				r*=t;
// 				r%=prime;
// 			}
// 			t*=t;
// 			t%=prime;
// 		}
// 		e.push_back(r);
// 	}

// 	y=e;

// 	for (uint64_t w=0;w<e.size();++w){
// 		auto a=e.cut_left(w);
// 		auto p=a.lower_bound(e[0])-a.begin();
// 		e.add_left(a);
// 		auto z=e.cut_left(w+1);
// 		auto x=z.cut_left(p);
// 		z.rev();
// 		x.add_right(z);
// 		z=x.cut_left(p+1);
// 		x.rev();
// 		x.add_left(z);
// 		e.add_left(x);
// 	}
// 	e.push_front(0);
// 	for (uint64_t w=0;w<e.size();++w){
// 		assert(e[w]==w);
// 	}

// 	e=y;

// 	for (uint64_t w=0;w<e.size();++w){
// 		auto s=e.cut_left(w);
// 		e.add(-e[0]);
// 		e.add_left(s);
// 	}

// 	for (uint64_t w=0;w<e.size();++w){
// 		assert(e[w]==0);
// 	}

// 	for (uint64_t w=0;w<e.size();++w){
// 		auto s=e.cut_left(w);
// 		e.add(1);
// 		e.add_left(s);
// 	}

// 	for (uint64_t w=0;w<e.size();++w){
// 		assert(e[w]==w+1);
// 	}

// 	e.mul(0);

// 	// py_func(r,{
// 	// def r():
// 	// 	return '-'*9999
// 	// })
// 	// ic(r())

// 	// e=y;

// 	y=e;
// 	// ic(e,y)

// 	auto g=vector<long>(e.begin(), e.end());
// 	for (uint64_t w=0;w<co;++w){
// 		assert(e.next_permutation()==next_permutation(g.begin(),g.end()));
// 		assert(vector<long>(e)==g);
// 	}
// 	for (uint64_t w=0;w<co;++w){
// 		assert(e.prev_permutation()==prev_permutation(g.begin(),g.end()));
// 		assert(vector<long>(e)==g);
// 	}
// 	assert(g==vector<long>(y));
// 	assert(e==y);



// 	// for (size_t w=0;w<102400;++w){
// 	// 	e.push_back(w);
// 	// }
// 	// for (size_t w=0;w<e.size();++w){
// 	// 	assert(w==e[w]);
// 	// }
// 	// for (size_t w=0;w<102400;++w){
// 	// 	assert(w==e.pop_front());
// 	// }
// 	// assert(e.empty());
// 	// for (size_t w=0;w<102400;++w){
// 	// 	e.push_front(w);
// 	// }
// 	// reverse(e.begin(),e.end());
// 	// for (size_t w=0;w<e.size();++w){
// 	// 	assert(w==e[w]);
// 	// }
// 	// reverse(e.rbegin(),e.rend());
// 	// for (size_t w=0;w<102400;++w){
// 	// 	assert(w==e.pop_back());
// 	// }
// 	// e.resize(102400);
// 	// assert(e.size()==102400);
// 	// e.resize(102001);
// 	// assert(e.size()==102001);
// 	// for (size_t w=1;w<102001;++w){
// 	// 	size_t r=1;
// 	// 	size_t t=w; 
// 	// 	for (size_t y=1;y<102001;y*=2){
// 	// 		if (y&(102001-2)){
// 	// 			r*=t;
// 	// 			r%=102001;
// 	// 		}
// 	// 		t*=t;
// 	// 		t%=102001;
// 	// 	}
// 	// 	e[r]=w;
// 	// }
// 	// for (size_t w=1;w<102001;++w){
// 	// 	size_t r=1;
// 	// 	size_t t=w; 
// 	// 	for (size_t y=1;y<102001;y*=2){
// 	// 		if (y&(102001-2)){
// 	// 			r*=t;
// 	// 			r%=102001;
// 	// 		}
// 	// 		t*=t;
// 	// 		t%=102001;
// 	// 	}
// 	// 	assert(e[w]==r);
// 	// }
// 	// for (size_t w=1;w<102001;++w){
// 	// 	assert(w*e[w]%102001==1);
// 	// }
// 	// z=e.begin();
// 	// x=e.end();
// 	// for (size_t w=0;w<102400-102001;++w){
// 	// 	e.insert(w*2prime+1,w);
// 	// }
// 	// assert(vector<long>(z,x)==vector<long>(e));
// 	// y=e;
// 	// for (size_t w=0;w<102400;++w){
// 	// 	swap(y,e);
// 	// }
// 	// swap(y,y);
// 	// assert(y==e);
// 	// e.resize(102001);
// 	// y.resize(e.size());
// 	// assert(y==e);

// 	// copy(w.begin(),w.end(),inserter(q,q.begin()+4));
// 	// assert(vector<long>(q)==vector<long>({-1,0,1,2,0,1,2,3,4,5,6}));
// 	// q=q;
// 	// assert(vector<long>(q)==vector<long>({-1,0,1,2,0,1,2,3,4,5,6}));

// 	// treap<B> r;
// 	// r.emplace_back(1);
// 	// assert(r[0].d==1);
// 	// r.emplace_front(2);
// 	// assert(r[0].d==2);
// 	// assert(r[1].d==1);
// 	// auto t=r.cut_left(1);
// 	// assert(t[0].d==2);
// 	// assert(r[0].d==1);
// 	// r.clear();
// 	// t.clear();
// 	// for (size_t w=0;w<102001;++w){
// 	// 	r.emplace_back(w);
// 	// 	t.emplace_back(w);
// 	// }
// 	// for (size_t w=0;w<102001;++w){
// 	// 	assert(r[w].d==t[w].d);
// 	// 	assert(r[w].d==w);
// 	// }
// 	// for (size_t w=0;w<y.size();++w){
// 	// 	auto q=r.cut_right(51001);
// 	// 	r.add_left(q);
// 	// 	assert(r.size()==t.size());
// 	// 	assert(q.empty());
// 	// }
// 	// for (size_t w=0;w<y.size();++w){
// 	// 	auto q=t.cut_left(51001);
// 	// 	t.add_right(q);
// 	// 	assert(t.size()==r.size());
// 	// 	assert(q.empty());
// 	// }
// 	// for (size_t w=0;w<102001;++w){
// 	// 	assert(r[w].d==t[w].d);
// 	// 	assert(r[w].d==w);
// 	// }

// 	// e.resize(102400);
// 	// e.resize(102001);
// 	// y=e;
// 	// for (size_t w=0;w<y.size();++w){
// 	// 	auto q=y.cut_right(51001);
// 	// 	y.add_left(q);
// 	// 	assert(y.size()==e.size());
// 	// 	assert(q.empty());
// 	// }
// 	// assert(e==y);
// 	// for (size_t w=0;w<y.size();++w){
// 	// 	auto q=e.cut_left(51001);
// 	// 	e.add_right(q);
// 	// 	assert(e.size()==y.size());
// 	// 	assert(q.empty());
// 	// }
// 	// assert(e==y);
// 	// for (size_t w=0;w<y.size();++w){
// 	// 	y.insert(lower_bound(y.begin(),y.begin()+w,y[w]),y[w]);
// 	// 	y.erase(w+1);
// 	// }
// 	// sort(e.begin(),e.end());
// 	// assert(e==y);

// 	// assert(q.cut(4,7)==w);
// 	// assert(vector<long>(q)==vector<long>({-1,0,1,2,3,4,5,6}));
// 	// assert(vector<long>(w)==vector<long>({0,1,2}));
// 	// sort(w.rbegin(),w.rend());
// 	// assert(vector<long>(w)==vector<long>({2,1,0}));
// 	// q.insert(q.begin(),w);
// 	// assert(vector<long>(q)==vector<long>({2,1,0,-1,0,1,2,3,4,5,6}));
// 	// assert(vector<long>(w)==vector<long>({}));
// 	// q.rev();
// 	// w=q.cut_right(3);
// 	// w.rev();
// 	// assert(vector<long>(w)==vector<long>({2,1,0}));
// 	// q.rev();
// 	// assert(vector<long>(q)==vector<long>({-1,0,1,2,3,4,5,6}));
// 	// q.add_left(w);
// 	// assert(vector<long>(q)==vector<long>({2,1,0,-1,0,1,2,3,4,5,6}));
// 	// for (uint64_t t=0;t<q.size();++t){
// 	// 	q.rev();
// 	// 	w=q.cut_left(5);
// 	// 	q.rev();
// 	// 	w.rev();
// 	// 	q.add_left(w);
// 	// }	
// 	// assert(vector<long>(q)==vector<long>({2,1,0,-1,0,1,2,3,4,5,6}));
// 	// q.rev();
// 	// w=q;
// 	// q.rev();
// 	// w.rev();
// 	// assert(q==w);
// 	// w.rev();
// 	// for (uint64_t t=0;t<q.size();++t){
// 	// 	assert(q[t*5%q.size()]==w[w.size()-1-t*5%w.size()]);
// 	// }
// 	// assert(vector<long>(q)==vector<long>({2,1,0,-1,0,1,2,3,4,5,6}));

// 	// e.resize(102001);
// 	// for (size_t w=1;w<102001;++w){
// 	// 	size_t r=1;
// 	// 	size_t t=w; 
// 	// 	for (size_t y=1;y<102001;y*=2){
// 	// 		if (y&(102001-2)){
// 	// 			r*=t;
// 	// 			r%=102001;
// 	// 		}
// 	// 		t*=t;
// 	// 		t%=102001;
// 	// 	}
// 	// 	e[r]=w;
// 	// }
// 	// for (uint64_t t=0;t<e.size();++t){
// 	// 	e.rev();
// 	// 	y=e.cut_left(51001);
// 	// 	e.rev();
// 	// 	y.rev();
// 	// 	e.add_left(y);
// 	// }	
// 	// for (size_t w=1;w<102001;++w){
// 	// 	size_t r=1;
// 	// 	size_t t=w; 
// 	// 	for (size_t y=1;y<102001;y*=2){
// 	// 		if (y&(102001-2)){
// 	// 			r*=t;
// 	// 			r%=102001;
// 	// 		}
// 	// 		t*=t;
// 	// 		t%=102001;
// 	// 	}
// 	// 	assert(e[w]==r);
// 	// }
// 	// sort(e.begin(),e.end());
// 	// y=e;
// 	// sort(e.rbegin(),e.rend());
// 	// e.rev();	
// 	// y.rev();
// 	// sort(y.begin(),y.end());
// 	// assert(y==e);

// 	// e.clear();
// 	// y.clear();
// 	// assert(e.is_ascending());
// 	// assert(y.is_ascending());
// 	// assert(e.is_descending());
// 	// assert(y.is_descending());
// 	// e.push_back(0);
// 	// assert(e.is_ascending());
// 	// assert(e.is_descending());
// 	// e.push_back(1);
// 	// assert(e.is_ascending());
// 	// assert(!e.is_descending());
// 	// e.push_back(2);
// 	// assert(e.is_ascending());
// 	// assert(!e.is_descending());
    

// 	// e.clear();
// 	// for (size_t w=1;w<102001;++w){
// 	// 	size_t r=1;
// 	// 	size_t t=w; 
// 	// 	for (size_t y=1;y<102001;y*=2){
// 	// 		if (y&(102001-2)){
// 	// 			r*=t;
// 	// 			r%=102001;
// 	// 		}
// 	// 		t*=t;
// 	// 		t%=102001;
// 	// 	}
// 	// 	e.push_back(r);
// 	// }
// 	// assert(!e.is_ascending());
// 	// assert(!e.is_descending());
// 	// y=e;
// 	// for (size_t w=0;w<y.size();++w){
// 	// 	y.insert(lower_bound(y.begin(),y.begin()+w,y[w]),y[w]);
// 	// 	y.erase(w+1);
// 	// }
// 	// assert(y.is_ascending());
// 	// assert(!y.is_descending());
// 	// y.resize(20);
// 	// assert(y.is_ascending());
// 	// assert(!y.is_descending());
// 	// y.rev();
// 	// assert(!y.is_ascending());
// 	// assert(y.is_descending());

// 	// e.clear();
// 	// for (size_t w=1;w<102001;++w){
// 	// 	size_t r=1;
// 	// 	size_t t=w; 
// 	// 	for (size_t y=1;y<102001;y*=2){
// 	// 		if (y&(102001-2)){
// 	// 			r*=t;
// 	// 			r%=102001;
// 	// 		}
// 	// 		t*=t;
// 	// 		t%=102001;
// 	// 	}
// 	// 	e.push_back(r);
// 	// }
// 	// y=e;
// 	// for (size_t w=0;w<y.size();++w){
// 	// 	auto d=y.cut_left(w);
// 	// 	long p=0;
// 	// 	p=d.lower_bound(y[0])-d.begin();
// 	// 	y.add_left(d);
// 	// 	auto s=y.cut_left(w+1);
// 	// 	auto a=s.cut_left(p);
// 	// 	assert(a.is_ascending());
// 	// 	s.rev();
// 	// 	a.add_right(s);
// 	// 	s=a.cut_left(p+1);
// 	// 	assert(s.is_ascending());
// 	// 	assert(a.is_descending());
// 	// 	a.rev();
// 	// 	assert(a.is_ascending());
// 	// 	a.add_left(s);
// 	// 	y.add_left(a);
// 	// }
// 	// assert(y.is_ascending());
// 	// assert(vector<long>(q)==vector<long>({2,1,0,-1,0,1,2,3,4,5,6}));
// 	// w=q.cut_left(6);
// 	// bool f=1;
// 	// vector<long> g(w.begin(),w.end());
// 	// while (f){
// 		// f=next_permutation(g.begin(),g.end());
// 		// assert(f==w.next_permutation());
// 		// assert(g==vector<long>(w));
// 	// }
// 	// assert(w.is_ascending());
// 	// q.add_left(w);
// 	// w=q.cut_left(8);
// 	// while(w.next_permutation()){}
// 	// assert(w.is_ascending());
// 	// q.add_left(w);
// 	// e.clear();
// 	// for (size_t w=1;w<102001;++w){
// 	// 	size_t r=1;
// 	// 	size_t t=w; 
// 	// 	for (size_t y=1;y<102001;y*=2){
// 	// 		if (y&(102001-2)){
// 	// 			r*=t;
// 	// 			r%=102001;
// 	// 		}
// 	// 		t*=t;
// 	// 		t%=102001;
// 	// 	}
// 	// 	e.push_back(r);
// 	// }
// 	// for (uint64_t w=0;w<e.size();++w){
// 	// 	e.next_permutation();
// 	// }
// 	// vector<long> h(e.begin(),e.end());
// 	// for (uint64_t w=0;w<9;++w){
// 	// 	assert(next_permutation(h.begin(),h.end())==e.next_permutation());
// 	// 	assert(h==vector<long>(e));
// 	// }
// 	// q.next_permutation();
// 	// q.next_permutation();
// 	// q.next_permutation();
// 	// q.next_permutation();
// 	// w=q;
// 	// q.next_permutation();
// 	// q.prev_permutation();
// 	// assert(w==q);
// 	// w=q.cut_left(6);
// 	// w.prev_permutation();
// 	// g=vector<long>(w.begin(),w.end());
// 	// while (f){
// 		// f=prev_permutation(g.begin(),g.end());
// 		// assert(f==w.prev_permutation());
// 		// assert(g==vector<long>(w));
// 	// }
// 	// assert(w.is_descending());
// 	// h=vector<long> (e.begin(),e.end());
// 	// for (uint64_t w=0;w<9;++w){
// 	// 	assert(prev_permutation(h.begin(),h.end())==e.prev_permutation());
// 	// 	assert(h==vector<long>(e));
// 	// }
// 	// for (uint64_t w=0;w<e.size();++w){
// 	// 	e.prev_permutation();
// 	// }
// 	// for (size_t w=1;w<102001;++w){
// 	// 	size_t r=1;
// 	// 	size_t t=w; 
// 	// 	for (size_t y=1;y<102001;y*=2){
// 	// 		if (y&(102001-2)){
// 	// 			r*=t;
// 	// 			r%=102001;
// 	// 		}
// 	// 		t*=t;
// 	// 		t%=102001;
// 	// 	}
// 	// 	assert(e[w-1]==r);
// 	// }
// 	// assert(e.sum()==5202051000);
// 	// e.mul(2);
// 	// assert(e.sum()==5202051000*2);
// 	// e.add(3);
// 	// assert(e.sum()==e.size()*3+5202051000*2);


// 	// for (uint64_t w=0;w<e.size();++w){
// 	// 	e.insert(lower_bound(e.begin(),e.begin()+w,e[w]),e[w]);
// 	// 	e.erase(w+1);
// 	// }
// 	// assert(e.is_ascending());

// 	// e.resize(0);
// 	// for (uint64_t w=0;w<12;++w){
// 	// 	e.push_back(w);
// 	// }

// 	// for (uint64_t w=0;w<9;++w){
// 	// 	auto z=e.cut_left(rand()%e.size());
// 	// 	auto x=e.cut_left(rand()%e.size());
// 	// 	x.mul(-1);
// 	// 	x.rev();
// 	// 	if (x.size()){
// 	// 		x.add(-x[x.size()-1]-x[0]);
// 	// 	}
// 	// 	e.add_left(x);
// 	// 	e.add_left(z);
// 	// 	assert(e.is_ascending());
// 	// }



// 	// e.resize(1'234'567);
// 	// e.out();
// 	// cout<<e.e->d<<"\n";
// }
    
}
