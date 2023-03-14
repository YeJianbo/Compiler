#include "FA.h"

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

//���дʷ�����
set<Token> LAbyLine(FA dfa, string line){

}

//�ʷ�����
//����dfa���ļ�·����������дʷ��������зֵõ�Token��
set<Token> LexicalAnalyze(FA dfa,string path){
    set<Token> tokens;
    string line;
    ifstream file(path);
    if (!file){
        cout << "�Ҳ���Դ�����ļ���" << endl;
        return tokens;
    }
    //��file�л�ȡһ�У�������line��
    while(getline(file,line)){
        addAllElements(tokens,LAbyLine(dfa, line));
    }
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
    set<Token> tokens = LexicalAnalyze(dfa,SOURCE_PATH);
    for (set<Token>::iterator it = tokens.begin(); it != tokens.end(); ++it) {
        cout << it->line << ": " << it->type <<" "<< it->value << endl; // ��ӡset�е�ֵ
    }
    return 0;
}


