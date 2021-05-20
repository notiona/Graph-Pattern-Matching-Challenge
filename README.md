# Graph Pattern Matching Challenge
## Compile and Execute 
### main program
```
mkdir build
cd build
cmake ..
make
./main/program <data graph file> <query graph file> <candidate set file>
```
### Sample Data, Query, CS 파일 돌리는 코드!(/build directory 안에서)
```
mkdir build
cd build
cmake ..
make
./main/program ../data/exdata.igraph ../query/exdata.igraph ../candidate_set/excandidate.cs
```

### executable program that outputs a candidate set
```
./executable/filter_vertices <data graph file> <query graph file>
```
### Sample Data, Query 파일 돌리는 코드!
```
./executable/filter_vertices ./data/exdata.igraph ./query/exquery.igraph
```

### References
[1] Myoungji Han, Hyunjoon Kim, Geonmo Gu, Kunsoo Park, and Wook-Shin Han. 2019. Efficient Subgraph Matching: Harmonizing Dynamic Programming, Adaptive Matching Order, and Failing Set Together. In Proceedings of the 2019 International Conference on Management of Data (SIGMOD '19). Association for Computing Machinery, New York, NY, USA, 1429–1446. DOI:https://doi.org/10.1145/3299869.3319880
