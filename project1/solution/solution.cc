#include <string>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <map>

#include "IR.h"
#include "IRMutator.h"
#include "IRVisitor.h"
#include "IRPrinter.h"
#include "type.h"


namespace PARSE{

// 类声明
struct MyP;
struct MyS;
struct MyLHS;
struct MyRHS;
struct MySRef;
struct MyTRef;
struct MyCList;
struct MyAList;
struct MyIdExpr;
struct MyConst;
struct MyIntV;
struct MyFloatV;
struct MyData;

struct MyIdExpr_A;
struct MyIdExpr_B;
struct MyRHS_A;
struct MyRHS_B;
struct MyOP;

struct MyData;
struct FileData;

struct TEMP_Definition;

// 全局变量
namespace Global{
    std::string parse_string;
    int now_index; 
    int parse_string_size;
    char op[] = {'+','-','*','/','%'};
    const int op_kind = 5;
    FileData* now_fileData;
    std::map<std::string, int> vlist;
    std::vector<int> clist;
    std::vector<std::string> clist_str;    
    int upbound;
    bool is_complex;
}


/**
 * MyP :: (S)*
 */
struct MyP{
    std::vector<MyS*> s;
    MyP(){
        s.clear();
    }
};

/**
 *  MYS :: LHS = RHS
 *  等号省略
 */
struct MyS{
    MyLHS* lhs;
    MyRHS* rhs;
    MyS(){
        lhs = nullptr;
        rhs = nullptr;
    }
};

/**
 *  LHS :: TREF
 */
struct MyLHS{
    MyTRef* tref;
    MyLHS(){
        tref = nullptr;
    }
};

/**
 * RHS :: RHS_A RSH_B
 */
struct MyRHS{
    MyRHS_A* rhsa;
    MyRHS_B* rhsb;
    MyRHS(){
        rhsa = nullptr;
        rhsb = nullptr;
    }
};

/**
 * RHS_A ::= (RHS)  (0)  
 *         | TRef   (1)
 *         | SRef   (2)
 *         | Const  (3)
 *
 */  
struct MyRHS_A{
    int type;
    MyRHS* rhs;
    MyTRef* tref;
    MySRef* sref;
    MyConst* const_value;
    MyRHS_A(){
        type = -1;
        rhs = nullptr;
        tref = nullptr;
        sref = nullptr;
        const_value = nullptr;
    }
};


/**
 * RHS_B ::= (OP RHS_A)*
 */
struct MyRHS_B{
    std::vector<std::pair<MyOP*,MyRHS_A*>> node;
    MyRHS_B(){
        node.clear();
    }
};


/**
 * OP ::= +     (0)
 *    | -       (1)     
 *    | *       (2) 
 *    | /       (3) 
 *    | %       (4) 
*/
struct MyOP{
    int type;
    char op;
    MyOP():type(-1){}
};

/**
 * TREF :: Id < CList > [ AList ] 
 */
struct MyTRef{
    std::string id;
    MyCList* clist;
    MyAList* alist;
    MyTRef(){
        alist = nullptr;
        clist = nullptr;
    }
};

/**
 * SREF :: Id < CList > 
 */
struct MySRef{
    std::string id;
    MyCList* clist;
    MySRef(){
        clist = nullptr;
    }
};


/**
 * CList :: (IntV)* 
 */
struct MyCList{
    std::vector<MyIntV*> node;
    MyCList(){
        node.clear();
    }
};

/**
 * AList :: (IdExpr)*
 */
struct MyAList{
    std::vector<MyIdExpr*> node;
    // 记录原始语句
    std::vector<std::string> origin_expr;
    // 记录是否为复合语句(是否需要 if-else 判断)
    std::vector<bool> is_complex;
    MyAList(){
        node.clear();
        origin_expr.clear();
        is_complex.clear();
    }
};

/**
 * IdExpr ::= IdExpr_B (IdExpr_A)*
*/
struct MyIdExpr{
    MyIdExpr_B* expb;
    std::vector<MyIdExpr_A*> expa;
    MyIdExpr(){
        expb = nullptr;
        expa.clear();
    }
};


/**
 * IdExpr_A ::= OP IntV     (0)
*             | + IdExpr    (1)
 */ 
struct MyIdExpr_A{
    int type;
    MyOP* op;
    MyIntV* intv;
    MyIdExpr* exp;
    MyIdExpr_A(){
        type = -1;
        op = nullptr;
        intv = nullptr;
        exp = nullptr;
    }
};

/**
 * IdExpr_B ::= Id      (0)
 *          | (IdExpr)  (1)
 */ 
struct MyIdExpr_B{
    int type;
    std::string id;
    MyIdExpr* exp;
    MyIdExpr_B(){
        type = -1;
        exp = nullptr;
    }
};

/**
 * Const :: FloatV 
 *        | IntV
 */ 
struct MyConst{
    int type;
    MyFloatV* floatv;
    MyIntV* intv;
    MyConst(){
        floatv = nullptr;
        intv = nullptr;
    }
};

/**
 * name = value.toString() 
 */
struct MyIntV{
    std::string name;
    int value;
    MyIntV(){}
};

/**
 * name = value.toString() 
 */
struct MyFloatV{
    std::string name;
    float value;
    MyFloatV(){}
};

// 用于保存 json 文件中读出来的信息
struct MyData{
    std::string name;
    std::vector<std::string> ins;
    std::string outs;// 认为 outs 只有一个
    std::string data_type;
    std::string kernel;
    MyData(){
        ins.clear();
        outs.clear();
    }
};

// 每个文件的输出信息
struct FileData{
    // 循环变量
    std::vector<std::vector<std::string>> variable;
    // 循环变量的上界(不含)
    std::vector<std::vector<int>> upbound;
    // 最终的表达式
    std::vector<std::string> expr;
    // 条件判断
    std::vector<std::vector<std::string>> judge;
    // 条件判断的的上界(不含)
    std::vector<std::vector<std::string>> judge_upbound;
    
    FileData(){
        variable.clear();
        upbound.clear();
        expr.clear();
        judge.clear();
        judge_upbound.clear();
    }
};

// 附加的 main 内部的 TEMP 的声明
struct TEMP_Definition{
    std::vector<std::vector<std::string>> definition;

    TEMP_Definition(int size){
        definition.clear();
        definition.resize(size);
        for(int i = 0; i < size; ++i){
            definition[i].clear();
        }
    }
};

// 解析 json 文件
void parse_file(std::vector<MyData> &datas);

// 解析函数头,记录输出文件名,去掉了最后面的 "{}"
void parse_function_head(std::vector<std::string> &function_head, std::vector<std::string>& output_filename);

// 从一个文件中读出 data
MyData get_data(std::string filename);

// 从 kernel 字符串中解析出 P
void parse_kernel(std::string kernel);

// 去除 string 两边的 (引号)(空格)(逗号)
void strip(std::string& s);

// 去除字符串中所有的空格
void cleanBlank(std::string &str);

// 获取文件夹下的所有文件名
void get_file_list(std::vector<std::string>& file_name, const char* dir_name);

// 将 P 中的语句 S 进行拆分
void split_p(MyData &data, std::vector<std::string> &temp_definition);

// parse
void parse_MyP (MyP* arg);
void parse_MyS (MyS* arg);
void parse_MyLHS (MyLHS* arg);
void parse_MyRHS (MyRHS* arg);
void parse_MySRef (MySRef* arg);
void parse_MyTRef (MyTRef* arg);
void parse_MyCList (MyCList* arg);
void parse_MyAList (MyAList* arg);
void parse_MyIdExpr(MyIdExpr* arg);
void parse_MyConst (MyConst* arg);
void parse_MyIntV (MyIntV* arg);
void parse_MyFloatV(MyFloatV* arg);
void parse_MyIdExpr_A(MyIdExpr_A* arg);
void parse_MyIdExpr_B(MyIdExpr_B* arg);
void parse_MyRHS_A(MyRHS_A* arg);
void parse_MyRHS_B(MyRHS_B* arg);
void parse_MyOP(MyOP* arg);

// 当前字符是否匹配上 c(不匹配报错)
void match(char c);

// 当前字符是否匹配上 c(不报错)
bool match_with_no_error(char c);
// 获取一个字符串(id)
void get_Id(std::string& s);

// 是否为数字
inline bool is_digit(char op);

// 是否为字母
inline bool is_letter(char op);

// 是否为 +-*/%(operator),判断出具体哪一个
inline int is_op(char op);

// parse error
void parse_error();

// 获取一个 int
void get_int(MyIntV* v);

// 获取一个 float
void get_float(MyFloatV* v);

// 去除字符串中所有的空格(同时消除整除符号)
void clearBlank(std::string &str);

// 从 Global::parse_string 中截取字符串([l, r))
std::string get_substring(int l, int r);

// visit
void visit_MyP (MyP* arg);
void visit_MyS (MyS* arg);
void visit_MyLHS (MyLHS* arg);
void visit_MyRHS (MyRHS* arg);
void visit_MySRef (MySRef* arg);
void visit_MyTRef (MyTRef* arg);
void visit_MyCList (MyCList* arg);
void visit_MyAList (MyAList* arg);
void visit_MyIdExpr(MyIdExpr* arg);
void visit_MyConst (MyConst* arg);
void visit_MyIntV (MyIntV* arg);
void visit_MyFloatV(MyFloatV* arg);
void visit_MyIdExpr_A(MyIdExpr_A* arg);
void visit_MyIdExpr_B(MyIdExpr_B* arg);
void visit_MyRHS_A(MyRHS_A* arg);
void visit_MyRHS_B(MyRHS_B* arg);
void visit_MyOP(MyOP* arg);

// 将 MyP,MyData 解析成 FileData
FileData* get_fileData(MyP* p, MyData* data);

////////////////////////////////////////////////////////////////

//////////////////////
// PARSE START

// 获取一个 int
void get_int(MyIntV* v){
    char name[35] = {};
    int index = 0;
    while(true){
        char now = Global::parse_string[Global::now_index];
        if(is_digit(now)){
           name[index++] = now;
           ++ Global::now_index;
        }
        else break;
    }
    name[index] = '\0';
    v->name = std::string(name);
    v->value = atoi(name);
}

// 获取一个 float
void get_float(MyFloatV* v){
    char name[35];// 认为长度不超过32
    int index = 0;
    bool has_dot = false;
    while(true){
        char now = Global::parse_string[Global::now_index];
        if(is_digit(now)){
           name[index++] = now;
           ++ Global::now_index;
        }
        else if(now == '.' || !has_dot){
            has_dot = true;
            name[index++] = now;
            ++ Global::now_index;
        }
        else break;
    }
    name[index] = '\0';
    v->name = std::string(name);
    v->value = atof(name);
}

// parse error
void parse_error(){
    std::cerr << "parse error!" << std::endl;
    exit(2);
}

// 是否为数字
inline bool is_digit(char op){
    return (op >= '0' && op <= '9');
}

// 是否为字母
inline bool is_letter(char op){
    return (op <= 'Z' && op >= 'A')||(op <= 'z' && op >= 'a');
}

// 是否为 +-*/%(operator),判断出具体哪一个
inline int is_op(char op){
    for(int i = 0;i < Global::op_kind; ++i){
        if(op == Global::op[i])
            return (i + 1);
    }
    return 0;
}

// 获取一个字符串(id)
void get_Id(std::string& s){
    char name[25];// 认为名字不超过 20
    int index = 0;
    char now = Global::parse_string[Global::now_index];
    // 字母下划线
    if(is_letter(now) || now == '_'){
        name[index++] = now;
        ++ Global::now_index;
    }
    else parse_error();

    // 数字字母下划线
    while(true){
        now = Global::parse_string[Global::now_index];
        if(is_letter(now) || now == '_' || is_digit(now)){
            name[index++] = now;
            ++ Global::now_index;
        }
        else break;
    }
    name[index] = '\0';
    s = std::string(name);
}

// 当前字符是否匹配上 c(不匹配报错)
void match(char c){
    if(Global::parse_string[Global::now_index] != c)
        parse_error();
    ++ Global::now_index;
}

// 当前字符是否匹配上 c(不报错)
bool match_with_no_error(char c){
    return Global::parse_string[Global::now_index] == c;
}

// 从 Global::parse_string 中截取字符串([l, r))
std::string get_substring(int l, int r){
    char temp[50] = {};
    int i = 0;
    for(int j = l; j < r; ++j){
        temp[i++] = Global::parse_string[j];
    }
    temp[i++] = '\0';
    return std::string(temp);
}

void parse_MyP (MyP* arg){
    MyS* s = new MyS();
    arg->s.push_back(s);
    parse_MyS(s);
    match(';');
    while(Global::now_index != Global::parse_string_size){
        s = new MyS();
        arg->s.push_back(s);
        parse_MyS(s);
        match(';');
    }
}

void parse_MyS (MyS* arg){
    arg->lhs = new MyLHS();
    parse_MyLHS(arg->lhs);
    match('=');
    arg->rhs = new MyRHS();
    parse_MyRHS(arg->rhs);
}

void parse_MyLHS (MyLHS* arg){
    arg->tref = new MyTRef();
    parse_MyTRef(arg->tref);
}

void parse_MyRHS (MyRHS* arg){
    arg->rhsa = new MyRHS_A();
    parse_MyRHS_A(arg->rhsa);
    arg->rhsb = new MyRHS_B();
    parse_MyRHS_B(arg->rhsb);
}

void parse_MySRef (MySRef* arg){
    get_Id(arg->id);
    match('<');
    arg->clist = new MyCList();
    parse_MyCList(arg->clist);
    match('>');
}

void parse_MyTRef (MyTRef* arg){
    get_Id(arg->id);
    match('<');
    arg->clist = new MyCList();
    parse_MyCList(arg->clist);
    match('>');
    match('[');
    arg->alist = new MyAList();
    parse_MyAList(arg->alist);
    match(']');
}

void parse_MyCList (MyCList* arg){
    MyIntV* v = new MyIntV();
    get_int(v);
    arg->node.push_back(v);
    while(match_with_no_error(',')){
        match(',');
        v = new MyIntV();
        get_int(v);
        arg->node.push_back(v);
    }
}

void parse_MyAList (MyAList* arg){
    MyIdExpr* exp = new MyIdExpr();
    // for origin_expr
    int l, r;
    l = Global::now_index;
    Global::is_complex = false;
    parse_MyIdExpr(exp);
    arg->is_complex.push_back(Global::is_complex);
    arg->node.push_back(exp);
    while(match_with_no_error(',')){
        r = Global::now_index;
        arg->origin_expr.push_back(get_substring(l, r));
        match(',');
        l = Global::now_index;
        exp = new MyIdExpr();
        Global::is_complex = false;
        parse_MyIdExpr(exp);
        arg->is_complex.push_back(Global::is_complex);
        arg->node.push_back(exp);
    }
    r = Global::now_index;
    arg->origin_expr.push_back(get_substring(l, r));
}

void parse_MyIdExpr(MyIdExpr* arg){
    arg->expb = new MyIdExpr_B();
    parse_MyIdExpr_B(arg->expb);
    MyIdExpr_A* expa;
    while(is_op(Global::parse_string[Global::now_index])){
        Global::is_complex = true;
        expa = new MyIdExpr_A();
        parse_MyIdExpr_A(expa);
        arg->expa.push_back(expa); 
    }
}


void parse_MyIdExpr_A(MyIdExpr_A* arg){
    char next_char = Global::parse_string[Global::now_index + 1]; 
    if(is_digit(next_char)){
        arg->type = 0;
        arg->op = new MyOP();
        parse_MyOP(arg->op);
        arg->intv = new MyIntV();
        parse_MyIntV(arg->intv);
    }
    else{
        arg->type = 1;
        match('+');
        arg->exp = new MyIdExpr();
        parse_MyIdExpr(arg->exp);
    }
}

void parse_MyIdExpr_B(MyIdExpr_B* arg){
    if(match_with_no_error('(')){
        match('(');
        arg->type = 1;
        arg->exp = new MyIdExpr();
        parse_MyIdExpr(arg->exp);
        match(')');
    }
    else{
        arg->type = 0;
        get_Id(arg->id);
    }
}


void parse_MyRHS_A(MyRHS_A* arg){
    char now = Global::parse_string[Global::now_index];
    if(match_with_no_error('(')){
        match('(');
        arg->type = 0;
        arg->rhs = new MyRHS();
        parse_MyRHS(arg->rhs);
        match(')');
    }
    else if(is_digit(now)){
        arg->type = 3;
        arg->const_value = new MyConst();
        parse_MyConst(arg->const_value);
    }
    else{
        int restore_index = Global::now_index;
        while(!match_with_no_error('<'))
            ++ Global::now_index;
        match('<');
        while(!match_with_no_error('>'))
            ++ Global::now_index;
        match('>');
        if(match_with_no_error('[')){
            arg->type = 1;
            arg->tref = new MyTRef();
            Global::now_index = restore_index;
            parse_MyTRef(arg->tref);
        }
        else{
            arg->type = 2;
            arg->sref = new MySRef();
            Global::now_index = restore_index;
            parse_MySRef(arg->sref);
        }
    }
}

void parse_MyRHS_B(MyRHS_B* arg){
    MyOP* op;
    MyRHS_A* rhsa;
    while(is_op(Global::parse_string[Global::now_index])){
       op = new MyOP();
       parse_MyOP(op);
       rhsa = new MyRHS_A();
       parse_MyRHS_A(rhsa);
       arg->node.push_back(std::make_pair(op, rhsa)); 
    }
}

void parse_MyOP(MyOP* arg){
    arg->type = is_op(Global::parse_string[Global::now_index]) - 1;
    if(arg->type == -1)
        parse_error();
    arg->op = Global::op[arg->type];
    ++ Global::now_index;
}


void parse_MyConst (MyConst* arg){
    // 先获取一个 int, 如果接下来是小数点的话就说明是个 float
    int restore_index = Global::now_index;
    MyIntV* int_v = new MyIntV();
    get_int(int_v);
    if(match_with_no_error('.')){
        Global::now_index = restore_index;
        MyFloatV* float_v = new MyFloatV();
        get_float(float_v);
        arg->type = 0;
        arg->floatv = float_v;
    }
    else{
        arg->type = 1;
        arg->intv = int_v;
    }
}

void parse_MyIntV (MyIntV* arg){
    get_int(arg);
}

void parse_MyFloatV(MyFloatV* arg){
    get_float(arg);
}


// PARSE END
//////////////////////

//////////////////////
// VISIT START
// visit
void visit_MyP (MyP* arg){
    int size = arg->s.size();
    for(int i = 0; i < size; ++ i){
        Global::now_index = i;
        Global::vlist.clear();
        // add
        Global::now_fileData->variable.resize(i + 1);
        Global::now_fileData->variable[i].clear();
        Global::now_fileData->upbound.resize(i + 1);
        Global::now_fileData->upbound[i].clear();
        Global::now_fileData->judge.resize(i + 1);
        Global::now_fileData->judge[i].clear();
        Global::now_fileData->judge_upbound.resize(i + 1);
        Global::now_fileData->judge_upbound[i].clear();

        visit_MyS(arg->s[i]);
        int size = Global::now_fileData->variable[i].size();
        for(int j = 0; j < size; ++j){
            Global::now_fileData->upbound[i].push_back(
                Global::vlist[Global::now_fileData->variable[i][j]]
                );
        }
    }
}
void visit_MyS (MyS* arg){
    visit_MyLHS(arg->lhs);
    visit_MyRHS(arg->rhs);
}

void visit_MyLHS (MyLHS* arg){
    visit_MyTRef(arg->tref);
}

void visit_MyRHS (MyRHS* arg){
    visit_MyRHS_A(arg->rhsa);
    visit_MyRHS_B(arg->rhsb);
}

void visit_MySRef (MySRef* arg){
    return;
}

void visit_MyTRef (MyTRef* arg){
    visit_MyCList(arg->clist);
    visit_MyAList(arg->alist);
}

void visit_MyCList (MyCList* arg){
    int size = arg->node.size();
    Global::clist.clear();
    Global::clist_str.clear();
    for(int i = 0; i < size; ++i){
        Global::clist.push_back(arg->node[i]->value);
        Global::clist_str.push_back(arg->node[i]->name);
    }
}

void visit_MyAList (MyAList* arg){
    int size = arg->node.size();
    for(int i = 0;i < size; ++i) {
        Global::upbound = Global::clist[i];
        if(arg->is_complex[i]){
            Global::now_fileData->judge[Global::now_index].push_back(arg->origin_expr[i]);
            Global::now_fileData->judge_upbound[Global::now_index].push_back(Global::clist_str[i]);
        }
        visit_MyIdExpr(arg->node[i]);
    }
}

void visit_MyIdExpr(MyIdExpr* arg){
    visit_MyIdExpr_B(arg->expb);
    int size = arg->expa.size();
    for(int i = 0;i < size;++i){
        visit_MyIdExpr_A(arg->expa[i]);
    }
}

void visit_MyConst (MyConst* arg){
    return;
}

void visit_MyIntV (MyIntV* arg){
    return;
}

void visit_MyFloatV(MyFloatV* arg){
    return;
}

void visit_MyIdExpr_A(MyIdExpr_A* arg){
    if(arg->type == 0){
        return;
    }
    else{
        visit_MyIdExpr(arg->exp);
    }
}

void visit_MyIdExpr_B(MyIdExpr_B* arg){
    if(arg->type == 0){
        std::string id = arg->id;
        // exsits
        if(Global::vlist.find(id) != Global::vlist.end()){
            if(Global::upbound < Global::vlist[id])
                Global::vlist[id] = Global::upbound;
        }
        // set it
        else{
            Global::vlist[id] = Global::upbound;
            Global::now_fileData->variable[Global::now_index].push_back(id);
        }
    }
    else{
        visit_MyIdExpr(arg->exp);
    }
}

void visit_MyRHS_A(MyRHS_A* arg){
    if(arg->type == 0){
        visit_MyRHS(arg->rhs);
    }
    else if(arg->type == 1){
        visit_MyTRef(arg->tref);
    }
    else if(arg->type == 2){
        visit_MySRef(arg->sref);
    }
    // else if(arg->type == 3) visit_MyConst(arg->const_value);
}

void visit_MyRHS_B(MyRHS_B* arg){
    int size = arg->node.size();
    for(int i = 0;i < size; ++i){
        // visit_MyOP(arg->node[i].first);
        visit_MyRHS_A(arg->node[i].second);
    }
}

void visit_MyOP(MyOP* arg){
    return;
}
// VISIT END
//////////////////////



// 去除字符串中所有的空格(同时消除整除符号)
void clearBlank(std::string &str){
    int size = str.size();
    char t[1024];
    int index = -1;
    for(int i = 0; i < size; ++i){
        if(str[i] != ' '){
            t[++index] = str[i];
            // 消除整除符号
            if(str[i] == '/' && i+1 < size  && str[i+1] == '/'){
                ++i;
            }
        }
    }
    t[++index] = '\0';
    str = std::string(t);
}

// 去除 string 两边的 (引号)(空格)(逗号)
void strip(std::string& s){
    int lindex = 0;
    int rindex = s.size() - 1;
    while(s[lindex] == ' ' || s[lindex] == '\"' || s[lindex] == ',')
        ++lindex;
    while(s[rindex] == ' ' || s[rindex] == '\"' || s[rindex] == ',')
        --rindex;
    s = s.substr(lindex, rindex - lindex + 1);
}

// 从一个文件中读出 data
MyData get_data(std::string filename){
    std::ifstream in(filename.c_str());
    MyData ret;
    std::string temp;
    char tempc[1024];// 认为 kernel 长度不超过 1024
    for(int i = 0; i < 7; ++i){
        in.getline(tempc, 1024);
        temp = std::string(tempc);
        int lindex, rindex;
        if(temp.find("\"name\"") != -1){
            lindex = temp.find(":") + 1;
            rindex = temp.size() - 2;
            temp = temp.substr(lindex , rindex - lindex);
            strip(temp);
            ret.name = temp;
        }
        else if(temp.find("ins") != -1){
            lindex = temp.find("[") + 1;
            rindex = temp.find("]");
            temp = temp.substr(lindex, rindex - lindex);
            rindex = -1;
            while(true){
                lindex = rindex + 1;
                rindex = temp.find(",", lindex);
                if(rindex != -1){
                    std::string t = temp.substr(lindex, rindex - lindex);
                    strip(t);
                    ret.ins.push_back(t);
                }
                else{
                    std::string t = temp.substr(lindex, temp.size() - lindex);
                    strip(t);
                    ret.ins.push_back(t);
                    break;
                }
            }
        }
        else if(temp.find("\"outs\"") != -1){
            lindex = temp.find("[") + 1;
            rindex = temp.find("]");
            temp = temp.substr(lindex , rindex - lindex);
            strip(temp);
            ret.outs = temp;
        }
        else if(temp.find("\"kernel\"") != -1){
            lindex = temp.find(":") + 1;
            rindex = temp.size() - 1;
            temp = temp.substr(lindex , rindex - lindex);
            strip(temp);
            clearBlank(temp);
            ret.kernel = temp;
        } 
        else if(temp.find("\"data_type\"") != -1){
            lindex = temp.find(":") + 1;
            rindex = temp.size() - 2;
            temp = temp.substr(lindex , rindex - lindex);
            strip(temp);
            ret.data_type = temp;
        } 
    }
    return ret;
}

////////////////
// JUST DEBUG //
////////////////
void test(std::vector<MyData>& datas){
    for(int i = 0; i < datas.size(); ++i ){
        std::cout << i << std::endl;
        std::cout << datas[i].name << std::endl;
        for(int j = 0; j < datas[i].ins.size(); ++j){
            std::cout << "\t" << datas[i].ins[j] << std::endl;
        }
        std::cout << datas[i].outs << std::endl;
        std::cout << datas[i].kernel << std::endl;
        std::cout << datas[i].data_type << std::endl;
    }
}

// 获取文件夹下的所有文件名
void get_file_list(std::vector<std::string>& file_name, const char* dir_name){
    std::cout << "start getting files from " << dir_name << std::endl;
    // 打开文件夹
    DIR* dir;
    if((dir = opendir(dir_name)) == nullptr){
        std::cerr << "file not found!" << std::endl;
        exit(1);
    }

    // 获取文件名
    struct dirent *ptr;
    while((ptr = readdir(dir)) != nullptr){
        // "." or ".."
        if(strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
            continue;
        // file(8), link-file(10), dir(4)
        if(ptr->d_type == 8)
            file_name.push_back(ptr->d_name);
    }
    // 关闭文件夹
    closedir(dir);

    // 排序保证文件顺序的一致性
    sort(file_name.begin(), file_name.end());
}

// 解析 json 文件
void parse_file(std::vector<MyData>& datas){
    // 记录文件名
    std::vector<std::string> file_name;
    file_name.clear();

    // 获取文件
    get_file_list(file_name, "./cases");

    // 解析
    int size = file_name.size();
    for(int i = 0; i < size ; ++i){
        datas.push_back(get_data("./cases/" + file_name[i]));
    }
}

// 解析函数头,记录输出文件名,去掉了最后面的 "{}"
void parse_function_head(std::vector<std::string> &function_head, std::vector<std::string> &output_filename){
    // 获取文件名
    get_file_list(output_filename, "./kernels");
    int size = output_filename.size();
    for(int i = 0; i < size; ++i){
        std::ifstream in("./kernels/" + output_filename[i]);
        std::ostringstream ss;
        ss << in.rdbuf();
        std::string head = ss.str();
        int last = head.size() - 1;
        while(head[last] != '}') --last;
        --last;
        while(head[last] != '{') --last;
        function_head.push_back(head.substr(0, last - 0));
    }
}

// 将 MyP,MyData 解析成 FileData
FileData* get_fileData(MyP* p, MyData &data){
    FileData* ret = new FileData();

    // ret->expr
    char expr[1024]= {};
    int now_index = 0;
    char now_char;
    int kernel_length = data.kernel.size();
    for(int i = 0; i < kernel_length; ++i){
        now_char = data.kernel[i];
        if(now_char == '<'){
            while(data.kernel[++i] != '>');
        }
        else if(now_char == ','){
            expr[now_index++] = ']';
            expr[now_index++] = '[';
        }
        else if(now_char == ';'){
            expr[now_index++] = ';';
            expr[now_index++] = '\0';
            ret->expr.push_back(std::string(expr));
            now_index = 0;
        }
        else{
            expr[now_index++] = now_char;            
        }
    }

    // ret->judge, ret->judge_upbound, ret->variable, ret->upbound;
    Global::now_fileData = ret;
    visit_MyP(p);

    // return
    return ret;
}

// 将 P 中的语句 S 进行拆分
void split_p(MyData &data, std::vector<std::string> &temp_definition){
    std::string temp_prefix = "TEMP__";
    int index_for_temp = 0;
    std::string type = data.data_type;
    std::string new_kernel = "";
    int old_kernel_last = data.kernel.size() - 1;

    // 将 kernel 按照 ';' 切分
    std::vector<std::string> ps;
    ps.clear();
    int l = 0, r = -1;
    while(r != old_kernel_last){
        l = r + 1;
        r = data.kernel.find(";", l);
        // ps 不带分号
        ps.push_back(data.kernel.substr(l, r - l));
    }

    // 对每一个语句进行转化
    char t[1024];
    int index_for_t, index_for_ps;
    int ps_size = ps.size();
    for(int i = 0; i < ps_size; ++i){
        int semicolon = 0;
        int length = ps[i].size();
        int equal_pos = ps[i].find("=");
        int shape_start;
        // 添加变量的定义
        index_for_t = 0;
        t[index_for_t ++] = ' ';        
        for(int j = 0;; ++j){
            char tmp01 = ps[i][j];
            if(tmp01 == '>'){
                t[index_for_t ++] = ']';
                break;
            }
            else if(tmp01 == '<'){
                shape_start = j;
                index_for_t=0;
                t[index_for_t ++] = '[';                
            }
            else if(tmp01 == ','){
                t[index_for_t ++] = ']';
                t[index_for_t ++] = '[';
            }
            else{
                t[index_for_t ++] = tmp01;              
            }
        }
        t[index_for_t++] = ';';
        t[index_for_t] = '\0';
        std::string temp_name = temp_prefix + std::to_string(++index_for_temp);
        temp_definition.push_back(type + " " + temp_name + std::string(t));
        
        // 进行逐项的转化                
        std::string temp_tensor = temp_name + ps[i].substr(shape_start, equal_pos - shape_start);
        // 开头添加 : 初始化为 0
        new_kernel = new_kernel + (temp_tensor + "=0;");
        bool first = true;
        char op = '+';
        for(int j = equal_pos + 1; j < length; ++j){
            index_for_t = 0;
            // op
            if(first){
               first = false; 
            }
            else{
                op = ps[i][j];
                // std::cout << op << std::endl;
                ++ j;
            }
            // factor
            l = j;
            int lbracket = 0;
            std::string& p_now = ps[i];
            for(;j<length;++j){
                char tt = p_now[j];
                // TODO 认为括号匹配 
                if(tt == '(' || tt == '<' || tt == '['){
                    ++lbracket;
                }
                else if(tt == ')' || tt == '>' || tt == ']'){
                    --lbracket;
                }
                else if((tt == '+' || tt == '-') && lbracket == 0){
                    --j;// 回退
                    break;
                }
            }
            r = j + 1;
            new_kernel = new_kernel 
                + (temp_tensor + "=" + temp_tensor + op + p_now.substr(l ,r - l) + ';');
        }
        // restore
        new_kernel = new_kernel
            +(ps[i].substr(0, equal_pos - 0) + '=' + temp_tensor + ';'); 
    }
    // end
    data.kernel = new_kernel;
}

}


//namespace PRINT
namespace PRINT
{
    //根据Filedata和Temp_Definition生成IR语法树
    void generate_IR(const PARSE::FileData *data, std::vector<Boost::Internal::Stmt> &IR_roots, std::fstream& log)
    {
        IR_roots.clear();
        using namespace Boost::Internal;
        Type nonetype;
        nonetype.code = TypeCode::String;
        Type index_type = Type::int_scalar(32);
        for (int i = 0; i < data->expr.size(); i++)
        {//每次循环将生成一个loopnest结构
            //生成主语句
            Expr main_expr = StringImm::make(nonetype, data->expr[i]);
            Stmt main_stmt = String_Stmt::make(main_expr);

            //生成循环下标
            std::vector<Expr> indexs;
            indexs.clear();
            for (int j = 0; j < data->variable[i].size(); j++)
            {
                Expr index = myIndex::make(index_type, data->variable[i][j], 0, data->upbound[i][j]);
                indexs.push_back(index);
            }

            //生成判断语句
            Stmt new_main_stmt;
            if (data->judge[i].size() > 0)
            {
                std::vector<Expr> judges;
                judges.clear();
                for (int j = 0; j < data->judge[i].size(); j++)
                {
                    Expr index_name = StringImm::make(index_type, data->judge[i][j]);
                    Expr upbound = StringImm::make(index_type, data->judge_upbound[i][j]);
                    Expr cmp = Compare::make(index_type, CompareOpType::LT, index_name, upbound);
                    judges.push_back(cmp);
                }
                std::vector<Expr> conds;
                conds.push_back(judges[0]);
                for (int j=1;j<judges.size(); j++){
                    Expr cond=Binary::make(index_type, BinaryOpType::And, conds[j-1], judges[j]);
                    conds.push_back(cond);
                }
                Expr blank_expr = StringImm::make(nonetype, "");
                Stmt blank_stmt = String_Stmt::make(blank_expr);
                new_main_stmt = IfThenElse::make(conds[conds.size()-1], main_stmt, blank_stmt);
            }
            else {
                new_main_stmt = main_stmt;
            }

            //生成循环语句
            Stmt loop_stmt = LoopNest::make(indexs, {new_main_stmt});


            IR_roots.push_back(loop_stmt);
        }
    }
    //just use for debug
    void printInfo(const PARSE::FileData* data, std::fstream& log){
        log << "FileData:\n";
        int size = data->expr.size();
        log << "indexs:\n";
        for (int i=0; i<size; i++){
            for (int j=0; j<data->variable[i].size();j++){
                log << data->variable[i][j] << ' ';
            }
            log << '\n';
        }
        log << "exprs:\n";
        for (int i=0;i<size; i++){
            log << data->expr[i] << '\n';
        }
    }

} // namespace PRINT
//namespace PRINT end

////////////
////////////
//  MAIN  //
////////////
////////////

int main(){
    std::cout << "solution begin" << std::endl;
    
    // all_list 记录所有的 P
    std::vector<PARSE::MyP*> all_list;
    all_list.clear();

    // function_head 记录所有的函数头
    std::vector<std::string> function_head;
    function_head.clear();
    
    // 记录 data
    std::vector<PARSE::MyData> datas;
    datas.clear();
    
    // 记录输出的文件名
    std::vector<std::string> output_filename;
    output_filename.clear();
    
    // 解析
    PARSE::parse_file(datas);
    PARSE::parse_function_head(function_head, output_filename);
    
    // test(datas);
    // 将 P 进行切分
    /**
     * 1. a[i,j] = a[j,i]
     * 2. a[i] = b[j] + c[k];
     * 
     * 1.
     *   type temp[i_width][j_width] = {};// 直接输出
     *   temp[i, j] = 0;
     *   temp[i, j] = temp[i, j] + a[j, i];
     *   a[i, j] = temp[i, j];
     * 2.
     *   type temp[i_width] = {};// 直接输出
     *   temp[i] = 0;
     *   temp[i] = temp[i] + b[j];
     *   temp[i] = temp[i] + c[k];
     *   a[i] = temp[i];
    */
    int size = output_filename.size();
    PARSE::TEMP_Definition* temp_definition = new PARSE::TEMP_Definition(size);
    for(int i = 0; i < size; ++i){
        PARSE::split_p(datas[i], temp_definition->definition[i]);
    }

    // 解析成 MyP
    for(int i = 0; i < size; ++i){
        PARSE::Global::parse_string = datas[i].kernel;
        PARSE::Global::now_index = 0;
        PARSE::Global::parse_string_size = datas[i].kernel.size();
        all_list.push_back(new PARSE::MyP());
        // std::cout << PARSE::Global::parse_string << std::endl;
        PARSE::parse_MyP(all_list[i]);
    }

    // 解析成 FileData
    std::vector<PARSE::FileData*> infos;
    infos.clear();

    for(int i = 0; i < size; ++i){
        infos.push_back(PARSE::get_fileData(all_list[i], datas[i]));
    }

    ///////////////////////
    /// PRINTER // TODO ///
    ///////////////////////
    std::fstream log("./log.txt",std::ios::out);  //输出调试

    std::vector<Boost::Internal::Stmt> codes;
    for(int i=0;i<size;i++){
        PRINT::printInfo(infos[i], log);
        std::fstream ofs("./kernels/"+output_filename[i], std::ios::out);
        if (ofs.bad()){
            std::cout << "failed open file " << output_filename[i] << std::endl;
            return -1;
        }
        //函数头
        ofs << function_head[i] << " {\n";
        //临时变量定义
        for (int j=0; j<temp_definition->definition[i].size(); j++){
            ofs << "  " << temp_definition->definition[i][j] << std::endl;
        }
        //代码主体
        PRINT::generate_IR(infos[i], codes, log);
        log << "result_code:\n";
        for (int j=0;j<codes.size();j++){
            Boost::Internal::IRPrinter printer;
            printer.enter();
            std::string code=printer.print(codes[j]);
            log << code;
            ofs << code;
        }
        ofs << "}\n";
        ofs.close();
    }

    // 释放空间
    // TODO
    return 0;
}