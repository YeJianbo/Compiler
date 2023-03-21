//
// Created by YeJianbo on 2023/3/16.
//

#include "LR.h"


template<class T>
void reverseStack(stack<T> &s) {
    if (s.empty()) {
        return;
    }
    auto temp = s.top();
    s.pop();
    reverseStack(s);
    stack<T> tempStack;
    while (!s.empty()) {
        tempStack.push(s.top());
        s.pop();
    }
    s.push(temp);
    while (!tempStack.empty()) {
        s.push(tempStack.top());
        tempStack.pop();
    }
}

void addAllElement(set<char> &destSet, const set<char> &sourceSet) {
    for (const auto& element : sourceSet)
    {
        destSet.insert(element);
    }
}

bool Production::operator==(const Production &o) const {
    return l == o.l && r == o.r;
}

bool Production::operator<(const Production &o) const {
    if (l != o.l)
        return l<o.l;
    else
        return r < o.r;
}

bool Item::operator==(const Item &o) const {
    return rule == o.rule && dot == o.dot && lookahead == o.lookahead;
}

bool Item::operator<(const Item &o) const {
    if (rule == o.rule){
        if (dot == o.dot){
            //如果点的位置相同，需要根据lookahead不同来区分
            //注意：此处代码可能有问题！！！能跑，以后修
            return lookahead < o.lookahead;
        }
        return dot < o.dot;
    }
    return rule < o.rule;
}

bool ItemSet::operator==(const ItemSet &o) const {
    return items == o.items;
}

bool ItemSet::operator<(const ItemSet &o) const {
    return stoi(name.substr(1, name.size())) < stoi(o.name.substr(1, o.name.size()));
}

void LR::printToken() {
    cout << left << setw(20) << "line";
    cout << left << setw(20) << "Type";
    cout << left << setw(20) << "value" << endl;
    for (vector<Token>::iterator it = tokens.begin(); it != tokens.end(); ++it) {
        cout << left << setw(20) << it->line << right;
        switch (it->type) {
            case 0:
                cout << left << setw(20) << "KEYWORD";
                break;
            case 1:
                cout << left << setw(20) << "IDENTIFIER";
                break;
            case 2:
                cout << left << setw(20) << "CONSTANT";
                break;
            case 3:
                cout << left << setw(20) << "DELIMITER";
                break;
            case 4:
                cout << left << setw(20) << "OPERATOR";
                break;
            default:
                cout << left << setw(20) << "unknown";
        }
        if (it->value == " "){
            it->value = "\\0";
        }
        if (it->value == "  "){
            it->value = "\\t";
        }
        if (it->value == "\n"){
            it->value = "\\n";
        }
        cout << left << setw(20) << it->value << endl;
    }
}

//计算字符串的first集
set<char> calc_first_s(string &s, map<char, set<char>> &first_set, vector<Production> &prods) {
    set<char> first;
    set<char> visited;
    //如果s的第一个符号是终结符(小写字母)，直接将其加入FIRST集合中
    if (!isupper(s[0]) || s[0] == '$') {
        if (s[0] != '$') {
            first.insert(s[0]);
        }
        else {
            first.insert('$');
        }
    }
    //否则，计算第一个符号的FIRST集合
    else {
        auto tmp_set = calc_first(s[0], first_set, visited, prods);
        first.insert(tmp_set.begin(), tmp_set.end());
        if (tmp_set.find('$') != tmp_set.end()) {
            int i = 1;
            while (i < s.size()) {
                tmp_set = calc_first(s[i], first_set, visited, prods);
                first.insert(tmp_set.begin(), tmp_set.end());
                if (tmp_set.find('$') == tmp_set.end()) {
                    break;
                }
                i++;
            }
        }
    }
    return first;
}

//计算单个字符的FIRST集
set<char> calc_first(const char s, map<char, set<char>> &first_set, set<char> &visited, vector<Production> &prods) {
    set<char> first;
    //如果s已经被访问过，说明该符号的FIRST集合已经计算过，直接返回
    if (visited.find(s) != visited.end()) {
        return first_set[s];
    }
    //如果s是终结符，直接将其加入FIRST集合中
    if (!isupper(s) || s == '$') {
        first.insert(s);
    }
        //否则，遍历产生式集合，找到所有以s为左部的产生式，并计算其右部符号的FIRST集合
    else {
        for (auto& p : prods) {
            if (p.l == s) {
                //如果右部第一个符号为s，则需要将其FIRST集合中除了空串的符号加入当前符号的FIRST集合中
                if (!isupper(p.r[0]) || p.r[0] == '$') {
                    if (p.r[0] != '$') {
                        first.insert(p.r[0]);
                    }
                    else {
                        first.insert('$');
                    }
                }
                else {
                    auto tmp_set = calc_first(p.r[0], first_set, visited, prods);
                    first.insert(tmp_set.begin(), tmp_set.end());
                    if (tmp_set.find('$') != tmp_set.end()) {
                        int i = 1;
                        while (i < p.r.size()) {
                            tmp_set = calc_first(p.r[i], first_set, visited, prods);
                            first.insert(tmp_set.begin(), tmp_set.end());
                            if (tmp_set.find('$') == tmp_set.end()) {
                                break;
                            }
                            i++;
                        }
                    }
                }
            }
        }
    }
    visited.insert(s);
    first_set[s] = first;
    return first;
}

//读取二型文法，规定该二型文法的VN与VT均为单个字符，保存在productions中
vector<Production> LR::readGrammar(string path) {
    ifstream file(path);
    vector<Production> productions;
    string line;
    //打开文件
    if (!file){
        cout<<"找不到关键字文件！"<<endl;
        return productions;
    }
    // 思路：获取一行，切割两边，如果右边有"|"，再切割右边，
    // 逐个放入productions中
    getline(file,line);
    while(!line.empty()){
        //获取到一行
        line = trim(line);
        //跳过注释
        if (line[0] == '/' && line[1] == '/'){
            line.clear();
            getline(file,line);
            if (line.empty()) break;
            continue;
        }
        size_t p = line.find("->");
        if (p == string::npos){
            cout << "输入的语法有误！"<<endl;
            return vector<Production>();
        }
        string h = trim(line.substr(0,p));
        line = trim(line.substr(p + 2));
        //得到单条产生式
        if (line.find('|') == string::npos){
            Production production;
//            vector<Symbol> sy;
            //得到了左边h，右边line
                production.r += line;
            if (!isupper(h[0])){
                cout << "您输入的语法有误！"<<endl;
                return vector<Production>();
            }
            for (auto& sss:production.r) {
                if (isupper(sss)){
                    terminals.insert(sss);
                }else
                    nonTerminals.insert(sss);
            }
            production.l = h[0];
            terminals.insert(h[0]);
            productions.push_back(production);
        } else{
            stringstream ss(line);
            string s;
            while(getline(ss,s,'|')){
                //得到h（左边）与trim(s)（右边）
                Production production;
                production.r = trim(s);
                if (!isupper(h[0])){
                    cout << "您输入的语法有误！"<<endl;
                    return vector<Production>();
                }
                for (auto& sss:production.r) {
                    if (isupper(sss)){
                        terminals.insert(sss);
                    }else
                        nonTerminals.insert(sss);
                }
                production.l = h[0];
                terminals.insert(h[0]);
                productions.push_back(production);
            }
        }
        line.clear();
        getline(file,line);
    }
    //对得到的文法进行增广
    augmentGrammar(productions);
    Productions = productions;
    return productions;
}

//对文法进行增广处理
void LR::augmentGrammar(vector<Production> &grammar) {
//    vector<Production> productions;
    char start = 'Z';
    Production production = {start, {grammar[0].l}};
    grammar.insert(grammar.begin(), production);
    grammar[0].l = start;
}

// 用于构造LR(1)分析的项集族
// 构造方法：首先获取初始状态的项集，对该项集依次输入所有点后的字符，转移到新状态，
// 对该新状态求项集族闭包，闭包结果为该状态所有项集，依次处理，直到没有项集新增，得到项集族
set<ItemSet> LR::construct_LR1_itemsets() {
    // 初始项集为Z -> .S, $
    ItemSet initial_itemset;
    initial_itemset.items.insert(Item{ Production{ 'Z', "S" }, 0, { '$' } });
    // 项集族
    set<ItemSet> itemsets;
    initial_itemset.name = "I" + to_string(itemsets.size());
    itemsets.insert(closure(initial_itemset));
    // 待处理项集队列
    queue<ItemSet> itemset_queue;
    // 处理初始状态
    itemset_queue.push(initial_itemset);
    // 遍历所有待处理的项集I，对于每个项中的每个文法符号，GOTO(I, X)非空且不在闭包中，则将GOTO加入闭包
    while (!itemset_queue.empty()) {
        //取项集，遍历每个项，取项点后的输入符号，产生新状态，新状态求项集族闭包，得到项集，加入项集族中
        //直到没有新项集加入项集族，程序终止
        ItemSet itemset = itemset_queue.front();
        itemset_queue.pop();
        // 对每个文法符号进行扩展
        map<char, ItemSet> next_itemsets;
        set<char> charSet;
        map<char,ItemSet> itemMap;
        //遍历项集中的每一个项，取点后的第一个字符
        for (const auto& item : itemset.items){
            auto item1 = item;
            //如果点不在最后，为移进/待约项目，点后移
            if (item.dot < item.rule.r.length()){
                charSet.insert(item.rule.r[item.dot]);
                item1.dot++;
                itemMap[item.rule.r[item.dot]].items.insert(item1);
            }else{
                //点在最后，归约，新增Action表，第二个ItemSet中有一个产生式（表示使用这条产生式归约）
                for (const auto &l: item.lookahead){
                    ItemSet iss;
                    Item ii;
                    Production p = item.rule;
                    ii.rule = p;
                    iss.items.insert(ii);
                    Action[itemset][l] = {iss, true};
                    //对于Z -> S.，状态为acc
                    if (p.l == 'Z' && p.r == "S"){
                        iss.name = "ACC";
                        Action[itemset][l] = {iss, false};
                    }

                }
            }
        }
        //遍历所有可输入项，对于每个可输入项，在项集中寻找项的表达式，组成一个项集，将其求闭包，闭包结果新建状态
        for (auto& c:charSet) {
//            cout<<c<<"|";
            ItemSet iss = itemMap[c];
            //求完闭包后，得到新状态，新状态如果不存在，那么新增，得到状态后（无论是否新增），添加关系转移表
            closure(iss);
            //标记flag，值不变说明没有找到已有项集，此时新建一个项集
            int flag = 0;
            for (auto& i:itemsets) {
                //i == iss，说明找到了完全一致的itemset，不需要新增状态
                if (i == iss){
                    flag = 1;
                    iss.name = i.name;
                    break;
                }
            }
            //没找到一致的状态,新增
            if (!flag){
                iss.name = "I" + to_string(itemsets.size());
                itemset_queue.push(iss);
                itemsets.insert(iss);
            }
            //将状态转移结果加入ACTION/GOTO表
            //对于大写字母（非终结符），将状态转移加入GOTO表
            if (isupper(c)){
                Goto[itemset][c] = iss;
            }else{
                //对于非终结符，状态加入Action，为移进状态
                Action[itemset][c] = {iss, false};
            }
        }
    }
    is = itemsets;
    return itemsets;
}


//计算项集族闭包
//遍历每个产生式，如果点后字符串是非终结符，将该非终结符的产生式加入，点在第一位，分析新加入的产生式
ItemSet LR::closure(ItemSet& items) {
//    ItemSet item_set;
    queue<Item> item_queue;
    //遍历输入的ItemSet，把每个Item放入队列中
    for (auto& item : items.items) {
        item_queue.push(item);
    }
    //根据队列顺序，依次处理
    while (!item_queue.empty()) {
        //取一个项，比如"Z -> .S", $, 有 rule.l = '\'', rule.r = S, dot = 0, lookahead = {$}
        auto item = item_queue.front();
        item_queue.pop();
        //点号在最右端，不需要处理，跳过
        if (item.dot == item.rule.r.size()) {
            continue;
        }
        //取点后第一个字符
        char next_symbol = item.rule.r[item.dot];
        //点后是终结符（比如小写字母），不需要处理，跳过
        if (!isupper(next_symbol)) {
            continue;
        }
        //点号不在最右端，点后不是终结符，所以点后是非终结符（大写字母），此时需要处理
        //将非终结符的推导加入点在第一位，展望符是从这一位开始到结尾的FIRST集
        //遍历产生式集合，找到需要加入的项
        for (auto& prod : Productions) {
            //找到对应的式子
            if (prod.l == next_symbol) {
                //求从这一位开始到结尾的FIRST集合，作为展望符
                string s = item.rule.r.substr(item.dot + 1);
                set<char> lookahead;
                for (auto & l:item.lookahead) {
                    string ss = s + l;
                    set<char> c = calc_first_s(ss, first_set, Productions);
                    addAllElement(lookahead,c);
                }
                //加入项中
                Item new_item{prod, 0, lookahead};
                //找不到，加入集合
                if (items.items.find(new_item) == items.items.end()) {
                    items.items.insert(new_item);
                    item_queue.push(new_item);
                }
            }
        }
    }
    return items;
}

//输出项集族与状态转移
void LR::printItemSet() {
    cout<<"ItemSet:"<<endl;
    for (const auto& i:is) {
        cout<<i.name<<": "<<endl;
        for (const auto& item : i.items){
            auto ii = item;
            ii.rule.r.insert(item.dot,".");
            cout<<item.rule.l<<" -> "<<ii.rule.r<<", lookahead: {";
            for(const auto& l:item.lookahead){
                cout<<l<<" ";
            }
            cout<<"}"<<endl;
        }
        nonTerminals.insert('$');
        //遍历ACTION及GOTO表，输出该项集出的状态转移
        for (const auto &c: nonTerminals){
            auto a = Action[i][c];
            if (!a.first.name.empty()){
                cout<<"--"<< c << "-->" << a.first.name<<"\t";
            } else {
                Production pp = a.first.items.begin()->rule;
                if (pp.l != '\0')
                    cout<<"--"<< c << "--> {" << pp.l << " -> "<<pp.r<<"}\t";
            }
        }
        for (const auto &c: terminals){
            auto g = Goto[i][c];
            if (!g.name.empty()){
                cout<<"--"<< c << "-->" << g.name<<"\t";
            }
        }
        cout<<endl;
    }
    cout<<endl;
}

void LR::printProduction() {
    cout<<"Productions:"<<endl;
    for (auto const& p:Productions) {
        cout<<p.l<<" -> "<<p.r<<endl;
    }
}

//语法分析
//输入Token的路径path，读取Token表，对该Token进行语法分析，
//如果通过编译，输出"YES"，否则输出"NO"
void LR::parse(string path) {
    //记录处理次数，用于输出处理过程
    int n = 0;
    //读取Token至Token集合, 并将Token表转换为可接受的符号串
    string ts = readToken(path);
    //符号栈
    stack<char> charStack;
    //状态栈
    stack<ItemSet> stateStack;
    //设置初态
    charStack.push('$');
    ItemSet iss = *is.begin();
    stateStack.push(iss);
    //先设置DFA的初态，然后依次读取字符，按照ACTION及GOTO表进行状态转移，到达ACC说明被接受；
    //如果不被接受给出相关提示
    //从第一个字符开始遍历tokenString，对输入的字符依次按照ACTION及GOTO表处理
    //如果是移进/待约状态，正常入栈出栈即可
    //如果是归约状态，首先不要立即入栈，首先归约项目，确保没有归约项后再移进字符
    for (int i = 0; i < ts.size(); ++i) {
        //取栈顶元素
        auto& top = stateStack.top();
        ItemSet next;
        //大写字母（非终结符），查GOTO表，否则查ACTION表
        if (isupper(ts[i])){
            next = Goto[top][ts[i]];

        } else {
            //
            auto p = Action[top][ts[i]];

            //bool为真，此时需要归约
            //归约过程，取归约串，将串中内容连同其对应状态依次出栈，然后将归约式子左边入栈，
            //取状态栈栈顶，查询新入栈元素的GOTO表，将找到的状态加入状态栈，接着查看下一个状态
            while (p.second == true){
                //取归约式
                Production pp = p.first.items.begin()->rule;
                //逆序遍历归约式右边，将内容依次出栈
                for (int j = pp.r.size() - 1; j >= 0; --j) {
                    stateStack.pop();
                    charStack.pop();
                }
                //左边入栈
                charStack.push(pp.l);
                //查GOTO表
                auto ppp = Goto[stateStack.top()][pp.l];
                //状态入栈
                stateStack.push(ppp);
                p = Action[stateStack.top()][ts[i]];
                cout<<"第 "<<++n<<" 次处理"<<endl;
                cout<<"当前读取字符："<<ts[i]<<endl;
                cout<<"charStack: ";
                auto cc = charStack;
                reverseStack(cc);
                for (auto temp = cc; !temp.empty(); temp.pop()) {
                    cout << temp.top() << " ";
                }
                cout<<endl<<"stateStack:";
                auto ss = stateStack;
                reverseStack(ss);
                for (auto temp = ss; !temp.empty(); temp.pop()) {
                    cout << temp.top().name << " ";
                }
                cout<<endl<<"动作：通过产生式【"<<pp.l<<" -> "<<pp.r<<"】归约"<<endl<<endl;
            }
            //归约处理完，移进
            next = Action[stateStack.top()][ts[i]].first;
        }
        //如果名称非空，说明不存在下一状态，此时输出提醒，终止分析
        if (next.name.empty()){
            //取ts[i]所在Token的行数，一并输出
            auto token = tokenLine[i];
            cout << "错误：输入字符【"<<ts[i]<<"】，找不到下一状态！行数："<<token.line<<endl;

            break;
        }
        //入栈
        charStack.push(ts[i]);
        stateStack.push(next);
        cout<<"第 "<<++n<<" 次处理"<<endl;
        cout<<"当前读取字符："<<ts[i]<<endl;
        cout<<"charStack: ";
        auto cc = charStack;
        reverseStack(cc);
        for (auto temp = cc; !temp.empty(); temp.pop()) {
            cout << temp.top() << " ";
        }
        cout<<endl<<"stateStack:";
        auto ss = stateStack;
        reverseStack(ss);
        for (auto temp = ss; !temp.empty(); temp.pop()) {
            cout << temp.top().name << " ";
        }
        cout<<endl<<"动作：移进"<<endl<<endl;
    }
    cout<<"接收状态：";
    if (stateStack.top().name == "ACC"){
        cout<<"YES"<<endl;
    }else
        cout<<"NO"<<endl;
}

string LR::readToken(string path) {
    readDic(MATCH_PATH);
    //打开文件，将Token读取进Token集中，然后对Token进行分析
    string line;
    ifstream file(path);
    if (!file){
        cout << "找不到源代码文件！" << endl;
        return "";
    }
    string s = "";
    //从file中获取一行，保存在line中，逐行处理，存入Token表
    while(getline(file,line)){
        istringstream iss(line);
        string l, type, value;

        //根据空格切分，依次得到行号、类型、值
        iss >> l >> type >> value;
        Token token;
        token.line = stoi(l);
        token.value = value;
        if (!type.compare("KEYWORD")){
            token.type = KEYWORD;
            if (!dic[token.value]){
                dic[token.value] = 'k';
            }
        }else if (!type.compare("IDENTIFIER")){
            token.type = IDENTIFIER;
            dic[token.value] = 't';
        } else if (!type.compare("CONSTANT")){
            token.type = CONSTANT;
            dic[token.value] = 'd';
        } else if (!type.compare("DELIMITER")){
            if (token.value == "\\0" || token.value == "\\t" || token.value == "\\n"){
                //跳过空格
                continue;
            }
            token.type = DELIMITER;
            dic[token.value] = token.value[0];
        } else if (!type.compare("OPERATOR")){
            token.type = OPERATOR;
            if (!dic[token.value]){
                dic[token.value] = token.value[0];
            }
        }
        tokenLine[s.size()] = token;
        s += dic[token.value];
        tokens.push_back(token);
        if (line.empty()){
            break;
        }
    }
    file.close();
    s += '$';
    return s;
}
/*

//将Token表转换为自动机能识别的符号串
string LR::tokenToString() {
    //后续优化：读取字典文件 不手动赋值，功能考虑合并到readToken()中
    string s = "";
    for (const auto &t: tokens){
        if (t.type == CONSTANT){
            s += "d";
        } else if (t.type == DELIMITER){
            if (t.value == "\\0" || t.value == "\\t" || t.value == "\\n"){
                //跳过空格
                continue;
            }
            s += t.value;
        } else if (t.type == OPERATOR){
            if (t.value == "+" || t.value == "-")
                s+="p";
            else if (t.value == "*" || t.value == "/")
                s+="m";
            else
                s+="r";
        } else if (t.type == KEYWORD){
            //单独设置keyword对照表处理
        } else if (t.type == IDENTIFIER){
            s += "t";
        } else{
            //报错
        }
    }
    s += '$';
    return s;
}
*/

//读取关键字对应的字典
void LR::readDic(string path) {
    //打开文件，将关键字的对应读入，保存在类中字典中
    string line;
    ifstream file(path);
    if (!file){
        cout << "找不到源代码文件！" << endl;
        return ;
    }
    getline(file,line);
    //从file中获取一行，保存在line中，逐行处理，存入dic
    while(!line.empty()){
        //跳过注释
        line = trim(line);
        if (line[0] == '/' && line[1] == '/'){
            line.clear();
            getline(file,line);
            if (line.empty()) break;
            continue;
        }
        istringstream iss(line);
        string key;
        char value;
        //根据空格切分，得到key与value
        iss >> key >> value;
        dic[key] = value;
        getline(file,line);
    }
    file.close();
}





