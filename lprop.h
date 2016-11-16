#ifndef LPROP_H
#define LPROP_H
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include <cassert>
#include <stdexcept>
#include <algorithm>
#include "graph.h"
namespace ssl_lprop {

class LP {
    public:
    LP () : N_(0), L_(0), U_(0), eps_(1.0e-9) {};
    explicit LP (double eps) : N_(0), L_(0), U_(0), eps_(eps) {};
    ~LP () { clear(); }

    void clear();
    bool read(const std::string mfilename,
              const std::string lfilename);
    bool train(const int max_iter);
    bool write(const char* filename,
               const unsigned int prec);
    void show(const unsigned int prec);
   // bool compare(const queNode& a,const queNode& b);
    void initialize_matrix_queue();
    void update_weight_queue(const int node_idx,
                             const int idx_in_queue);
    private:
    unsigned int N_; // size of nodes
    unsigned int L_; // size of labeled nodes
    unsigned int U_; // size of unlabeled nodes
    unsigned int C_;
    double eps_;
    ssl_lprop::Matrix trans; // transition matrix
    ssl_lprop::Matrix norm; // row-normalized transition matrix
    ssl_lprop::Labels labels;
    ssl_lprop::Matrix norm_uu; // UU part of norm
    ssl_lprop::Matrix norm_ul; // UL part of norm
    ssl_lprop::Matrix trans_norm_uu;
    ssl_lprop::Matrix trans_norm_ul;
    ssl_lprop::LabelMatrix y_u; // labels
    ssl_lprop::LabelMatrix y_l; // labels
    std::vector<int> labeled_nodes, unlabeled_nodes;
    std::vector<int> node_index_v;
    std::vector<queNode> prior_que;
  
};

}

#endif
