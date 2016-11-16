//
//  ACautomaton.cpp
//  labelPropagation_similarity
//
//  Created by 李一楠 on 16/5/15.
//  Copyright (c) 2015 yinan. All rights reserved.
//

#include "ACautomaton.h"


void ACAutomaton::init()
{
    
    ifstream wordDict(DIRECTION"labelPropagation_similarity/userdic_C_en.txt");
    stringstream buf;
    buf<<wordDict.rdbuf();
    string dictContent(buf.str());
    unsigned long charCount=0;
    //统计术语库中有多少种字符，以及建立map_char_id;
    for(unsigned long i=0;i<dictContent.size();i++){
        auto iter=map_char_id.find(dictContent[i]);
        if(iter==map_char_id.end()){
            map_char_id.insert(pair<char,int>(dictContent[i],charCount));
            charCount++;
            if(charCount%100==0) cout<<"a";
        }
    }
    wordDict.close();
    this->CLD_NUM=charCount;
    for(int i=0;i<MAX_N;i++){
        this->trie[i]=new int[CLD_NUM];
    }
}

void ACAutomaton::reset()
{
    memset(trie[0], -1, sizeof(trie[0]));
    tag[0] = 0;
    n = 1;
}

//插入模式串s，构造单词树(keyword tree)
void ACAutomaton::add(const char *s)
{
    int p = 0;
    while (*s)
    {
        int i = map_char_id[*s];
        if ( -1 == trie[p][i] )
        {
            memset(trie[n], -1, sizeof(trie[n]));
            tag[n] = 0;
            trie[p][i] = n++;
        }
        p = trie[p][i];
        s++;
    }
    tag[p]++;         //因题而异
}

//构造AC自动机，用BFS来计算每个结点的fail指针，就是构造trie图
void ACAutomaton::construct()
{
    queue<int> Q;
    fail[0] = 0;
    for (int i = 0; i < CLD_NUM; i++)
    {
        if (-1 != trie[0][i])
        {
            fail[trie[0][i]] = 0; //root下的第一层结点的fail指针都指向root
            Q.push(trie[0][i]);
        }
        else
        {
            trie[0][i] = 0;    //这是阶段一中的第2步
        }
    }
    while ( !Q.empty() )
    {
        int u = Q.front();
        Q.pop();
        for (int i = 0; i < CLD_NUM; i++)
        {
            int &v = trie[u][i];
            if ( -1 != v )
            {
                Q.push(v);
                fail[v] = trie[fail[u]][i];
                tag[u] += tag[fail[u]];     //因题而异，某些题目中不需要这句话
            }
            else
            {            //当trie[u][i]==-1时，设置其为trie[fail[u]][i]，就构造了trie图
                v = trie[fail[u]][i];
            }
        }
    }
}

//因题而异
//在目标串t中匹配模式串
void ACAutomaton::solve(const char *t,map<int,double> &map_wordIdx_tfIdf)
{
    int q = 0, ret = 0;
    while ( *t )
    {
        q = trie[q][map_char_id[*t]];
        int u = q;
        while ( u != 0 )
        {
            map_wordIdx_tfIdf[u]+=tag[u];
            u = fail[u];
        }
        t++;
    }
}
