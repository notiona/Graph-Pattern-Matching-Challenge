/**
 * @file main.cc
 *
 */

#include "backtrack.h"
#include "candidate_set.h"
#include "common.h"
#include "graph.h"

int main(int argc, char* argv[]) {
  if (argc < 4) {
    std::cerr << "Usage: ./program <data graph file> <query graph file> <candidate set file>\n";
    return EXIT_FAILURE;
  }

  std::string data_file_name = argv[1];
  std::string query_file_name = argv[2];
  std::string candidate_set_file_name = argv[3];

  std::cout<<"data: "<<data_file_name<<std::endl;
  std::cout<<"query: "<<query_file_name<<std::endl;
  std::cout<<"cs: "<<candidate_set_file_name<<std::endl;

  clock_t start = clock();
  Graph data(data_file_name);
  std::cout<<"\ndata: "<<(double) (clock() - start)/1000<<"s"<<std::endl; start = clock();
  Graph query(query_file_name, true);
  std::cout<<"query: "<<(double) (clock() - start)/1000<<"s"<<std::endl; start = clock();
  CandidateSet candidate_set(candidate_set_file_name);
  std::cout<<"candidate_set: "<<(double) (clock() - start)/1000<<"s"<<std::endl; 

  Backtrack backtrack;
  std::cout<<"\nstart backtrack.PrintAllMatches...\n";
  backtrack.PrintAllMatches(data, query, candidate_set);

  return EXIT_SUCCESS;
}
