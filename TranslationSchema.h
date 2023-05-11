// C语言词法分析器
#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
using namespace std;

vector <string> split(string const& s) {
    vector <string> result ;
    istringstream stm(s);
    string token;
    string lines;
    while (getline(stm, lines)) {
        istringstream linestm(lines);
        while (linestm >> token) result.push_back(token);
        if (token != "$") {
            result.push_back("E");
        }
    }
    return result;
}

enum Type {
    Real, Int, Temp
};

enum Comparator {
    greater, greaterEqual, less, lessEqual, equal
};

enum Operator {
    plus, sub, divide, mult
};

struct Ident {
    string name;
    Type type;
    double value;

    Ident() {};

    Ident(string name, Type t, double v) {
        this->name = name;
        this->type = t;
        this->value = v;
    }
};

map <string, Ident> IDMap;

string src;
int pos;
int line;
bool hasError;
vector <string> tokens;

void conversionError(int line) {
    cout << "error message:line " <<  line << ",realnum can not be translated into int type" << endl;
}

void divisionByZeroError(int line) {
    cout << "error message:line " << line << ",division by zero\n" << endl;

}


class Exp {
public:
    int line;

    Exp() {
        line = 0;
    }
};

class AssignExp: public Exp {
private:
    Ident* target;
    Ident* srcLeft;
    Ident* srcRight;
    Operator opt;
public:
    double exectute();

    AssignExp(Ident *target, Ident *srcLeft, Ident *srcRight, Operator opt);
};

AssignExp::AssignExp(Ident *target, Ident *srcLeft, Ident *srcRight, Operator opt) :
target(target), srcLeft(srcLeft), srcRight(srcRight), opt(opt) {}

double AssignExp::exectute() {
    switch (opt) {
        case Operator::plus:
            target->value = srcLeft->value + srcRight->value;
            break;
        case Operator::sub:
            target->value = srcLeft->value - srcRight->value;
            break;
        case Operator::mult:
            target->value = srcLeft->value * srcRight->value;
            break;
        case Operator::divide:
            if (srcRight->value == 0) {
                divisionByZeroError(this->line);
                return 0;
            }
            target->value = srcLeft->value / srcRight->value;
            break;
    }
    return target->value;
}

class BoolExp: public Exp {
private:
    Ident* left;
    Ident* right;
    Comparator cmp;
public:
    bool eval();

    BoolExp(Ident *left, Ident *right, Comparator cmp);
};

BoolExp::BoolExp(Ident *left, Ident *right, Comparator cmp) : left(left), right(right), cmp(cmp) {}

bool BoolExp::eval() {
    switch (cmp) {
        case Comparator::greater:
            return left->value > right->value;
        case Comparator::greaterEqual:
            return left->value >= right->value;
        case Comparator::less:
            return left->value < right->value;
        case Comparator::lessEqual:
            return left->value <= right->value;
        case Comparator::equal:
            return left->value == right->value;
    }
}

class IfExp : public Exp {
private:
    BoolExp boolExp;
    AssignExp thenExp;
    AssignExp elseExp;
public:
    void execute();

    IfExp(const BoolExp &boolExp, const AssignExp &thenExp, const AssignExp &elseExp) :
        boolExp(boolExp), thenExp(thenExp),elseExp(elseExp) {}
};

void IfExp::execute() {
    if(boolExp.eval()) {
        thenExp.exectute();
    }
    else {
        elseExp.exectute();
    }
}

void forward(int count) {
    for(int i = 0; i < count;) {
        if (tokens[pos] == "E") {
            line++;
        }
        else {
            i++;
        }
        pos++;
    }
    if (tokens[pos] == "E") {
        line++;
        pos++;
    }
}

void swallowDeclare() {
    string varName = tokens[pos + 1];
    string type = tokens[pos];
    double value = stod(tokens[pos + 3]);
    Ident id;
    if (type == "int") {
        id = Ident(varName, Int, value);
        if (tokens[pos + 3].find('.') != string::npos) {
            conversionError(line);
            hasError = true;
        }
    }
    else if(type == "real") {
        id = Ident(varName, Real, value);
    }
    IDMap[varName] = id;
    forward(4);
}

AssignExp* swallowAssign() {
    Ident* target = &IDMap[tokens[pos]];
    Ident* srcLeft;
    Ident* srcRight;
    string Lstr = tokens[pos + 2];
    string Rstr = tokens[pos + 4];
    if (Lstr[0] >= '0' && Lstr[0] <= '9') {
        srcLeft = new Ident("null", Type::Temp, stod(Lstr));
    }
    else{
        srcLeft = &IDMap[tokens[pos + 2]];
    }
    if (Rstr[0] >= '0' && Rstr[0] <= '9') {
        srcRight = new Ident("null", Type::Temp, stod(Rstr));
    }
    else {
        srcRight = &IDMap[tokens[pos + 4]];
    }
    char optChar = tokens[pos + 3][0];
    AssignExp* assignExp = NULL;
    switch (optChar) {
        case '+':
            assignExp = new AssignExp(target, srcLeft, srcRight, Operator::plus);
            break;
        case '-':
            assignExp = new AssignExp(target, srcLeft, srcRight, Operator::sub);
            break;
        case '*':
            assignExp = new AssignExp(target, srcLeft, srcRight, Operator::mult);
            break;
        case '/':
            assignExp = new AssignExp(target, srcLeft, srcRight, Operator::divide);
            break;
    }
    assignExp->line = line;
    forward(6);
    string next = tokens[pos - 1];

    if (next == "+" || next == "-" || next == "*" || next == "/") {
        double tempValue = assignExp->exectute();
        string RRstr = tokens[pos];
        Ident* srcRRight = new Ident("null", Type::Temp, stod(RRstr));
        Ident* srcLLeft = new Ident("null", Type::Temp, tempValue);
        char optChar = next[0];
        switch (optChar) {
            case '+':
                assignExp = new AssignExp(target, srcLLeft, srcRRight, Operator::plus);
                break;
            case '-':
                assignExp = new AssignExp(target, srcLLeft, srcRRight, Operator::sub);
                break;
            case '*':
                assignExp = new AssignExp(target, srcLLeft, srcRRight, Operator::mult);
                break;
            case '/':
                assignExp = new AssignExp(target, srcLLeft, srcRRight, Operator::divide);
                break;
        }
        forward(2);
    }

    return assignExp;
}

BoolExp* swallowBool() {
    Ident* left = &IDMap[tokens[pos]];
    Ident* right = &IDMap[tokens[pos + 2]];
    string cmp = tokens[pos + 1];
    BoolExp * boolExp = NULL;
    if (cmp == "<") {
        boolExp = new BoolExp(left, right, Comparator::less);
    }
    else if(cmp == ">") {
        boolExp = new BoolExp(left, right, Comparator::greater);
    }
    else if(cmp == ">=") {
        boolExp = new BoolExp(left, right, Comparator::greaterEqual);
    }
    else if(cmp == "<=") {
        boolExp = new BoolExp(left, right, Comparator::lessEqual);
    }
    else if(cmp == "==") {
        boolExp = new BoolExp(left, right, Comparator::equal);
    }
    forward(3);
    return boolExp;
}

IfExp * swallowIf() {
    forward(2); 
    BoolExp * boolExp = swallowBool();
    forward(2); 
    AssignExp * thenExp = swallowAssign();
    forward(1);
    AssignExp * elseExp = swallowAssign();
    return new IfExp(*boolExp, *thenExp, *elseExp);
}

void printResult() {
    for(auto it = IDMap.begin(); it != IDMap.end(); it++) {
        if (it->second.type == Type::Int) {
            cout << it->first << ": " << it->second.value << endl;
        }
        else {
            ostringstream oss;
            oss << it->second.value;
            cout << it->first << ": " << oss.str() << endl;
        }
    }
}

void analysis(string &prog) {

    src = prog;
    src += " $";
    line = 1;
    pos = 0;
    hasError = false;
    tokens = split(src);

    while(1) {
        string next = tokens[pos];
        if (next == "$") {
            if (!hasError) {
                printResult();
            }
            break;
        }
        if (next == "E") {
            line++;
            pos++;
            continue;
        }
        if (next == ";" || next == "{" || next == "}") {
            pos++;
            continue;
        }
        if (next == "real" || next == "int") {
            swallowDeclare();
        }
        else if(next == "if") {
            swallowIf()->execute();
        }
        else {
            swallowAssign()->exectute();
        }
    }
}

/* 不要修改这个标准输入函数 */
void read_prog(string& prog)
{
    char c;
    while(scanf("%c",&c)!=EOF){
        prog += c;
    }
}
/* 你可以添加其他函数 */

void Analysis()
{
    string prog;
    read_prog(prog);
    /* 骚年们 请开始你们的表演 */
    /********* Begin *********/
    
    analysis(prog);
    
    /********* End *********/
    
}