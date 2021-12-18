#ifndef RENDERER_H
#define RENDERER_H

#include <chrono>

class Renderer {
   public:
    Renderer();
    Renderer(Renderer &&) = delete;
    Renderer(const Renderer &) = delete;
    Renderer &operator=(Renderer &&) = delete;
    Renderer &operator=(const Renderer &) = delete;
    ~Renderer() = default;

    void render() const;

   private:
    std::chrono::steady_clock::time_point begin;
};

#endif  // !RENDERER_H
