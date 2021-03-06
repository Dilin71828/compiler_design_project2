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
    char op[] = {'+','-','*','/','%', '$'};
    const int op_kind = 6;
    FileData* now_fileData;
    std::map<std::string, int> vlist;
    std::vector<int> clist;
    std::vector<std::string> clist_str;    
    int upbound;
    bool is_complex;
    bool judge_upbound; // 是否需要判断存在上界(有无减号) // TODO ADD
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
*             | OP IdExpr    (1)
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
    std::vector<std::string> grad_to;
    MyData(){
        ins.clear();
        outs.clear();
        grad_to.clear();
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

/// p2
void complex2easy(std::string &str);

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
        // match('+');
        arg->op = new MyOP();
        parse_MyOP(arg->op);
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
        // 如果当前语句中含有减号,则不进行上界判断
        // 投机,输入中没有括号
        Global::judge_upbound = true;
        for(int j = 0;j < arg->node[i]->expa.size(); ++j){
            if(arg->node[i]->expa[j]->op->type == 1){
                Global::judge_upbound = false;
                break;
            }
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
        if(Global::judge_upbound) {
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
                t[index] = '$';
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
    for(int i = 0; i < 8; ++i){
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
        else if(temp.find("\"grad_to\"") != -1){
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
                    ret.grad_to.push_back(t);
                }
                else{
                    std::string t = temp.substr(lindex, temp.size() - lindex);
                    strip(t);
                    ret.grad_to.push_back(t);
                    break;
                }
            }
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
                index_for_t = 0;
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

// project2 start

// 是否为常数
bool is_constant(std::string &str){
    int size = str.size();
    for(int i = 0; i < size; ++i){
        if(str[i] != '.' && !is_digit(str[i]))
        return false;
    }
    return true;
}

// 删除常数,常数必须以单独的加法或者减法出现
void clear_constant(std::string &old){
    int start =  old.find('=') + 1;
    int equal_pos_plus_1 = start;
    // 注意 substr(start, length)
    int size = old.size();
    int now = start - 1;
    int open = 0;// 表示是否匹配上括号,若匹配上,说明不是单独的常数
    std::vector<std::string> constant_to_replace;// 记录需要替换的常数
    std::vector<int> constant_pos;// 记录需要替换常数的位置
    constant_to_replace.clear();
    constant_pos.clear();
    std::string long_blank(20, ' ');// 认为常数长度不超过20
    while(++now < size){
        char tempc = old[now];
        if(tempc == '(' || tempc == '[' || tempc == '<'){
            ++open;
        }
        else if(tempc == ')' || tempc == ']' || tempc == '>'){
            --open;
            if(open == 0){
                int skip = 1;
                // skip a op(+/-)
                if(old[now + 1] == '+' || old[now + 1] == '-') ++skip;
                start = now + skip;
            }
        }
        else if(tempc == '+' || tempc == '-'){
            // 可能出现由右括号更新的情况(now>start -> OK)
            if(open != 0 || now <= start){
                continue;
            }
            std::string temp_str = old.substr(start, now - start);
            if(is_constant(temp_str)) {
                if(old[start - 1] != '=') {
                    temp_str = old[start - 1] + temp_str;
                    --start;
                }
                constant_to_replace.push_back(temp_str);
                constant_pos.push_back(start);
            }
            start = now + 1;
        }
        else if(tempc == ';'){
            // 表示结尾
            // 可能出现由右括号更新的情况(now>start -> OK)
            if(now <= start){
                continue;
            }
            std::string temp_str = old.substr(start, now - start);
            if(is_constant(temp_str)) {
                if(old[start - 1] != '=') {
                    temp_str = old[start - 1] + temp_str;
                    --start;
                }
                constant_to_replace.push_back(temp_str);
                constant_pos.push_back(start);
            }
        }
    }
    // 替换
    for(int i = 0;i < constant_to_replace.size(); ++i){
        old.replace(constant_pos[i], constant_to_replace[i].size(),
            long_blank, 0, constant_to_replace[i].size()); 
    }
    clearBlank(old);
    if(old[equal_pos_plus_1] == '+'){
        old.replace(equal_pos_plus_1, 1, "", 0, 0);
    }
    if(old[equal_pos_plus_1] == '-'){
        old.replace(equal_pos_plus_1, 1, "0-", 0, 2);
    }
}

// 乘法分配律
void distribute(std::string &str){
    int size = str.size();
    // 检验是否需要初始化
    std::string new_str = "";
    int equal_pos = str.find('=');
    int r = 0;
    while(str[r] != '<') ++r;
    std::string out = str.substr(0, r);
    std::string out_all = str.substr(0, equal_pos);
    // 从后往前找到的第一个out为开头,说明右边没有用到out,可以初始化
    if(str.find_last_of(out) == 0){
        new_str += out_all + "=0;";
    }
    // 分配律,这里只考虑最简单的情况(exp+exp-exp)/exp2
    // 认为 exp2 就是简单的数组访问/数字,不含括号
    int bracket = 0, l_for_bracket, r_for_bracket;
    // 找到括号
    for(int i = equal_pos + 1; i < size; ++i){
        if(str[i] == '('){
            if(++bracket == 1) 
                l_for_bracket = i;
        }
        else if(str[i] == ')'){
            if(--bracket == 0){
                r_for_bracket = i;
                break;
            }
        }
    }
    // 括号处理 + 拼接
    std::string l_str = str.substr(0, l_for_bracket) + out_all;
    std::string r_str = str.substr(r_for_bracket + 1, size - r_for_bracket - 1);
    int l_for_trunc = l_for_bracket + 1;
    // 其实不需要初始化, 保证为0
    bracket = 0;
    for(int i = l_for_bracket + 1;i < r_for_bracket; ++i){
        if(str[i] == '(' || str[i] == '[' || str[i] == '<') ++bracket;
        else if(str[i] == ')' || str[i] == ']' || str[i] == '>') -- bracket;
        else{
            if(bracket != 0) continue;
            if(str[i] == '+' || str[i] == '-'){
                char op = str[l_for_trunc - 1];
                // if(op != '-' && op != '+') op = '+';
                if(op != '-') op = '+';
                new_str += l_str + op + str.substr(l_for_trunc, i - l_for_trunc) + r_str;
                l_for_trunc = i + 1;
            }
        }
    }
    // 处理最后一个
    // 一定自带符号
    new_str += l_str 
        + str.substr(l_for_trunc - 1, r_for_bracket - l_for_trunc + 1) 
        + r_str;
    // update
    str = new_str;
}

// 生成偏导的语句
// 这里的引用只是为了快
std::string generate(const std::string &deal_str,
    const std::string grad_to, const std::string out){
    // 1. C = A*B      ->  dA = dC*B
    // 2. C = C + A*B  ->  dA = dA + dC*B
    //    因为进行了拆分,而且 dA 初始化为0,因此就算多了一步也没事
    // 3. C = A*A      -> dA = dC*A + A*dC

    std::string str_ret = "";
    int equal_pos = deal_str.find('=');
    // 记录 id 到整个结构(id<>[])的映射
    std::string out_d = "d" + deal_str.substr(0, equal_pos);
    int size_out_d = out_d.size();
    // 下标全为简单的个数
    // std::string grad_to_d_empty;
    
    // grad_to 的位置
    // 找到第一个 grad_to 的位置
    int l = deal_str.find(grad_to), r = 0;
    int size = deal_str.size();
    bool first = true;
    while(l != -1){
        r = l;
        // 找到右边界
        for(++r;r<size;++r){
            if(deal_str[r] == ']')
                break;
        }
        // replace
        std::string grad_to_d = "d" + deal_str.substr(l , r + 1 - l);
        // 默认 deep copy
        std::string str_temp = deal_str;
        /**
         * string& replace (
         *   size_t pos, size_t len, const string& str, size_t subpos, size_t sublen
         * );
         */

        // grad_to -> out_d
        // 一个
        str_temp.replace(l, r + 1 - l , out_d, 0, size_out_d);
        // out -> grad_to_d
        // 多个
        int l_for_out_d = 0;
        int size_grad_to_d = grad_to_d.size();
        while(l_for_out_d != -1){
            // -1 因为没有 d
            str_temp.replace(l_for_out_d, size_out_d - 1,  grad_to_d, 0, size_grad_to_d);
            // 可能是第一次的 replace 导致的
            while(true){
                // +2 因为还有 d(但是实际上是不同的字母,+1问题也不大)
                l_for_out_d = str_temp.find(out, l_for_out_d + 2);
                if(str_temp[l_for_out_d - 1] != 'd') break;
            }
        }
        if(first) first = false;
        else{
            // 说明是 C=A*A 的形式(一个式子生成多个表达式)
            // dA = dC*A
            // dA = A*dC+dA
            str_temp.insert(str_temp.size()-1,"+" + grad_to_d);
        }
        complex2easy(str_temp);
        str_ret += str_temp;
        // 更新 l 
        // +2 因为还有 d(但是实际上是不同的字母,+1问题也不大)
        l = deal_str.find(grad_to, l + 2);
    }
    return str_ret;
}

// 处理整除符号 '$'->'/'
void map_divisible(std::string &str){
    int size = str.size();
    for(int i = 0; i < size; ++i){
        if(str[i] == '$')
            str[i] = '/';
    }
}

// 是否为不带 op 的语句
bool is_easy(std::string &str){
    int size = str.size();
    for(int i = 0;i < size; ++i){ 
        if(is_op(str[i]))
            return false;
    }
    return true;
}

// 判断是否为边界
bool is_border(char c){
    if(c == ',' || c == '[' || c == ']')
        return true;
    return false;
}

// 将左边的复合下标拆解
void complex2easy(std::string &str) {
    // 认为复合语句只是包含 + 或者 /,%
    // 认为左边出现的每一个式子在右边都会出现
    char index_for_use = 'a';
    int l_bracket = str.find('[') + 1;
    int r_bracket = str.find(']');
    // 记录更新映射
    std::map<std::string, std::string> old2new;
    old2new.clear();
    int mod_pos = str.find('%'), div_pos = str.find('$');
    if (mod_pos != -1 || div_pos != -1) {
        // TODO 认为 a//b,c%d => a=c,b=d
        std::string str_mod_l, str_mod_r, str_div_l, str_div_r;
        // 垃圾代码
        if (mod_pos != -1) {
            for (int i = mod_pos;; --i) {
                if (is_border(str[i])) {
                    str_mod_l = str.substr(i + 1, mod_pos - i - 1);
                    break;
                }
            }
            for (int i = mod_pos;; ++i) {
                if (is_border(str[i])) {
                    str_mod_r = str.substr(mod_pos + 1, i - mod_pos - 1);
                    break;
                }
            }
        }
        if (div_pos != -1) {
            for (int i = div_pos;; --i) {
                if (is_border(str[i])) {
                    str_div_l = str.substr(i + 1, div_pos - i - 1);
                    break;
                }
            }
            for (int i = div_pos;; ++i) {
                if (is_border(str[i])) {
                    str_div_r = str.substr(div_pos + 1, i - div_pos - 1);
                    break;
                }
            }
        }
        if (mod_pos != -1 && div_pos != -1) {
            if (str_div_r != str_mod_r || str_div_l != str_mod_l) {
                std::cerr << "The code can't hold the exp:" + str << std::endl;
                exit(4);
            }
        }
        std::string l_map = "";
        // 若有乘法就更新
        if (div_pos != -1) {
            std::string str_div = str_div_l + "$" + str_div_r;
            std::string new_index = std::string("__") + (index_for_use++);
            int start_find = 0;
            // replace all
            while ((start_find = str.find(str_div, start_find)) != -1) {
                str.replace(start_find, str_div.size(), new_index, 0, new_index.size());
                ++start_find;
            }
            l_map += new_index  + "*" + str_div_r;
        }
        // 若有取模,再次更新
        if (mod_pos != -1) {
            std::string str_mod = str_mod_l + "%" + str_mod_r;
            std::string new_index = std::string("__") + (index_for_use++);
            int start_find = 0;
            // replace all
            while ((start_find = str.find(str_mod, start_find)) != -1) {
                str.replace(start_find, str_mod.size(), new_index, 0, new_index.size());
                ++start_find;
            }
            if (div_pos == -1) l_map = new_index + "+" + str_mod_r;
            else l_map += "+" + new_index;
            if (div_pos == -1) l_map = str_mod_r + l_map;
        }
        // 替换
        int start_find = 0;
        std::string old_index = (mod_pos == -1) ? str_div_l : str_mod_l;
        // replace all
        while ((start_find = str.find(old_index, start_find)) != -1) {
            str.replace(start_find, old_index.size(), l_map, 0, l_map.size());
            ++start_find;
        }
    }
    else {
        int l = l_bracket;
        for (int i = l_bracket; i <= r_bracket; ++i) {
            if (str[i] != ',' && i != r_bracket) continue;
            std::string str_replace = str.substr(l, i - l);
            l = i + 1;
            if (is_easy(str_replace)) continue;
            // complex
            // 统计这个复合结构中所有的因子
            std::vector<std::string> seg;
            seg.clear();
            int l_seg = 0;
            for (int j = 0; j < str_replace.size(); ++j) {
                if (str_replace[j] == '+') {
                    seg.push_back(str_replace.substr(l_seg, j - l_seg));
                    l_seg = j + 1;
                }
            }
            seg.push_back(str_replace.substr(l_seg, str_replace.size()));
            // 找到所有的映射
            int seg_size = seg.size();
            std::string new_index = std::string("__") + (index_for_use++);
            for (int j = 0; j < seg_size; ++j) {
                std::string new_map_index = new_index;
                for (int k = 0; k < seg_size; ++k) {
                    if (j != k) new_map_index += '-' + seg[k];
                }
                old2new[seg[j]] = new_map_index;
            }
            // 首先更新
            int start_find = 0;
            while ((start_find = str.find(str_replace, start_find)) != -1) {
                str.replace(start_find, str_replace.size(), new_index, 0, new_index.size());
                ++start_find;
            }
            // 更新剩下的(注意只更新简单的,不然会重复更新)
            bool has_chosen = false;
            for (int j = seg_size - 1; j >= 0 ; --j) {
                if(is_constant(seg[j])) continue;
                if(has_chosen) break;
                else has_chosen = true;
                start_find = 0;
                while ((start_find = str.find(seg[j], start_find)) != -1) {
                    // 简单的要求两边为 ',', '[', ']'
                    if (is_border(str[start_find - 1]) && is_border(str[start_find + seg[j].size()])) {
                        std::string new_index_other = old2new[seg[j]];
                        str.replace(start_find, seg[j].size(),
                            new_index_other, 0, new_index_other.size());
                    }
                    ++start_find;
                }
            }
            // TODO 合并同项 (a+b-b -> a)
        }
    }
}

// project 2
void change_kernel(MyData &data){
    std::string new_kernel = "";
    // 处理多条语句,但是实际上样例中只有一个语句
    std::string old_kernel = data.kernel;
    int l = 0, r = 0;
    while(r != old_kernel.size()){
        l = r;
        r = old_kernel.find(';', l) + 1;
        std::string deal_str = old_kernel.substr(l, r - l);
        // 删除常数
        clear_constant(deal_str);
        // TODO 处理加法(会影响到 distribute 语句, distribute 默认一条一个语句)
        // ex : A = B*C + B*D => A = B*C; A = A + B*D
        // 注意现在的 kernel 还有整除符号(/) 不用管他
        // 判断是否有括号,如果有需要考虑分配律
        bool has_bracket = (deal_str.find("(") != -1);
        if(has_bracket){
            distribute(deal_str);
        }
        // 注意此时的 kernel 里面可能含有多个语句
        // 认为 out 只有一个
        // 生成偏导语句
        std::string temp_str = "";
        int l_for_gen = 0, r_for_gen = 0;
        while(r_for_gen != deal_str.size()){
            l_for_gen = r_for_gen;
            r_for_gen = deal_str.find(';', l_for_gen) + 1;
            std::string temp_str_gen = deal_str.substr(l_for_gen, r_for_gen - l_for_gen);
            for(int i = 0; i < data.grad_to.size(); ++i){
                // 考虑 C = B 的情况
                // TODO 特殊处理 C = 0 的语句(现在是不处理,因为有初始化)
                int equal_pos_plus_1 = temp_str_gen.find('=') + 1;
                // -1 表示去掉标点 ';'
                std::string constant
                        = temp_str_gen.substr(
                            equal_pos_plus_1, temp_str_gen.size()- equal_pos_plus_1 - 1
                        );
                if(is_constant(constant)){
                    // 注意有 ';'
                    if(constant.size() == 1 && constant[0] == '0'){
                        continue;
                    }
                    else{
                        std::cerr << "The solution has bug! Constant should be 0!" << std::endl;
                        exit(3);
                    }
                }
                // 处理无关语句 C = B
                if(constant.find(data.grad_to[i]) == -1)
                    continue;
                std::string str_debug = temp_str_gen;                
                // deal
                temp_str += generate(temp_str_gen, data.grad_to[i], data.outs);
            }
        }
        map_divisible(new_kernel);
        new_kernel += temp_str;
    }
    // update kernel
    data.kernel = new_kernel;
}
// project2 end

// DEBUG
void print_kernel(std::vector<PARSE::MyData> &datas){
    for(int i = 0;i < datas.size(); ++i){
        std::cout << "\t|" <<  datas[i].name << "|" << std::endl;
        std::string ker = datas[i].kernel;
        int l = 0, r = 0;
        while(l != ker.size()){
            r = ker.find(';', l) + 1;
            std::cout << ker.substr(l ,r - l) << std::endl;
            l = r;
        }
    }
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
                    Expr cmp1 = Compare::make(index_type, CompareOpType::LT, index_name, upbound);
                    Expr cmp2 = Compare::make(index_type, CompareOpType::GE, index_name, 0);
                    judges.push_back(cmp1);
                    judges.push_back(cmp2);
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


////////////
////////////
//  MAIN  //
////////////
////////////

// TODO mark for main
int main(){
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
    
    // 文件个数
    int size = output_filename.size();

    // TODO project2
    // 修改 kernel
    // DEBUG
    // PARSE::print_kernel(datas);
    for(int i = 0; i < size; ++i){
        PARSE::change_kernel(datas[i]);
    }
    // DEBUG
    PARSE::print_kernel(datas);
    // exit(4);
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