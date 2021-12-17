#ifndef RENDERER_H
#define RENDERER_H

class Renderer {
   public:
    Renderer();
    Renderer(Renderer &&) = delete;
    Renderer(const Renderer &) = delete;
    Renderer &operator=(Renderer &&) = delete;
    Renderer &operator=(const Renderer &) = delete;
    ~Renderer() = default;

    void render() const;
};

#endif  // !RENDERER_H
