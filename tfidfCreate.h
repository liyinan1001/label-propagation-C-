//
//  tfidfCreate.h
//  labelPropagation_similarity
//
//  Created by 李一楠 on 16/5/15.
//  Copyright (c) 2015 yinan. All rights reserved.
//

#ifndef __labelPropagation_similarity__tfidfCreate__
#define __labelPropagation_similarity__tfidfCreate__

#include <stdio.h>
#include <map>
#include"ACautomaton.h"
namespace tfidf{
extern ACAutomaton ac;
extern vector<map<int,double>> vecTfIdf;    //存储每个结点的tf-idf，结点顺序是newidx
void makeActree();
void makeWordCount();
void normalizeTfIdf();
}
#endif /* defined(__labelPropagation_similarity__tfidfCreate__) */
