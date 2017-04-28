#include <iostream>         // console i/o
#include <fstream>          // file i/o
#include <memory>           // unique_ptr
#include <utility>          // forward
#include <stack>            // stack
#include <unordered_set>    // unordered set
#include <vector>           // vector
using std::cout;
using std::endl;
using std::string;
using std::unique_ptr;
using std::ifstream;
using std::forward;
using std::runtime_error;
using std::isalpha;
using std::isdigit;
using std::isspace;
using std::isalnum;
using std::unordered_set;
using std::move;
using std::ostream;
using std::vector;


/*******************  UTILITY FUNCTION FROM C++ 14 ******************/
template<typename T, typename... Args>
unique_ptr<T> make_unique(Args&&... args) {
    return unique_ptr<T>(new T(forward<Args>(args)...));
}



/**************************** CONSTRUCTS ****************************/
enum State {
    START,
    IDENTIFIER,
    INTEGER,
    OPEN_SINGLE_QUOTE,
    OPEN_DOUBLE_QUOTE,
    OPEN_CURLY_BRACKET,
    OPEN_ANGLE_BRACKET,
    CLOSE_ANGLE_BRACKET,
    COLON,
    COLON_EQUALS,
    DOT,
    OCTOTHORPE,
    FINAL
};
enum Token_Type {
    KEYWORD,
    ID,
    INT,
    CHAR,
    STRING,
    COMMENT,
    DONT_CARE,
    END_TOKEN
};
struct Token {
    Token_Type token_type;
    string value;
    bool operator!=(const Token& t) const {
        return (token_type != t.token_type) || (value.compare(t.value) != 0);
    }
    bool operator==(const Token& t) const {
        return (token_type == t.token_type) && (value.compare(t.value) == 0);
    }
};
struct TreeNode {
    int num_children;
    Token token;
    unique_ptr<TreeNode> left, right;
};
ostream& operator << (ostream &o, const unique_ptr<TreeNode> &p) {
    o << p -> token.value << "(" << p -> num_children << ")" << endl;
    return o;
}


/**************************** SCANNER FD ****************************/
Token Scan();
void Handle_Start_State(State& S, Token& t);
void Handle_Identifier_State(State& S, Token& t);
void Handle_Open_Curly_Bracket_State(State& S, Token& t);
void Handle_Colon_State(State& S, Token& t);
void Handle_Colon_Equals_State(State& S, Token& t);
void Handle_Open_Angle_Bracket_State(State& S, Token& t);
void Handle_Close_Angle_Bracket_State(State& S, Token& t);
void Handle_Integer_State(State& S, Token& t);
void Handle_Dot_State(State& S, Token& t);
void Handle_Octothorpe_State(State& S, Token& t);
void Handle_Open_Single_Quote_State(State& S, Token& t);
void Handle_Open_Double_Quote_State(State& S, Token& t);



/**************************** PARSER FD ****************************/
void Read(const Token& t);
void Build_Tree(string& s, int n);
void Tiny();
void Name();
void Consts();
void Const();
void ConstValue();
void Types();
void Type();
void LitList();
void Dclns();
void Dcln();
void SubProgs();
void Fcn();
void Params();
void Body();
void Statement();
void Assignment();
void Expression();
void Term();
void Factor();
void Primary();
void OutExp();
void StringNode();
void ForStat();
void ForExp();
void Caseclause();
void CaseExpression();
void OtherwiseClause();



/**************************** GLOBALS ****************************/
ifstream inf;
char c;
Token Next_Token;
std::stack<unique_ptr<TreeNode>> S;
unordered_set<string> keywords =
        {
                "program", "var", "const", "type", "function",  "return", "begin",
                "end", "output", "if",  "then", "else", "while", "do",
                "case", "of", "otherwise", "repeat", "for", "until",  "loop",
                "pool", "exit", "mod", "and", "or", "not", "read",
                "succ", "pred", "chr", "ord", "eof"
        };
const Token T_program = Token{KEYWORD, "program"},
        T_const = Token{KEYWORD, "const"},
        T_type = Token{KEYWORD, "type"},
        T_var = Token{KEYWORD, "var"},
        T_function = Token{KEYWORD, "function"},
        T_begin = Token{KEYWORD, "begin"},
        T_end = Token{KEYWORD, "end"},
        T_output = Token{KEYWORD, "output"},
        T_or = Token{KEYWORD, "or"},
        T_and = Token{KEYWORD, "and"},
        T_mod = Token{KEYWORD, "mod"},
        T_not = Token{KEYWORD, "not"},
        T_eof = Token{KEYWORD, "eof"},
        T_succ = Token{KEYWORD, "succ"},
        T_pred = Token{KEYWORD, "pred"},
        T_chr = Token{KEYWORD, "chr"},
        T_ord = Token{KEYWORD, "ord"},
        T_if = Token{KEYWORD, "if"},
        T_else = Token{KEYWORD, "else"},
        T_then = Token{KEYWORD, "then"},
        T_while = Token{KEYWORD, "while"},
        T_do = Token{KEYWORD, "do"},
        T_until = Token{KEYWORD, "until"},
        T_repeat = Token{KEYWORD, "repeat"},
        T_for = Token{KEYWORD, "for"},
        T_loop = Token{KEYWORD, "loop"},
        T_pool = Token{KEYWORD, "pool"},
        T_otherwise = Token{KEYWORD, "otherwise"},
        T_of = Token{KEYWORD, "of"},
        T_case = Token{KEYWORD, "case"},
        T_read = Token{KEYWORD, "read"},
        T_exit = Token{KEYWORD, "exit"},
        T_return = Token{KEYWORD, "return"},
        T_colon = Token{DONT_CARE, ":"},
        T_equals = Token{DONT_CARE, "="},
        T_comma = Token{DONT_CARE, ","},
        T_semicolon = Token{DONT_CARE, ";"},
        T_open_parenthesis = Token{DONT_CARE, "("},
        T_close_parenthesis = Token{DONT_CARE, ")"},
        T_colon_equals = Token{DONT_CARE, ":="},
        T_colon_equals_colon = Token{DONT_CARE, ":=:"},
        T_less_equals = Token{DONT_CARE, "<="},
        T_less = Token{DONT_CARE, "<"},
        T_greater = Token{DONT_CARE, ">"},
        T_greater_equals = Token{DONT_CARE, ">="},
        T_not_equals = Token{DONT_CARE, "<>"},
        T_plus = Token{DONT_CARE, "+"},
        T_minus = Token{DONT_CARE, "-"},
        T_star = Token{DONT_CARE, "*"},
        T_slash = Token{DONT_CARE, "/"},
        T_dotdot = Token{DONT_CARE, ".."},
        T_dot = Token{DONT_CARE, "."};



/**************************** MAIN ****************************/

void PreOrderTreeTraversal(const unique_ptr<TreeNode>& root, int N) {
    if (root) {
        for (int i = 0; i < N; i++) {
            cout << ". ";
        }
        cout << root;
        PreOrderTreeTraversal(root -> left, N+1);
        PreOrderTreeTraversal(root -> right, N);
    }
}
void command_line_args_error() {
  throw runtime_error("Invalid command-line args.\n"
                       "The following 2 ways are acceptable:\n"
                       "\t1) 'p1 path/to/testprog'\n"
                       "\t2) 'p1 -ast path/to/testprog'");
}
void file_open_error() {
    throw runtime_error("Failed to open given filepath for testprogram.");
}

int main(int argc, char* argv[]) {
    vector<string> v;
    for (int i = 1; i < argc; ++i)
        v.push_back(std::string(argv[i]));
    if (v.size() == 2) {
        if (v.at(0) == "-ast") {
            inf.open(v.at(1));
            if (inf) {
                inf.get(c);
                Next_Token = Scan();
                Tiny();
                PreOrderTreeTraversal(S.top(), 0);
            } else {
                file_open_error();
            }
        } else {
            command_line_args_error();
        }
    } else if (v.size() == 1) {
        inf.open(v.at(0));
        if (inf) {
            inf.get(c);
            Next_Token = Scan();
            Tiny();
        } else {
            file_open_error();
        }
    } else {
        command_line_args_error();
    }
    inf.close();
    return 0;
}



/**************************** SCANNER ****************************/

Token Scan() {
    Token t = Token();
    State S = START;
    while (S != FINAL && inf.good()) {
        switch (S) {
            case START:
                Handle_Start_State(S, t);
                break;
            case IDENTIFIER:
                Handle_Identifier_State(S, t);
                break;
            case OPEN_CURLY_BRACKET:
                Handle_Open_Curly_Bracket_State(S, t);
                break;
            case COLON:
                Handle_Colon_State(S, t);
                break;
            case COLON_EQUALS:
                Handle_Colon_Equals_State(S, t);
                break;
            case OPEN_ANGLE_BRACKET:
                Handle_Open_Angle_Bracket_State(S, t);
                break;
            case CLOSE_ANGLE_BRACKET:
                Handle_Close_Angle_Bracket_State(S, t);
                break;
            case INTEGER:
                Handle_Integer_State(S, t);
                break;
            case DOT:
                Handle_Dot_State(S, t);
                break;
            case OCTOTHORPE:
                Handle_Octothorpe_State(S, t);
                break;
            case OPEN_SINGLE_QUOTE:
                Handle_Open_Single_Quote_State(S, t);
                break;
            case OPEN_DOUBLE_QUOTE:
                Handle_Open_Double_Quote_State(S, t);
                break;
            default:
                throw runtime_error("Could not resolve state S");
                break;
        }
    }

    // Ignore Comment
    if (t.token_type == COMMENT)
        return Scan();

    // Edge case: Last char in file is space. So reading this causes t to be empty.
    if (t.value.empty())
        t.token_type = END_TOKEN;

    return t;
}

void Handle_Start_State(State& S, Token& t) {
    if (c == '_' || isalpha(c))
        S = IDENTIFIER;
    else if (c == '{')
        S = OPEN_CURLY_BRACKET;
    else if (c == ':')
        S = COLON;
    else if (c == ';' || c == '(' ||c == ')' ||c == '+' ||c == '-' || c == '*' ||
             c == '/' || c == '=' || c == ',') {
        S = FINAL;
        t.token_type = DONT_CARE;
    }
    else if (c == '<')
        S = OPEN_ANGLE_BRACKET;
    else if (c == '>')
        S = CLOSE_ANGLE_BRACKET;
    else if (isdigit(c))
        S = INTEGER;
    else if (c == '.')
        S = DOT;
    else if (c == '#')
        S = OCTOTHORPE;
    else if (c == '\'')
        S = OPEN_SINGLE_QUOTE;
    else if (c == '\"')
        S = OPEN_DOUBLE_QUOTE;
    else if (isspace(c))
        {}
    else
        throw runtime_error(string(1, c));

    if (!isspace(c))
        t.value += c;
    inf.get(c);
}

void Handle_Identifier_State(State& S, Token& t) {
    if (c == '_' || isalnum(c)) {
        S = IDENTIFIER;
        t.value += c;
        inf.get(c);
    } else {
        if (keywords.find(t.value) != keywords.end())
            t.token_type = KEYWORD;
        else
            t.token_type = ID;
        S = FINAL;
    }
}

void Handle_Open_Curly_Bracket_State(State& S, Token& t) {
    if (c != '}')
        S = OPEN_CURLY_BRACKET;
    else {
        t.token_type = COMMENT;
        S = FINAL;
    }
    t.value += c;
    inf.get(c);
}

void Handle_Colon_State(State& S, Token& t) {
    if (c == '=') {
        S = COLON_EQUALS;
        t.value += c;
        inf.get(c);
    } else {
        t.token_type = DONT_CARE;
        S = FINAL;          // Must take empty to final state
    }
}

void Handle_Colon_Equals_State(State& S, Token& t) {
    if (c == ':') {
        S = FINAL;
        t.value += c;
        inf.get(c);
    } else {
        t.token_type = DONT_CARE;
        S = FINAL;          // Must take empty to final state
    }
}

void Handle_Open_Angle_Bracket_State(State& S, Token& t){
    if (c == '>' || c == '=') {
        t.token_type = DONT_CARE;
        S = FINAL;
        t.value += c;
        inf.get(c);
    } else {
        t.token_type = DONT_CARE;
        S = FINAL;          // Must take empty to final state
    }
}

void Handle_Close_Angle_Bracket_State(State& S, Token& t){
    if (c == '=') {
        t.token_type = DONT_CARE;
        S = FINAL;
        t.value += c;
        inf.get(c);
    } else {
        t.token_type = DONT_CARE;
        S = FINAL;          // Must take empty to final state
    }
}

void Handle_Integer_State(State& S, Token& t) {
    if (isdigit(c)) {
        S = INTEGER;
        t.value += c;
        inf.get(c);
    } else {
        t.token_type= INT;
        S = FINAL;          // Must take empty to final state
    }
}

void Handle_Dot_State(State& S, Token& t) {
    if (c == '.') {
        t.token_type = DONT_CARE;
        S = FINAL;
        t.value += c;
        inf.get(c);
    } else {
        t.token_type = DONT_CARE;
        S = FINAL;          // Must take empty to final state
    }
}

void Handle_Octothorpe_State(State& S, Token& t) {
    if (c != '\n') {
        S = OCTOTHORPE;
        t.value += c;
        inf.get(c);
    } else {
        t.token_type = COMMENT;
        S = FINAL;          // Must take empty to final state
    }
}

void Handle_Open_Single_Quote_State(State& S, Token& t) {
    if (c != '\'') {
        S = OPEN_SINGLE_QUOTE;
    } else {
        t.token_type = CHAR;
        S = FINAL;
    }
    t.value += c;
    inf.get(c);
}

void Handle_Open_Double_Quote_State(State& S, Token& t) {
    if (c != '\"') {
        S = OPEN_DOUBLE_QUOTE;
    } else {
        t.token_type = STRING;
        S = FINAL;
    }
    t.value += c;
    inf.get(c);
}


/**************************** PARSER ****************************/

void Read(const Token& t) {
    if (t != Next_Token)
        throw runtime_error("Token did not match expected value.");

    if (t.token_type != KEYWORD && t.token_type != DONT_CARE) {
        unique_ptr<TreeNode> N;

        if (t.token_type == ID) {
            N = make_unique<TreeNode>(TreeNode{1, Token{DONT_CARE, "<identifier>"}, nullptr, nullptr});
            N -> left = make_unique<TreeNode>(TreeNode{0, Token{DONT_CARE, t.value}, nullptr, nullptr});
            S.push(move(N));
            N.release();
        } else if (t.token_type == INT) {
            N = make_unique<TreeNode>(TreeNode{1, Token{DONT_CARE, "<integer>"}, nullptr, nullptr});
            N -> left = make_unique<TreeNode>(TreeNode{0, Token{DONT_CARE, t.value}, nullptr, nullptr});
            S.push(move(N));
            N.release();
        } else if (t.token_type == CHAR) {
            N = make_unique<TreeNode>(TreeNode{1, Token{DONT_CARE, "<char>"}, nullptr, nullptr});
            N -> left = make_unique<TreeNode>(TreeNode{0, Token{DONT_CARE, t.value}, nullptr, nullptr});
            S.push(move(N));
            N.release();
        } else if (t.token_type == STRING) {
            N = make_unique<TreeNode>(TreeNode{1, Token{DONT_CARE, "<string>"}, nullptr, nullptr});
            N -> left = make_unique<TreeNode>(TreeNode{0, Token{DONT_CARE, t.value}, nullptr, nullptr});
            S.push(move(N));
            N.release();
        } else {
            throw runtime_error("Unresolved Token_Type in Read()");
        }
    }
    Next_Token = Scan();
}

void Build_Tree(string& s, int n){
    unique_ptr<TreeNode> p;
    unique_ptr<TreeNode> c;
    for (int i = 1; i <= n; ++i) {
        c = move(S.top());
        S.pop();
        c -> right = move(p);
        p = move(c);
    }
    unique_ptr<TreeNode> N = make_unique<TreeNode>(TreeNode{n, Token{KEYWORD, s}, nullptr, nullptr});
    N -> left = move(p);
    S.push(move(N));
    p.release();
    c.release();
    N.release();
}

void Tiny() {
    Read(T_program);
    Name();
    Read(T_colon);
    Consts();
    Types();
    Dclns();
    SubProgs();
    Body();
    Name();
    Read(T_dot);
    string s = "program";
    Build_Tree(s, 7);
}

void Name() {
    if (Next_Token.token_type == ID)
        Read(Next_Token);
    else
        throw runtime_error("Name() Expected an identifier");
}

void Consts() {
    int N = 1;
    string s = "consts";
    if (Next_Token == T_const) {
        Read(T_const);
        Const();
        while (Next_Token == T_comma) {
            Read(T_const);
            Const();
            N++;
        }
        Read (T_semicolon);
        Build_Tree(s, N);
    } else {
        Build_Tree(s, 0);
    }
}

void Const() {
    Name();
    Read(T_equals);
    ConstValue();
    string s = "const";
    Build_Tree(s, 2);
}

void ConstValue() {
    switch (Next_Token.token_type){
        case INT:
            Read(Next_Token);
            break;
        case CHAR:
            Read(Next_Token);
            break;
        case ID:
            Name();
            break;
        default:
            throw runtime_error("Unresolved Next_Token.token_type in ConstValue()");
            break;
    }

}

void Types() {
    int N = 1;
    string s = "types";
    if (Next_Token == T_type) {
        Read(T_type);
        Type();
        Read(T_semicolon);
        while (Next_Token.token_type == ID) {
            Type();
            Read(T_semicolon);
            N++;
        }
        Build_Tree(s, N);
    } else {
        Build_Tree(s, 0);
    }
}

void Type() {
    Name();
    Read(T_equals);
    LitList();
    string s = "type";
    Build_Tree(s, 2);
}

void LitList() {
    int N = 1;
    string s = "lit";
    Read(T_open_parenthesis);
    Name();
    while (Next_Token == T_comma) {
        Read(T_comma);
        Name();
        N++;
    }
    Read(T_close_parenthesis);
    Build_Tree(s, N);
}

void Dclns() {
    int N = 1;
    string s = "dclns";
    if (Next_Token == T_var){
        Read(T_var);
        Dcln();
        Read(T_semicolon);
        while (Next_Token.token_type == ID) {
            Dcln();
            Read(T_semicolon);
            N++;
        }
        Build_Tree(s, N);
    } else {
        Build_Tree(s, 0);
    }

}

void Dcln() {
   int N = 1;
   string s = "var";
   Name();
   while (Next_Token == T_comma) {
       Read(T_comma);
       Name();
       N++;
   }
   Read(T_colon);
   Name();
   Build_Tree(s, N+1);
}

void SubProgs() {
    int N = 0;
    string s = "subprogs";
    while (Next_Token == T_function) {
        Fcn();
        N++;
    }
    Build_Tree(s, N);
}

void Fcn() {
    Read(T_function);
    Name();
    Read(T_open_parenthesis);
    Params();
    Read(T_close_parenthesis);
    Read(T_colon);
    Name();
    Read(T_semicolon);
    Consts();
    Types();
    Dclns();
    Body();
    Name();
    Read(T_semicolon);
    string s = "fcn";
    Build_Tree(s, 8);
}

void Params() {
    int N = 1;
    string s = "params";
    Dcln();
    while (Next_Token == T_semicolon) {
        Read(T_semicolon);
        Dcln();
        N++;
    }
    Build_Tree(s, N);
}

void Body() {
    int N = 1;
    string s = "block";
    Read(T_begin);
    Statement();
    while (Next_Token == T_semicolon) {
        Read(T_semicolon);
        Statement();
        N++;
    }
    Read(T_end);
    Build_Tree(s, N);

}

void Statement() {
    if (Next_Token.token_type == ID) {
        Assignment();
    } else{
        int N = 1;
        if (Next_Token == T_output) {
            Read(T_output);
            Read(T_open_parenthesis);
            OutExp();
            while (Next_Token == T_comma) {
                Read(T_comma);
                OutExp();
                N++;
            }
            Read(T_close_parenthesis);
            string s = "output";
            Build_Tree(s, N);
        } else if (Next_Token == T_if) {
            Read(T_if);
            Expression();
            Read(T_then);
            Statement();
            if (Next_Token == T_else) {
                Read(T_else);
                Statement();
                N++;
            }
            string s = "if";
            Build_Tree(s, N+1);
        } else if (Next_Token == T_while) {
            Read(T_while);
            Expression();
            Read(T_do);
            Statement();
            string s = "while";
            Build_Tree(s, 2);
        } else if (Next_Token == T_repeat) {
            Read(T_repeat);
            Statement();
            while (Next_Token == T_semicolon) {
                Read(T_semicolon);
                Statement();
                N++;
            }
            Read(T_until);
            Expression();
            string s = "repeat";
            Build_Tree(s, N+1);
        } else if (Next_Token == T_for) {
            Read(T_for);
            Read(T_open_parenthesis);
            ForStat();
            Read(T_semicolon);
            ForExp();
            Read(T_semicolon);
            ForStat();
            Read(T_close_parenthesis);
            Statement();
            string s = "for";
            Build_Tree(s, 4);
        } else if (Next_Token == T_loop) {
            Read(T_loop);
            Statement();
            while (Next_Token == T_semicolon) {
                Read(T_semicolon);
                Statement();
                N++;
            }
            Read(T_pool);
            string s = "loop";
            Build_Tree(s, N);
        } else if (Next_Token == T_case) {
            Read(T_case);
            Expression();
            Read(T_of);
            Caseclause();
            Read(T_semicolon);
            while (Next_Token.token_type == ID || Next_Token.token_type == CHAR
                   ||Next_Token.token_type == INT) {
                Caseclause();
                Read(T_semicolon);
                N++;
            }
            int P = 0;
            if (Next_Token == T_otherwise)
                P++;
            OtherwiseClause();
            Read(T_end);
            string s = "case";
            Build_Tree(s, N+P+1);
        } else if (Next_Token == T_read) {
            Read(T_read);
            Read(T_open_parenthesis);
            Name();
            while (Next_Token == T_comma) {
                Read(T_comma);
                Name();
                N++;
            }
            Read(T_close_parenthesis);
            string s = "read";
            Build_Tree(s, N);
        } else if (Next_Token == T_exit) {
            Read(T_exit);
            string s = "exit";
            Build_Tree(s, 0);
        } else if (Next_Token == T_return) {
            Read(T_return);
            Expression();
            string s = "return";
            Build_Tree(s, 1);
        } else if (Next_Token == T_begin) {
            Body();
        } else {
            string s = "<null>";
            Build_Tree(s, 0);
        }
    }
}

void Assignment() {
    Name();
    if (Next_Token == T_colon_equals) {
        Read(T_colon_equals);
        Expression();
        string s = "assign";
        Build_Tree(s, 2);
    } else {
        Read(T_colon_equals_colon);
        Name();
        string s = "swap";
        Build_Tree(s, 2);
    }
}

void Expression() {
    Term();
    if (Next_Token == T_less_equals) {
        Read(T_less_equals);
        Term();
        string s = "<=";
        Build_Tree(s, 2);
    } else if (Next_Token == T_less) {
        Read(T_less);
        Term();
        string s = "<";
        Build_Tree(s, 2);
    } else if (Next_Token == T_greater) {
        Read(T_greater);
        Term();
        string s = ">";
        Build_Tree(s, 2);
    } else if (Next_Token == T_greater_equals) {
        Read(T_greater_equals);
        Term();
        string s = ">=";
        Build_Tree(s, 2);
    } else if (Next_Token == T_equals) {
        Read(T_equals);
        Term();
        string s = "=";
        Build_Tree(s, 2);
    } else if (Next_Token == T_not_equals) {
        Read(T_not_equals);
        Term();
        string s = "<>";
        Build_Tree(s, 2);
    }
}

void Term() {
    Factor();
    while (Next_Token == T_plus) {
        Read(T_plus);
        Factor();
        string s = "+";
        Build_Tree(s, 2);
    }
    while (Next_Token == T_minus) {
        Read(T_minus);
        Factor();
        string s = "-";
        Build_Tree(s, 2);
    }
    while (Next_Token == T_or) {
        Read(T_or);
        Factor();
        string s = "or";
        Build_Tree(s, 2);
    }
}

void Factor() {
    Primary();
    while (Next_Token == T_star) {
        Read(T_star);
        Primary();
        string s = "*";
        Build_Tree(s, 2);
    }
    while (Next_Token == T_slash) {
        Read(T_slash);
        Primary();
        string s = "/";
        Build_Tree(s, 2);
    }
    while (Next_Token == T_and) {
        Read(T_and);
        Primary();
        string s = "and";
        Build_Tree(s, 2);
    }
    while (Next_Token == T_mod) {
        Read(T_mod);
        Primary();
        string s = "mod";
        Build_Tree(s, 2);
    }
}

void Primary() {
    int N = 1;
    if (Next_Token.token_type == ID) {
        Name();
        if (Next_Token == T_open_parenthesis){
            Read(T_open_parenthesis);
            Expression();
            while (Next_Token == T_comma) {
                Read(T_comma);
                Expression();
                N++;
            }
            Read(T_close_parenthesis);
            string s = "call";
            Build_Tree(s, N+1);
        }
    } else if (Next_Token.token_type == INT) {
        Read(Next_Token);
    } else if (Next_Token.token_type == CHAR) {
        Read(Next_Token);
    } else {
        if (Next_Token == T_minus) {
            Read(T_minus);
            Primary();
            string s = "-";
            Build_Tree(s, 1);
        } else if (Next_Token == T_plus) {
            Read(T_plus);
            Primary();
        } else if (Next_Token == T_not) {
            Read(T_not);
            Primary();
            string s = "not";
            Build_Tree(s, 1);
        } else if (Next_Token == T_eof) {
            Read(T_eof);
            string s = "eof";
            Build_Tree(s, 0);
        } else if (Next_Token == T_open_parenthesis) {
            Read(T_open_parenthesis);
            Expression();
            Read(T_close_parenthesis);
        } else if (Next_Token == T_succ) {
            Read(T_succ);
            Read(T_open_parenthesis);
            Expression();
            Read(T_close_parenthesis);
            string s = "succ";
            Build_Tree(s, 1);
        } else if (Next_Token == T_pred) {
            Read(T_pred);
            Read(T_open_parenthesis);
            Expression();
            Read(T_close_parenthesis);
            string s = "pred";
            Build_Tree(s, 1);
        } else if (Next_Token == T_chr) {
            Read(T_chr);
            Read(T_open_parenthesis);
            Expression();
            Read(T_close_parenthesis);
            string s = "chr";
            Build_Tree(s, 1);
        } else if (Next_Token == T_ord){
            Read(T_ord);
            Read(T_open_parenthesis);
            Expression();
            Read(T_close_parenthesis);
            string s = "ord";
            Build_Tree(s, 1);
        }
    }
}

void OutExp() {
    if (Next_Token.token_type == STRING) {
        StringNode();
        string s = "string";
        Build_Tree(s, 1);
    } else {
        Expression();
        string s = "integer";
        Build_Tree(s, 1);
    }
}

void StringNode() {
    if (Next_Token.token_type == STRING)
        Read(Next_Token);
    else
        throw runtime_error("Could not resolve Next_Token.token_type in StringNode()");
}

void ForStat() {
    if (Next_Token.token_type == ID)
        Assignment();
    else {
        string s = "<null>";
        Build_Tree(s, 0);
    }
}

void ForExp() {
    if (Next_Token.token_type == ID || Next_Token.token_type == CHAR || Next_Token.token_type == INT
            || Next_Token == T_minus || Next_Token == T_plus || Next_Token == T_not
            || Next_Token == T_eof || Next_Token == T_succ || Next_Token == T_pred
            || Next_Token == T_chr || Next_Token == T_ord || Next_Token == T_open_parenthesis) {
        Expression();
    } else {
        string s = "true";
        Build_Tree(s, 0);
    }
}

void Caseclause() {
    int N = 1;
    CaseExpression();
    while (Next_Token == T_comma) {
        Read(T_comma);
        CaseExpression();
        N++;
    }
    Read(T_colon);
    Statement();
    string s = "case_clause";
    Build_Tree(s, N+1);
}

void CaseExpression() {
    ConstValue();
    if (Next_Token == T_dotdot) {
        Read(T_dotdot);
        ConstValue();
        string s = "..";
        Build_Tree(s, 2);
    }
}

void OtherwiseClause() {
    if (Next_Token == T_otherwise) {
        Read(T_otherwise);
        Statement();
        string s = "otherwise";
        Build_Tree(s, 1);
    } else if (Next_Token == T_semicolon) {
        Read(T_semicolon);
    }
}
