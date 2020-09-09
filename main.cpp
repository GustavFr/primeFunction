#include <algorithm>
#include <cmath>
#include <cstring>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <stack>
#include <stdlib.h>
#include <utility>
#include <vector>

using namespace std;

//double origin[ORIGIN_COUNT] = {2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71,73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,163,167,173,179,181,191,193,197,199,211,223,227,229,233,239,241,251,257,263,269,271,277,281,283,293,307,311,313,317,331,337,347,349,353,359,367,373,379,383,389,397,401,409,419,421,431,433,439,443,449,457,461,463,467,479,487,491,499,503,509,521,523,541};
//vector<int> origin = { 2, 3, 5, 7, 11, 13};

vector<string> sign = { "+", "-", "*", "/", "%", "^" };
vector<string> num = { "x", "2", "(x*2)", "(x*x)", "(x+1)", "((x%2))", "1" };
const int N = (sign.size() * num.size()) - 4;
vector<string> varinats;
vector<int> origin = { 2, 3, 5, 7, 11 };
//vector<int> origin = { 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048 };


double parse(char* formula);

struct node {
    unsigned error = 0;
    string f = "";
    int level = 1;
    int localError = 0;
    node** variants = (node**)malloc(sizeof(node*) * N);
};

vector<node*> glob;

int f(int x)
{
    return x - 1 + x - 2 + 2 / x;
}

std::string StringReplacer(const std::string& inputStr, const std::string& src, const std::string& dst)
{
    std::string result(inputStr);

    size_t pos = result.find(src);
    while (pos != std::string::npos) {
        result.replace(pos, src.size(), dst);
        pos = result.find(src, pos);
    }

    return result;
}

void calc(node* root)
{
    for (size_t i = 0; i < origin.size(); ++i) {
        string tmp = StringReplacer(root->f, "x", std::to_string(i + 1));
        char str[tmp.size() + 1];
        strcpy(str, tmp.c_str());

        int x = parse(str);
        root->localError += abs(x - origin[i]);
    }
    root->error = root->localError + root->level;
}

void addNode(node* root, string f, int i)
{
    node* newNode = new node();
    newNode->f = root->f + f;
    newNode->level = root->level + 1;
    root->variants[i] = newNode;
    calc(newNode);
    glob.push_back(newNode);
}

class cmp {
public:
    bool operator()(const node* a, const node* b)
    {
        return a->error < b->error;
    }
} compare;

int main()
{

    for (size_t j = 0; j < num.size(); ++j) {
        for (size_t i = 0; i < sign.size(); ++i) {
            if (varinats.size() < N) {
                varinats.push_back(sign[i] + num[j]);
            }
        }
    }

    //            for(int i = 1; i < 10; ++i){
    //             cout << f(i) << endl;
    //             }

    //            int x;
    //             cin >>x;

    node* root = new node();
    root->f = "x";
    root->localError = 10000000;
    root->error = 10000000;

    glob.push_back(root);

label:
    int index = 0;

    std::sort(glob.begin(), glob.end(), compare);

    cout << "Error: " << glob[index]->localError << "\n"
         << "Func: " << glob[index]->f << "\n"
         << "Level: " << glob[index]->level << "\n"
         << "Glob error: " << glob[index]->error << "\n"
         << "-------------------------" << endl;

    if (glob[index]->localError == 0) {
        cout << "STOP";
        int a;
        cin >> a;
    }

    printf("\e[1;1H\e[2J");

    for (size_t i = 0; i < N; ++i) {
        addNode(glob[index], varinats[i], i);
    }

    free(glob[index]);
    glob.erase(glob.begin() + index);

    goto label;

    return 0;
}

double parse(char* formula)
{

    stack<double> s;
    stack<pair<int, char>> ops;

    auto p = [&s, &ops](function<int(int, int)>& f) {double r=s.top();s.pop();r=f(s.top(),r);s.pop();s.push(r);ops.pop(); };

    map<char, pair<int, function<int(int, int)>>> m = { { '+', { 1, [](int a, int b) { return a + b; } } }, { '-', { 1, [](int a, int b) { return a - b; } } },
        { '*', { 2, [](int a, int b) { return a * b; } } }, { '/', { 2, [](int a, int b) { return a / b; } } } ,
       { '%', { 2, [](int a, int b) { return a % b; } } }, { '^', { 2, [](int a, int b) { return pow(a , b); } } } };

    const int order = 2;
    int level = 0;
    for (char* sp = formula;; ++sp) {
        while (*sp == '(') {
            level += order;
            ++sp;
        }

        s.push(strtod(sp, &sp));

        while (*sp == ')') {
            level -= order;
            ++sp;
        }

        if (!*sp) {
            while (!ops.empty())
                p(m[ops.top().second].second);
            break;
        }

        const int op = m[*sp].first + level;
        while (!ops.empty() && ops.top().first >= op)
            p(m[ops.top().second].second);

        ops.push(make_pair(op, *sp));
    }
    return s.top();
}
