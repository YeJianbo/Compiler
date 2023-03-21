//#include "FA.h"
#include "LR.h"


int main() {
    //�����ķ���ȡNFA
    FA fa;
    fa.GrammarToNFA(GRAMMAR_PATH);
    //����̨�����NFA
    cout<<"NFA:"<<endl;
    fa.printEdge();
    FA dfa;
    dfa.TransToDFA(fa);
    //����NFAתΪDFA,Ȼ����С��,�����С����DFA
    cout<<"DFA:"<<endl;
    dfa.printDFA();
    //���дʷ�����
    vector<Token> tokens = LexicalAnalyze(dfa,SOURCE_PATH);
    cout<<"�ʷ����������"<<endl;
    printTokens(tokens);
    LR lr;
    //���﷨
    lr.readGrammar(GRAMMAR_2NF_PATH);
    lr.printProduction();

//    lr.is = lr.closure();
    lr.construct_LR1_itemsets();
    lr.printItemSet();
    lr.parse(TOKEN_PATH);
//    lr.printToken();
    return 0;
}