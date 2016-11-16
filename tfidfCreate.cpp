//
//  tfidfCreate.cpp
//  labelPropagation_similarity
//
//  Created by 李一楠 on 16/5/15.
//  Copyright (c) 2015 yinan. All rights reserved.
//

#include "tfidfCreate.h"
#include <iostream>
#include<fstream>
#include<string>
#include<sstream>
#include<assert.h>
#include"ACautomaton.h"
#include<math.h>
ACAutomaton tfidf::ac;
std::vector<std::map<int,double>> tfidf::vecTfIdf;
    void tfidf::makeActree(){
        
        ac.init();
        ac.reset();
        ifstream wordDict(DIRECTION"labelPropagation_similarity/userdic_C_en.txt");
        while(!wordDict.eof()){
            string line;
            getline(wordDict,line);
            const char *s=line.c_str();
            ac.add(s);
        }
        ac.construct();
    }
    void tfidf::makeWordCount(){
        ifstream ifs_idxDoi("/Users/liyinan/Documents/gradthesis/labelPropagation_similarity/newIdx_doi.txt");
        ifstream ifs_doiFile("/Users/liyinan/Documents/gradthesis/labelPropagation_similarity/doi_filename.txt");
        map<string,int> map_doi_fileNum;  //first establish doi_filenum map
        while(!ifs_doiFile.eof()){
            string line;
            getline(ifs_doiFile,line);
            int pos=0;
            for(int i=0;i<line.size();i++){
                if(line[i]==':'&&line[i+1]==':'){
                    pos=i;
                    break;
                }
            }
            string doi=line.substr(0,pos);
            int filenum=atoi(line.substr(pos+3,5).c_str());
            map_doi_fileNum.insert(pair<string,int>(doi,filenum));
            ifs_doiFile.peek();
        }
        while(!ifs_idxDoi.eof()){
            string line;
            getline(ifs_idxDoi,line);
            int pos=0;
            for(int i=0;i<line.size();i++){
                if(line[i]==':'){
                    pos=i+2;
                    break;
                }
            }
            int idx=atoi(line.substr(0,pos-2).c_str());
            string doi=line.substr(pos,line.size()-pos);
            int filenum=0;
            auto iter=map_doi_fileNum.find(doi);
            if(iter!=map_doi_fileNum.end())
                filenum=iter->second;
            char filename[100];
            assert(filenum!=0);
            sprintf(filename,DIRECTION"newCrawWithBigSeedProc/%05d",filenum);
            ifstream ifs_file(filename);
            stringstream buf;
            buf<<ifs_file.rdbuf();
            string temp(buf.str());
            ifs_file.close();
            unsigned long startPos=temp.find("<refInfo/>");
            unsigned long endPos=temp.find("<text/>");
            string content=temp.substr(startPos,endPos-startPos);
            const char *t=content.c_str();
            map<int,double> map_tfIdf;      //map的first是由ac自动机构造前缀树时就决定了的
            ac.solve(t,map_tfIdf);
            vecTfIdf.push_back(map_tfIdf);
        }
    }
    void tfidf::normalizeTfIdf(){    //convert wordCount to tf*idf
        for(int i=0;i<vecTfIdf.size();i++){
            int wordSum=0;
            for(auto iter=vecTfIdf[i].begin();iter!=vecTfIdf[i].end();iter++){   //计算tf
                wordSum+=iter->second;
            }
            for(auto iter=vecTfIdf[i].begin();iter!=vecTfIdf[i].end();iter++){
                iter->second/=double(wordSum);
            }
        }
        unsigned long nodeCount=vecTfIdf.size();        //计算idf，以及tf*idf
        for(int i=0;i<ac.MAX_N;i++){
            double appearCount=0;
            for(int j=0;j<vecTfIdf.size();j++){
                auto iter=vecTfIdf[j].find(i);
                if(iter!=vecTfIdf[j].end()){
                    appearCount+=1;
                }
            }
            double idf=log(nodeCount/appearCount);
            for(int j=0;j<vecTfIdf.size();j++){
                auto iter=vecTfIdf[j].find(i);
                if(iter!=vecTfIdf[j].end()){
                    iter->second*=idf;
                }
            }
        }
        //归一化tfIdf向量的长度
        for(int i=0;i<vecTfIdf.size();i++){
            double squareL=0;
            for(auto iter=vecTfIdf[i].begin();iter!=vecTfIdf[i].end();iter++){
                squareL+=pow(iter->second,2);
            }
            double L=pow(squareL,0.5);
            for(auto iter=vecTfIdf[i].begin();iter!=vecTfIdf[i].end();iter++){
                iter->second/=L;
            }
        }
    }
    

