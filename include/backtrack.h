/**
 * @file backtrack.h
 *
 */

#ifndef BACKTRACK_H_
#define BACKTRACK_H_

#include "candidate_set.h"
#include "common.h"
#include "graph.h"

class Backtrack {
 public:
  Backtrack();
  ~Backtrack();

  void PrintAllMatches(const Graph &data, const Graph &query, const CandidateSet &cs);
  void printEmbedding(const std::vector<Vertex> &embedding);
  bool isEmbedding(const std::vector<Vertex> &embedding, const Graph &data, const Graph &query);
  void buildDAG(const Graph &G, const Graph &q);
  int C_ini(const Graph &G, const Graph &q, Vertex u);
  void backtracking(const CandidateSet &cs);
  inline void initVisited(size_t n);

  Vertex extendable();
  Vertex* C_m(Vertex u);

  static std::vector<std::pair<Vertex, Vertex>> M; // (partial) embedding. vector가 아니라 배열로 하는 게 나을 수도?
  bool* visited; //buildDAG, backtracking에서 두 번 쓰임

 private:
  Vertex root;
  size_t q_size;
  std::vector<Vertex>* q_D; //query->DAG. adjacency list 형태로 저장
};

/**
 * @brief backtrack.cc에서 반복해서 사용되는 visited[]를 처음 선언할 때 false로 초기화시켜주는 함수
 * @param n size of array
 * @return none
 * @author Jinhyeong Kim
 */
inline void Backtrack::initVisited(size_t n){
  visited = new bool[n];
  for(int i=0; i<n; ++i){
    visited[i]=false;
  }
}

#endif  // BACKTRACK_H_
