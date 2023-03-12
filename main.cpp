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
};

class FA{   //�Զ���M = (K, ��, f, S, Z)
public:
    vector<Node> startState;                   //��̬��S
    vector<Node> endState;                     //��̬��Z
    vector<Node> States;                       //״̬��K
    set<char> charSet;                      //��ĸ��
    map<Node,map<char,set<Node>>> transNFA; //NFA״̬ת��fN(Node:��ǰ״̬ map����ͬ�����Ӧ��״̬ת��)
    map<Node,map<char,Node>> transDFA;      //DFA״̬ת��fD
//    string stringMod;                       //����ʽ
    void modifyToDFA();                     //����������NFA->DFA
    void minimizeDFA();                     //������������С��DFA
    int count = 0;                          //�ڵ����

//    FA(string stringMod){       //��������ʽ->����NFA->ȷ����ΪDFA->DFA��С��
//        this->stringMod = stringMod;
//    }
    void GrammarToNFA(string path);
    //���FA
    void printEdge(){
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

    void printDFA(){
        for(const auto& from : transDFA){
            for (const auto& e : from.second) {
                cout<<"from "<<from.first.id<<"["<<from.first.name<<"] through ["<<e.first<<"] to "<<e.second.id<<"["<<e.second.name<<"]";

                cout<<endl;
            }
        }
    }

    pair<Node,Node> createNFA();

    bool check(string in);
    
    pair<Node,Node> createNFA(int& l);

    void deal(string line);

    Node insertIntoStartState(string name){
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

    Node insertIntoEndState(string name){
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

    Node insertIntoState(string name){
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
};
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
        //����ע��
        if(line[0] == '/' && line[1] == '/'){
            line.clear();
            getline(cin,line);
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
            case 't':
                input = '\t';
            case 'n':
                input = '\n';
            default:
                input = '$';
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
//    cout<<insertIntoState(rs).id<<" "<<rs<<endl;
    transNFA[node][input].insert(insertIntoState(rs));
}

void FA::modifyToDFA()
{
    queue<set<Node>> Q;
    set<Node> startSet;
    for (auto s : startState)
    {
        startSet.insert(s);
    }
    Q.push(startSet);
    map<set<Node>, Node> newState;   // DFA��״̬��Ӧ�ľ�״̬����
    while (!Q.empty())
    {
        set<Node> currentSet = Q.front();
        Q.pop();
        Node currentState;
        currentState.id = count++;
        for (auto s : currentSet)
        {
            currentState.name += s.name;
            currentState.name += ',';
        }
        States.push_back(currentState);
        newState[currentSet] = currentState;

        for (auto c : charSet)
        {
            set<Node> newSet;
            for (auto s : currentSet)
            {
                if (transNFA[s].find(c) != transNFA[s].end())
                {
                    newSet.insert(transNFA[s][c].begin(), transNFA[s][c].end());
                }
            }
            if (!newSet.empty())
            {
                if (newState.find(newSet) == newState.end())
                {
                    Q.push(newSet);
                    Node newStateNode;
                    newStateNode.id = count++;
                    for (auto s : newSet)
                    {
                        newStateNode.name += s.name;
                        newStateNode.name += ',';
                    }
                    States.push_back(newStateNode);
                    newState[newSet] = newStateNode;
                }
                transDFA[currentState][c] = newState[newSet];
            }
        }
    }

    for (auto s : States)
    {
        for (auto end : endState)
        {
            if (s.name.find(end.name) != string::npos)
            {
                s.name += '*';
                break;
            }
        }
    }
}

int main() {
    //�����ķ���ȡNFA
    FA fa;
    fa.GrammarToNFA(GRAMMAR_PATH);
    //����̨�����NFA
    fa.printEdge();
    //����NFAתΪDFA,Ȼ����С��,�����С����DFA
    fa.modifyToDFA();
    cout<<"DFA:"<<endl;
//    fa.minimizeDFA();
    fa.printDFA();
    //���дʷ�����
    return 0;
}
