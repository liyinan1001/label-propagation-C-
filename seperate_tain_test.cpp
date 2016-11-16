//
//  seperate_tain_test.cpp
//  labelPropagation
//
//  Created by 李一楠 on 2/5/15.
//  Copyright (c) 2015 yinan. All rights reserved.
//
#include<fstream>
#include<random>
#include "seperate_tain_test.h"
using namespace std;
void seperate_train_test(){
    char ifilename[100]="/Users/liyinan/Documents/gradthesis/idx_label.txt";
    char trainSetFile[100]="/Users/liyinan/Documents/gradthesis/idx_label_trainSet.txt";
    char testSetFile[100]="/Users/liyinan/Documents/gradthesis/idx_label_testSet.txt";

    ifstream ifs(ifilename);
    ofstream trainfs(trainSetFile);
    ofstream testfs(testSetFile);
    int linenum=1;
    while(!ifs.eof()){
        string line;
        getline(ifs,line);
        if(line=="?\r"||line=="?"||line=="? \r"){
            trainfs<<"?"<<endl;
        }
        else{
            random_device rd;
            double ratio=double((rd()-random_device::min()))/(random_device::max()-random_device::min());
            if(ratio>=0.9){
                testfs<<linenum<<endl;
                trainfs<<"?"<<endl;
            }
            else{
                trainfs<<line<<endl;
            }
        }
        linenum++;
        ifs.peek();
    }
    ifs.clear();
    trainfs.clear();
    testfs.clear();
    ifs.close();
    trainfs.close();
    testfs.close();
    return;

}
