#include "lprop.h"

namespace ssl_lprop {

    void LP::clear()
    {
  // clear
    }
    bool LP::read (const std::string mfilename,
               const std::string lfilename)
    {
        try {
            ssl_lprop::load_mat(trans, norm);
            C_ = ssl_lprop::load_lab(labels, lfilename);
        } catch (const std::exception& e) {
            std::cout << "ERROR: " << e.what() << std::endl;
            return false;
        }
        assert(trans.size() == labels.size());
        N_ = trans.size();
        L_ = 0;
        std::vector<int> unlabeled;
        for (int i=0; i<N_; i++) {
            if (labels[i][0] < 0) {
                unlabeled.push_back(i+1);
            } else {
                L_++;
            }
        }
        U_ = N_ - L_;
        node_index_v    = std::vector<int>(N_);
        labeled_nodes   = std::vector<int>(L_);
        unlabeled_nodes = std::vector<int>(U_);

        int l=0, u=0;
        for (int i=0; i<N_; i++) {
            if (labels[i][0] < 0) {
                node_index_v[i] = u;
                unlabeled_nodes[u++] = i;
            } else {
                node_index_v[i] = l;
                labeled_nodes[l++] = i;
            }
        }
        assert(l==L_);
        assert(u==U_);
        assert(static_cast<int>(unlabeled_nodes.size() == U_));
        assert(static_cast<int>(labeled_nodes.size() == L_));
        assert(U_ == static_cast<int>(unlabeled.size()));

        try {
            if (N_ <= 0) throw std::runtime_error ("N is invalid.");
            if (L_ <= 0) throw std::runtime_error ("L is invalid.");
            if (U_ <= 0) throw std::runtime_error ("U is invalid.");
            if (N_ != L_ + U_)
                throw std::runtime_error ("matrix and labels are invalid.");
        } catch (const std::exception& e) {
            std::cerr << "ERROR: " << e.what() << std::endl;
            return false;
        }

        norm_uu = ssl_lprop::Matrix(U_, ssl_lprop::Array());
        norm_ul = ssl_lprop::Matrix(U_, ssl_lprop::Array());
        load_submatrix(norm, norm_uu, norm_ul,
                       U_, L_, unlabeled_nodes, labeled_nodes,
                       labels);
        trans_norm_uu=norm_uu;
        trans_norm_ul=norm_ul;
        y_u = ssl_lprop::LabelMatrix(C_, std::vector<double>(U_, 0.0));
        y_l = ssl_lprop::LabelMatrix(C_, std::vector<double>(L_, 0.0));
        for (unsigned int i=0; i<labeled_nodes.size(); i++) {
            const int node_index = labeled_nodes[i];
            //assert(labels[node_index] != 0);
            if (labels[node_index][0] > 0) {
                const int labelCount=labels[node_index].size();
                for(int j=0;j<labelCount;j++){
                    const int label_index = labels[node_index][j] - 1;
                    y_l[label_index][i] = 1.0/double(labelCount);
                }
            }
        }

        return true;
    }
    bool compare(const queNode& a,const queNode& b){
        return a.count_labelNeibor>b.count_labelNeibor;
    }
    void LP::initialize_matrix_queue(){
        for(int i=0;i<U_;i++){
            const int label_neibor_count=norm_ul[i].size();
            prior_que.push_back(queNode(i,label_neibor_count));
        }
        assert(prior_que.size()==U_);
        std::sort(prior_que.begin(),prior_que.end(),compare);
        for(int i=0;i<U_;i++){
            int ULneiborSize=norm_uu[i].size();
            double ULneiborWeight_sum=0;
            for(int j=0;j<ULneiborSize;j++){  //set unlabeled neibor's weight to 0, and put those weight to labeled neibor
                ULneiborWeight_sum+=norm_uu[i][j].weight;
                norm_uu[i][j].weight=0;
            }
            int LneiborSize=norm_ul[i].size();
            double magRatio=1.0/(1-ULneiborWeight_sum);
            for(int j=0;j<LneiborSize;j++){
                norm_ul[i][j].weight*=magRatio;
                double test=norm_ul[i][j].weight;
            }
        }
    }
   
    bool LP::train (const int max_iter)
    {
        try {
            if (N_ <= 0) throw std::runtime_error ("N is invalid.");
            if (L_ <= 0) throw std::runtime_error ("L is invalid.");
            if (U_ <= 0) throw std::runtime_error ("U is invalid.");
            if (eps_ <= 1.0e-200) throw std::runtime_error ("eps is too small.");
            if (N_ != L_ + U_)
                throw std::runtime_error ("matrix and labels are invalid.");
        } catch (const std::exception& e) {
            std::stringstream ss;
            ss << "LP::train() says: " << e.what();
            throw std::runtime_error (ss.str());
        }

        std::cout << "Number of nodes:              " << N_ << std::endl;
        std::cout << "Number of labeled nodes:      " << L_ << std::endl;
        std::cout << "Number of unlabeled nodes:    " << U_ << std::endl;
        std::cout << "eps:                          " << eps_ << std::endl;
        std::cout << "max iteration:                " << max_iter << std::endl;

        for (int iter = 1;iter<max_iter+1;iter++) {
            double err = 0.0;
            std::vector<std::vector<double> > y_ret(C_, std::vector<double>(U_, 0.0));
            if(iter==1){
                initialize_matrix_queue();
            }
            for(unsigned int i=0; i < U_; i++){
                const int node_idx=prior_que[i].index;
                //const int node_idx=i;
                for(unsigned int c=0; c<C_; c++){
                    const ssl_lprop::Array arry = norm_ul[node_idx];
                    const int edge_sz = arry.size();
                    for (int j=0; j<edge_sz; j++) {
                        const double w = arry[j].weight;
                        const int src_index = arry[j].node - 1;
                        const double y_u_w = y_l[c][node_index_v[src_index]];
                        if (y_u_w < 1.0e-200 || w < 1.0e-200) continue;
                        y_ret[c][node_idx] += w * y_u_w;
                    }
                    const ssl_lprop::Array arry1 = norm_uu[node_idx];
                    const int edge_sz1 = arry1.size();
                    for (int j=0; j<edge_sz1; j++) {
                        const double w = arry1[j].weight;
                        const int src_index = arry1[j].node - 1;
                        const double y_u_w = y_u[c][node_index_v[src_index]];
                        if (y_u_w < 1.0e-200 || w < 1.0e-200) continue;
                        y_ret[c][node_idx] += w * y_u_w;
                    }
                    err += y_ret[c][node_idx] > y_u[c][node_idx] ?
                    y_ret[c][node_idx]-y_u[c][node_idx] : y_u[c][node_idx]-y_ret[c][node_idx];
                }
                if(iter==1){
                    update_weight_queue(node_idx,i);
                }
            }
            
            if (iter % 2 == 1) std::cout << ".";
            if (iter % 140 == 0) {
                std::cout << std::endl;
                std::cout << "[iteration: " << iter << " times"
                    << ", error: " << err << "]" << std::endl;
            }
            y_u = y_ret;

            if (eps_ > err || iter > max_iter-1) {
                std::cout << std::endl << "done" << std::endl;
                std::cout << "iteration: " << iter << " times"
                    << ", error: " << err << std::endl;

                break;
            }
        }

        return true;
    }
void LP::update_weight_queue(const int node_idx,
                             const int idx_in_queue){        //update node_idx's neibor's weight and prior_que
        int neiborSize=norm_uu[node_idx].size();
        for(int i=0;i<neiborSize;i++){
            int neiborNode=norm_uu[node_idx][i].node-1;
            int neiborUidx=node_index_v[neiborNode];
            prior_que[neiborUidx].count_labelNeibor++;
            double pastWeightSum=0;
            for(int j=0;j<norm_uu[neiborUidx].size();j++){
                if((norm_uu[neiborUidx][j].node-1)==unlabeled_nodes[node_idx]||norm_uu[neiborUidx][j].weight!=0){   //update "labeled" neibors' weight and new labeled node's weight
                    pastWeightSum+=trans_norm_uu[neiborUidx][j].weight;
                }
            }
            for(int j=0;j<norm_ul[neiborUidx].size();j++){
                pastWeightSum+=trans_norm_ul[neiborUidx][j].weight;
            }
            for(int j=0;j<norm_uu[neiborUidx].size();j++){
                if((norm_uu[neiborUidx][j].node-1)==unlabeled_nodes[node_idx]||norm_uu[neiborUidx][j].weight!=0){
                    norm_uu[neiborUidx][j].weight=trans_norm_uu[neiborUidx][j].weight/pastWeightSum;   //magnify ratio=1/pastWeightSum
                }
            }
            for(int j=0;j<norm_ul[neiborUidx].size();j++){
                norm_ul[neiborUidx][j].weight=trans_norm_ul[neiborUidx][j].weight/pastWeightSum;
            }
            
        }
        if(idx_in_queue==prior_que.size()-1){
            sort(prior_que.begin(),prior_que.end(),compare);
        }
        else
            sort(prior_que.begin()+idx_in_queue+1,prior_que.end(),compare);
    }
bool LP::write (const char* filename,
                const unsigned int prec)
{
  //  std::ofstream ofs(filename);
  const std::string label_filename(std::string(filename) + ".result");
  const std::string weight_filename(std::string(filename) + ".weight");
  std::ofstream ofs(weight_filename.c_str());
  ofs.setf(std::ios::fixed, std::ios::floatfield);
  ofs.precision(prec);
  for (int i=0; i<N_; i++) {
    if (labels[i][0] >= 0) {
      ofs << i+1<<"::"<<"L: ";
      for (int c=0; c<C_; c++) {
        if (c!=0) ofs << " ";
        ofs << y_l[c][node_index_v[i]];
      }
      ofs << std::endl;
    } else {
      ofs << i+1<<"::"<<"U: ";
      for (int c=0; c<C_; c++) {
        if (c!=0) ofs << " ";
        ofs << y_u[c][node_index_v[i]];
      }
      ofs << std::endl;
    }
  }
  ofs.close();
  ofs.clear();
  ofs.open(label_filename.c_str());
  ofs.setf(std::ios::fixed, std::ios::floatfield);
  ofs.precision(prec);
  for (int i=0; i<N_; i++) {
    int argmax = 0;
    double argmax_val = -1.0;
    for (int c=0; c<C_; c++) {
      if (labels[i][0] >= 0) {
        for (int c=0; c<C_; c++) {
          if (argmax_val < y_l[c][node_index_v[i]]) {
            argmax_val = y_l[c][node_index_v[i]];
            argmax = c;
          }
        }
      } else {
        for (int c=0; c<C_; c++) {
          if (argmax_val < y_u[c][node_index_v[i]]) {
            argmax_val = y_u[c][node_index_v[i]];
            argmax = c;
          }
        }
      }
    }
    ofs << i+1<<"::"<<argmax+1 << std::endl;
  }

  ofs.close();

  return true;
}
void LP::show (const unsigned int prec)
{
  { // bar
    const int kline_sz = 70;
    std::stringstream mkline;
    for (int i = 0; i < kline_sz; i++) mkline << "=";
    std::cout << mkline.str() << std::endl;
  }
  std::stringstream ss;
  ss.setf(std::ios::fixed, std::ios::floatfield);
  ss.precision(prec);

  for (int i=0; i<N_; i++) {
    if (labels[i][0]>= 0) {
      ss << "L: ";
      for (int c=0; c<C_; c++) {
        if (c!=0) ss << " ";
        ss << y_l[c][node_index_v[i]];
      }
      ss << std::endl;
    } else {
      ss << "U: ";
      for (int c=0; c<C_; c++) {
        if (c!=0) ss << " ";
        ss << y_u[c][node_index_v[i]];
      }
      ss << std::endl;
    }
  }
  std::cout << ss.str();
  ss.str("");
  ss.clear();

  for (int i=0; i<N_; i++) {
    int argmax = 0;
    double argmax_val = -1.0;
    for (int c=0; c<C_; c++) {
      if (labels[i][0] >= 0) {
        for (int c=0; c<C_; c++) {
          if (argmax_val < y_l[c][node_index_v[i]]) {
            argmax_val = y_l[c][node_index_v[i]];
            argmax = c;
          }
        }
      } else {
        for (int c=0; c<C_; c++) {
          if (argmax_val < y_u[c][node_index_v[i]]) {
            argmax_val = y_u[c][node_index_v[i]];
            argmax = c;
          }
        }
      }
    }
    ss << argmax+1 << std::endl;
  }

  std::cout << ss.str();
}

} // end of graph namespace
