/**
 * @file backtrack.cc
 *
 */

#include "backtrack.h"

Backtrack::Backtrack() {}
Backtrack::~Backtrack() {}
// declaration of static M(해줘야 에러 안 남)
std::vector<std::pair<Vertex, Vertex>> Backtrack::M;

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
  backtracking(data, query, cs);
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
  // deg = 0이 될 수 있으므로 역수의 최대값을 구하는 문제로 전환
  double max = deg / size_C_ini;
  Vertex max_idx = 0;
  for(size_t u = 1; u < q_size; ++u){
    deg = q.GetDegree(u);
    size_C_ini = C_ini(G, q, u);
    double val = deg / size_C_ini;
    if (max < val) {
      max = val;
      max_idx = u;
    }
  }
  root = max_idx;

  // Traverse query in BFS order
  // BFS에서 same level의 경우에 순서를 정하는 것은 아직 따로 구현하지 않았음(논문에는 있지만)
  initVisited(q_size);
  visited[root] = true;

  Vertex* visit_order = new Vertex[q_size];
  visit_order[root] = 0;

  q_D = new std::vector<Vertex>[q_size]; // query->DAG. adjacency list 형태로 저장
  q_D_1 = new std::vector<Vertex>[q_size]; // q_D에서 모든 edge를 반대로 reverse한 DAG, adjacency list 형태로 저장
  std::queue<int> queue;
  queue.push(root);

  for(size_t i = q.GetNeighborStartOffset(root); i < q.GetNeighborEndOffset(root); ++i){
    Vertex v = q.GetNeighbor(i); // vertex v adjacent to u, 지금은 vertex 오름차순 순으로 들어온다
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

  // q_D_1 reversal
  transposeDAG(q_D, q_D_1);

  //DAG adjacency list 출력 (확인용)
  printDAG(q_D);
  printDAG(q_D_1);
}
  
// function to get Transpose of a graph taking adjacency
// list of given graph and that of Transpose graph
void Backtrack::transposeDAG(std::vector<Vertex>* &adj, std::vector<Vertex>* &transpose){
    // traverse the adjacency list of given graph and
    // for each edge (u, v) add an edge (v, u) in the
    // transpose graph's adjacency list
    for (int i = 0; i < q_size; i++)
        for (int j = 0; j < adj[i].size(); j++)
            transpose[adj[i][j]].push_back(i); 
}

// function to print adjacency list of a graph
void Backtrack::printDAG(std::vector<Vertex>* &adj){
    for (int i = 0; i < q_size; i++) {
        std::cout << i << "--> ";
        for (int j = 0; j < adj[i].size(); j++)
            std::cout << adj[i][j] << "  ";
        std::cout << "\n";
    }
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
    if (G.GetLabel(v) == q.GetLabel(u) && G.GetDegree(v) >= q.GetDegree(u)){
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
void Backtrack::backtracking(const Graph &data, const Graph &query, const CandidateSet &cs){
  if(M.size() == q_size){
    // buildDAG과 backtracking 과정에서 isEmbedding의 condition1, condition2가 이미 만족되었다면
    // isEmbedding(M, query)만으로 판별가능
    // 지금은 확실한 확인을 위해 backtracking의 인수로 data, query graph 받아와야 할 것 같아요
    
    // std::vector<std::pair<Vertex, Vertex>>-> vector<Vertex> 형으로 변환
    std::vector <Vertex> M_vector (query.GetNumVertices());
    for (size_t i = 0; i < query.GetNumVertices(); ++i){
      // 자동으로 query vertex 순서대로 들어간다
      M_vector[ M[i].first ] = M[i].second;
    }
    // do printEmbedding()
    if (isEmbedding(M_vector, data, query)){
      printEmbedding(M_vector);
      // initialize M
      M.clear();
    }
  }
  else if (M.size() == 0){
    for(size_t i = 0; i < cs.GetCandidateSize(root); ++i){
      Vertex v = cs.GetCandidate(root, i);
      // do M <- (root, v); 
      std::pair<Vertex, Vertex> root_candidate;
      root_candidate.first = root;
      root_candidate.second = v;
      M.push_back(root_candidate);

      visited[v] = true;
      backtracking(data, query, cs);
      visited[v] = false;
    }
  }
  else{
    Vertex u = extendable();
      for(size_t j = 0; j < sizeof(C_m(u)); ++j){
        Vertex v = C_m(u)[j];
        if (!visited[v]){
          // do M' <- M U (u,v);
          std::pair<Vertex, Vertex> u_candidate;
          u_candidate.first = u;
          u_candidate.second = v;
          M.push_back(u_candidate);

          visited[v] = true;
          backtracking(data, query, cs);
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
  // unvisited query vertex u is extendable if all parents of u are matched in M 
  std::vector <Vertex> unvisited;
  for(size_t i = 0; i < q_size; i++) {
    if(visited[i]) {
      unvisited.push_back(i);
    }
  }
  std::vector <Vertex> extendable_vector;
  for (auto it = unvisited.begin(); it != unvisited.end(); ++it) {
    size_t unvisited_vertex = *it;
    // parent 있는지 확인 위해 q_D_1 필요
    std::vector<Vertex> M_first;
    for (auto jt = M.begin(); jt != M.end(); ++jt){
      M_first.push_back((*jt).first);
    }
    int counter = 0;
    for (Vertex parent : q_D_1[unvisited_vertex]){
      if(std::find(M_first.begin(), M_first.end(), parent) != M_first.end()) {
        // M_first contains parent
        counter++;
      } 
    }
    if(counter == q_size)
      extendable_vector.push_back(unvisited_vertex);
  }
  // extendable vertex 중 |C_m(u)|가 최소인 vertex 선택
  size_t min = SIZE_MAX;
  for (auto it = extendable_vector.begin(); it != extendable_vector.end(); ++it) {
    int query_index = std::distance(extendable_vector.begin(), it);
    Vertex extendable_vertex = *it;
    size_t C_m_value = sizeof(C_m(extendable_vertex)) / sizeof(Vertex);
    if (min > C_m_value)
      min = C_m_value;
  }
  Vertex u = min;  
  return u;
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