//
//  ACautomaton.h
//  labelPropagation_similarity
//
//  Created by 李一楠 on 16/5/15.
//  Copyright (c) 2015 yinan. All rights reserved.
//

#ifndef __labelPropagation_similarity__ACautomaton__
#define __labelPropagation_similarity__ACautomaton__
#define DIRECTION "/Users/liyinan/Documents/gradthesis/"

#include <stdio.h>
#include<iostream>
#include<queue>
#include<map>
#include<fstream>
#include<sstream>
#include<string>
using namespace std;
class ACAutomaton
{
public:
    static const int MAX_N = 107909 * 120 + 5;
    //最大结点数：模式串个数 X 模式串最大长度
    int CLD_NUM;  //术语库中包含有希腊字母，特殊符号，等等。
    map<char,int> map_char_id;      //字母x对应的结点编号为id[x]
    int n;                          //trie树当前结点总数
    int fail[MAX_N];                //fail指针
    int tag[MAX_N];                 //根据题目而不同
    int *trie[MAX_N];       //trie树，也就是goto函数
    
    void init();
    
    void reset();
    
    //插入模式串s，构造单词树(keyword tree)
    void add(const char *s);
    
    //构造AC自动机，用BFS来计算每个结点的fail指针，就是构造trie图
    void construct();
    
    //因题而异
    //在目标串t中匹配模式串
    void solve(const char *t, map<int,double> &map_wordIdx_tfIdf);
};

#endif /* defined(__labelPropagation_similarity__ACautomaton__) */
