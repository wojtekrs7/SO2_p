#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <chrono>
#include <memory>
#include <atomic>
#include <cstdlib>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"

int num_philosophers;
std::atomic<bool> running{ false };

void print_colored(const std::string& message, const std::string& color = RESET) {
    std::cout << color << message << RESET;
}

//Mutex implementation
class Mutex {
private:
    std::atomic<bool> locked{ false };  // Flag indicating whether the mutex is locked

public:
    void lock() {
        bool expected = false;
        while (!locked.compare_exchange_strong(expected, true)) {
            expected = false;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    void unlock() {
        locked.store(false);
    }
};

// Waiter class 
class Waiter {
private:
    Mutex mtx;
    std::vector<bool> fork_available;

public:
    // Initialize all forks as available
    Waiter(int num_forks) : fork_available(num_forks, true) {}

    // Philosopher requests permission to eat
    void request_to_eat(int left, int right) {
        while (true) {
            mtx.lock();
            if (fork_available[left] && fork_available[right]) {
                fork_available[left] = false;
                fork_available[right] = false;
                mtx.unlock();
                break;
            }
            mtx.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(1)); // busy wait
        }
    }

    // Philosopher notifies waiter after eatings
    void done_eating(int left, int right) {
        mtx.lock();
        fork_available[left] = true;
        fork_available[right] = true;
        mtx.unlock();
    }
};

std::unique_ptr<Waiter> waiter;             // Single waiter instance
std::vector<std::thread> philosophers;      // Philosopher threads

// Philosopher's behavior
void philosopher(int index) {
    int left_fork = index;
    int right_fork = (index + 1) % num_philosophers;

    while (running) {
        // Philosopher is thinking
        print_colored("[THINKING]->[No." + std::to_string(index) + "]\n", YELLOW);
        std::this_thread::sleep_for(std::chrono::seconds(rand() % 3 + 1));

        // Request permission from waiter
        waiter->request_to_eat(left_fork, right_fork);

        // Philosopher is eating
        print_colored("[EATING]->[No." + std::to_string(index) + "]\n", GREEN);
        std::this_thread::sleep_for(std::chrono::seconds(rand() % 3 + 1));

        // Notify waiter that philosopher is done eating
        waiter->done_eating(left_fork, right_fork);
        print_colored("[FINISHED]->[No." + std::to_string(index) + "]\n", RED);
    }
}

// Start philosopher threads
void start_philosophers() {
    philosophers.clear();
    for (int i = 0; i < num_philosophers; i++) {
        philosophers.emplace_back(philosopher, i);
    }
    running = true;
}

// Stop philosopher threads
void stop_philosophers() {
    running = false;
    for (auto& p : philosophers) {
        if (p.joinable()) {
            p.join();
        }
    }
    philosophers.clear();
}

int main() {
    std::cout << "Enter the number of philosophers: ";
    std::cin >> num_philosophers;

    if (num_philosophers < 2) {
        std::cerr << "Number of philosophers must be at least 2.\n";
        return 1;
    }

    srand(time(nullptr)); // Initialize random seed

    waiter = std::make_unique<Waiter>(num_philosophers);

    std::string command;
    std::cout << "Dining Philosophers Simulation\n";
    std::cout << "Commands:\ns -> start\np -> pause\n";

    while (true) {
        std::cout << "> ";
        std::cin >> command;

        if (command == "s") {
            std::cout << "Starting philosophers...\n";
            running = true;
            start_philosophers();
        }
        else if (command == "p") {
            std::cout << "Stopping philosophers...\n";
            stop_philosophers();
            break;
        }
        else {
            std::cout << "Unknown command. Use: s, p, q\n";
        }
    }

    return 0;
}
