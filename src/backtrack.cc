/**
 * @file backtrack.cc
 *
 */

#include "backtrack.h"

Backtrack::Backtrack() {}
Backtrack::~Backtrack() {}

// declaration of static M, M_dict, visited_query, visited_cs
std::vector<std::pair<Vertex, Vertex>> Backtrack::M;
std::map<Vertex, Vertex> Backtrack::M_dict;
std::map<Vertex, bool> Backtrack::visited_cs;

void Backtrack::PrintAllMatches(const Graph &data, const Graph &query, const CandidateSet &cs) {
  
  printf("t %zu\n", query.GetNumVertices()); // Output format

  buildDAG(data, query);

  for(size_t i = 0; i < q_size; i++){ // init visited_cs;
    for(size_t j = 0; j < cs.GetCandidateSize(i); j++){
      Vertex candidate_set_vertex = cs.GetCandidate(i, j);
      visited_cs.insert(std::pair<Vertex, bool>(candidate_set_vertex, false)); // cs에 중복으로 나오는 건 알아서 걸러짐
    }
  }

  backtracking(data, query, cs);
}

/**
 * @brief Prints embedding to stdout.
 * @param embedding complete embedding.
 * @return None
 */
void Backtrack::printEmbedding(const std::vector<Vertex> &embedding){
  printf("a");
  for (int x : embedding){
    printf(" %d",x);
  }
  printf("\n");
}

/**
 * @brief Return true if given embedding satisfies embedding conditions, otherwise return false.
 * @param embedding embedding to be tested.
 * @param data data graph.
 * @param query query graph.
 * @param cs candidate set.
 * @return bool
 */
bool Backtrack::isEmbedding(const std::vector<Vertex> &embedding, const Graph &data, const Graph &query){

  // condition 3: if exist edge in query, edge must exist in embedding
  for (auto it = embedding.begin(); it != embedding.end(); ++it) {
    int vertex_query = std::distance(embedding.begin(), it);
    size_t offset = query.GetNeighborStartOffset(vertex_query);
    
    while (offset < query.GetNeighborEndOffset(vertex_query)){  
      int vertex_neighbor = query.GetNeighbor(offset);
      
      if (vertex_query < vertex_neighbor){
        if(!data.IsNeighbor(embedding[vertex_query], embedding[vertex_neighbor])){
          //cout << "Condition 3 Not Satisfied!\n";
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
 */
void Backtrack::buildDAG(const Graph &G, const Graph &q){
  // Select root
  // root = argmin ( |C_ini(u)|/degree(u) )
  // 하지만 deg = 0이 될 수 있으므로 역수의 최대값을 구하는 문제로 전환
  q_size = q.GetNumVertices();
  size_t deg = q.GetDegree(0);
  double size_C_ini = C_ini(G, q, 0);
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
  // BFS에서 same level의 경우에 순서를 정하는 것은 아직 따로 구현하지 않았음 (논문에는 있지만)
  std::vector<bool> visited_query(q_size);
  visited_query[root] = true;

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
      if (visited_query[v] == false) {
        visited_query[v] = true;
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
}
  
/**
 * @brief function to get Transpose of a graph taking adjacency
 * @param adj given graph to reverse
 * @param transpose transposed graph
 * */
void Backtrack::transposeDAG(std::vector<Vertex>* &adj, std::vector<Vertex>* &transpose){
    // traverse the adjacency list of given graph and
    // for each edge (u, v) add an edge (v, u) in the
    // transpose graph's adjacency list
    for (size_t i = 0; i < q_size; i++)
        for (size_t j = 0; j < adj[i].size(); j++)
            transpose[adj[i][j]].push_back(i); 
}

/**
 * @brief get the size of initial CS of query node u.
 * @details v is in C_ini(u), if label(v)=label(u) && degree(v)>=degree(u)
 *          v: node of data graph, u: node of query graph
 * @param G data graph
 * @param q query graph
 * @param u node of query graph
 * @return size of C_ini(u)
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
 * @brief Backtrack. See <Algorithm 2> in the paper.
 * @details M, visited[]는 class 변수 (parameter X)
 * @return none
 */
void Backtrack::backtracking(const Graph &data, const Graph &query, const CandidateSet &cs){

  if(M.size() == q_size){    
    // std::vector<std::pair<Vertex, Vertex>> -> vector<Vertex> 형으로 변환
    std::vector <Vertex> M_vector (query.GetNumVertices());
    for (size_t i = 0; i < query.GetNumVertices(); ++i){
      // 자동으로 query vertex 순서대로 들어간다
      M_vector[ M[i].first ] = M[i].second;
    }
    // do printEmbedding()
    if (isEmbedding(M_vector, data, query)){
      printEmbedding(M_vector);
    } 
    //M_dict.erase(M.back().first);
    M.pop_back();
  }

  else if (M.size() == 0){
    for(size_t i = 0; i < cs.GetCandidateSize(root); ++i){
      Vertex v = cs.GetCandidate(root, i);
      // do M <- (root, v); 
      std::pair<Vertex, Vertex> root_candidate;
      root_candidate.first = root;
      root_candidate.second = v;
      M.push_back(root_candidate);
      M_dict[root] = v;

      visited_cs[v] = true;
      backtracking(data, query, cs);
      visited_cs[v] = false;
    }
  }

  else{
    Vertex u = extendable(data, cs);
    std::vector<Vertex> C_m_ = C_m(u, data, cs);

    for(size_t j = 0; j < C_m_.size(); ++j){
      Vertex v = C_m_[j];
      if (!visited_cs[v]){
        // do M' <- M U (u,v);
        std::pair<Vertex, Vertex> u_candidate;
        u_candidate.first = u;
        u_candidate.second = v;
        M.push_back(u_candidate);
        M_dict[u] = v;

        visited_cs[v] = true;
        backtracking(data, query, cs);
        visited_cs[v] = false;
      }
    }
    //M_dict.erase(M.back().first);
    M.pop_back();
  }  
}

Vertex Backtrack::extendable(const Graph &data, const CandidateSet &cs){
  // Condition: unvisited query vertex u is extendable if all parents of u are matched in M

  std::vector<Vertex> M_first; // query의 vertex 중 M에 매핑된 것들
  for (auto it = M.begin(); it != M.end(); ++it){
    M_first.push_back((*it).first);
  }

  // M에 매핑된 query vertex들에 대해서, child의 합집합을 구한다
  std::set<Vertex> child = std::set<Vertex>();
  for (auto iter : M_first){
    for (auto u : q_D[(iter)]){
      child.insert(u);
    }
  }
  
  //이미 M 안에 있는 것은 뺀다
  for (auto iter : M_first){ 
    child.erase(iter);
  }

  // child에 있는 애들 중에서 parent가 모두 M에 mapping돼있는 애들만 걸러서 extendable_set에 넣는다
  std::set<Vertex> extendable_set = std::set<Vertex>();
  for (auto child_ : child){
    bool valid = true; //parent가 다 M에 매핑돼있는지 확인하는 플래그
    for (auto parent : q_D_1[child_]){
      if( *find(M_first.begin(), M_first.end(), parent) != parent ){ 
        valid = false;
        break;
      } 
    }
    if (valid){
      extendable_set.insert(child_);
    }
  }
  
  // set to vector
  std::vector <Vertex> extendable_vector;
  std::copy(extendable_set.begin(), extendable_set.end(), std::back_inserter(extendable_vector));

  // extendable vertex 중 |C_m(u)|가 최소인 vertex 선택
  size_t min = 100000000;
  Vertex min_extendable_vertex = 100000000;
  for (auto extendable_vertex : extendable_vector) {
    size_t C_m_value = C_m(extendable_vertex, data, cs).size();
    if (min > C_m_value){
      min_extendable_vertex = extendable_vertex;
    }
  }
  
  return min_extendable_vertex;
}

/**
 * @brief returns C_m(u), set of extendable candidates of u regarding partial embedding M
 * @details see <ch15. graph_pattern_matching> p19.
 */
std::vector<Vertex> Backtrack::C_m(Vertex u, const Graph &data, const CandidateSet &cs){

  std::vector<Vertex> result;
  Vertex u_p = q_D_1[u][0];  // 반복적으로 intersection하기 위해서 맨 첫번째 (i=0)일 때는 미리 구해놓음
  result = N_u(u, M_dict[u_p], data, cs);

  for(size_t i = 1; i < q_D_1[u].size(); ++i){
    u_p = q_D_1[u][i];
    std::vector<Vertex> N = N_u(u, M_dict[u_p], data, cs);
    std::vector<Vertex> tmp;
    std::set_intersection(result.begin(), result.end(),
                          N.begin(), N.end(),
                          std::back_inserter(tmp));
    result = tmp;
  }
  return result;
}

/**
 * @brief N sup(u_p) sub(u)를 구현. (see <ch15. graph_pattern_matching> p19.)
 * @param u vertex in query
 * @param v_p = M(u_p)
 * @return set of vertices v, those adjacent to v_p in G such taht v in C(u)
 */
std::vector<Vertex> Backtrack::N_u(Vertex u, Vertex v_p, const Graph &data, const CandidateSet &cs){
  std::vector<Vertex> result;
  for(size_t i = 0; i < cs.GetCandidateSize(u); ++i){
    Vertex v =cs.GetCandidate(u, i);
    if (data.IsNeighbor(v, v_p)){
      result.push_back(v);
    }
  }
  std::sort(result.begin(), result.end()); // intersection 작업을 위해 sorting해놓음
  return result;
}