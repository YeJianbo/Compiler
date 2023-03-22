#include "FA.h"


//����һ��ڵ㣬����һ������input������charset��,�󾭹���input�������һ״̬
//����״̬��հ������ظ�״̬
set<Node> FA::move(char input,set<Node> node,FA nfa) {
//    cout<<input<<endl;
    set<Node> n;
//    cout<<"[ ";
    //����״̬�е�����Ԫ�أ�����������ַ����������ӽ���״̬
    for (auto it = node.begin(); it != node.end(); ++it) {
        addAllElements(n,transNFA[*it][input]);
    }
    for (auto it__ = n.begin(); it__ != n.end(); ++it__) {
        addAllElements(n,nfa.closure(*it__));
    }
    for (auto it2 = n.begin(); it2 != n.end(); ++it2) {
//        cout<<it2->id<<" "<<it2->name<<" ";
    }
//    cout<<"]"<<endl;
    return n;
}

//������������ΪNFA
void FA::GrammarToNFA(string path){
    string line;
    ifstream file(path);
    if(!file){
        cout<<"�Ҳ����ķ��ļ���"<<endl;
        return ;
    }
    //��ȡ��һ�д���line��
    do {
        getline(file,line);
        line = trim(line);
    }while(line[0] == '/' && line[1] == '/');
    int i = 0;
    while (i < line.length() && line[i] != ' ' && line[i] != '-')
        i++;
    //��̬
    string start = line.substr(0,i);
    //�����̬
    insertIntoStartState(start);
    do{
        //ȥ��ǰ��Ŀո�
        line = trim(line);
        //����ע��(��һ�в�����)
        if(line[0] == '/' && line[1] == '/'){
            line.clear();
            getline(file,line);
            if(line.empty()) break;
            continue;
        }

        //û��'|'��˵���ǵ�������ʽ��ֱ�Ӵ���
        if (line.find('|') == string::npos){
            size_t p2 = line.find("->");
            string h = trim(line.substr(0,p2));
            line = trim(line.substr(p2+2));
//            cout<<line;
            deal(h +" -> "+ line);
        }else{
            size_t p = line.find("->");
            //û�ҵ�������ʽ����
            if (p == string::npos){
                cout<<"������﷨����"<<endl;
                return;
            }
            string h = trim(line.substr(0,p));
            line = trim(line.substr(p+2));
//            cout<<line;
            stringstream ss(line);
            string production;
            while (getline(ss, production, '|')) {
                deal(h+" -> "+trim(production));
            }
        }
        //������ɣ�����ַ���
        line.clear();
        getline(file,line);
    }while(!line.empty());
    file.close();
}

//����������ʽ����ȥ���ո�
//��S -> aA
void FA::deal(string line){
    istringstream iss(line);
//    cout<<iss.str()<<endl;
    string ls, arrow, rs; //ls:���    arrow����ͷ    rs���ұ�
    char input;
    //���ݿո��з�
    iss >> ls >> arrow >> rs;
//    cout<<ls<<" "<<arrow<<" "<<rs<<endl;
    Node node = insertIntoState(ls);
    //����Ǵ�д��ĸ��ͷ������Ϊ?���գ�
    if (isupper(rs[0])){
        input = '$';
    }else if (rs[0] == '\\'){
        //'\'��ͷ��ʾת���ַ�����������
        switch (rs[1]) {
            case '0':
                input = ' ';
                break;
            case 't':
                input = '\t';
                break;
            case 'n':
                input = '\n';
                break;
            default:
                input = '$';
        }
        rs = rs.substr(2);
        //����̬����¼���
        if (rs.empty()){
            string ss = "";
            //��ʶ���͹ؼ���
            if (startsWith("I",ls)){
                ss = "_I";
            }else if (startsWith("O",ls)){
                //�����
                ss = "_O";
            }else if (startsWith("Separator",ls)){
                //Separator
                ss = "_S";
            }else if (startsWith("A",ls) || startsWith("B",ls) || startsWith("C",ls) || startsWith("D",ls)){
                //����
                ss = "_C";
            }
            rs = "EndState_"+to_string(endState.size()+1) + ss;
            insertIntoEndState(rs);
        }
    }else{
        //����������
        input = rs[0];
        //ȥ����һλ��ʣ�µ�����һ״̬
        rs = rs.substr(1);
        if (rs.empty()){
            string ss = "";
            //��ʶ���͹ؼ���
            if (startsWith("I",ls)){
                ss = "_I";
            }else if (startsWith("O",ls)){
                //�����
                ss = "_O";
            }else if (startsWith("Separator",ls)){
                //Separator
                ss = "_S";
            }else if (startsWith("A",ls) || startsWith("B",ls) || startsWith("C",ls) || startsWith("D",ls)){
                //����
                ss = "_C";
            }
            rs = "EndState_"+to_string(endState.size()+1)+ss;
            insertIntoEndState(rs);
        }
    }
//    cout<<input<<endl;
    charSet.insert(input);
//    cout<<insertIntoState(rs).id<<" "<<rs<<endl;
    transNFA[node][input].insert(insertIntoState(rs));
}

//��NFAת��ΪDFA
void FA::TransToDFA(FA nfa){
    set<Node> n = nfa.closure(nfa.startState);
//    for (set<Node>::iterator it2 = n.begin(); it2 != n.end(); ++it2) {
//        cout<<it2->id<<" "<<it2->name<<endl;
//    }
    Node start = insertIntoStartState("Start");
    stateCorr["Start"] = n;
    charSet = nfa.getCharSet();
    charSet.erase('$');
    deal2(nfa,start,n);
}

//�������ݣ�������õ�DFA
void FA::deal2(FA nfa, Node start,set<Node> n) {
    //����charSet
    for (set<char>::iterator it2 = charSet.begin(); it2 != charSet.end(); ++it2) {
        int flag = 0, flag2 = 0,flag3 = 1;
        // ȡ����һ״̬�ļ��ϣ������հ���
        set<Node> n2 = nfa.move(*it2,n,nfa);
        string buf;
        string ss = "";
        //flag3 = 0,˵��û����һ״̬
        if(n2.empty())
            flag3 = 0;
        //��������Ľڵ�������̬�����
        if (nodeStartsWith(n2,"EndState")){
            flag2 = 1;
            if(hasNode(n2, "EndState", "I")){
                ss = "_I";
            }else if (hasNode(n2,"EndState","O")){
                ss = "_O";
            }else if (hasNode(n2,"EndState","S")){
                ss = "_S";
            }else if(hasNode(n2,"EndState","C")){
                ss = "_C";
            }
        }
        //����״̬�Ƿ����
        for (auto it4 = stateCorr.begin(); it4 != stateCorr.end(); ++it4) {
//            //��������Ľڵ�������̬�����
//            if (nodeStartsWith(it4->second,"EndState")){
//                flag2 = 1;
//            }
            //�����״̬�Ѿ����ڣ�����
            if (setsAreEqual(it4->second,n2)) {
                buf = it4->first;
                flag = 1;
                break;
            }
        }
        if(flag3) {
            //flag == 0,˵��״̬�����ڣ���ʱ�����½ڵ㣬�����½ڵ���д���
            Node newN;
            string name = "State" + to_string(States.size() + 1);
            if (!flag) {
                //flag2 == 1,˵����״̬Ϊ��̬����Ҫ��ӽ���̬��
                if (flag2) {
                    name = "EndState" + to_string(endState.size() + 1) + ss;
                    newN = insertIntoEndState(name);
                }
                newN = insertIntoState(name);
                stateCorr[name] = n2;
                deal2(nfa, newN, n2);
            } else {
                //״̬���ڣ������Ӧ��ϵ
                newN = insertIntoState(buf);
            }
            transDFA[start][*it2] = newN;
        }
        //������ɣ��������
    }
}

void FA::printEdge() {
    for(const auto& from : transNFA){
        for (const auto& e : from.second) {
            cout<<"from "<<from.first.id<<"["<<from.first.name<<"] through ["<<e.first<<"] to ";
            int n = 0;
            for (const auto& to : e.second) {
                if(n > 0){
                    cout << " And ";
                }
                cout<<to.id<<"["<<to.name<<"]";
                n++;
            }
            cout<<endl;
        }
    }
}

void FA::printDFA() {
    for(const auto& from : transDFA){
        for (const auto& e : from.second) {
            cout<<"from "<<from.first.id<<"["<<from.first.name<<"] through ["<<e.first<<"] to "<<e.second.id<<"["<<e.second.name<<"]";

            cout<<endl;
        }
    }
}

void FA::printCharSet() {
    for (set<char>::iterator it = charSet.begin(); it != charSet.end(); ++it) {
        cout << *it << " "; // ��ӡset�е�ֵ
    }

}

Node FA::insertIntoStartState(string name) {
    // ʹ�õ��������� startState
    if (startState.name.empty()) {
        startState.name = name;
        startState.id = 1;
        count++;
        States.insert(startState);
    }
    return startState;
}

Node FA::insertIntoEndState(string name) {
    int flag = 0;
    Node node;
    // ʹ�õ��������� startState
    for (auto it = endState.begin(); it != endState.end(); ++it) {
        //���и�״̬���򲻲���
        if (it->name == name) {
            flag = 1;
            node.name = it->name;
            node.id = it->id;
            break;
        }
    }
    //�Ҳ���״̬,������״̬
    if (!flag){
        node = {++count,name};
        endState.insert(node);
        States.insert(node);
    }
    return node;
}

Node FA::insertIntoState(string name) {
    int flag = 0;
    Node node;
    // ʹ�õ��������� startState
    for (auto it = States.begin(); it != States.end(); ++it) {
        //���и�״̬���򲻲���
        if (it->name == name) {
            flag = 1;
            node.name = it->name;
            node.id = it->id;
            break;
        }
    }
    //�Ҳ���״̬,������״̬
    if (!flag){
        node = {++count,name};
        States.insert(node);
    }
    return node;
}

//������ڵ�Ħ�-�հ�
set<Node> FA::closure(const Node& node) {
    set<Node> result;
    stack<Node> stack;
    stack.push(node);
    result.insert(node);
    while (!stack.empty()) {
        Node current = stack.top();
        stack.pop();

        if (transNFA.count(current) > 0 && transNFA[current].count('$') > 0) {
            // ������ǰ�ڵ�����Ц�ת��
            for (Node next : transNFA[current]['$']) {
                if (result.count(next) == 0) {
                    result.insert(next);
                    stack.push(next);
                }
            }
        }
    }
    return result;
}

const set<char> &FA::getCharSet() const {
    return charSet;
}

const map<Node, map<char, set<Node>>> &FA::getTransNfa() const {
    return transNFA;
}

const map<Node, map<char, Node>> &FA::getTransDfa() const {
    return transDFA;
}

const set<Node> &FA::getEndState() const {
    return endState;
}

const Node &FA::getStartState() const {
    return startState;
}

string trim(string str) {
    size_t first = str.find_first_not_of(' ');
    if (first == string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, last - first + 1);
}

bool nodeStartsWith(set<Node> nodes, string prefix) {
    for (const auto& node : nodes) {
        if (node.name.compare(0, prefix.size(), prefix) == 0) {
            return true;
        }
    }
    return false;
}

bool setsAreEqual(set<Node> s1, set<Node> s2) {
    if (s1.size() != s2.size()) {
        return false;
    }
    for (Node x : s1) {
        if (s2.find(x) == s2.end()) {
            return false;
        }
    }
    return true;
}

template<typename T>
void addAllElements(set<T> &destSet, const set<T> &sourceSet) {
    for (const auto& element : sourceSet)
    {
        destSet.insert(element);
    }
}

bool Node::operator<(const Node &o) const {
    return id < o.id;
}

bool Node::operator==(const Node &o) const {
    return id == o.id;
}

bool startsWith(const string &prefix, const string &str) {
    if (str.size() < prefix.size()) {
        return false;
    }
    return std::equal(prefix.begin(), prefix.end(), str.begin());
}

bool hasNode(set<Node>& nodes, string string1, string string2) {
    for (auto it = nodes.begin(); it != nodes.end(); ++it) {
        if (it->name.substr(0, string1.length()) == string1 && it->name.substr(it->name.length() - string2.length()) == string2) {
            return true;
        }
    }
    return false;
}

bool isNodeNameEndsWith(const Node& node, const string& string2) {
    return (node.name.size() >= string2.size() &&
            node.name.compare(node.name.size() - string2.size(), string2.size(), string2) == 0);
}

set<string> readWordsFromFile(string path) {
    set<string> words;
    ifstream file(path);
    string word;
    if (!file){
        cout<<"�Ҳ����ؼ����ļ���"<<endl;
        return words;
    }
    while (getline(file,word)) {
        words.insert(trim(word));
    }
    file.close();
    return words;
}

template <typename T>
void addVector(vector<T>& v1, vector<T>& v2) {
    v2.insert(v2.end(), v1.begin(), v1.end());
}

void printTokens(vector<Token> tokens) {
    ofstream  out(TOKEN_PATH);
    if (!out.is_open()){
        cout<<"��Token�ļ�ʧ�ܣ�"<<endl;
        return;
    }
    cout << left << setw(20) << "line";
    cout << left << setw(20) << "Type";
    cout << left << setw(20) << "value" << endl;
    for (vector<Token>::iterator it = tokens.begin(); it != tokens.end(); ++it) {
        cout << left << setw(20) << it->line << right;
        out << it->line <<"\t";
        switch (it->type) {
            case 0:
                cout << left << setw(20) << "KEYWORD";
                out << "KEYWORD\t";
                break;
            case 1:
                cout << left << setw(20) << "IDENTIFIER";
                out << "IDENTIFIER\t";
                break;
            case 2:
                cout << left << setw(20) << "CONSTANT";
                out << "CONSTANT\t";
                break;
            case 3:
                cout << left << setw(20) << "DELIMITER";
                out << "DELIMITER\t";
                break;
            case 4:
                cout << left << setw(20) << "OPERATOR";
                out << "OPERATOR\t";
                break;
            default:
                cout << left << setw(20) << "unknown";
                out << "unknown\t";
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
        out << it->value << "\n";
    }
    out.close();
}

bool Token::operator<(const Token &o) const {
    return value.compare(o.value);
}

//���дʷ�����
vector<Token> LAbyLine(FA dfa, string line, int n) {
    vector<Token> t;
    string l = trim(line);
    //������������������ַ�
    string buf = "";
    //��ǰ״̬Ϊ��̬
    Node state = dfa.getStartState();
    map<Node,map<char,Node>> transDFA = dfa.getTransDfa();
    set<Node> e = dfa.getEndState();
    int i = 0;
    Token ttt;
    //������
    while ( i < l.length() ){
        state = transDFA[state][l[i]];
//        if (l=="float b = 1.145e+14.1;"){
//            int iii = 0;
//        }
        //�Ҳ�����һ״̬
        if (state.id == 0){
//            cout << buf << endl;
//            cout<<"��������ڵ� "<< n <<" ��"<<endl;
            vector<Token> v;
            string ssssss = "";
            ssssss+=l[i];
            //�ǳ�������e��˵���ǿ�ѧ��������������Ϣ��ʾ����
            if (ttt.type == CONSTANT && !(ttt.value.find('e') == string::npos && ttt.value.find('E') == string::npos)){
                //1Ϊ������
                v.push_back({ERROR,ssssss,1});
                v.push_back(ttt);
            } else if (ttt.type == CONSTANT && isdigit(ttt.value[0])){
                v.push_back({ERROR,ssssss,2});
                v.push_back(ttt);
            } else {
                v.push_back({ERROR,ssssss,9});
                v.push_back(ttt);
            }

            return v;
        }
        buf += l[i];
        //�����̬�����Ҳ���state��˵��������̬
        //��ʱ���������ȡ��һ���ַ�
        if(e.find(state) == e.end()){
            i++;
            continue;
        }else{
            if ((isdigit(buf[0]) && isalpha(l[i])) || isdigit(ttt.value[0]) && isalpha(l[i])){
                if (!(l[i] == 'i' || l[i] == 'e' || l[i] == 'E')){
                    vector<Token> v;
                    string ssssss = "";
                    ssssss+=l[i];
                    v.push_back({ERROR,ssssss,2});
                    v.push_back(ttt);
                    return v;
                }
            }
            //������̬������һ���ַ��ܷ񱻽��ܣ�������ܱ����գ�����Token����ȡ��һ����
            if(transDFA[state][l[i+1]].id == 0){
                Token token;
                if (isNodeNameEndsWith(state, "I")) {
                    //��ʶ���͹ؼ��֣������ؼ��ּ��ϣ��ж��Ƿ��ǹؼ���
                    set<string> keyword = readWordsFromFile(KEYWORD_PATH);
                    //�ǹؼ���
                    if (keyword.find(buf) != keyword.end()) {
                        token.type = KEYWORD;
                    } else{
                        token.type = IDENTIFIER;
                        if (isdigit(buf[0])){
                            cout << "��������ڵ� " << n << "�з����˲��Ϸ��ı�ʶ����" << buf << endl;
                        }
                    }
                } else if (isNodeNameEndsWith(state, "O")) {
                    token.type = OPERATOR;
                } else if (isNodeNameEndsWith(state, "S")) {
                    token.type = DELIMITER;
                } else if (isNodeNameEndsWith(state, "C")) {
                    token.type = CONSTANT;
                }
                //�������̬,���ﱻ���ܣ�����Token
//            token.type = ;
                token.line = n;
                token.value = buf;
                buf.clear();
                t.push_back(token);
                i++;
                state = dfa.getStartState();
                ttt = token;
                if (i < l.length())
                    continue;
                break;
            }
            else{
                //��һ��input�ܱ�����
                i++;
                continue;
            }
        }
    }
    return t;
}

//�ʷ�����
//����dfa���ļ�·����������дʷ��������зֵõ�Token��
int LexicalAnalyze(FA dfa, string path) {
    vector<Token> tokens;
    string line;
    int n = 0;
    ifstream file(path);
    if (!file){
        cout << "�Ҳ���Դ�����ļ���" << endl;
        return 0;
    }
    //��file�л�ȡһ�У�������line��
    while(getline(file,line)){
        n++;
        line = trim(line);
        //��ͷ����ע�ͣ�����
        if (line[0] == '/' && line[1] == '/'){
//            n++;
            continue;
        }
        //��β��ע�ͣ�ȥ�������ע��
        if (line.find("//") != string::npos){
            line = line.substr(0, line.find("//"));
        }
        vector<Token> to = LAbyLine(dfa, line, n);
        //����
        if (to[0].type == ERROR){
            cout<<"�����ڵ� "<< n <<" ��"<<endl;
            cout<<to[1].value<<to[0].value<<endl;
            for (int i = 0; i < to[1].value.length(); ++i) {
                cout<<" ";
            }
            cout<<"^"<<endl;
            switch (to[0].line) {
                case 1:
                    cout<<"��ѧ����������"<<endl;
                    break;
                case 2:
                    cout<<"��ʶ������"<<endl;
                    break;
                default:
                    cout<<"�����ַ� '"<<to[0].value<<"' �ڴ˴����Ϸ���"<<endl;
            }
            return 0;
        }
        tokens.insert(tokens.end(),to.begin(),to.end());
    }
    file.close();
    cout<<"�ʷ����������"<<endl;
    printTokens(tokens);
    return 1;
}
