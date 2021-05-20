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
  
  // change below values to create embedding
  embedding[0] = 1;
  embedding[1] = 2;
  embedding[2] = 4;
  embedding[3] = 9;
  
  if (isEmbedding(embedding, data, query))
    printEmbedding(embedding);
  
  std::cout << "is Embedding?: "<< (isEmbedding(embedding, data, query) ? "true" : "false") << "\n";
}

/**
 * @brief Prints embedding to stdout.
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

/**
 * @brief Return true if given embedding satisfies embedding conditions, otherwise return false.
 *
 * @param embedding embedding to be tested.
 * @param data data graph.
 * @param query query graph.
 * @param cs candidate set.
 * @return bool
 */
bool Backtrack::isEmbedding(const std::vector<Vertex> &embedding, const Graph &data, const Graph &query){

  // condition 1: injective
  std::set<Vertex> s(embedding.begin(), embedding.end()); // may want to use std::unordered_set for performance
  bool isInjective = s.size() == embedding.size();
  if (!isInjective){
    std::cout << "Condition 1 Not Satisfied!\n";
    return false;
  }
  // condition 2: embedding and data graph should have same labels
  for (auto it = embedding.begin(); it != embedding.end(); ++it) {
    int vertex_query = std::distance(embedding.begin(), it);  // equivalent to index of embedding
    int vertex_data = *it;
    int label_query = query.GetLabel(vertex_query);
    int label_data = data.GetLabel(vertex_data);
    if (label_query != label_data){
      std::cout << "Condition 2 Not Satisfied!\n";
      return false;
    }
  }
  // condition 3: if exist edge in query, edge must exist in embedding
  for (auto it = embedding.begin(); it != embedding.end(); ++it) {
    int vertex_query = std::distance(embedding.begin(), it);
    size_t offset = query.GetNeighborStartOffset(vertex_query);
    
    while (offset < query.GetNeighborEndOffset(vertex_query)){  
      int vertex_neighbor = query.GetNeighbor(offset);
      
      if (vertex_query < vertex_neighbor){
        if(!data.IsNeighbor(embedding[vertex_query], embedding[vertex_neighbor])){
          std::cout << "Condition 3 Not Satisfied!\n";
          return false;
        }
      }
      offset++;
    }
  }
  return true;
}