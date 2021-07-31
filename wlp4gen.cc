#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_set>
#include <unordered_map>

using namespace std;

const unordered_set<string> terminals = {"BOF", "BECOMES", "COMMA", "ELSE", "EOF", "EQ", "GE", "GT", "ID", "IF", "INT", "LBRACE", "LE", "LPAREN", "LT", "MINUS", "NE", "NUM", "PCT", "PLUS", "PRINTLN", "RBRACE", "RETURN", "RPAREN", "SEMI", "SLASH", "STAR", "WAIN", "WHILE", "AMP", "LBRACK", "RBRACK", "NEW", "DELETE", "NULL"};
const int INT = 1;
const int INT_PTR = 2;

struct Exception : public std::exception
{
    std::string s;
    Exception(string s) : s(s) {}
    Exception() throw() {}
    const char *what() const throw() { return s.c_str(); }
};

struct Node
{
    string procedure;
    string val;
    vector<unique_ptr<Node> > children;

    bool isProcedure = false;
    int exprType = 0;
    int termType = 0;

    Node() {}
    Node(string val)
        : val(val) {}
    Node(string val, string procedure)
        : val(val), procedure(procedure) {}
};

string getVarname(Node *dcl)
{
    return dcl->children[1]->children[0]->val;
}

bool isTerminal(string s)
{
    return terminals.find(s) != terminals.end();
}

vector<string> split(string s)
{
    vector<string> ans;
    string str;
    for (char c : s)
    {
        if (c == ' ')
        {
            ans.push_back(str);
            str = "";
        }
        else
            str += c;
    }
    ans.push_back(str);
    return ans;
}

void buildTree(vector<vector<string> > &input, Node *node, int &i, vector<string> &stack, string procedure, unordered_map<string, int> &procedures)
{
    int n = input[i].size();
    const string FIRST = input[i][0];

    if (FIRST == "main")
    {
        procedure = "wain";
        if (procedures.find(procedure) != procedures.end())
            throw Exception("ERROR: Duplicate procedure.");
        procedures.insert(make_pair(procedure, procedures.size()));
    }
    else if (FIRST == "procedure")
    {
        procedure = input[i + 2][1];
        if (procedures.find(procedure) != procedures.end())
            throw Exception("ERROR: Duplicate procedure.");
        procedures.insert(make_pair(procedure, procedures.size()));
    }

    if (isTerminal(input[i][0]))
    {
        unique_ptr<Node> tmp = make_unique<Node>(input[i][0], procedure);
        tmp->children.push_back(make_unique<Node>(input[i][1], procedure));
        node->children.push_back(move(tmp));
        return;
    }

    node->children.push_back(make_unique<Node>(input[i][0], procedure));

    for (int j = n - 1; j > 0; --j)
    {
        stack.push_back(input[i][j]);
    }

    Node *last = node->children[node->children.size() - 1].get();

    for (int j = 1; j < n; ++j)
    {
        buildTree(input, last, ++i, stack, procedure, procedures);
        stack.pop_back();
    }
}

void visitingNode(Node *top, unordered_map<string, unordered_map<string, vector<Node *> > > &umap, unordered_map<string, unordered_map<string, int> > &declaredVariables, unordered_map<string, int> &procedures)
{
    string currProcedure = top->procedure;
    umap[currProcedure][top->val].push_back(top);

    unordered_map<string, int> *declaredVariablesForProcedure = &declaredVariables[currProcedure];
    if (top->val == "dcl")
    {
        string varName = getVarname(top);
        if (declaredVariablesForProcedure->find(varName) != declaredVariablesForProcedure->end())
            throw Exception("ERROR: Variable name already used.");
        declaredVariablesForProcedure->insert(make_pair(varName, 0));
    }

    // procedure INT ID LPAREN params RPAREN LBRACE dcls statements RETURN expr SEMI RBRACE
    if (top->val == "procedure" && top->children.size() == 12)
        top->children[1]->isProcedure = true;

    // factor ID LPAREN arglist RPAREN
    // factor ID LPAREN RPAREN
    if (top->val == "factor")
    {
        if (top->children.size() == 4 && top->children[0]->val == "ID" && top->children[1]->val == "LPAREN" && top->children[2]->val == "arglist" && top->children[3]->val == "RPAREN")
            top->children[0]->isProcedure = true;
        if (top->children.size() == 3 && top->children[0]->val == "ID" && top->children[1]->val == "LPAREN" && top->children[2]->val == "RPAREN")
            top->children[0]->isProcedure = true;
    }

    if (top->val == "ID")
    {
        if (!top->isProcedure && declaredVariablesForProcedure->find(top->children[0]->val) == declaredVariablesForProcedure->end())
        {
            throw Exception("ERROR: Use of undefined variable name.");
        }
        else if (top->isProcedure && procedures.find(top->children[0]->val) == procedures.end())
        {
            throw Exception("ERROR: Use of undefined procedure name.");
        }
        else if (top->isProcedure && procedures[currProcedure] < procedures[top->children[0]->val])
        {
            throw Exception("ERROR: Use of procedure name before defintion.");
        }
    }
}

void dfs(Node *root, unordered_map<string, unordered_map<string, vector<Node *> > > &umap, unordered_map<string, unordered_map<string, int> > &declaredVariables, unordered_map<string, int> &procedures)
{
    vector<Node *> stack;
    stack.push_back(root);
    while (!stack.empty())
    {
        int size = stack.size();
        for (int i = 0; i < size; ++i)
        {
            Node *top = stack.back();
            stack.pop_back();

            visitingNode(top, umap, declaredVariables, procedures);
            int n = top->children.size();
            for (int j = n - 1; j >= 0; --j)
            {
                stack.push_back(top->children[j].get());
            }
        }
    }
}

string formatType(Node *dcl)
{
    if (dcl->children[0]->children.size() == 1)
        return "int";
    return "int*";
}

int dclCount(Node *node)
{
    int ans = 0;
    if (node->children.size() == 0)
        return 0;
    if (node->val == "dcl")
        return 1;
    for (auto &child : node->children)
    {
        ans += dclCount(child.get());
    }
    return ans;
}

void printTypes(unordered_map<string, unordered_map<string, vector<Node *> > > &umap, unordered_map<string, unordered_map<string, int> > &declaredVariables, unordered_map<string, vector<int> > &proceduresArg)
{
    string currProcedure;
    for (auto &it : umap)
    {
        if (it.first == "")
            continue;
        currProcedure = it.first;
        vector<Node *> dclVec = umap[currProcedure]["dcl"];

        if (currProcedure != "wain")
        {
            int numParams = dclCount(umap[currProcedure]["params"][0]);
            // cerr << currProcedure << ": ";
            for (int k = 0; k < numParams; ++k)
            {
                int argType = INT;
                if (formatType(dclVec[k]) == "int*")
                    argType = INT_PTR;
                proceduresArg[currProcedure].push_back(argType);
                // cerr << formatType(dclVec[k]);
                // if (k != numParams - 1)
                //     cerr << " ";
            }
            // cerr << endl;
        }
        else
        {
            int argType = INT;
            if (formatType(dclVec[0]) == "int*")
                argType = INT_PTR;

            proceduresArg[currProcedure].push_back(argType);

            if (formatType(dclVec[1]) == "int*")
                argType = INT_PTR;
            else
                argType = INT;

            if (argType == INT_PTR)
                throw Exception("TYPE ERROR: Second argument for WAIN cannot be a pointer.");

            proceduresArg[currProcedure].push_back(argType);
            // cerr << currProcedure << ": " << formatType(dclVec[0]) << " " << formatType(dclVec[1]) << endl;
        }
        for (auto currNode : dclVec)
        {
            string varName = getVarname(currNode);
            int varType = INT;
            if (formatType(currNode) == "int*")
                varType = INT_PTR;
            declaredVariables[currProcedure][varName] = varType;
            // cerr << varName << " " << formatType(currNode) << endl;
        }
    }
}

string getVarnameID(Node *id)
{
    return id->children[0]->val;
}

int getlvType(Node *lv, unordered_map<string, int> &declaredVariables, unordered_map<string, vector<int> > &proceduresArg);

int getExprType(Node *expr, unordered_map<string, int> &declaredVariables, unordered_map<string, vector<int> > &proceduresArg);

void checkArguments(string procedure, Node *args, unordered_map<string, int> &declaredVariables, unordered_map<string, vector<int> > &proceduresArg, int index)
{
    int eType = getExprType(args->children[0].get(), declaredVariables, proceduresArg);

    if (index >= proceduresArg[procedure].size())
        throw Exception("TYPE ERROR: Too many arguments for procedure.");
    if (proceduresArg[procedure][index] != eType)
    {
        throw Exception("TYPE ERROR: Argument mistmatch for procedure.");
    }

    if (args->children.size() == 3)
    {
        checkArguments(procedure, args->children[2].get(), declaredVariables, proceduresArg, index + 1);
    }
    else if (index + 1 < proceduresArg[procedure].size())
        throw Exception("TYPE ERROR: Too few arguments for procedure.");
}

int getFactorType(Node *f, unordered_map<string, int> &declaredVariables, unordered_map<string, vector<int> > &proceduresArg)
{
    string first = f->children[0]->val;
    if (first == "ID" && f->children.size() == 1)
    {
        return declaredVariables[getVarnameID(f->children[0].get())];
    }
    else if (first == "NUM")
    {
        return INT;
    }
    else if (first == "NULL")
    {
        return INT_PTR;
    }
    else if (first == "LPAREN")
    {
        return getExprType(f->children[1].get(), declaredVariables, proceduresArg);
    }
    else if (first == "AMP")
    {
        int lvType = getlvType(f->children[1].get(), declaredVariables, proceduresArg);
        if (lvType != INT)
            throw Exception("TYPE ERROR: Cannot get address of a pointer.");
        return INT_PTR;
    }
    else if (first == "STAR")
    {
        int fType = getFactorType(f->children[1].get(), declaredVariables, proceduresArg);
        if (fType != INT_PTR)
            throw Exception("TYPE ERROR: Cannot get dereference a int.");
        return INT;
    }
    else if (first == "NEW")
    {
        int eType = getExprType(f->children[3].get(), declaredVariables, proceduresArg);
        if (eType != INT)
            throw Exception("TYPE ERROR: NEW key word must have a int expr.");
        return INT_PTR;
    }
    else if (first == "ID" && f->children.size() == 3) // procedure -> ID LP RP
    {
        if (proceduresArg[getVarnameID(f->children[0].get())].size() > 0)
            throw Exception("TYPE ERROR: Too few arguments for procedure.");
        return INT;
    }
    else // procedure -> ID LP arglist RP
    {
        checkArguments(getVarnameID(f->children[0].get()), f->children[2].get(), declaredVariables, proceduresArg, 0);
        return INT;
    }
}

int getlvType(Node *lv, unordered_map<string, int> &declaredVariables, unordered_map<string, vector<int> > &proceduresArg)
{
    if (lv->children[0]->val == "ID")
    {
        return declaredVariables[getVarnameID(lv->children[0].get())];
    }
    else if (lv->children[0]->val == "STAR")
    {
        int fType = getFactorType(lv->children[1].get(), declaredVariables, proceduresArg);
        if (fType != INT_PTR)
            throw Exception("TYPE ERROR: Type error cannot dereference an int.");
        return INT;
    }
    else
    {
        return getlvType(lv->children[1].get(), declaredVariables, proceduresArg);
    }
}

int getTermType(Node *t, unordered_map<string, int> &declaredVariables, unordered_map<string, vector<int> > &proceduresArg)
{
    if (t->children[0]->val == "factor")
    {
        int facType = getFactorType(t->children[0].get(), declaredVariables, proceduresArg);
        t->termType = facType;
        return facType;
    }
    else
    {
        int firstTermType = getTermType(t->children[0].get(), declaredVariables, proceduresArg);
        int secondFactorType = getFactorType(t->children[2].get(), declaredVariables, proceduresArg);
        if (firstTermType != INT || secondFactorType != INT)
            throw Exception("TYPE ERROR: Type error term must be int for MULT/DIV/PCT.");
        t->termType = INT;
        return INT;
    }
}

int getExprType(Node *expr, unordered_map<string, int> &declaredVariables, unordered_map<string, vector<int> > &proceduresArg)
{
    if (expr->children.size() == 3)
    {
        int exprType = getExprType(expr->children[0].get(), declaredVariables, proceduresArg);
        int termType = getTermType(expr->children[2].get(), declaredVariables, proceduresArg);

        if (exprType == INT && termType == INT)
        {
            expr->exprType = INT;
            return INT;
        }
        if (exprType == INT_PTR && termType == INT)
        {
            expr->exprType = INT_PTR;
            return INT_PTR;
        }
        if (expr->children[1]->val == "PLUS" && exprType == INT && termType == INT_PTR)
        {
            expr->exprType = INT_PTR;
            return INT_PTR;
        }
        if (expr->children[1]->val == "MINUS" && exprType == INT_PTR && termType == INT_PTR)
        {
            expr->exprType = INT;
            return INT;
        }

        throw Exception("TYPE ERROR: Expr unhandled case");
    }
    int termType = getTermType(expr->children[0].get(), declaredVariables, proceduresArg);
    expr->exprType = termType;
    return termType;
}

void testType(Node *t, unordered_map<string, int> &declaredVariables, unordered_map<string, vector<int> > &proceduresArg)
{
    int exprFirst = getExprType(t->children[0].get(), declaredVariables, proceduresArg);
    int exprSecond = getExprType(t->children[2].get(), declaredVariables, proceduresArg);

    if (exprFirst != exprSecond)
        throw Exception("TYPE ERROR: Comparisons need to be of the same type.");
}

void proccessStatement(Node *s, unordered_map<string, int> &declaredVariables, unordered_map<string, vector<int> > &proceduresArg)
{
    string first = s->children[0]->val;
    if (first == "lvalue")
    {
        int lvalueType = getlvType(s->children[0].get(), declaredVariables, proceduresArg);
        int exprType = getExprType(s->children[2].get(), declaredVariables, proceduresArg);

        if (lvalueType != exprType)
            throw Exception("TYPE ERROR: Type error.");
    }
    else if (first == "IF")
    {
        testType(s->children[2].get(), declaredVariables, proceduresArg);
    }
    else if (first == "WHILE")
    {
        testType(s->children[2].get(), declaredVariables, proceduresArg);
    }
    else if (first == "PRINTLN")
    {
        int exprType = getExprType(s->children[2].get(), declaredVariables, proceduresArg);
        if (exprType != INT)
            throw Exception("TYPE ERROR: PrintLn expects an int.");
    }
    else if (first == "DELETE")
    {
        int exprType = getExprType(s->children[3].get(), declaredVariables, proceduresArg);
        if (exprType != INT_PTR)
            throw Exception("TYPE ERROR: Only a pointer can be deleted.");
    }
}

void proccessDcls(Node *d, unordered_map<string, int> &declaredVariables, unordered_map<string, vector<int> > &proceduresArg)
{
    if (d->children.size() != 5)
        return;
    int type = d->children[1]->children[0]->children.size();
    if (d->children[3]->val == "NUM")
    {
        if (type != 1)
            throw Exception("TYPE ERROR: Assignment of NUM to non int.");
    }
    else
    {
        if (type != 2)
            throw Exception("TYPE ERROR: Assignment of NULL to non int*.");
    }
}

void typeCheck(unordered_map<string, unordered_map<string, vector<Node *> > > &umap, unordered_map<string, unordered_map<string, int> > &declaredVariables, unordered_map<string, vector<int> > &proceduresArg)
{
    string currProcedure;
    for (auto &it : umap)
    {
        if (it.first == "")
            continue;
        currProcedure = it.first;
        vector<Node *> expr = umap[currProcedure]["statement"];
        for (Node *statement : expr)
        {
            proccessStatement(statement, declaredVariables[currProcedure], proceduresArg);
        }
        expr = umap[currProcedure]["dcls"];
        for (Node *dcls : expr)
        {
            proccessDcls(dcls, declaredVariables[currProcedure], proceduresArg);
        }
        Node *returnExpr;
        if (currProcedure == "wain")
        {
            returnExpr = umap[currProcedure]["main"][0]->children[11].get();
        }
        else
        {
            returnExpr = umap[currProcedure]["procedure"][0]->children[9].get();
        }

        int procedureReturn = getExprType(returnExpr, declaredVariables[currProcedure], proceduresArg);

        if (procedureReturn != INT)
        {
            throw Exception("TYPE ERROR: Procedure return must be of type int.");
        }
    }
}
/********************************************************** A8 ****************************************************/
const int REG0 = 0;
const int REG1 = 1;
const int REG2 = 2;
const int REG3 = 3;
const int REG4 = 4;
const int REG5 = 5;
const int REG6 = 6;
const int REG7 = 7;

const int REG10 = 10;
const int REG11 = 11;
const int REG12 = 12;

const int REG29 = 29;
const int REG30 = 30;
const int REG31 = 31;

void printLis(int reg, string num)
{
    cout << "lis $" << reg << endl
         << ".word " << num << endl;
}
void printAddSub(int reg1, int reg2, int reg3, int isAdd = false)
{
    if (!isAdd)
        cout << "sub $";
    else
        cout << "add $";
    cout << reg1 << ", $" << reg2 << ", $" << reg3 << endl;
}

void printMultDiv(int reg1, int reg2, bool isMult = false)
{
    if (!isMult)
        cout << "div $";
    else
        cout << "mult $";
    cout << reg1 << ", $" << reg2 << endl;
}

void printLwSw(int reg1, int offset, int reg2, bool isLw = false)
{
    if (!isLw)
        cout << "sw $";
    else
        cout << "lw $";
    cout << reg1 << ", " << offset << "($" << reg2 << ")" << endl;
}

void printJr(int reg)
{
    cout << "jr $" << reg << endl;
}

void printJalr(int reg)
{
    cout << "jalr $" << reg << endl;
}

void printBeq(int reg1, int reg2, string label)
{
    cout << "beq $" << reg1 << ", $" << reg2 << ", " << label << endl;
}

void printBne(int reg1, int reg2, string label)
{
    cout << "bne $" << reg1 << ", $" << reg2 << ", " << label << endl;
}

void printSLT(int reg1, int reg2, int reg3)
{
    cout << "slt $" << reg1 << ", $" << reg2 << ", $" << reg3 << endl;
}

void printSLTU(int reg1, int reg2, int reg3)
{
    cout << "sltu $" << reg1 << ", $" << reg2 << ", $" << reg3 << endl;
}

void comment(string s)
{
    cout << "; " << s << endl;
}
void push(int reg, int offset, int reg2);
void pop(int reg, int offset, int reg2);

void prologue(vector<Node *> &dcl, vector<Node *> &dcls, int &stackCounter, unordered_map<string, int> &offsetTable, unordered_map<string, vector<int> > &proceduresArg)
{
    cout << ".import print" << endl;
    cout << ".import init" << endl;
    cout << ".import new" << endl;
    cout << ".import delete" << endl;

    // Constants
    printLis(REG4, "4");
    printLis(REG10, "print");
    printLis(REG11, "1");
    printLis(REG12, "init");

    // Set up frame pointer
    printAddSub(REG29, REG30, REG4, false);

    // alloc init
    push(REG31, 1, REG30);
    if (proceduresArg["wain"][0] == INT)
    {
        push(REG2, 1, REG30);
        printLis(REG2, "0");
        printJalr(12);
        pop(REG2, 1, REG30);
    }
    else
        printJalr(12);
    pop(REG31, 1, REG30);

    // first
    push(REG1, stackCounter, REG29);
    offsetTable[getVarname(dcl[0])] = stackCounter * -4;
    stackCounter++;

    push(REG2, stackCounter, REG29);
    offsetTable[getVarname(dcl[1])] = stackCounter * -4;
    stackCounter++;

    int n = dcls.size();
    for (int i = n - 1; i >= 0; --i)
    {
        if (dcls[i]->children.size() == 5)
        {
            string value = dcls[i]->children[3]->children[0]->val;
            if (value == "NULL")
                value = "1";
            printLis(REG5, value);
            Node *currDcl = dcls[i]->children[1].get();
            push(REG5, stackCounter, REG29);
            offsetTable[getVarname(currDcl)] = stackCounter * -4;
            stackCounter++;
        }
    }
}

void epilogue(int stackCounter)
{
    while (stackCounter > 0)
    {
        printAddSub(REG30, REG30, REG4, true);
        stackCounter--;
    }
    printJr(REG31);
}

void code(string varname, unordered_map<string, int> &offsetTable)
{
    comment("Code(" + varname + ")");
    printLwSw(REG3, offsetTable[varname], REG29, true);
}

void push(int reg, int offset, int reg2)
{
    string p = "Push $" + to_string(reg);
    comment(p);
    printLwSw(reg, offset * -4, reg2); // SW
    printAddSub(REG30, REG30, REG4);
}

void pop(int reg, int offset, int reg2)
{
    string p = "Pop $" + to_string(reg);
    comment(p);
    printAddSub(REG30, REG30, REG4, true);
    printLwSw(reg, offset * -4, reg2, true); // LW
}

void evalExpr(Node *expr, unordered_map<string, int> &offsetTable);

string getlvID(Node *lv)
{
    if (lv->children[0]->val == "ID")
        return getVarnameID(lv->children[0].get());
    else if (lv->children[0]->val == "LPAREN")
        return getlvID(lv->children[1].get());
    return "";
}

void handleArglist(Node *arg, unordered_map<string, int> &offsetTable, int &counter)
{
    counter++;
    evalExpr(arg->children[0].get(), offsetTable);
    push(REG3, 1, REG30);
    if (arg->children.size() > 1)
    {
        handleArglist(arg->children[2].get(), offsetTable, counter);
    }
}

void evalFactor(Node *factor, unordered_map<string, int> &offsetTable)
{
    string first = factor->children[0]->val;
    if (first == "ID" && factor->children.size() == 1)
    {
        code(getVarnameID(factor->children[0].get()), offsetTable);
    }
    else if (first == "NUM")
    {
        printLis(3, factor->children[0]->children[0]->val);
    }
    else if (first == "NULL")
    {
        printAddSub(3, 0, 11, true);
    }
    else if (first == "LPAREN")
        evalExpr(factor->children[1].get(), offsetTable);
    else if (first == "STAR")
    {
        evalFactor(factor->children[1].get(), offsetTable);
        printLwSw(3, 0, 3, true);
    }
    else if (first == "AMP")
    {
        if (factor->children[1]->children[0]->val == "STAR")
            evalFactor(factor->children[1]->children[1].get(), offsetTable);
        else // lvalue -> id
        {
            string lvId = getlvID(factor->children[1].get());
            printLis(3, to_string(offsetTable[lvId]));
            printAddSub(3, 3, 29, true);
        }
    }
    else if (first == "NEW")
    {
        push(REG1, 1, REG30); // assume register 1 is used

        evalExpr(factor->children[3].get(), offsetTable);
        printAddSub(REG1, REG3, REG0, true);
        push(REG31, 1, REG30);
        printLis(REG5, "new");
        printJalr(REG5);
        pop(REG31, 1, REG30);
        printBne(REG3, REG0, "1");
        printAddSub(REG3, REG11, REG0, true);

        pop(REG1, 1, REG30);
    }
    else if (first == "ID" && factor->children.size() == 3)
    {
        push(REG29, 1, REG30);
        push(REG31, 1, REG30);

        printLis(5, "PROCEDURE" + factor->children[0]->children[0]->val);
        printJalr(5);

        pop(REG31, 1, REG30);
        pop(REG29, 1, REG30);
    }
    else if (first == "ID" && factor->children.size() == 4)
    {
        push(REG29, 1, REG30);
        push(REG31, 1, REG30);

        int counter = 0;
        handleArglist(factor->children[2].get(), offsetTable, counter);

        printLis(5, "PROCEDURE" + factor->children[0]->children[0]->val);
        printJalr(5);

        for (int i = 0; i < counter; ++i)
        {
            pop(REG0, 1, REG30);
        }

        pop(REG31, 1, REG30);
        pop(REG29, 1, REG30);
    }
}

void evalTerm(Node *term, unordered_map<string, int> &offsetTable)
{
    string first = term->children[0]->val;
    int n = term->children.size();
    if (first == "factor")
        evalFactor(term->children[0].get(), offsetTable);
    else if (n == 3 && (term->children[1]->val == "STAR" || term->children[1]->val == "SLASH" || term->children[1]->val == "PCT"))
    {
        evalTerm(term->children[0].get(), offsetTable);
        push(REG3, 1, REG30);
        evalFactor(term->children[2].get(), offsetTable);
        pop(REG5, 1, REG30);
        if (term->children[1]->val == "STAR")
            printMultDiv(REG5, REG3, true);
        else
            printMultDiv(REG5, REG3);
        if (term->children[1]->val == "PCT")
            cout << "mfhi $3" << endl;
        else
            cout << "mflo $3" << endl;
    }
}

void evalExpr(Node *expr, unordered_map<string, int> &offsetTable)
{
    string first = expr->children[0]->val;
    int n = expr->children.size();
    if (first == "term")
        evalTerm(expr->children[0].get(), offsetTable);
    else if (n == 3 && (expr->children[1]->val == "PLUS" || expr->children[1]->val == "MINUS"))
    {
        if (expr->children[0]->exprType == INT_PTR && expr->children[2]->termType == INT_PTR)
        {
            evalExpr(expr->children[0].get(), offsetTable);
            push(REG3, 1, REG30);
            evalTerm(expr->children[2].get(), offsetTable);
            pop(REG5, 1, REG30);
            if (expr->children[1]->val == "PLUS")
                printAddSub(REG3, REG5, REG3, true);
            else
                printAddSub(REG3, REG5, REG3);
            printMultDiv(REG3, REG4);
            cout << "mflo $3" << endl;
        }
        else if (expr->children[0]->exprType == INT_PTR)
        {
            evalExpr(expr->children[0].get(), offsetTable);
            push(REG3, 1, REG30);
            evalTerm(expr->children[2].get(), offsetTable);
            printMultDiv(REG3, REG4, true);
            cout << "mflo $3" << endl;
            pop(REG5, 1, REG30);
            if (expr->children[1]->val == "PLUS")
                printAddSub(REG3, REG5, REG3, true);
            else
                printAddSub(REG3, REG5, REG3);
        }
        else if (expr->children[2]->termType == INT_PTR)
        {
            evalTerm(expr->children[2].get(), offsetTable);
            push(REG3, 1, REG30);
            evalExpr(expr->children[0].get(), offsetTable);
            printMultDiv(REG3, REG4, true);
            cout << "mflo $3" << endl;
            pop(REG5, 1, REG30);
            if (expr->children[1]->val == "PLUS")
                printAddSub(REG3, REG3, REG5, true);
            else
                printAddSub(REG3, REG3, REG5);
        }
        else
        {
            evalExpr(expr->children[0].get(), offsetTable);
            push(REG3, 1, REG30);
            evalTerm(expr->children[2].get(), offsetTable);
            pop(REG5, 1, REG30);
            if (expr->children[1]->val == "PLUS")
                printAddSub(REG3, REG5, REG3, true);
            else
                printAddSub(REG3, REG5, REG3);
        }
    }
}

void evalTest(Node *t, unordered_map<string, int> &offsetTable)
{
    if (t->children[1]->val == "LT" || t->children[1]->val == "GT" || t->children[1]->val == "LE" || t->children[1]->val == "GE")
    {
        evalExpr(t->children[0].get(), offsetTable);
        push(REG3, 1, REG30);
        evalExpr(t->children[2].get(), offsetTable);
        pop(REG5, 1, REG30);
        if (t->children[1]->val == "LT" || t->children[1]->val == "GE")
        {
            if (t->children[0]->exprType == INT_PTR)
                printSLTU(REG3, REG5, REG3);
            else
                printSLT(REG3, REG5, REG3);

            if (t->children[1]->val == "GE")
            {
                printAddSub(REG3, REG11, REG3);
            }
        }
        else
        {
            if (t->children[0]->exprType == INT_PTR)
                printSLTU(REG3, REG3, REG5);
            else
                printSLT(REG3, REG3, REG5);

            if (t->children[1]->val == "LE")
            {
                printAddSub(REG3, REG11, REG3);
            }
        }
    }
    else if (t->children[1]->val == "NE" || t->children[1]->val == "EQ")
    {
        evalExpr(t->children[0].get(), offsetTable);
        push(REG3, 1, REG30);
        evalExpr(t->children[2].get(), offsetTable);
        pop(REG5, 1, REG30);
        if (t->children[0]->exprType == INT_PTR)
        {
            printSLTU(REG6, REG3, REG5);
            printSLTU(REG7, REG5, REG3);
        }
        else
        {
            printSLT(REG6, REG3, REG5);
            printSLT(REG7, REG5, REG3);
        }
        printAddSub(REG3, REG6, REG7, true);
        if (t->children[1]->val == "EQ")
            printAddSub(REG3, REG11, REG3);
    }
}

void handleStatements(Node *s, unordered_map<string, int> &offsetTable, string procedure, int &labelUniqueCount);

void handleStatement(Node *statement, unordered_map<string, int> &offsetTable, string procedure, int &labelUniqueCount)
{
    if (statement->children[0]->val == "PRINTLN")
    {
        push(REG1, 1, REG30); // assume register 1 is used

        evalExpr(statement->children[2].get(), offsetTable);
        printAddSub(REG1, REG3, REG0, true);
        push(REG31, 1, REG30);
        printLis(REG5, "print");
        printJalr(REG5);
        pop(REG31, 1, REG30);

        pop(REG1, 1, REG30);
    }
    else if (statement->children[0]->val == "lvalue")
    {
        if (statement->children[0]->children[0]->val == "STAR")
        {
            evalExpr(statement->children[2].get(), offsetTable);
            push(REG3, 1, REG30);
            evalFactor(statement->children[0]->children[1].get(), offsetTable);
            pop(REG5, 1, REG30);
            printLwSw(5, 0, 3);
        }
        else // lvalue -> id
        {
            string varname = getlvID(statement->children[0].get());
            evalExpr(statement->children[2].get(), offsetTable);
            printLwSw(3, offsetTable[varname], 29);
        }
    }
    else if (statement->children[0]->val == "WHILE")
    {
        labelUniqueCount++;
        string loopStart = "loop" + procedure + to_string(labelUniqueCount);
        string loopEnd = "endLoop" + procedure + to_string(labelUniqueCount);

        // push(REG3, 1, REG30);

        cout << loopStart << ":" << endl;
        evalTest(statement->children[2].get(), offsetTable);
        printBeq(3, 0, loopEnd);
        handleStatements(statement->children[5].get(), offsetTable, procedure, labelUniqueCount);
        printBeq(0, 0, loopStart);
        cout << loopEnd << ":" << endl;

        // pop(REG3, 1, REG30);
    }
    else if (statement->children[0]->val == "IF")
    {
        labelUniqueCount++;
        string elseStart = "else" + procedure + to_string(labelUniqueCount);
        string elseEnd = "endif" + procedure + to_string(labelUniqueCount);

        // push(REG3, 1, REG30);

        evalTest(statement->children[2].get(), offsetTable);
        printBeq(3, 0, elseStart);
        handleStatements(statement->children[5].get(), offsetTable, procedure, labelUniqueCount);
        printBeq(0, 0, elseEnd);
        cout << elseStart << ":" << endl;
        handleStatements(statement->children[9].get(), offsetTable, procedure, labelUniqueCount);
        cout << elseEnd << ":" << endl;

        // pop(REG3, 1, REG30);
    }
    else if (statement->children[0]->val == "DELETE")
    {
        labelUniqueCount++;
        string skipLabel = "skipDelete" + procedure + to_string(labelUniqueCount);

        evalExpr(statement->children[3].get(), offsetTable);
        printBeq(3, 11, skipLabel);
        printAddSub(1, 3, 0, true);
        push(REG31, 1, REG30);
        printLis(5, "delete");
        printJalr(5);
        pop(REG31, 1, REG30);
        cout << skipLabel << ":" << endl;
    }
}

void handleStatements(Node *s, unordered_map<string, int> &offsetTable, string procedure, int &labelUniqueCount)
{
    if (s->children.size() > 0)
    {
        handleStatements(s->children[0].get(), offsetTable, procedure, labelUniqueCount);
        handleStatement(s->children[1].get(), offsetTable, procedure, labelUniqueCount);
    }
}

void procedurePrologue()
{
    printAddSub(REG29, REG30, REG4);
    push(REG5, 1, REG30);
    push(REG6, 1, REG30);
    push(REG7, 1, REG30);
}

void procedureEpilogue()
{
    pop(REG7, 1, REG30);
    pop(REG6, 1, REG30);
    pop(REG5, 1, REG30);
    printAddSub(REG30, REG29, REG4, true);
    printJr(REG31);
}

void handleDcl(Node *dcl, unordered_map<string, int> &offsetTable, int &stackCounter)
{
    stackCounter--;
    offsetTable[getVarname(dcl)] = stackCounter * 4;
}

void handleParamlist(Node *paramlist, unordered_map<string, int> &offsetTable, int &stackCounter)
{
    handleDcl(paramlist->children[0].get(), offsetTable, stackCounter);

    if (paramlist->children.size() > 1)
    {
        handleParamlist(paramlist->children[2].get(), offsetTable, stackCounter);
    }
}

int main()
{
    string s;
    vector<string> stack;
    vector<string> svec;

    vector<vector<string> > input;

    while (getline(cin, s))
    {
        input.push_back(split(s));
    }

    unique_ptr<Node> root = make_unique<Node>();

    unordered_map<string, int> procedures;
    unordered_map<string, vector<int> > proceduresArg;
    unordered_map<string, unordered_map<string, vector<Node *> > > umap; // procedure, token -> list Node
    unordered_map<string, unordered_map<string, int> > declaredVariables;

    try
    {
        int i = 0;
        buildTree(input, root.get(), i, stack, "", procedures);

        dfs(root.get(), umap, declaredVariables, procedures);
        printTypes(umap, declaredVariables, proceduresArg);

        typeCheck(umap, declaredVariables, proceduresArg);
    }
    catch (Exception &caught)
    {
        cerr << caught.what() << endl;
        return 1;
    }

    /************************ A8 ************************/
    unordered_map<string, unordered_map<string, int> > offsetTable;
    int labelUniqueCount = 0;

    printBeq(0, 0, "PROCEDUREwain");

    for (auto &procedure : umap)
    {
        if (procedure.first == "")
            continue;

        int stackCounter = 0;

        comment("--------------" + procedure.first + "--------------");
        cout << "PROCEDURE" << procedure.first << ":" << endl;

        if (procedure.first == "wain")
        {
            prologue(procedure.second["dcl"], procedure.second["dcls"], stackCounter, offsetTable[procedure.first], proceduresArg);

            Node *statements = procedure.second["main"][0]->children[9].get();
            handleStatements(statements, offsetTable[procedure.first], procedure.first, labelUniqueCount);

            cout << endl;

            Node *expr = procedure.second["main"][0]->children[11].get();
            evalExpr(expr, offsetTable[procedure.first]);

            cout << endl;

            epilogue(stackCounter);
        }
        else
        {
            procedurePrologue();

            stackCounter = proceduresArg[procedure.first].size() + 1;

            // params
            Node *params = procedure.second["procedure"][0]->children[3].get();
            if (params->children.size() > 0)
            {
                handleParamlist(params->children[0].get(), offsetTable[procedure.first], stackCounter);
            }

            stackCounter = 3; // for preserved registers

            // dcls
            vector<Node *> dcls = umap[procedure.first]["dcls"];
            int n = dcls.size();
            for (int i = n - 1; i >= 0; --i)
            {
                if (dcls[i]->children.size() == 5)
                {
                    string value = dcls[i]->children[3]->children[0]->val;
                    if (value == "NULL")
                        value = "1";
                    printLis(REG5, value);
                    Node *currDcl = dcls[i]->children[1].get();
                    push(REG5, stackCounter, REG29);
                    offsetTable[procedure.first][getVarname(currDcl)] = stackCounter * -4;
                    stackCounter++;
                }
            }

            // statements
            Node *statements = procedure.second["procedure"][0]->children[7].get();
            handleStatements(statements, offsetTable[procedure.first], procedure.first, labelUniqueCount);

            cout << endl;

            // expr
            Node *expr = procedure.second["procedure"][0]->children[9].get();
            evalExpr(expr, offsetTable[procedure.first]);

            cout << endl;

            procedureEpilogue();
        }
    }

    return 0;
}