#include "FA.h"



//���дʷ�����
vector<Token> LAbyLine(FA dfa, string line, int n){
    vector<Token> t;
    string l = trim(line);
    //������������������ַ�
    string buf = "";
    //��ǰ״̬Ϊ��̬
    Node state = dfa.getStartState();
    map<Node,map<char,Node>> transDFA = dfa.getTransDfa();
    set<Node> e = dfa.getEndState();
    int i = 0;
    //������
    while ( i < l.length() ){
//        if (l[i] == ' '){
//            if (++i < l.length()){
//                continue;
//            }
//            break;
//        }
        state = transDFA[state][l[i]];
        if (state.id == 0){
            cout<<"��������ڵ� "<< n <<" ��"<<endl;
        }
        buf += l[i];
        //�����̬�����Ҳ���state��˵��������̬
        //��ʱ���������ȡ��һ���ַ�
        if(e.find(state) == e.end()){
            i++;
            continue;
        }else{
            //������̬������һ���ַ��ܷ񱻽��ܣ�������ܱ����գ�����Token����ȡ��һ����
            if(transDFA[state][l[i+1]].id == 0){
                Token token;
                if (isNodeNameEndsWith(state, "I")) {
                    //��ʶ���͹ؼ��֣������ؼ��ּ��ϣ��ж��Ƿ��ǹؼ���
                    set<string> keyword = readWordsFromFile(KEYWORD_PATH);
                    //�ǹؼ���
                    if (keyword.find(buf) != keyword.end()) {
                        token.type = KEYWORD;
                    } else
                        token.type = IDENTIFIER;
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
vector<Token> LexicalAnalyze(FA dfa,string path){
    vector<Token> tokens;
    string line;
    int n = 0;
    ifstream file(path);
    if (!file){
        cout << "�Ҳ���Դ�����ļ���" << endl;
        return tokens;
    }
    //��file�л�ȡһ�У�������line��
    while(getline(file,line)){
        n++;
        vector<Token> to = LAbyLine(dfa, line, n);
        tokens.insert(tokens.end(),to.begin(),to.end());
    }
    file.close();
    return tokens;
}




int main() {
    //�����ķ���ȡNFA
    FA fa;
    fa.GrammarToNFA(GRAMMAR_PATH);
    //����̨�����NFA
    cout<<"NFA:"<<endl;
    fa.printEdge();
//    cout<<"charSet:"<<"[ ";
//    fa.printCharSet();
//    cout<<"]"<<endl;
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
    vector<Token> tokens = LexicalAnalyze(dfa,SOURCE_PATH);
    cout<<"�ʷ����������"<<endl;
    printTokens(tokens);
    return 0;
}


