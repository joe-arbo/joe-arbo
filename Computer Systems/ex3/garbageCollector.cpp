#include <stdio.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <queue>
#include <map>
#include <list>

class Graph 
{
public:
    std::map<int, bool> visited;
    std::map<int, std::list<int>> adj;
    std::vector<int> sizeVector;

    void addEdge(int v, int w);
    void DFS(int v, int &rootTotal);
};
  
void Graph::addEdge(int v, int w)
{
    //Add w to v’s list
    adj[v].push_back(w); 
}
  
void Graph::DFS(int v, int &rootTotal)
{
    //Mark the current node as visited
    visited[v] = true;
    rootTotal += sizeVector.at(v);
  
    //Recur for all the vertices adjacent to this vertex
    std::list<int>::iterator i;
    for (i = adj[v].begin(); i != adj[v].end(); ++i)
        if (!visited[*i])
            DFS(*i, rootTotal);
}

int main(int argc, char** argv)
{
    std::string line;
    std::vector<int> rawData;

    while (std::getline(std::cin, line))
    {
        std::istringstream stream(line);
        int n;
        while (stream >> n)
            rawData.push_back(n);
    }

    int numNodes = rawData.at(0);
    int numRoots = rawData.at(2);

    //collect the sizes
    std::vector<int> sizeVector;
    for (int i = 0; i < numNodes; ++i)
        sizeVector.push_back(rawData.at(i + 3));

    //collect the roots
    std::vector<int> rootsVector;
    for (int i = 0; i < numRoots; ++i)
        rootsVector.push_back(rawData.at(rawData.size() - numRoots + i));

    //graph the pairs
    Graph g;
    g.sizeVector = sizeVector;
    for (int i = 3 + numNodes; i < rawData.size() - numRoots; i = i + 2)
        g.addEdge(rawData.at(i), rawData.at(i + 1));

    //get total size
    int totalSize = 0;
    for (int i = 0; i < sizeVector.size(); ++i)
        totalSize += sizeVector.at(i);

    //print root totals
    int activeTotal = 0;
    std::vector<int> rootTotalVector;
    for (int i = 0; i < rootsVector.size(); ++i)
    {
        int rootTotal = 0;
        g.DFS(rootsVector.at(i), rootTotal);
        rootTotalVector.push_back(rootTotal);
        activeTotal += rootTotal;
    }

    std::cout << activeTotal << " " << totalSize - activeTotal << "\n";
    for (int i = 0; i < rootTotalVector.size(); ++i)
        std::cout << rootTotalVector.at(i) << std::endl;
}

