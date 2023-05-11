// C语言词法分析器
#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <map>
#include <set>
#include <sstream>
#include <vector>
#include <stack>
#include <regex>
using namespace std;

vector <pair <string, string>> grammers;
map <pair <string, string>, int> table;
set <string> keywords;
vector <string> result;

/* 不要修改这个标准输入函数 */
void read_prog(string& prog)
{
	char c;
	while(scanf("%c",&c)!=EOF){
		prog += c;
	}
}
/* 你可以添加其他函数 */

void initKeyWords ()
{
	keywords.insert ("{");
    keywords.insert ("}");
    keywords.insert ("if");
    keywords.insert ("(");
    keywords.insert (")");
    keywords.insert ("then");
    keywords.insert ("else");
    keywords.insert ("while");
    keywords.insert ("ID");
    keywords.insert ("=");
    keywords.insert (">");
    keywords.insert ("<");
    keywords.insert (">=");
    keywords.insert ("<=");
    keywords.insert ("==");
    keywords.insert ("+");
    keywords.insert ("-");
    keywords.insert ("*");
    keywords.insert ("/");
    keywords.insert ("NUM");
    keywords.insert ("E");
    keywords.insert (";");
}

void initGrammer () 
{
	
	grammers.push_back (pair <string, string> ("program", "compoundstmt"));

	grammers.push_back (pair <string, string> ("stmt", "ifstmt"));
    grammers.push_back (pair <string, string> ("stmt", "whilestmt"));
    grammers.push_back (pair <string, string> ("stmt", "assgstmt"));
    grammers.push_back (pair <string, string> ("stmt", "compoundstmt"));
    
	grammers.push_back (pair <string, string> ("compoundstmt", "{ stmts }"));
    
	grammers.push_back (pair <string, string> ("stmts", "stmt stmts"));
    grammers.push_back (pair <string, string> ("stmts", "E"));
    
    grammers.push_back (pair <string, string> ("ifstmt", "if ( boolexpr ) then stmt else stmt"));

    grammers.push_back (pair <string, string> ("whilestmt", "while ( boolexpr ) stmt"));

    grammers.push_back (pair <string, string> ("assgstmt", "ID = arithexpr ;"));
    
	grammers.push_back (pair <string, string> ("boolexpr", "arithexpr boolop arithexpr"));

    grammers.push_back (pair <string, string> ("boolop", "<"));
    grammers.push_back (pair <string, string> ("boolop", ">"));
    grammers.push_back (pair <string, string> ("boolop", "<="));
    grammers.push_back (pair <string, string> ("boolop", ">="));
    grammers.push_back (pair <string, string> ("boolop", "=="));
    
	grammers.push_back (pair <string, string> ("arithexpr", "multexpr arithexprprime"));
    
	grammers.push_back (pair <string, string> ("arithexprprime", "+ multexpr arithexprprime"));
    grammers.push_back (pair <string, string> ("arithexprprime", "- multexpr arithexprprime"));
    grammers.push_back (pair <string, string> ("arithexprprime", "E"));
    
	grammers.push_back (pair <string, string> ("multexpr", "simpleexpr multexprprime"));
    
	grammers.push_back (pair <string, string> ("multexprprime", "* simpleexpr multexprprime"));
    grammers.push_back (pair <string, string> ("multexprprime", "/ simpleexpr multexprprime"));
    grammers.push_back (pair <string, string> ("multexprprime", "E"));
    
	grammers.push_back (pair <string, string> ("simpleexpr", "ID"));
    grammers.push_back (pair <string, string> ("simpleexpr", "NUM"));
    grammers.push_back (pair <string, string> ("simpleexpr", "( arithexpr )"));

}

void initTable ()
{

	table.insert (pair <pair <string, string>, int> (pair <string, string> ("program", "{"), 1));

    table.insert (pair <pair <string, string>, int> (pair <string, string> ("stmt", "if"), 2));
    table.insert (pair <pair <string, string>, int> (pair <string, string> ("stmt", "while"), 3));
    table.insert (pair <pair <string, string>, int> (pair <string, string> ("stmt", "ID"), 4));
    table.insert (pair <pair <string, string>, int> (pair <string, string> ("stmt", "{"), 5));

    table.insert (pair <pair <string, string>, int> (pair <string, string> ("compoundstmt", "{"), 6));

    table.insert (pair <pair <string, string>, int> (pair <string, string> ("stmts", "if"), 7));
    table.insert (pair <pair <string, string>, int> (pair <string, string> ("stmts", "while"), 7));
    table.insert (pair <pair <string, string>, int> (pair <string, string> ("stmts" ,"ID"), 7));
    table.insert (pair <pair <string, string>, int> (pair <string, string> ("stmts", "{"), 7));
    table.insert (pair <pair <string, string>, int> (pair <string, string> ("stmts", "}"), 8));

    table.insert (pair <pair <string, string>, int> (pair <string, string> ("ifstmt", "if"), 9));

    table.insert (pair <pair <string, string>, int> (pair <string, string> ("whilestmt", "while"), 10));

    table.insert (pair <pair <string, string>, int> (pair <string, string> ("assgstmt", "ID"), 11));

    table.insert (pair <pair <string, string>, int> (pair <string, string> ("boolexpr", "ID"), 12));
    table.insert (pair <pair <string, string>, int> (pair <string, string> ("boolexpr", "NUM"), 12));
    table.insert (pair <pair <string, string>, int> (pair <string, string> ("boolexpr", "("), 12));

    table.insert (pair <pair <string, string>, int> (pair <string, string> ("boolop", "<"), 13));
    table.insert (pair <pair <string, string>, int> (pair <string, string> ("boolop" ,">"), 14));
    table.insert (pair <pair <string, string>, int> (pair <string, string> ("boolop", "<="), 15));
    table.insert (pair <pair <string, string>, int> (pair <string, string> ("boolop", ">="), 16));
    table.insert (pair <pair <string, string>, int> (pair <string, string> ("boolop", "=="), 17));

    table.insert (pair <pair <string, string>, int> (pair <string, string> ("arithexpr", "ID"), 18));
    table.insert (pair <pair <string, string>, int> (pair <string, string> ("arithexpr", "NUM"), 18));
    table.insert (pair <pair <string, string>, int> (pair <string, string> ("arithexpr", "("), 18));

    table.insert (pair <pair <string, string>, int> (pair <string, string> ("arithexprprime", "+"), 19));
    table.insert (pair <pair <string, string>, int> (pair <string, string> ("arithexprprime", "-"), 20));
    table.insert (pair <pair <string, string>, int> (pair <string, string> ("arithexprprime", ";"), 21));
    table.insert (pair <pair <string, string>, int> (pair <string, string> ("arithexprprime", "<"), 21));
    table.insert (pair <pair <string, string>, int> (pair <string, string> ("arithexprprime", ">"), 21));
    table.insert (pair <pair <string, string>, int> (pair <string, string> ("arithexprprime", "<="), 21));
    table.insert (pair <pair <string, string>, int> (pair <string, string> ("arithexprprime", ">="), 21));
    table.insert (pair <pair <string, string>, int> (pair <string, string> ("arithexprprime", "=="), 21));
    table.insert (pair <pair <string, string>, int> (pair <string, string> ("arithexprprime", ")"), 21));

    table.insert (pair <pair <string, string>, int> (pair <string, string> ("multexpr", "ID"), 22));
    table.insert (pair <pair <string, string>, int> (pair <string, string> ("multexpr", "NUM"), 22));
    table.insert (pair <pair <string, string>, int> (pair <string, string> ("multexpr", "("), 22));

    table.insert (pair <pair <string, string>, int> (pair <string, string> ("multexprprime", "*"), 23));
    table.insert (pair <pair <string, string>, int> (pair <string, string> ("multexprprime", "/"), 24));
    table.insert (pair <pair <string, string>, int> (pair <string, string> ("multexprprime", "+"), 25));
    table.insert (pair <pair <string, string>, int> (pair <string, string> ("multexprprime", "-"), 25));
    table.insert (pair <pair <string, string>, int> (pair <string, string> ("multexprprime", ";"), 25));
    table.insert (pair <pair <string, string>, int> (pair <string, string> ("multexprprime", "<"), 25));
    table.insert (pair <pair <string, string>, int> (pair <string, string> ("multexprprime", ">"), 25));
    table.insert (pair <pair <string, string>, int> (pair <string, string> ("multexprprime", "<="), 25));
    table.insert (pair <pair <string, string>, int> (pair <string, string> ("multexprprime", ">="), 25));
    table.insert (pair <pair <string, string>, int> (pair <string, string> ("multexprprime", "=="), 25));
    table.insert (pair <pair <string, string>, int> (pair <string, string> ("multexprprime", ")"), 25));
    table.insert (pair <pair <string, string>, int> (pair <string, string> ("simpleexpr", "ID"), 26));
	table.insert (pair <pair <string, string>, int> (pair <string, string> ("simpleexpr", "NUM"), 27));
    table.insert (pair <pair <string, string>, int> (pair <string, string> ("simpleexpr", "("), 28));



}

vector <string> tokenize (const string str, const regex re)
{
    sregex_token_iterator it{ str.begin(), str.end(), re, -1 };
    vector <string> tokenized{ it, {} };
 
    // Additional check to remove empty strings
    tokenized.erase(
        remove_if (tokenized.begin (), tokenized.end (),
                       [](string const& s) {
                           return s.size () == 0;
                       }),
        tokenized.end ());
 
    return tokenized;
}


void print (int level, string cur)
{
	string tmp = "";
	for (int i = 0; i < level; i ++)
		tmp += "\t";
	tmp += cur;
	result.push_back (tmp);
}

void recovery (int line, int level, string X, string a)
{
	if (keywords.find (X) != keywords.end ())
		cout << "语法错误,第" << line << "行,缺少\"" << X << "\"" << endl;
	else 
		print (level, "E");
}

void analysis (string prog)
{
    const regex re(R"([\s|,]+)");

	int line = 1;
	stack <int> levels;
	levels.push (0);
	levels.push (0);

	stack <string> stack;
	stack.push ("$");
	stack.push ("program");

    int r = prog.find('\n');
    while (r != string::npos) {
        if (r != string::npos){
            prog.replace(r, 1, " ");
            r = prog.find('\n');
        }
    }

	vector <string> input = tokenize (prog, re);
	input.push_back ("$");

	int p = 0;
	while (!stack.empty ()) {
		int level = levels.top ();
		levels.pop ();
		string X = stack.top ();
		stack.pop ();
		string a = input[p];

		if (X == a) {
			if (X == "$")
				return ;
			else {
				print (level, X);
				p ++;
			}
		}
		else {
			print (level, X);
			pair <string, string> entry = make_pair (X, a);
			map <pair <string, string>, int>::iterator it = table.find (entry);

			int num;

			if (it == table.end ())
				num = 0;
			else 
				num = table.find (entry)->second;

			if (num == 0)
				recovery (line, level + 1, X, a);
			else {
				pair <string, string> grammer = grammers[num - 1];
				string grammer_right = grammer.second;
				vector <string> grammer_tokens = tokenize (grammer_right, re);

				if (grammer_tokens.size () == 1 && grammer_tokens[0] == "E")
					print (level + 1, "E");
				else 
					for (int i = grammer_tokens.size () - 1; i >= 0; i --) {
						levels.push (level + 1);
						stack.push (grammer_tokens[i]);
					}
			}
		}

		if (X == "{" || X == "}" || X == ")" || X == ";")
			line ++;
	}

}

void printResult ()
{
	for (int i = 0; i < result.size (); i ++)
		cout << result[i] << endl;
}

void Analysis()
{
	string prog;
	read_prog(prog);
	/* 骚年们 请开始你们的表演 */
    /********* Begin *********/
    
	initKeyWords();
    initGrammer();
    initTable();

	analysis (prog);
	printResult ();
    
    /********* End *********/
	
}