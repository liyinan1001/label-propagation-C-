//
//  calculate_correct_raito.cpp
//  labelPropagation
//
//  Created by 李一楠 on 2/5/15.
//  Copyright (c) 2015 yinan. All rights reserved.
//

#include "calculate_correct_raito.h"
//#include<fstream>
using namespace std;
double calculate_correct_ratio(){
    ifstream ifs_testSet("/Users/liyinan/Documents/gradthesis/idx_label_testSet.txt");//创建一个输入文件流，名字叫ifs_testSet.文件地址是ballalala
    ifstream ifs_originalLabel("/Users/liyinan/Documents/gradthesis/idx_label.txt");
    ifstream ifs_resultLabel("/Users/liyinan/Documents/gradthesis/train_return.txt.result");
    ofstream ofs_falseNode("/Users/liyinan/Documents/gradthesis/falseNode.txt");
    set<int> testSet;
    //create testset
    while(!ifs_testSet.eof()){
        string line;
        getline(ifs_testSet,line);
        testSet.insert(atoi(line.c_str()));
        
        ifs_testSet.peek();
    }
    
    int linenum=1;
    int correctnum=0;
    while((!ifs_originalLabel.eof())&&(!ifs_resultLabel.eof())){
        string originalLine;
        string resultLine;
        getline(ifs_originalLabel,originalLine);
        getline(ifs_resultLabel,resultLine);
        resultLine.erase(0,resultLine.find("::")+2);
        int resultlabelIdx=atoi(resultLine.c_str());
        if(testSet.find(linenum)!=testSet.end()){    //is a test node
            stringstream originalLabels(originalLine);
            int flag=0;
            while(!originalLabels.eof()){
                string label;
                originalLabels>>label;
                int labelIdx=atoi(label.c_str());
                if(resultlabelIdx==labelIdx) {
                    correctnum++;
                    flag=1;
                    break;
                }
            }
            if(flag==0)
                ofs_falseNode<<linenum<<endl;
        }
        linenum++;
        ifs_originalLabel.peek();
        ifs_resultLabel.peek();
    }
    double correctRatio=double(correctnum)/(testSet.size());
    cout<<correctRatio<<endl;
    ifs_originalLabel.clear();
    ifs_resultLabel.clear();
    ifs_testSet.clear();
    ifs_originalLabel.close();
    ifs_resultLabel.close();
    ifs_testSet.close();
    ofs_falseNode.close();
    return correctRatio;
    system("pause");

}
