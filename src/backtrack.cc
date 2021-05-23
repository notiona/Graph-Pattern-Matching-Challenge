/**
 * @file backtrack.cc
 *
 */

#include "backtrack.h"
#include <queue>

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

  buildDAG(data, query);

  initVisited(cs.GetCSSize());
  // backtracking(cs);
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

/**
 * @brief build DAG(q_D) from query
 * @details 메서드의 맨 아래에 만들어진 DAG(adjacency_list 형태)를 출력하는 코드가 주석처리 돼있습니다. 
 * @param G data graph
 * @param q query graph
 * @return none
 * @author Jinhyeong Kim
 */
void Backtrack::buildDAG(const Graph &G, const Graph &q){
  // Select root
  // root = argmin |C_ini(u)|/degree(u)
  q_size = q.GetNumVertices();
  size_t deg = q.GetDegree(0);
  double size_C_ini = C_ini(G, q, 0);
  double min = deg / size_C_ini;
  Vertex min_idx = 0;
  for(size_t u = 1; u < q_size; ++u){
    deg = q.GetDegree(u);
    size_C_ini = C_ini(G, q, u);
    double val = deg / size_C_ini;
    if (min > val) {
      min = val;
      min_idx = u;
    }
  }
  root = min_idx;

  // Traverse query in BFS order
  // BFS에서 same level의 경우에 순서를 정하는 것은 아직 따로 구현하지 않았음(논문에는 있지만)
  initVisited(q_size);
  visited[root] = true;

  Vertex* visit_order = new Vertex[q_size];
  visit_order[root] = 0;

  q_D = new std::vector<Vertex>[q_size]; // query->DAG. adjacency list 형태로 저장
  std::queue<int> queue;
  queue.push(root);

  for(size_t i = q.GetNeighborStartOffset(root); i < q.GetNeighborEndOffset(root); ++i){
    Vertex v = q.GetNeighbor(i); // vertex v adjacent to u
    q_D[root].push_back(v);
    }
  Vertex cnt = 0;
  while(!queue.empty()){
    Vertex u = queue.front(); queue.pop();
    for(size_t i = q.GetNeighborStartOffset(u); i < q.GetNeighborEndOffset(u); ++i){
      Vertex v = q.GetNeighbor(i);
      if (visited[v] == false) {
        visited[v] = true;
        queue.push(v);
        visit_order[v] = ++cnt;
      }
    }
  }

  // Direct all edges from earlier to later visited vertices.
  for(Vertex u = 0; u < static_cast<Vertex>(q_size); ++u){
    if(u != root){
      for(size_t i = q.GetNeighborStartOffset(u); i < q.GetNeighborEndOffset(u); ++i){
        Vertex v = q.GetNeighbor(i);
        if(visit_order[u] < visit_order[v]){
          q_D[u].push_back(v);
        }
      }
    }
  }

  // DAG adjacency list 출력 (확인용)
  // for(int i=0; i<q.GetNumVertices(); ++i){
  //   std::cout<<i;
  //   for(int j=0; j<q_D[i].size(); ++j){
  //     std::cout << " " << q_D[i][j];
  //   }
  //   std::cout<<"\n";
  // }
}

/**
 * @brief get the size of initial CS of query node u.
 * @details v is in C_ini(u), if label(v)=label(u) && degree(v)>=degree(u)
 *          v: node of data graph, u: node of query graph
 * @param G data graph
 * @param q query graph
 * @param u node of query graph
 * @return size of C_ini(u)
 * @author Jinhyeong Kim
 */
int Backtrack::C_ini(const Graph &G, const Graph &q, Vertex u){
  int cnt = 0;
  for(size_t v=0; v < G.GetNumVertices(); ++v){ 
    if (G.GetLabel(v) == q.GetLabel(v) && G.GetDegree(v) >= q.GetDegree(u)){
      ++cnt;
    }
  }
  return cnt;
}

/**
 * @brief Backtrack. <Algorithm 2> in the paper.
 * @details 구현 안 한 부분은 주석 처리해놓고서 논문의 pseudo code를 그대로 적었습니다.
   M, visited[]는 class 변수로 두어서 따로 parameter에 넣지 않았습니다. 물론 이게 제대로 작동하는 건지는 저도 모르니 마음대로 수정해주시면 감사합니다.
 * @param cs CandidateSet 
 * @return none
 * @author Jinhyeong Kim
 */
void Backtrack::backtracking(const CandidateSet &cs){
  if(sizeof(M) == q_size){
    // do printEmbedding()
  }
  else if (sizeof(M)==0){
    for(size_t i = 0; i < cs.GetCandidateSize(root); ++i){
      Vertex v = cs.GetCandidate(root, i);
      // do M <- (root, v); 
      visited[v] = true;
      backtracking(cs);
      visited[v] = false;

    }
  }
  else{
    Vertex u = extendable();
      for(size_t j = 0; j < sizeof(C_m(u)); ++j){
        Vertex v = C_m(u)[j];
        if (!visited[v]){
          // do M' <- M U (u,v);
          visited[v] = true;
          backtracking(cs);
          visited[v] = false;
        }
      }
  }  
}

/**
 * @author Jinhyeong Kim
 */
Vertex Backtrack::extendable(){
 // return a extendable vertex
 // (아래는 단지 에러 일으키지 않기 위해 넣은 것)
 Vertex tmp = 0;
 return tmp;
}

/**
 * @author Jinhyeong Kim
 */
Vertex* Backtrack::C_m(Vertex u){
  // return C_m(u), set of extendable candidates of u regarding partial embedding M
  // Vertex들의 배열을 반환
 // (아래는 단지 에러 일으키지 않기 위해 넣은 것)
  return &root;
}