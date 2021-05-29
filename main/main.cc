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
  bool verbose = true; // true: prints procedure and check time.  false: for submission

  std::string data_file_name = argv[1];
  std::string query_file_name = argv[2];
  std::string candidate_set_file_name = argv[3];

  if (verbose){
    cout<<"data: "<<data_file_name<<endl;
    cout<<"query: "<<query_file_name<<endl;
    cout<<"cs: "<<candidate_set_file_name<<endl;
  }
 
  clock_t start;
  if (verbose) start = clock();
  Graph data(data_file_name);
  if (verbose) cout<<"\ndata: "<<(double) (clock() - start)/1000<<"s"<<endl; start = clock();
  Graph query(query_file_name, true);
  if (verbose) cout<<"query: "<<(double) (clock() - start)/1000<<"s"<<endl; start = clock();
  CandidateSet candidate_set(candidate_set_file_name);
  if (verbose) cout<<"candidate_set: "<<(double) (clock() - start)/1000<<"s"<<endl; 

  Backtrack backtrack;
  if (verbose) cout<<"\nstart backtrack.PrintAllMatches...\n";
  backtrack.PrintAllMatches(data, query, candidate_set);

  return EXIT_SUCCESS;
}
