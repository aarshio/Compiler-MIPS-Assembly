#include <iostream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <algorithm>
#include <memory>

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
        int n;
        string s;

        cin >> n;
        for (int i = 0; i < n; ++i)
        {
            cin >> s;
            terminals.insert(s);
        }
        cin >> n;
        for (int i = 0; i < n; ++i)
        {
            cin >> s;
            nonTerminals.insert(s);
        }

        cin >> start;

        cin >> n;

        getline(cin, s);

        // Rules

        for (int i = 0; i < n; ++i)
        {
            getline(cin, s);
            rules.push_back(split(s));
        }

        cin >> n;

        for (int i = 0; i < n; ++i)
        {
            states.push_back(make_unique<State>());
        }

        cin >> n;

        int k;
        string symbol;
        string type;
        int to;

        for (int i = 0; i < n; ++i)
        {
            cin >> k;
            cin >> symbol;
            cin >> type;
            cin >> to;

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
                throw Exception(errMsg(orgSize - input.size()));

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
                    throw Exception(errMsg(orgSize - input.size()));

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

void printFinalTree(TreeNode *root, DFA *currDFA, vector<string> &ans)
{
    if (root->children.size() == 0 && (currDFA->nonTerminals.find(root->val) != currDFA->nonTerminals.end()))
    {
        ans.push_back(root->val);
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

    for (int i = 0; i < n; ++i)
    {
        printFinalTree(root->children[i].get(), currDFA, ans);
    }
}

int main()
{
    unique_ptr<DFA> currDFA = make_unique<DFA>();

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
    string s;

    while (cin >> s)
    {
        input.push_back(s);
    }

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
    printFinalTree(currDFA->treeStack[0].get(), currDFA.get(), ans);
    int ansSize = ans.size();
    for (int i = ansSize - 1; i >= 0; --i)
    {
        cout << ans[i] << endl;
    }
    return 0;
}
