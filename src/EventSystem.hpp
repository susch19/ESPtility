
#pragma once
#include <unordered_map>
#include <functional>

namespace eventsystem {
template <typename... Args>
class List {
 public:
  int execute(Args... args) {
    for (auto&& f : callbacks) {
      f(args...);
    }
    return callbacks.size();
  }

  void push_back(std::function<void(Args...)> func) {
    callbacks.push_back(func);
  }

  void operator+=(std::function<void(Args...)> func){
    callbacks.push_back(func);
  } 

  void operator-=(std::function<void(Args...)> func){
    callbacks.remove(func);
  } 

  int size(){
    return callbacks.size();
  };
  
 protected:
  std::vector<std::function<void(Args...)>> callbacks;
};

template <typename... Args>
class EventCallbackList {
 public:
  void onEvent(int id, std::function<void(Args...)> func) {
    callbackMap[id].push_back(func);
  }

  int execute(int id, Args... args) { return callbackMap[id].execute(args...); }

 protected:
  std::unordered_map<int, List<Args...>> callbackMap;
};
}  // namespace callback