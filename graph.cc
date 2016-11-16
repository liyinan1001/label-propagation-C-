#include "graph.h"
#include"tfidfCreate.h"
//double ssl_lprop::neiborDistEps_=0;
namespace ssl_lprop {
   
void normalize(Matrix& trans_mat,
               Matrix& norm_trans_mat) {
  const int V = trans_mat.size();
  //std::vector<double> in_weight(V, 0.0);
  for (int i = 0; i < V; i++) {
    const int edges_sz = trans_mat[i].size();
    double edge_weight_sum = 0.0;
    for (int j = 0; j < edges_sz; j++) {
      edge_weight_sum += trans_mat[i][j].weight;
    }
    for (int j = 0; j < edges_sz; j++) {
     
      trans_mat[i][j].weight /= edge_weight_sum;
      // calc in_weight for row-normalize
     // const int in_node = trans_mat[i][j].node;
     // assert(in_node > 0);
    //  in_weight[in_node - 1] += trans_mat[i][j].weight;
    }
  }
  norm_trans_mat = Matrix(V);
  // calc row-normalized transition matrix
  for (int i = 0; i < V; i++) {
    const int edges_sz = trans_mat[i].size();
    for (int j=0; j<edges_sz; j++) {
     // const int dst_index = trans_mat[i][j].node - 1;
      //assert(dst_index >= 0);
     // const double w = trans_mat[i][j].weight / in_weight[dst_index];
      norm_trans_mat[i].push_back(trans_mat[i][j]);
    }
  }
}
int load_lab(Labels& lab,
             const std::string& input) {
    std::ifstream ifs(input.c_str(), std::ios::in);
    lab = Labels();
    int max_label = 0;
    while (! ifs.eof()) {
        std::string line;
        std::getline(ifs, line);
        if (line == "?"||line=="?\r") {
            lab.push_back(std::vector<int>{-1});
        } else {
            std::stringstream lstream(line);
            std::vector<int> labVec;
            while(!lstream.eof()){
                std::string label;
                lstream>>label;
                const int id = atoi(label.c_str());
                max_label = std::max(id, max_label);
                if(id!=0)
                    labVec.push_back(id);
            }
            lab.push_back(labVec);
        }
        ifs.peek();
    }

    return max_label;
}
void load_mat (Matrix& trans_mat,
               Matrix& norm_trans_mat)
               
{
  try {
    trans_mat = Matrix();
      for(unsigned int i=0;i<tfidf::vecTfIdf.size();i++){
          unsigned int elemnum = 0;
          for(unsigned int j=0;j<tfidf::vecTfIdf.size();j++){
              if(i!=j){
                  double cosSim=0;
                  for(int k=0;k<tfidf::ac.MAX_N;k++){         //计算第i个结点与其他所有结点的tfidf向量的cos相似度，找到其中大于阈值的，作为结点i的邻居
                      auto iterI=tfidf::vecTfIdf[i].find(k);   //扫描所有的模式串，如果i结点和j结点中都出现过模式串k，则计算i和j的cos相似度
                      auto iterJ=tfidf::vecTfIdf[j].find(k);
                      if(iterI!=tfidf::vecTfIdf[i].end()&&iterJ!=tfidf::vecTfIdf[j].end()){
                          cosSim+=(iterI->second*iterJ->second);
                      }
                      if(cosSim>=0) elemnum++;  //neiborDistEps_
                  }
              }
          }
          Array arry(elemnum, Edge());
          for(unsigned int j=0;j<tfidf::vecTfIdf.size();j++){
              int neiborCount=0;
              if(i!=j){
                  double cosSim=0;
                  for(int k=0;k<tfidf::ac.MAX_N;k++){         //计算第i个结点与其他所有结点的tfidf向量的cos相似度，找到其中大于阈值的，作为结点i的邻居
                      auto iterI=tfidf::vecTfIdf[i].find(k);   //扫描所有的模式串，如果i结点和j结点中都出现过模式串k，则计算i和j的cos相似度
                      auto iterJ=tfidf::vecTfIdf[j].find(k);
                      if(iterI!=tfidf::vecTfIdf[i].end()&&iterJ!=tfidf::vecTfIdf[j].end()){
                          cosSim+=(iterI->second*iterJ->second);
                      }
                      if(cosSim>=0){ //neiborDistEps_
                          int dst=j+1;  //vecTfIdf[i]代表的map就是newidx为i+1的结点的map
                          double w=cosSim;
                          arry[neiborCount]=Edge(dst,w);
                          neiborCount++;
                      }
                  }
              }
          }
          trans_mat.push_back(arry);
      }
  } catch (const std::exception& e) {
    std::stringstream ss;
    ss << "load_mat() says: " << e.what();
    throw std::runtime_error (ss.str());
  }
  // normalize weights
  normalize(trans_mat, norm_trans_mat);
}
void load_submatrix(const Matrix& mat,
                    Matrix& mat_uu,
                    Matrix& mat_ul,
                    const int U, const int L,
                    const std::vector<int> unlabeled_nodes,
                    const std::vector<int> labeled_nodes,
                    const Labels& lab)
{
  for (unsigned int i=0; i<unlabeled_nodes.size(); i++) {
    const int node_index = unlabeled_nodes[i];
    const int edges_sz = mat[node_index].size();
    for (int j=0; j<edges_sz; j++) {
      const int src_index = mat[node_index][j].node - 1;
      if (lab[src_index][0] >= 0) {
        mat_ul[i].push_back( mat[node_index][j] );
      } else {
        mat_uu[i].push_back( mat[node_index][j] );
      }
    }
  }
    
  
}

    
}

// end of namespace ssl_lprop
