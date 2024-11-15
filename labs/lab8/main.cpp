#include <ctime>
#include <iostream>
#include <thread>
#include <vector>
#include <numeric> 
#include <chrono>  


void sumPart(const std::vector<int>& p, long& result, int start, int end) {
    result = std::accumulate(p.begin() + start, p.begin() + end, 0);
}

long seqSumArr(const std::vector<int>& p) {
    long sum = 0;
    for (int i = 0; i < p.size(); i++) {
        sum += p[i];
    }
    return sum;
}

long parSumArr(const std::vector<int>& p) {
    const int numThreads = 4; 
    const int chunkSize = p.size() / numThreads;

    std::vector<std::thread> threads;
    std::vector<long> partialSums(numThreads, 0);

    for (int i = 0; i < numThreads; ++i) {
        int start = i * chunkSize;
        int end = (i == numThreads - 1) ? p.size() : start + chunkSize;
        threads.emplace_back(sumPart, std::ref(p), std::ref(partialSums[i]), start, end);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    return std::accumulate(partialSums.begin(), partialSums.end(), 0);
}

int main(int argc, char **argv) {
    std::vector<int> obj(1000000, 10);
    
    auto startSeq = std::chrono::high_resolution_clock::now();
    long seqSum = seqSumArr(obj);
    auto endSeq = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> seqDuration = endSeq - startSeq;
 
    auto startPar = std::chrono::high_resolution_clock::now();
    long parSum = parSumArr(obj);
    auto endPar = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> parDuration = endPar - startPar;

    std::cout << "Sequential sum: " << seqSum << " (time: " << seqDuration.count() << "s)" << std::endl;
    std::cout << "Parallel sum: " << parSum << " (time: " << parDuration.count() << "s)" << std::endl;

    return 0;
}
