/**
 * @file backtrack.cc
 *
 */

#include "backtrack.h"

Backtrack::Backtrack() {}
Backtrack::~Backtrack() {}

void Backtrack::PrintAllMatches(const Graph &data, const Graph &query, const CandidateSet &cs) {
  std::cout << "t " << query.GetNumVertices() << "\n";
  std::vector <Vertex> embedding (query.GetNumVertices());
  
  //embedding[0] = 0;
  //embedding[1] = 3;
  //embedding[2] = 4;
  //embedding[3] = 9;
  
  printEmbedding(embedding);
}

/**
 * @brief Prints embedding to stdout
 *
 * @param embedding complete embedding.
 * @return None
 */
void Backtrack::printEmbedding(const std::vector<Vertex> &embedding){
  std::cout << "a";
  for (int x : embedding)
    std::cout << " " << x;
  std::cout << "\n";
}

