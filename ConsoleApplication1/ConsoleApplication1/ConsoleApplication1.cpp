#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <thread>
#include <chrono>
#include <atomic>
#include <future>

using namespace std;

void count_digits(const vector<string>& data, int start, int end, atomic<int>& even_count, atomic<int>& odd_count) {
    for (int i = start; i < end; ++i) {
        for (char ch : data[i]) {
            if ((ch - '0') % 2 == 0)
                even_count++;
            else
                odd_count++;
        }
    }
}

void measure_time(int num_threads) {
    // Р“РµРЅРµСЂР°С†РёСЏ РґР°РЅРЅС‹С…
    const int num_strings = 300;
    const int string_length = 200000;

    vector<string> data(num_strings);
    default_random_engine generator;
    uniform_int_distribution<int> distribution(0, 9);

    for (auto& str : data) {
        str.resize(string_length);
        for (int j = 0; j < string_length; ++j) {
            str[j] = '0' + distribution(generator);
        }
    }

    // РџРѕРґСЃС‡РµС‚ РІСЂРµРјРµРЅРё
    auto start_time = chrono::high_resolution_clock::now();

    atomic<int> even_count(0);
    atomic<int> odd_count(0);
    vector<future<void>> futures;

    int block_size = num_strings / num_threads;
    for (int i = 0; i < num_threads; ++i) {
        int start = i * block_size;
        int end = (i == num_threads - 1) ? num_strings : start + block_size;
        futures.push_back(async(launch::async, count_digits, ref(data), start, end, ref(even_count), ref(odd_count)));
    }

    for (auto& fut : futures) {
        fut.get();
    }

    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end_time - start_time;

    cout << "Threads: " << num_threads << ", Even: " << even_count << ", Odd: " << odd_count << ", Time: " << duration.count() << " seconds\n";
}

int main() {
    for (int n : {1, 2, 4, 10}) {
        measure_time(n);
    }
    return 0;
}