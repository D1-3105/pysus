//
// Created by oleg on 20.04.25.
//

#include "ThreadHolder.h"
#include <csignal>

#include "iostream"
#include "Python.h"
#include "queue"
#include "map"
#include "condition_variable"

#define CONTROL_SIG SIGUSR2

struct InterConnection {
    std::unique_ptr<threader::ThreadSafeQueue<bool>> q;
    std::unique_ptr<threader::ThreadSafeQueue<bool>> cont;
};

std::map<pthread_t, InterConnection*>* interThreadCommunicator = nullptr;


void handle_control_signal(int signum) {
    std::cout << "Caught signal " << signum << " in thread " << pthread_self() << std::endl;

    if (interThreadCommunicator != nullptr) {
        auto it = interThreadCommunicator->find(pthread_self());
        if (it != interThreadCommunicator->end()) {
            InterConnection* ic = it->second;
            std::cout << "Sending ack..." << std::endl;
            ic->q->push(true);
            std::cout << "ack sent, waiting for continue sig..." << std::endl;
            ic->cont->wait_and_pop();
            std::cout << "continue recv!" << std::endl;
        } else {
            std::cerr << "No entry for thread in interThreadCommunicator" << std::endl;
        }
    }
}

void pyLaunchControlledThread(const std::string& pyString, bool& pingDone) {
    struct sigaction sa{};
    sa.sa_handler = handle_control_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    sigaction(CONTROL_SIG, &sa, nullptr);

    Py_Initialize();
    PyRun_SimpleString(pyString.c_str());
    Py_Finalize();

    pingDone = true;
}


void remoteLaunchWatchDog(const bool& flag, const pthread_t correspondingId) {
    bool suspended = false;
    while (!flag) {
        std::cout << "\t1 - Stop thread 0" << std::endl;
        std::cout << "\t2 - Run thread 0" << std:: endl;
        std::cout << "Enter command: " << std::endl;
        std::string command;
        std::cin >> command;
        InterConnection* ic;
        if(interThreadCommunicator != nullptr) {
             ic = interThreadCommunicator->find(correspondingId)->second;
        } else {
            std::cout << "interThreadCommunicator is not initialized yet!.." << std::endl;
        }

        auto c = atoi(command.c_str());
        switch (c) {
            case 1:
                if (suspended) {
                    std::cout << "Thread already suspended" << std::endl;
                    break;
                }
                pthread_kill(correspondingId, CONTROL_SIG);
                suspended = ic->q->wait_and_pop();
                std::cout << "Thread " << correspondingId << " suspended..." << std::endl;
                break;
            case 2:
                if (!suspended) {
                    std::cout << "Unable to resume thread: it is not suspended!" << std::endl;
                }
                ic->cont->push(true);
                suspended = false;
                std::cout << "Thread resumed!" << std::endl;
                break;
            default:
                continue;
        }
    }
}


threader::ThreadHolder::ThreadHolder(const std::string& pyString) {
    if (interThreadCommunicator == nullptr) {
        interThreadCommunicator = new std::map<pthread_t, InterConnection*>();
    }
    stop_ = false;
    py_thread_ = new std::thread(pyLaunchControlledThread, pyString, std::ref(stop_));
    (*interThreadCommunicator)[py_thread_->native_handle()] = new InterConnection{
            std::make_unique<threader::ThreadSafeQueue<bool>>(),
            std::make_unique<threader::ThreadSafeQueue<bool>>(),
    };
    remote_thread_ = new std::thread(remoteLaunchWatchDog, stop_, py_thread_->native_handle());
}

threader::ThreadHolder::~ThreadHolder() {
    if (py_thread_) {
        py_thread_->join();
    }
    stop_ = true;
    if (remote_thread_) {
        remote_thread_->join();
    }
    if (interThreadCommunicator != nullptr and py_thread_) {
        interThreadCommunicator->erase(py_thread_->native_handle());
    }
    delete py_thread_;
    delete remote_thread_;
}

void threader::ThreadHolder::callStop() {
    stop_ = true;
}


