#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <memory>
#include <fstream>
#include <queue>

using namespace std;

struct Exception : public std::exception
{
    std::string s;
    Exception(string s) : s(s) {}
    Exception() throw() {}
    const char *what() const throw() { return s.c_str(); }
};

string errMsg(int i)
{
    string num = to_string(i + 1);
    return "ERROR at " + num;
}

vector<string> split(string str)
{
    vector<string> ans;
    string word = "";
    for (auto x : str)
    {
        if (x == ' ')
        {
            ans.push_back(word);
            word = "";
        }
        else
        {
            word = word + x;
        }
    }
    ans.push_back(word);
    return ans;
}

struct TreeNode
{
    string val;
    vector<unique_ptr<TreeNode> > children;

    TreeNode(string val)
        : val(val) {}
};

struct State
{
    int reducerRule = -1;
    unordered_set<string> reducerSymbols;
    unordered_map<string, int> shifter;
};

struct DFA
{
    const int REDUCE = 0;
    const int SHIFT = 1;

    vector<int> currStateStack;

    unordered_set<string> terminals;
    unordered_set<string> nonTerminals;
    string start;
    vector<vector<string> > rules;
    vector<unique_ptr<State> > states;
    vector<unique_ptr<TreeNode> > treeStack;

    void read()
    {
        ifstream infile("WLP4.lr1");
        int n;
        string s;

        infile >> n;
        for (int i = 0; i < n; ++i)
        {
            infile >> s;
            terminals.insert(s);
        }
        infile >> n;
        for (int i = 0; i < n; ++i)
        {
            infile >> s;
            nonTerminals.insert(s);
        }

        infile >> start;

        infile >> n;

        getline(infile, s);

        // Rules

        for (int i = 0; i < n; ++i)
        {
            getline(infile, s);
            rules.push_back(split(s));
        }

        infile >> n;

        for (int i = 0; i < n; ++i)
        {
            states.push_back(make_unique<State>());
        }

        infile >> n;

        int k;
        string symbol;
        string type;
        int to;

        for (int i = 0; i < n; ++i)
        {
            infile >> k;
            infile >> symbol;
            infile >> type;
            infile >> to;

            if (type == "reduce")
            {
                states[k]->reducerRule = to;
                states[k]->reducerSymbols.insert(symbol);
            }
            else if (type == "shift")
            {
                states[k]->shifter.insert(make_pair(symbol, to));
            }
            else
            {
                throw Exception("ERROR at 0");
            }
        }
    }

    void parse(vector<string> &input)
    {
        int longestPrefix = 0;
        int orgSize = input.size();
        int count = 0;
        currStateStack.push_back(0);
        int pick, n, currState;
        State *state;
        bool prevReduce = false;

        if (input.size() == 0)
            throw Exception(errMsg(input.size()));

        while (input.size() > 0)
        {
            n = input.size();
            string s = input[n - 1];

            currState = currStateStack[currStateStack.size() - 1];

            state = states[currState].get();

            bool isShift = state->shifter.find(s) != state->shifter.end();
            bool isReduce = state->reducerRule != -1;
            bool isValidReduceSymbol = state->reducerSymbols.find(s) != state->reducerSymbols.end();

            if (!isReduce && !isShift)
                throw Exception(errMsg(orgSize - input.size() - 1));

            if (isReduce && isShift)
            {
                if (isValidReduceSymbol)
                    pick = REDUCE;
                else
                    pick = SHIFT;
            }
            else if (isReduce)
            {
                pick = REDUCE;
            }
            else
            {
                pick = SHIFT;
            }

            if (pick == SHIFT)
            {
                currStateStack.push_back(state->shifter[s]);
                if (!prevReduce)
                    treeStack.push_back(make_unique<TreeNode>(s));
                prevReduce = false;
                input.pop_back();
                longestPrefix++;
            }
            else
            {
                if (!isValidReduceSymbol)
                    throw Exception(errMsg(orgSize - input.size() - 1));

                prevReduce = true;
                vector<string> rule = rules[state->reducerRule];
                int popSize = rule.size() - 1;

                unique_ptr<TreeNode> replace = make_unique<TreeNode>(rule[0]);

                for (int i = 0; i < popSize; ++i)
                {
                    replace->children.push_back(move(treeStack[treeStack.size() - 1]));
                    currStateStack.pop_back();
                    treeStack.pop_back();
                }

                treeStack.push_back(move(replace));
                input.push_back(rule[0]);
            }
        }

        currState = currStateStack[currStateStack.size() - 1];
        state = states[currState].get();

        vector<string> rule = rules[0];
        int popSize = rule.size() - 1;

        unique_ptr<TreeNode> replace = make_unique<TreeNode>(rule[0]);

        for (int i = 0; i < popSize; ++i)
        {
            replace->children.push_back(move(treeStack[treeStack.size() - 1]));
            currStateStack.pop_back();
            treeStack.pop_back();
        }

        treeStack.push_back(move(replace));
        input.push_back(rule[0]);
    }
};

void printFinalTree(TreeNode *root, DFA *currDFA, unordered_map<string, queue<string> > &umap, vector<string> &ans)
{
    if (root->children.size() == 0)
    {
        string str = root->val;
        if (umap.find(root->val) != umap.end())
        {
            queue<string> q = umap[root->val];
            str += " " + q.front();
            umap[root->val].pop();
        }
        ans.push_back(str);
        return;
    }
    string s;

    s += root->val + " ";
    int n = root->children.size();
    for (int i = n - 1; i >= 0; --i)
    {
        s += root->children[i]->val;
        if (i > 0)
            s += " ";
        else
            ans.push_back(s);
    }

    for (int i = n - 1; i >= 0; --i)
    {
        printFinalTree(root->children[i].get(), currDFA, umap, ans);
    }
}

int main()
{
    unique_ptr<DFA> currDFA = make_unique<DFA>();
    unordered_map<string, queue<string> > umap;

    try
    {
        currDFA->read();
    }
    catch (Exception &caught)
    {
        cerr << caught.what() << endl;
        return 1;
    }

    vector<string> input;
    string s1, s2;

    umap["BOF"].push("BOF");
    input.push_back("BOF");
    while (cin >> s1 && cin >> s2)
    {
        umap[s1].push(s2);
        input.push_back(s1);
    }
    input.push_back("EOF");
    umap["EOF"].push("EOF");

    try
    {
        reverse(input.begin(), input.end());
        currDFA->parse(input);
    }
    catch (Exception &caught)
    {
        cerr << caught.what() << endl;
        return 1;
    }

    vector<string> ans;
    printFinalTree(currDFA->treeStack[0].get(), currDFA.get(), umap, ans);
    int ansSize = ans.size();
    for (int i = 0; i < ansSize; ++i)
    {
        cout << ans[i] << endl;
    }
    return 0;
}
