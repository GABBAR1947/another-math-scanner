#include "ast.hpp"
#include "special.hpp"
using namespace std;


int main(){
    ast s, t, u; string result;

    s = ast("whatthefuck!");
    result = boost::apply_visitor(show(), s);
    cout<<result<<"\n";
    t = ast(5);
    result = boost::apply_visitor(show(), t);
    cout<<result<<"\n";
    u = ast(5.3);
    result = boost::apply_visitor(show(), u);
    cout<<result<<"\n";


    vector<ast*> args;
    args.push_back(&u);
    args.push_back(&t);
    args.push_back(&u);
    ast z;
    //z = functor("plus", args, _plus);
    z = functor("plus", args, _minus);
    result = boost::apply_visitor(show(), z);
    cout<<_plus(args)<<endl;
    cout<<join(" ~ ")(args)<<endl;
    cout<<result<<"\n";
    args.clear();

    args.push_back(&t);
    args.push_back(&u);

    z = functor("sqrt", args, root);
    result = boost::apply_visitor(show(), z);
    cout<<result<<"\n";
    args.push_back(&u);
    z = functor("sqrt", args, sum);
    result = boost::apply_visitor(show(), z);
    cout<<result<<"\n";

    z = functor("sqrt", args, product);
    result = boost::apply_visitor(show(), z);
    cout<<result<<"\n";
    return 0;

}
