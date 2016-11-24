#include "ast.hpp"
using namespace std;

auto _plus = join(" + ");
auto _minus = join(" - ");
auto _multiply = join(" * ");
auto _divide = join(" / ");
auto _exp = join("^");

auto wrap_curly = [](string s){
    return "{"+s+"}";
};

auto wrap_square = [](string s){
    return "["+s+"]";
};

auto wrap_round = [](string s){
    return "("+s+")";
};



auto root = [](vector <ast*> &args){
    /* returns pow(x, 1/n) */
    assert(args.size() == 2);
    string _n = boost::apply_visitor(show(), *args[0]);
    string _x = boost::apply_visitor(show(), *args[1]);
    return "\\sqrt" + wrap_square(_n) + wrap_curly(_x);
};

auto range_f = [](string keyword){
    return [=](vector <ast*> &args){
        vector<string> result;
        for(auto p: args)
            result.push_back(boost::apply_visitor(show(), *p));
        string start, end, term;
        start = result[0], end = result[1], term = result[2];
        return keyword +
                "_"+ wrap_curly(start) + 
                "^"+ wrap_curly(end) + 
                wrap_curly(term);
    };

};


auto sum = range_f("\\sum");
auto product = range_f("\\prod");
auto integral = range_f("\\int");
