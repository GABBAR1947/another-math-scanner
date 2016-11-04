#ifndef AST_H
#define AST_H

#include <string>
#include <vector>
#include <utility>
#include <functional>
#include <numeric>
#include <iostream>
#include <boost/variant.hpp>
using namespace std;

struct functor;
typedef boost::variant<int, double, string, functor> ast;



struct functor {
    string id;
    vector<ast*> args;
    function<string(vector<ast*> &)> _show;

    functor(string _id, vector<ast*> _args, 
            function<string(vector<ast*> &)> _f):
        id(_id){
            _show = _f;
            args = _args;

        };

    functor(){}

    string show(){
        return _show(args);
    }
};

class show: public boost::static_visitor<string> {
    public:
        string operator()(int x) const { return to_string(x); }
        string operator()(double y) const { return to_string(y); }
        string operator()(string s) const { return s; }
        string operator()(functor f) const {
            return f.show();
        }
};


auto join = [](string joiner){
    return [=](vector <ast*> &args){
        string result;
        auto p =  args.begin();
        while ( p != args.end()){
            result += boost::apply_visitor(show(), **p);
            ++p;
            if ( p != args.end()) result += joiner;
        }
        return result;
    
    };
};

#endif
