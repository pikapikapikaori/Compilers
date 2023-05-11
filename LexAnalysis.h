// C语言词法分析器
#include <cstdio>
#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
using namespace std;

string Reserved[32] = {
	"auto", "break", "case", "char", "const", "continue", 
	"default", "do", "double", "else", "enum", "extern", 
	"float", "for", "goto", "if", "int", "long", 
	"register", "return", "short", "signed", "sizeof", "static", 
	"struct", "switch", "typedef", "union", "unsigned", "void",
	"volatile", "while"
};

int cnt = 1;

/* 不要修改这个标准输入函数 */
void read_prog(string& prog)
{
	char c;
	while(scanf("%c",&c)!=EOF){
		prog += c;
	}
}
/* 你可以添加其他函数 */

bool isDigit (char c)
{
	if ('0' <= c && c <= '9')
		return true;
	else
		return false;
}

bool isLetter (char c)
{
	if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z'))
        return true;
    else
        return false;
}

bool isReserved (string tmpstr)
{
	for (int i = 0; i < 32; i ++)
		if (tmpstr == Reserved[i])
		{
			cout << cnt ++ <<  ": <" << Reserved[i] << "," << i + 1 << ">";
			return true; 
		}
	
	return false;
}

int judgeType (char c)
{
	if (isLetter (c))
		return 1;
	if (isDigit (c))
		return 2;
	return 3;
}

void analysis (string prog, int& p, bool& flag)
{
	while (prog[p] == ' ')
		p ++;
	
	string tmpstr;
	tmpstr += prog[p];
	flag = true;

	switch (judgeType (prog[p]))
	{
		case 1: p ++;
				while (isDigit (prog[p]) || isLetter (prog[p]))
				{
					tmpstr += prog[p];
					p ++;
				}
				p --;

				if (!isReserved (tmpstr))
					cout << cnt ++ << ": <" << tmpstr << ",81>";
				
				break;
		case 2: p ++;
				while (isDigit (prog[p]))
				{
					tmpstr += prog[p];
					p ++;
				}
				p --;

				cout << cnt ++ << ": <" << tmpstr << ",80>";

				break;
		case 3: switch (prog[p])
				{
					case '-': p ++;
							if (prog[p] == '-')
								cout << cnt ++ << ": <--,34>";
							else if (prog[p] == '=')
								cout << cnt ++ << ": <-=,35>";
							else if (prog[p] == '>')
								cout << cnt ++ << ": <->,36>";
							else
							{
								p --;
								cout << cnt ++ << ": <-,33>";
							}

							break;
					case '!': p ++;
							if (prog[p] == '=')
								cout << cnt ++ << ": <!=,38>";
							else
							{
								p --;
								cout << cnt ++ << ": <!,37>";
							}

							break;
					case '%': p ++;
							if (prog[p] == '=')
								cout << cnt ++ << ": <%=,40>";
							else if (isLetter (prog[p]))
							{
								tmpstr += prog[p];
								cout << cnt ++ << ": <" << tmpstr << ",81>";
							}
							else
							{
								p --;
								cout << cnt ++ << ": <%,39>";
							}

							break;
					case '&': p ++;
							if (prog[p] == '&')
								cout << cnt ++ << ": <&&,42>";
							else if (prog[p] == '=')
								cout << cnt ++ << ": <&=,43>";
							else
							{
								p --;
								cout << cnt ++ << ": <&,41>";
							}

							break;
					case '(': cout << cnt ++ << ": <(,44>";

							break;
					case ')': cout << cnt ++ << ": <),45>";

							break;
					case '*': p ++;
							if (prog[p] == '=')
								cout << cnt ++ << ": <*=,47>";
							else
							{
								p --;
								cout << cnt ++ << ": <*,46>";
							}

							break;
					case ',': cout << cnt ++ << ": <,,48>";

							break;
					case '.': cout << cnt ++ << ": <.,49>";

							break;
					case '/': p ++;
							if (prog[p] == '*')
							{
								while (prog[p] != '/')
								{
									tmpstr += prog[p];
									p ++;
								}
								tmpstr += prog[p];
								cout << cnt ++ << ": <" << tmpstr << ",79>";
							}
							else if (prog[p] == '/')
							{
								while (prog[p] != '\n')
								{
									tmpstr += prog[p];
									p ++;
								}
								cout << cnt ++ << ": <" << tmpstr << ",79>";
							}
							else if (prog[p] == '=')
								cout << cnt ++ << ": </=,51>";
							else
							{
								p --;
								cout << cnt ++ << ": </,50>";
							}

							break;
					case ':': cout << cnt ++ << ": <:,52>";

							break;
					case ';': cout << cnt ++ << ": <;,53>";

							break;
					case '?': cout << cnt ++ << ": <?,54>";

							break;
					case '[': cout << cnt ++ << ": <[,55>";

							break;
					case ']': cout << cnt ++ << ": <],56>";

							break;
					case '^': p ++;
							if (prog[p] == '=')
								cout << cnt ++ << ": <^=,58>";
							else
							{
								p --;
								cout << cnt ++ << ": <^,57>";
							}

							break;
					case '{': cout << cnt ++ << ": <{,59>";

							break;
					case '|': p ++;
							if (prog[p] == '|')
								cout << cnt ++ << ": < ||,61>";
							else if (prog[p] == '=')
								cout << cnt ++ << ": <|=,62>";
							else
							{
								p --;
								cout << cnt ++ << ": <|,60>";
							}

							break;
					case '}': cout << cnt ++ << ": <},63>";

							break;
					case '~': cout << cnt ++ << ": <~,64>";

							break;
					case '+': p ++;
							if (prog[p] == '+')
								cout << cnt ++ << ": <++,66>";
							else if (prog[p] == '=')
								cout << cnt ++ << ": <+=,67>";
							else
							{
								p --;
								cout << cnt ++ << ": <+,65>";
							}

							break;
					case '<': p ++;
							if (prog[p] == '<')
							{
								p ++;
								if (prog[p] == '=')
								cout << cnt ++ << ": <<<=,70>";
								else
								{
									p --;
									cout << cnt ++ << ": <<<,69>";
								}
							}
							else if (prog[p] == '=')
								cout << cnt ++ << ": <<=,71>";
							else
							{
								p --;
								cout << cnt ++ << ": <<,68>";
							}

							break;
					case '=': p ++;
							if (prog[p] == '=')
								cout << cnt ++ << ": <==,73>";
							else
							{
								p --;
								cout << cnt ++ << ": <=,72>";
							}

							break;
					case '>': p ++;
							if (prog[p] == '>')
							{
								p ++;
								if (prog[p] == '=')
								cout << cnt ++ << ": <>>=,77>";
								else
								{
									p --;
									cout << cnt ++ << ": <>>,76>";
								}
							}
							else if (prog[p] == '=')
								cout << cnt ++ << ": <>=,75>";
							else
							{
								p --;
								cout << cnt ++ << ": <>,74>";
							}

							break;
					case '\"': cout << cnt ++ << ": <\",78>";

							break;
					default: flag = false;
				}
	}
}

void Analysis()
{
	string prog;
	read_prog(prog);
	/* 骚年们 请开始你们的表演 */
    /********* Begin *********/
    
	bool flag = false;

	for (int p = 0; p < prog.size(); p ++)
	{
		if (flag)
			cout << endl;

		analysis (prog, p, flag);
	}
    
    /********* End *********/
	
}