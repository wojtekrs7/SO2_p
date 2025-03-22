#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <chrono>
#include <memory>

#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"                                                                                                                                  

int num_philosophers;

class Mutex {
private:
    bool locked = false;  // Flag indicating whether the mutex is locked

public:
    void lock() {
        while (true) {
            // Check if the mutex is free
            if (!locked) {
                // Attempt to lock the mutex
                locked = true;
                break;
            }
            // Busy-waiting
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    void unlock() {
        // Unlock the mutex
        locked = false;
    }
};

std::vector<std::unique_ptr<Mutex>> forks;  // Vector of unique pointers to mutexes representing forks
std::vector<std::thread> philosophers; // Vector of threads representing the philosophers
std::atomic<bool> running{ false };

void print_colored(const std::string& message, const std::string& color = RESET) {
    std::cout << color << message << RESET;
}

void philosopher(int index) {
    int left_fork = index;
    int right_fork = (index + 1) % num_philosophers;

    while (running) {
        // Thinking
        print_colored("[THINKING]->[No." + std::to_string(index) + "]\n", YELLOW);
        std::this_thread::sleep_for(std::chrono::seconds(rand() % 3 + 1));

        // Picking up forks
        if (index % 2 == 0) {
            forks[left_fork]->lock();
            forks[right_fork]->lock();
        }
        else {
            forks[right_fork]->lock();
            forks[left_fork]->lock();
        }

        // Eating
        print_colored("[EATING]->[No." + std::to_string(index) + "]\n", GREEN);
        std::this_thread::sleep_for(std::chrono::seconds(rand() % 3 + 1));

        // Putting down forks
        forks[left_fork]->unlock();
        forks[right_fork]->unlock();
        print_colored("[FINISHED]->[No." + std::to_string(index) + "]\n", RED);
    }
}

void start_philosophers() {
    philosophers.clear();
    for (int i = 0; i < num_philosophers; i++) {
        philosophers.emplace_back(philosopher, i);
    }
    running = true;
}

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

    srand(time(nullptr));

    // Initialize forks as custom mutexes
    for (int i = 0; i < num_philosophers; i++) {
        forks.emplace_back(std::make_unique<Mutex>());
    }

    std::string command;
    std::cout << "Dining Philosophers Simulation" << std::endl;
    std::cout << "Commands: \ns -> start \np -> pause\nq -> quit" << std::endl;

    while (true) {
        std::cout << "> ";
        std::cin >> command;

        if (command == "s") {
            std::cout << "Starting philosophers..." << std::endl;
            running = true;
            start_philosophers();
        }
        else if (command == "p") {
            std::cout << "Stopping philosophers..." << std::endl;
            stop_philosophers();
        }
        else if (command == "q") {
            std::cout << "Exiting program." << std::endl;
            stop_philosophers();
            break;
        }
        else {
            std::cout << "Unknown command. Use: s, p, q" << std::endl;
        }
    }

    return 0;
}