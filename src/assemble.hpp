#include "component.hpp"
#include <string>
#include <vector>
#include <opencv2/imgproc/imgproc.hpp>
using namespace std;

enum {
    RECT_NO_OVERLAP,
    RECT_INSIDE,
    RECT_OVERLAP
};

bool valueInRange(int value, int min, int max)
{ return (value >= min) && (value <= max); }

bool rectOverlap(Rect A, Rect B)
{
    bool xOverlap = valueInRange(A.x, B.x, B.x + B.width) ||
        valueInRange(B.x, A.x, A.x + A.width);

    bool yOverlap = valueInRange(A.y, B.y, B.y + B.height) ||
        valueInRange(B.y, A.y, A.y + A.height);

    return xOverlap && yOverlap;
}

int position(Rect a, Rect b){
    auto expand = [](Rect a){
        int tolerance = max(a.width, a.height)/4;;
        return Rect(a.x-tolerance, a.y-tolerance, 
                2*tolerance+a.width, 2*tolerance+a.height);
    };

    a = expand(a);
    b = expand(b);


    if(!rectOverlap(a, b)){
        return RECT_NO_OVERLAP;
    }
    else if ( b.x > a.x and b.y > a.y and b.x + b.width < a.x + a.width and
            b.y + b.height < a.y + a.height){
        return RECT_INSIDE;
    }

    else {
        return RECT_OVERLAP;
    }
}


string assemble(vector<component> components){
    // Group things which overlap in bounding boxes.
    int N = components.size();

    vector<vector<int>> hierarchy(N), overlap(N);
    int tolerance = 5;
    for(int i=0; i<N; i++){
        for(int j=0; j<N; j++){
            if ( i!= j){
                switch(position(components[i].bounding_box, components[j].bounding_box)){
                    case RECT_NO_OVERLAP:
                        break;
                    case RECT_INSIDE:
                        hierarchy[i].push_back(j);
                        break;
                    case RECT_OVERLAP:
                        overlap[i].push_back(j);
                        break;
                }
            }
        }
    }

    for(int i=0; i<N; i++){
        cout<<i<<":";
        for(auto p: overlap[i])
            cout<<p<<" ";
        cout<<endl;
    }

    sort(components.begin(), components.end(), [](const component &x, const component &y){
                if ( x.bounding_box.x < y.bounding_box.x )
                    return true;
                if ( x.bounding_box.x > y.bounding_box.x) return false;
                return y.bounding_box.x < y.bounding_box.y;
            });

    string s;
    for(auto p: components)
        s += p.label;

    return s;
}

