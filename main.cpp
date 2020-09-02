#include <iostream>
#include <cmath>
#include <cstring>
#include <vector>
#include <map>
#include <stack>
#include <functional>
#include <utility>
#include <stdlib.h>
#include <iomanip>

#define OPTIONS 7
#define ORIGIN_COUNT 5

using namespace std;

//double origin[ORIGIN_COUNT] = {2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,163,167,173,179,181,191,193,197,199,211,223,227,229,233,239,241,251,257,263,269,271,277,281,283,293,307,311,313,317,331,337,347,349,353,359,367,373,379,383,389,397,401,409,419,421,431,433,439,443,449,457,461,463,467,479,487,491,499,503,509,521,523,541};
vector<int> origin = {2,3,5,7,11};


double parse(char *formula);

struct node{
    double error = 0;
    string f = "";
    int level = 1;
    int localError = 0;
    double data[ORIGIN_COUNT] = {0};
    node *variants[OPTIONS] = {nullptr};
};

vector <node*> glob;

int f(int x){
    return x-1+x-2+2/x;
}

std::string StringReplacer(const std::string& inputStr, const std::string& src, const std::string& dst)
{
    std::string result(inputStr);

    size_t pos = result.find(src);
    while(pos != std::string::npos) {
        result.replace(pos, src.size(), dst);
        pos = result.find(src, pos);
    }

    return result;
}

void calc(node *root){
    for(int i = 0; i < ORIGIN_COUNT; ++i){
        string tmp = StringReplacer(root->f,"x",std::to_string(i+1));
        char str[tmp.size() + 1];
        strcpy(str, tmp.c_str());
        root->data[i] = parse(str);
        root->localError += abs(root->data[i] - origin[i]);

    }
    root->error = root->localError + (root->level*2);
}

void addNode(node *root, string f){
    for(int i = 0; i < OPTIONS; ++i){
        if(root->variants[i] == nullptr){
            node *newNode = new node();
            newNode->f = root->f + f;
            newNode->level = root->level+1;
            root->variants[i] = newNode;
            calc(newNode);
            glob.push_back(newNode);
            break;
        }
    }
}

int main()
{

    //for(int i = 1; i < 10; ++i){
        // cout << f(i) << endl;
   // }

    //int x;
    // cin >>x;

    vector <string> varinats = {"+x","-x","*x","+1","-1","%2","%x"};

    node *root = new node();
    root->f = "x";
    root->localError = 10000000;
    root->error = 10000000;

    glob.push_back(root);

label:
    int min = 10000000, index = 0;
    for(int k = 1; k < glob.size(); ++k){
        if(min > glob[k]->error) {
            min = glob[k]->error;
            index = k;
        }
    }

    cout << "Error: " << glob[index]->localError << "\n"
         << "Func: " << glob[index]->f << "\n"
         << "Level: " << glob[index]->level << "\n"
         << "Glob error: " << glob[index]->error << "\n"
         << "-------------------------" << endl;

    if(glob[index]->localError == 0) {
        cout << "STOP";
        int a;
        cin >> a;
    }

    for(int i = 0; i < varinats.size(); ++i){
        addNode(glob[index], varinats[i]);

    }


    delete glob[index];
    glob.erase(glob.begin() + index);



    goto label;

    return 0;
}



double parse(char *formula){

    stack<double> s; stack< pair<int, char> > ops;

    auto p = [&s, &ops] (function<int (int, int)>& f)
    {double r=s.top();s.pop();r=f(s.top(),r);s.pop();s.push(r);ops.pop();};

    map< char, pair< int, function<int (int, int)> > > m =
    {{'+', {1, [](int a, int b){return a+b;}}},{'-', {1, [](int a, int b){return a-b;}}},
    {'*', {2, [](int a, int b){return a*b;}}},{'%', {2, [](int a, int b){return a%b;}}}};

    const int order = 2; int level = 0;
    for (char* sp = formula;; ++sp) {
        while (*sp == '(') {level += order; ++sp;}

        s.push(strtod(sp, &sp));

        while (*sp == ')') {level -= order; ++sp;}

        if (!*sp) {while(!ops.empty()) p(m[ops.top().second].second); break;}

        const int op = m[*sp].first + level;
        while (!ops.empty() && ops.top().first >= op) p(m[ops.top().second].second);

        ops.push(make_pair(op, *sp));
    }
    return s.top();
}
