/**
 * @file backtrack.h
 *
 */

#ifndef BACKTRACK_H_
#define BACKTRACK_H_

#include "candidate_set.h"
#include "common.h"
#include "graph.h"
#include <map>

class Backtrack {
 public:
  Backtrack();
  ~Backtrack();

  void PrintAllMatches(const Graph &data, const Graph &query, const CandidateSet &cs);
  void printEmbedding(const std::vector<Vertex> &embedding);
  bool isEmbedding(const std::vector<Vertex> &embedding, const Graph &data, const Graph &query);
  void buildDAG(const Graph &G, const Graph &q);  
  int C_ini(const Graph &G, const Graph &q, Vertex u);
  void backtracking(const Graph &data, const Graph &query, const CandidateSet &cs);
  
  void transposeDAG(std::vector<Vertex>* &adj, std::vector<Vertex>* &transpose);
  void printDAG(std::vector<Vertex>* &adj);

  Vertex extendable(const Graph &data, const CandidateSet &cs);
  std::vector<Vertex> C_m(Vertex u, const Graph &data, const CandidateSet &cs);

  std::vector<Vertex> N_u(Vertex u, Vertex v_p, const Graph &data, const CandidateSet &cs);
  static std::vector<std::pair<Vertex, Vertex>> M; // (partial) embedding. vector가 아니라 배열로 하는 게 나을 수도?
  static std::map<Vertex, Vertex> M_dict; // M(u) 같은 연산이 필요하기 때문에 ditctionary 버전도 만듦. 
                                          // 일단 빠르게 완성하기 위해 M, M_dict 두개를 같이 쓰고, 나중에 가능하다면 hash table만 남기면 될 듯.
                                          // C++에 익숙하지 않아서 std::map을 썼는데 혹시 다른 자료구조가 더 좋은 게 있으면 바꿔주셔도 좋습니다.
  static std::map<Vertex, bool> visited_cs;

 private:
  Vertex root;
  size_t q_size;
  std::vector<Vertex>* q_D; //query->DAG. adjacency list 형태로 저장
  std::vector<Vertex>* q_D_1; //q_D에서 모든 edge를 반대로 reverse한 DAG, adjacency list 형태로 저장
};

#endif  // BACKTRACK_H_
