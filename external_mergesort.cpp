#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <algorithm>
#include <sstream>

using namespace std;

typedef long long ll;

ll seeks = 0;
ll transfers = 0;
ll mergePasses = 0;

struct MinHeapNode
{
 ll value;
 ll runIndex;
 MinHeapNode(ll v, ll ri) : value(v), runIndex(ri) {}
 bool operator>(const MinHeapNode &other) const
 {
  return value > other.value;
 }
};

void readBlock(ifstream &file, vector<ll> &block, ll blockSize , ll keysize)
{
 block.clear();
 ll value;
 ll count = 0;
 while (count < blockSize / sizeof(ll) && file >> value)
 {
  block.push_back(value);
  count++;
 }
 if (count > 0)
 {
  seeks++;
  transfers++;
 }
}

void writeBlock(ofstream &file, vector<ll> &block)
{
 for (ll value : block)
 {
  file << value << "\n";
 }
 seeks++;
 transfers++;
}

ll createInitialRuns(string inputFile, ll n,ll k, ll blockSize, ll m, vector<string> &runFiles)
{
 ifstream file(inputFile);
 ll runCount = 0;
 vector<ll> buffer;
 while (file)
 {
  readBlock(file, buffer, blockSize * m , k);
  if (buffer.empty())
   break;
  sort(buffer.begin(), buffer.end());
  string runFile = "run_" + to_string(runCount) + ".txt";
  ofstream run(runFile);
  writeBlock(run, buffer);
  runFiles.push_back(runFile);
  runCount++;
 }
 return runCount;
}

string mergeRuns(vector<string> &runFiles, ll blockSize, ll m, ll k)
{
 while (runFiles.size() > 1)
 {
  vector<string> newRuns;
  ll runIndex = 0;
  while (runIndex < runFiles.size())
  {
   vector<ifstream> inputs;
   priority_queue<MinHeapNode, vector<MinHeapNode>, greater<>> minHeap;
   vector<ll> buffer;
   vector<vector<ll>> buffers(m - 1);
   ll numRuns = min(m - 1, (ll)runFiles.size() - runIndex);
   for (ll i = 0; i < numRuns; i++)
   {
    inputs.emplace_back(runFiles[runIndex + i]);
    readBlock(inputs[i], buffers[i], blockSize,k);
    if (!buffers[i].empty())
    {
     minHeap.emplace(buffers[i][0], i);
     buffers[i].erase(buffers[i].begin());
    }
   }
   string mergedRunFile = "merge_" + to_string(mergePasses) + "_" + to_string(runIndex) + ".txt";
   ofstream output(mergedRunFile);
   while (!minHeap.empty())
   {
    MinHeapNode minNode = minHeap.top();
    minHeap.pop();
    output << minNode.value << "\n";
    transfers++;
    ll runIdx = minNode.runIndex;
    if (buffers[runIdx].empty())
    {
     readBlock(inputs[runIdx], buffers[runIdx], blockSize,k);
    }
    if (!buffers[runIdx].empty())
    {
     minHeap.emplace(buffers[runIdx][0], runIdx);
     buffers[runIdx].erase(buffers[runIdx].begin());
    }
   }
   newRuns.push_back(mergedRunFile);
   runIndex += numRuns;
  }
  runFiles = newRuns;
  mergePasses++;
 }
 return runFiles[0];
}

int main(int argc, char *argv[])
{
 if (argc != 6)
 {
  cerr << "Usage: ./program-name input-file.txt n k b m\n";
  return 1;
 }
 string inputFile = argv[1];
 ll n = stoi(argv[2]);
 ll k = stoi(argv[3]);
 ll b = stoi(argv[4]);
 ll m = stoi(argv[5]);
 vector<string> runFiles;
 ll runCount = createInitialRuns(inputFile, n,k, b, m, runFiles);
 cout << "Initial runs: " << runCount << "\n";
 cout << "Disk seeks: " <<seeks << " Transfers: " <<transfers << "\n";
 string sortedFile = mergeRuns(runFiles, b, m,k);
 cout << "Sorted file: " << sortedFile << "\n";
 cout << "Total merge passes:" << mergePasses << "\n";
 cout << "Total disk seeks: " <<seeks << " Total transfers: " << transfers << "\n";
 return 0;
}
