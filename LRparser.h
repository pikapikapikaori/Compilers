// C语言词法分析器
#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <algorithm>
#include <regex>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <stack>
using namespace std;

vector <pair <string, string>> grammers;
map <pair <int, string>, string> action_table;
map <pair <int, string>, int> goto_table;
stack <pair <string, string>> prs;
vector <vector <string>> rightmost_result;

const regex re(R"([\s|,]+)");

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

void initActionTable ()
{
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (1, "$"), "r 1"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (2,"$"),"acc"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (7,"("),"s 15"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (11,"("),"s 18"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (14,"("),"s 20"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (15,"("),"s 26"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (17,"$"),"r 6"));
 
    //pdf page2
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (18,"("),"s 26"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (20,"("),"s 35"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (21,"*"),"r 26"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (21,"+"),"r 26"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (21,"-"),"r 26"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (22,"*"),"r 27"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (22,"+"),"r 27"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (22,"-"),"r 27"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (24,"+"),"s 42"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (24,"-"),"s 43"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (25,"*"),"s 45"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (25,"+"),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (25,"-"),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (26,"("),"s 35"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (27,"*"),"r 26"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (27,"+"),"r 26"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (27,"-"),"r 26"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (28,"*"),"r 27"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (28,"+"),"r 27"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (28,"-"),"r 27"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (30,")"),"s 55"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (31,"+"),"s 56"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (31,"-"),"s 57"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (32,"*"),"s 59"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (32,"+"),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (32,"-"),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (33,")"),"s 62"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (35,"("),"s 35"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (36,")"),"r 26"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (36,"*"),"r 26"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (36,"+"),"r 26"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (36,"-"),"r 26"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (37,")"),"r 27"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (37,"*"),"r 27"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (37,"+"),"r 27"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (37,"-"),"r 27"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (38,")"),"s 64"));
 
    //page3
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (39,")"),"r 21"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (39,"+"),"s 65"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (39,"-"),"s 66"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (40,")"),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (40,"*"),"s 68"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (40,"+"),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (40,"-"),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (42,"("),"s 20"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (43,"("),"s 20"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (45,"("),"s 20"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (46,"("),"s 20"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (47,"+"),"r 22"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (47,"-"),"r 22"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (48,")"),"s 75"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (49,"("),"r 13"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (50,"("),"r 15"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (51,"("),"r 17"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (52,"("),"r 14"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (53,"("),"r 16"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (54,"("),"s 35"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (56,"("),"s 26"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (57,"("),"s 26"));
 
    //page 4
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (59,"("),"s 26"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (60,"("),"s 26"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (61,"+"),"r 22"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (61,"-"),"r 22"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (63,")"),"s 83"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (64,"*"),"r 28"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (64,"+"),"r 28"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (64,"-"),"r 28"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (65,"("),"s 35"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (66,"("),"s 35"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (67,")"),"r 18"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (68,"("),"s 35"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (69,"("),"s 35"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (70,")"),"r 22"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (70,"+"),"r 22"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (70,"-"),"r 22"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (71,"+"),"s 42"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (71,"-"),"s 43"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (72,"+"),"s 42"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (72,"-"),"s 43"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (73,"*"),"s 45"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (73,"+"),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (73,"-"),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (74,"*"),"s 45"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (74,"+"),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (74,"-"),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (75,"*"),"r 128"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (75,"+"),"r 128"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (75,"-"),"r 128"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (76,")"),"r 12"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (78,"+"),"s 56"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (78,"-"),"s 57"));
 
    //page 5
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (79,"+"),"s 56"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (79,"-"),"s 57"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (80,"*"),"s 59"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (80,"+"),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (80,"-"),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (81,"*"),"s 59"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (81,"+"),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (81,"-"),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (83,")"),"r 28"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (83,"*"),"r 28"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (83,"+"),"r 28"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (83,"-"),"r 28"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (84,")"),"r 21"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (84,"+"),"s 65"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (84,"-"),"s 66"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (85,")"),"r 21"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (85,"+"),"s 65"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (85,"-"),"s 66"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (86,")"),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (86,"*"),"s 68"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (86,"+"),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (86,"-"),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (87,")"),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (87,"*"),"s 68"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (87,"+"),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (87,"-"),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (90,"+"),"r 23"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (90,"-"),"r 23"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (91,"+"),"r 24"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (91,"-"),"r 24"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (95,"("),"s 110"));
 
    //page 6
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (98,"("),"s 112"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (103,"+"),"r 23"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (103,"-"),"r 23"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (104,"+"),"r 24"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (104,"-"),"r 24"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (105,")"),"r 19"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (106,")"),"r 20"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (107,")"),"r 23"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (107,"+"),"r 23"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (107,"-"),"r 23"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (108,")"),"r 24"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (108,"+"),"r 24"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (108,"-"),"r 24"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (109,"("),"s 20"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (110,"("),"s 26"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (112,"("),"s 26"));
 
    //page7
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (115,")"),"s 120"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (117,")"),"s 121"));
 
 
    //page8 / ; < <= =
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (4,"="),"s 14"));
 
    //page9
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (21,"/"),"r 26"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (21,";"),"r 26"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (22,"/"),"r 27"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (22,";"),"r 27"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (23,";"),"s 41"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (24,";"),"r 21"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (25,"/"),"s 46"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (25,";"),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (27,"/"),"r 26"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (27,"<"),"r 26"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (27,"<="),"r 26"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (28,"/"),"r 27"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (28,"<"),"r 27"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (28,"<="),"r 27"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (29,"<"),"s 49"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (29,"<="),"s 50"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (31,"<"),"r 21"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (31,"<="),"r 21"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (32,"/"),"s 60"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (32,"<"),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (32,"<="),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (36,"/"),"r 26"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (37,"/"),"r 27"));
 
    //page10
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (40,"/"),"s 69"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (44,";"),"r 18"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (47,";"),"r 22"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (58,"<"),"r 18"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (58,"<="),"r 18"));
 
    //page11
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (61,"<"),"r 22"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (61,"<="),"r 22"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (64,"/"),"r 28"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (64,";"),"r 28"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (71,";"),"r 21"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (72,";"),"r 21"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (73,"/"),"s 46"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (73,";"),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (74,"/"),"s 46"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (74,";"),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (75,"/"),"r 28"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (75,"<"),"r 28"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (75,"<="),"r 28"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (78,"<"),"r 21"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (78,"<="),"r 21"));
 
    //page12
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (79,"<"),"r 21"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (79,"<="),"r 21"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (80,"/"),"s 60"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (80,"<"),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (80,"<="),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (81,"/"),"s 60"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (81,"<"),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (81,"<="),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (82,"<"),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (82,"<="),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (83,"/"),"r 28"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (86,"/"),"s 69"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (87,"/"),"s 69"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (88,";"),"r 19"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (89,";"),"r 20"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (90,";"),"r 23"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (91,";"),"r 24"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (92,"="),"s 109"));
 
    //page13
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (101,"<"),"r 19"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (101,"<="),"r 19"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (102,"<"),"r 20"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (102,"<="),"r 20"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (103,"<"),"r 23"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (103,"<="),"r 23"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (104,"<"),"r 24"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (104,"<="),"r 24"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (114,";"),"s 119"));
 
    //page14 无新内容
 
    //page15 == > >= ID
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (3,"ID"),"s 4"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (5,"ID"),"r 4"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (6,"ID"),"r 5"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (8,"ID"),"r 2"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (9,"ID"),"s 4"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (12,"ID"),"r 3"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (13,"ID"),"s 4"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (14,"ID"),"s 21"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (15,"ID"),"s 27"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (18,"ID"),"s 27"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (20,"ID"),"s 36"));
 
    //page 16
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (26,"ID"),"s 36"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (27,"=="),"r 26"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (27,">"),"r 26"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (27,">="),"r 26"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (28,"=="),"r 27"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (28,">"),"r 27"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (28,">="),"r 27"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (29,"=="),"s 51"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (29,">"),"s 52"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (29,">="),"s 53"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (31,"=="),"r 21"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (31,">"),"r 21"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (31,">="),"r 21"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (32,"=="),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (32,">"),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (32,">="),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (34,"ID"),"r 6"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (35,"ID"),"s 36"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (41,"ID"),"r 11"));
 
    //page17
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (42,"ID"),"s 21"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (43,"ID"),"s 21"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (45,"ID"),"s 21"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (46,"ID"),"s 21"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (49,"ID"),"r 13"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (50,"ID"),"r 15"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (51,"ID"),"r 17"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (52,"ID"),"r 14"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (53,"ID"),"r 16"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (54,"ID"),"s 36"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (56,"ID"),"s 27"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (57,"ID"),"s 27"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (58,"=="),"r 18"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (58,">"),"r 18"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (58,">="),"r 18"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (59,"ID"),"s 27"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (60,"ID"),"s 27"));
 
    //page18
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (61,"=="),"r 22"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (61,">"),"r 22"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (61,">="),"r 22"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (62,"ID"),"s 4"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (65,"ID"),"s 36"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (66,"ID"),"s 36"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (68,"ID"),"s 36"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (69,"ID"),"s 36"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (75,"=="),"r 28"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (75,">"),"r 28"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (75,">="),"r 28"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (77,"ID"),"s 92"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (78,"=="),"r 21"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (78,">"),"r 21"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (78,">="),"r 21"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (79,"=="),"r 21"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (79,">"),"r 21"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (79,">="),"r 21"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (80,"=="),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (80,">"),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (80,">="),"r 25"));
 
    //page19
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (81,"=="),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (81,">"),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (81,">="),"r 25"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (82,"ID"),"r 10"));
 
    //page20
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (100,"ID"),"s 4"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (101,"=="),"r 19"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (101,">"),"r 19"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (101,">="),"r 19"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (102,"=="),"r 20"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (102,">"),"r 20"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (102,">="),"r 20"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (103,"=="),"r 23"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (103,">"),"r 23"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (103,">="),"r 23"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (104,"=="),"r 24"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (104,">"),"r 24"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (104,">="),"r 24"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (109,"ID"),"s 21"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (110,"ID"),"s 27"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (111,"ID"),"s 4"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (112,"ID"),"s 27"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (116,"ID"),"r 9"));
 
    //page21
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (121,"ID"),"s 92"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (122,"ID"),"s 92"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (125,"ID"),"s 92"));
 
    //page22 NUM else if then while
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (3,"if"),"s 7"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (3,"while"),"s 11"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (5,"if"),"r 4"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (5,"while"),"r 4"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (6,"if"),"r 5"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (6,"while"),"r 5"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (8,"if"),"r 2"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (8,"while"),"r 2"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (9,"if"),"s 7"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (9,"while"),"s 11"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (12,"if"),"r 3"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (12,"while"),"r 3"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (13,"if"),"s 7"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (13,"while"),"s 11"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (14,"NUM"),"s 22"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (15,"NUM"),"s 28"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (18,"NUM"),"s 28"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (20,"NUM"),"s 37"));
 
    //page 23
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (26,"NUM"),"s 37"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (34,"if"),"r 6"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (34,"while"),"r 6"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (35,"NUM"),"s 37"));
 
    //page24
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (41,"if"),"r 11"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (41,"while"),"r 11"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (42,"NUM"),"s 22"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (43,"NUM"),"s 22"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (45,"NUM"),"s 22"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (46,"NUM"),"s 22"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (49,"NUM"),"r 13"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (50,"NUM"),"r 15"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (51,"NUM"),"r 17"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (52,"NUM"),"r 14"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (53,"NUM"),"r 16"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (54,"NUM"),"s 37"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (55,"then"),"s 77"));
 
    //page25
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (56,"NUM"),"s 28"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (57,"NUM"),"s 28"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (59,"NUM"),"s 28"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (60,"NUM"),"s 28"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (62,"if"),"s 7"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (62,"while"),"s 11"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (65,"NUM"),"s 37"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (66,"NUM"),"s 37"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (68,"NUM"),"s 37"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (69,"NUM"),"s 37"));
 
    //page26
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (77,"if"),"s 95"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (77,"while"),"s 98"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (82,"if"),"r 10"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (82,"while"),"r 10"));
 
    //page 27
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (93,"else"),"r 4"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (94,"else"),"r 5"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (96,"else"),"r 2"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (97,"else"),"s 111"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (99,"else"),"r 3"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (100,"if"),"s 7"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (100,"while"),"s 11"));
 
    //page28
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (109,"NUM"),"s 22"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (110,"NUM"),"s 28"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (111,"if"),"s 7"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (111,"while"),"s 11"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (112,"NUM"),"s 28"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (116,"if"),"r 9"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (116,"while"),"r 9"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (118,"else"),"r 6"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (119,"else"),"r 11"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (120,"then"),"s 122"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (121,"if"),"s 95"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (121,"while"),"s 98"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (122,"if"),"s 95"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (122,"while"),"s 98"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (123,"else"),"r 10"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (124,"else"),"s 125"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (125,"if"),"s 95"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (125,"while"),"s 98"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (126,"else"),"r 9"));
 
    //page29
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (0,"{"),"s 3"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (3,"{"),"s 13"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (3,"}"),"r 8"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (5,"{"),"r 4"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (5,"}"),"r 4"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (6,"{"),"r 5"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (6,"}"),"r 5"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (8,"{"),"r 2"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (8,"}"),"r 2"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (9,"{"),"s 13"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (9,"}"),"r 8"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (10,"}"),"s 17"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (12,"{"),"r 3"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (12,"}"),"r 3"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (13,"{"),"s 13"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (13,"}"),"r 8"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (16,"}"),"r 7"));
 
    //page30
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (19,"}"),"s 34"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (34,"{"),"r 6"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (34,"}"),"r 6"));
 
    //page31
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (41,"{"),"r 11"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (41,"}"),"r 11"));
 
    //page32
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (62,"{"),"s 13"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (77,"{"),"s 100"));
 
    //page33
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (82,"{"),"r 10"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (82,"}"),"r 10"));
 
    //page34
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (100,"{"),"s 13"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (100,"}"),"r 8"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (111,"{"),"s 13"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (113,"}"),"s 118"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (116,"{"),"r 9"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (116,"}"),"r 9"));
 
    //page35
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (121,"{"),"s 100"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (122,"{"),"s 100"));
    action_table.insert (pair <pair <int, string>, string> (pair <int, string> (125,"{"),"s 100"));

}

void initGotoTable ()
{
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (3, "assgstmt"), 5));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (9,"assgstmt"),5));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (13,"assgstmt"),5));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (14,"arithexpr"),23));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (15,"arithexpr"),29));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (15,"boolexpr"),30));
 
    //page30
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (18,"arithexpr"),29));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (18,"boolexpr"),33));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (20,"arithexpr"),38));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (24,"arithexprprime"),44));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (26,"arithexpr"),48));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (29,"boolop"),54));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (31,"arithexprprime"),58));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (35,"arithexpr"),63));
 
    //page31
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (39,"arithexprprime"),67));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (54,"arithexpr"),76));
 
    //page32
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (62,"assgstmt"),5));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (71,"arithexprprime"),88));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (72,"arithexprprime"),89));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (77,"assgstmt"),93));
 
    //page33
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (77,"assgstmt"),93));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (78,"arithexprprime"),101));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (79,"arithexprprime"),102));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (84,"arithexprprime"),105));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (85,"arithexprprime"),106));
 
    //page34
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (100,"assgstmt"),5));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (109,"arithexpr"),114));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (110,"arithexpr"),29));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (110,"boolexpr"),115));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (111,"assgstmt"),5));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (112,"arithexpr"),29));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (112,"boolexpr"),117));
 
    //page35
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (121,"assgstmt"),93));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (122,"assgstmt"),93));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (125,"assgstmt"),93));
 
    //page36
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (0,"compoundstmt"),1));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (0,"program"),2));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (3,"compoundstmt"),6));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (3,"ifstmt"),8));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (3,"stmt"),9));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (3,"stmts"),10));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (3,"whilestmt"),12));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (9,"compoundstmt"),6));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (9,"ifstmt"),8));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (9,"stmt"),9));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (9,"stmts"),16));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (9,"whilestmt"),12));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (13,"compoundstmt"),6));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (13,"ifstmt"),8));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (13,"stmt"),9));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (13,"stmts"),19));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (13,"whilestmt"),12));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (14,"multexpr"),24));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (14,"simpleexpr"),25));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (15,"multexpr"),31));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (15,"simpleexpr"),32));
 
    //page37
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (18,"multexpr"),31));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (18,"simpleexpr"),32));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (20,"multexpr"),39));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (20,"simpleexpr"),40));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (25,"multexprprime"),47));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (26,"multexpr"),39));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (26,"simpleexpr"),40));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (32,"multexprprime"),61));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (35,"multexpr"),39));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (35,"simpleexpr"),40));
 
    //page38
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (40,"multexprprime"),70));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (42,"multexpr"),71));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (42,"simpleexpr"),25));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (43,"multexpr"),72));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (43,"simpleexpr"),25));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (45,"simpleexpr"),73));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (46,"simpleexpr"),74));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (54,"multexpr"),39));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (54,"simpleexpr"),40));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (56,"multexpr"),78));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (56,"simpleexpr"),32));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (57,"multexpr"),79));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (57,"simpleexpr"),32));
 
    //page39
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (59,"simpleexpr"),80));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (60,"simpleexpr"),81));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (62,"compoundstmt"),6));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (62,"ifstmt"),8));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (62,"stmt"),82));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (62,"whilestmt"),12));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (65,"multexpr"),84));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (65,"simpleexpr"),40));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (66,"multexpr"),85));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (66,"simpleexpr"),40));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (68,"simpleexpr"),86));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (69,"simpleexpr"),87));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (73,"multexprprime"),90));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (74,"multexprprime"),91));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (77,"compoundstmt"),94));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (77,"ifstmt"),96));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (77,"stmt"),97));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (77,"whilestmt"),99));
 
    //page40
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (80,"multeexprprime"),103));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (81,"multeexprprime"),104));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (86,"multeexprprime"),107));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (87,"multeexprprime"),108));
 
    //page41
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (100,"compoundstmt"),6));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (100,"ifstmt"),8));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (100,"stmt"),9));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (100,"stmts"),113));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (100,"whilestmt"),12));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (109,"multexpr"),24));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (109,"simpleexpr"),25));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (110,"multexpr"),31));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (110,"simpleexpr"),32));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (111,"compoundstmt"),6));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (111,"ifstmt"),8));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (111,"stmt"),116));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (111,"whilestmt"),12));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (112,"multexpr"),31));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (112,"simpleexpr"),32));
 
    //page42
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (121,"compoundstmt"),94));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (121,"ifstmt"),96));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (121,"stmt"),123));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (121,"whilestmt"),99));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (122,"compoundstmt"),94));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (122,"ifstmt"),96));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (122,"stmt"),124));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (122,"whilestmt"),99));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (125,"compoundstmt"),94));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (125,"ifstmt"),96));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (125,"stmt"),126));
    goto_table.insert (pair <pair <int, string>, int> (pair <int, string> (125,"whilestmt"),99));


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

void analysis (string prog)
{
    int line = 1;

	stack <string> stack;
	stack.push ("$");
	stack.push ("0");

    vector <string> input = tokenize (prog, re);
	input.push_back ("$");

    bool accept = false;
    int p = 0;
    int step = 1;
    while (!accept && !stack.empty ()) {
        int sm = stoi (stack.top ());
        string a = input[p];
        pair <int, string> action_table_key = pair <int, string> (sm, a);
        map <pair <int, string>, string>::iterator it = action_table.find (action_table_key);

		if (it == action_table.end ()) {

            cout << "语法错误，第" << line << "行，缺少\";\"" << endl;
            int n = input.size ();
            vector <string> new_input(n + 1);

            for (int i = 0; i < n+1; i++) {
                if(i<p)
                     new_input[i] = input[i];
                else if(i==p)
                    new_input[i] = ";";
                else 
                    new_input[i] = input[i - 1];
            }

            input = new_input;
            continue;
        }
        else {
            string action_table_value = action_table.find (action_table_key)->second;
            if (action_table_value == "acc") {
                accept = true;
                break;
            }
            vector <string> action_st = tokenize (action_table_value, re);
            string action = action_st[0];
            int action_num = stoi (action_st[1]);

            if (action == "s") {
                stack.push(a);
                stack.push(action_st[1]);
                p ++;
            }
            else if (action == "r") {
                pair <string, string> pr = grammers[action_num - 1];

                prs.push (pr);
                string pr_left = pr.first;
                string pr_right = pr.second;

                vector <string> pr_right_st = tokenize (pr_right, re);
                int pr_right_token_cnt = pr_right_st.size ();
                int i_cnt = 0;

                if (pr_right != "E") {
                    while (i_cnt < 2 * pr_right_token_cnt) {
                        stack.pop();
                        i_cnt ++;
                    }
                }

                int smr = stoi (stack.top ());

                pair <int, string> goto_table_entry = pair <int, string> (smr, pr_left);
                int goto_table_value = goto_table.find (goto_table_entry)->second;
                stack.push (pr_left);
                stack.push (to_string (goto_table_value));
            }
            else {
                cout << "action[" << sm << "," << a << "]查出的动作不是s/r/acc,而是无效的：" << action << endl;
            }

            std::stack <string> tmp_stack;
            tmp_stack= stack;
            tmp_stack.pop ();
            string xm = tmp_stack.top ();

            if (xm == "{" || xm == "}" || xm == ")" || xm == ";")
                line ++;
            }

        
    }
}

void rightMostDerivation ()
{

    vector <string> first_line;
    first_line.push_back("program");
    rightmost_result.push_back (first_line);

    int last_line_index = 0;
    while (!prs.empty ()) {

        pair <string, string> pr = prs.top ();
        prs.pop ();

        string pr_left = pr.first;
        vector <string> last_line = rightmost_result[last_line_index];
        vector <string> cur_line = last_line;

        int handle_index = 0;

        for (int i = 0; i < cur_line.size(); i++) {
            if( pr_left == cur_line[i]){
                handle_index = i;
            }
        }

        string pr_right = pr.second;
        vector <string> pr_right_st = tokenize (pr_right, re);
        cur_line.erase (cur_line.begin () + handle_index);
        int pr_right_st_cnt = pr_right_st.size ();

        if (pr_right != "E") {
            for (int i = 0; i < pr_right_st_cnt; i ++) {
                cur_line.insert (cur_line.begin () + handle_index + i, pr_right_st[i]);
            }
        }


        rightmost_result.push_back (cur_line);
        last_line_index ++;
    }
}

void printResult ()
{
    int n = rightmost_result.size();
    for (int i = 0; i < n; i ++) {
        vector <string> cur_line = rightmost_result[i];
        int m = cur_line.size ();
        for (int j = 0; j < m; j ++) {
            cout << cur_line[j];
            if (j != m - 1)
                cout << " ";
        }
        if (i != n - 1)
            cout << " => " << endl;
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
    initGrammer();
    initActionTable();
    initGotoTable();
    analysis(prog);
    rightMostDerivation();
    printResult();
    
    /********* End *********/
    
}