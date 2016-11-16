#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include<string>
namespace ssl_lprop {

typedef struct Edge {
  int node;
  double weight;
  Edge () : node(-1), weight(-1.0) {}
  Edge (int node, double weight)
  : node(node), weight(weight) {}
} Edge;
typedef struct queNode{
    int index;
    int count_labelNeibor;
    queNode() : index(-1),count_labelNeibor(-1){}
    queNode(int index,int count)
    : index(index),count_labelNeibor(count){}
}queNode;

typedef std::vector<Edge> Array;
typedef std::vector<Array> Matrix;
typedef std::vector<std::vector<int>> Labels;
typedef std::vector<std::vector<double> > LabelMatrix;
//extern double neiborDistEps_;

void normalize(Matrix& trans_mat,
               Matrix& norm_trans_mat);
void load_mat(Matrix& trans_mat,
              Matrix& norm_trans_mat);
int  load_lab(Labels& lab,
              const std::string& intpu);
void load_submatrix(const Matrix& mat,
                    Matrix& mat_uu,
                    Matrix& mat_ul,
                    const int U,
                    const int L,
                    const std::vector<int> unlabeled_nodes,
                    const std::vector<int> labeled_nodes,
                    const Labels& lab);
// end of namespace ssl_lprop
}
#endif
