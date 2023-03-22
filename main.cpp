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
    int i = LexicalAnalyze(dfa,SOURCE_PATH);
    if (!i){
        cout<<"�������������ֹ��"<<endl;
        return -1;
    }
    LR lr;
    //�﷨����
    lr.readGrammar(GRAMMAR_2NF_PATH);
    lr.printProduction();
    lr.construct_LR1_itemsets();
    lr.printItemSet();
    lr.parse(TOKEN_PATH);
    return 0;
}