#include <bits/stdc++.h>

#define GRAMMAR_PATH "E:\\Code\\JetBrain\\CLion\\LAXER\\GRAMMAR.txt"


using namespace std;

//������
char str[1024];
string line;

string rtrim(string str) {
    size_t pos = str.find_last_not_of(' ');
    if (pos != string::npos) {
        str.erase(pos + 1);
    }
    return str;
}

string ltrim(string str) {
    size_t first = str.find_first_not_of(' ');
    if (first == string::npos) {
        return "";
    }
    return str.substr(first);
}

string trim(string str) {
    size_t first = str.find_first_not_of(' ');
    if (first == string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(' ');
    return str.substr(first, last - first + 1);
}

// Token����ö��
enum TokenType {
    KEYWORD,
    IDENTIFIER,
    CONSTANT,
    DELIMITER,
    OPERATOR
};

// Token�ṹ��
struct Token {
    TokenType type; // Token����
    string value;   // Tokenֵ
    int line;       // ��������
};

struct Node{ //�ڵ�
    int id;
    string name;
    bool operator < (const Node &o) const
    {
        return id < o.id;
    }

    bool operator == (const Node &o) const{
        return id == o.id;
    }
};

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


template <typename T>
void addAllElements(std::set<T>& destSet, const std::set<T>& sourceSet)
{
    for (const auto& element : sourceSet)
    {
        destSet.insert(element);
    }
}

class FA{   //�Զ���M = (K, ��, f, S, Z)
    vector<Node> startState;                   //��̬��S
    vector<Node> endState;                     //��̬��Z
    vector<Node> States;                       //״̬��K
public:
    const vector<Node> &getStartState() const;

    void setStartState(const vector<Node> &startState);

    const vector<Node> &getEndState() const;

    void setEndState(const vector<Node> &endState);

    const vector<Node> &getStates() const;

    void setStates(const vector<Node> &states);

    const set<char> &getCharSet() const;

    void setCharSet(const set<char> &charSet);

    const map<Node, map<char, set<Node>>> &getTransNfa() const;

    void setTransNfa(const map<Node, map<char, set<Node>>> &transNfa);

    const map<Node, map<char, Node>> &getTransDfa() const;

    void setTransDfa(const map<Node, map<char, Node>> &transDfa);

    int getCount() const;

    void setCount(int count);

private:
    set<char> charSet;                      //��ĸ��
    map<Node,map<char,set<Node>>> transNFA; //NFA״̬ת��fN(Node:��ǰ״̬ map����ͬ�����Ӧ��״̬ת��)
    map<Node,map<char,Node>> transDFA;      //DFA״̬ת��fD
    int count = 0;                          //�ڵ����
    map<string,set<Node>> stateCorr;        //״̬��Ӧ��ϵ

public:
    FA TransToDFA();                     //����������NFA->DFA

    void minimizeDFA();                     //������������С��DFA

    void GrammarToNFA(string path);         //����������������﷨������ת��ΪNFA

    void printEdge();                       //���������NFA��״̬ת�ƹ�ϵ

    void printDFA();                        //���������DFA��״̬ת�ƹ�ϵ

    void printCharSet();                    //�����������ĸ�������е���ĸ

    void deal(string line);                 //��������������һ������

    Node insertIntoStartState(string name); //��������һ��״̬�����̬����ͬʱ����״̬������������ڣ����ظýڵ�

    Node insertIntoEndState(string name);   //��������һ��״̬������̬��

    Node insertIntoState(string name);      //��һ��״̬����״̬��

    set<Node> closure(const Node& node);    //������ڵ�Ħ�-�հ�

    void TransToDFA(FA nfa);

    void deal2(FA nfa,Node start, set<Node> n);

    set<Node> move(char input,set<Node> node,FA nfa);
};
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
    ifstream file(path);

    if(!file){
        cout<<"�Ҳ����ķ��ļ���"<<endl;
        return ;
    }
    //��ȡ��һ�д���line��
    getline(file,line);
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
        //����ע��(��ʱ������)
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
    }while(getline(file,line)&&!line.empty());


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
        rs = rs.substr(1);
        if (rs.empty()){
            rs = "EndState_"+to_string(endState.size()+1);
            insertIntoEndState(rs);
        }
    }else{
        //����������
        input = rs[0];
        //ȥ����һλ��ʣ�µ�����һ״̬
        rs = rs.substr(1);
        if (rs.empty()){
            rs = "EndState_"+to_string(endState.size()+1);
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
    set<Node> n = nfa.closure(nfa.startState.front());
//    for (set<Node>::iterator it2 = n.begin(); it2 != n.end(); ++it2) {
//        cout<<it2->id<<" "<<it2->name<<endl;
//    }
    Node start = insertIntoStartState("Start"+to_string((startState.size()+1)));
    stateCorr["Start_"+to_string(startState.size())] = n;
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
        //flag3 = 0,˵��û����һ״̬
        if(n2.empty())
            flag3 = 0;
        //��������Ľڵ�������̬�����
        if (nodeStartsWith(n2,"EndState")){
            flag2 = 1;
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
                    name = "EndState" + to_string(endState.size() + 1);
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
    int flag = 0;
    Node node;
    // ʹ�õ��������� startState
    for (auto it = startState.begin(); it != startState.end(); ++it) {
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
        node = {count++,name};
        startState.push_back(node);
        States.push_back(node);
    }
    return node;
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
        node = {count++,name};
        endState.push_back(node);
        States.push_back(node);
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
        node = {count++,name};
        States.push_back(node);
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

const vector<Node> &FA::getStartState() const {
    return startState;
}

void FA::setStartState(const vector<Node> &startState) {
    FA::startState = startState;
}

const vector<Node> &FA::getEndState() const {
    return endState;
}

void FA::setEndState(const vector<Node> &endState) {
    FA::endState = endState;
}

const vector<Node> &FA::getStates() const {
    return States;
}

void FA::setStates(const vector<Node> &states) {
    States = states;
}

const set<char> &FA::getCharSet() const {
    return charSet;
}

void FA::setCharSet(const set<char> &charSet) {
    FA::charSet = charSet;
}

const map<Node, map<char, set<Node>>> &FA::getTransNfa() const {
    return transNFA;
}

void FA::setTransNfa(const map<Node, map<char, set<Node>>> &transNfa) {
    transNFA = transNfa;
}

const map<Node, map<char, Node>> &FA::getTransDfa() const {
    return transDFA;
}

void FA::setTransDfa(const map<Node, map<char, Node>> &transDfa) {
    transDFA = transDfa;
}

int FA::getCount() const {
    return count;
}

void FA::setCount(int count) {
    FA::count = count;
}




int main() {
    //�����ķ���ȡNFA
    FA fa;
    fa.GrammarToNFA(GRAMMAR_PATH);
    //����̨�����NFA
    cout<<"NFA:"<<endl;
    fa.printEdge();
    cout<<"charSet:"<<"[ ";
    fa.printCharSet();
    cout<<"]"<<endl;
/*    set<Node> node = {{1,"111"},{2,"222"},{3,"ࣰ�"}};
    set<Node> n2 = {{1,"111"},{4,"������������"}};
    set<Node> n3;
    addAllElements(n3,node);
    for (set<Node>::iterator it = n3.begin(); it != n3.end(); ++it) {
        cout << it->id << ": " << it->name << endl; // ��ӡset�е�ֵ
    }*/
/*    //��-�հ�����
    set<Node> n = fa.closure(fa.getStartState().front());
    for (set<Node>::iterator it = n.begin(); it != n.end(); ++it) {
        cout << it->id << ": " << it->name << endl; // ��ӡset�е�ֵ
    }*/
    FA dfa;
    dfa.TransToDFA(fa);
    //����NFAתΪDFA,Ȼ����С��,�����С����DFA
//    fa.TransToDFA();
    cout<<"DFA:"<<endl;
//    fa.minimizeDFA();
    dfa.printDFA();
    //���дʷ�����
    return 0;
}
