#pragma once

#include "Walnut/Image.h"
#include <memory>
#include <glm/glm.hpp>

class Renderer {

public:
    Renderer() = default;

    void OnResize(u_int32_t width, uint32_t height);
    void Render();
    std::shared_ptr<Walnut::Image> GetFinalImage() const {return m_FinalImage; };


private:

    glm::vec4 PerPixel(glm::vec2 coord); //coord \in [0,1]²

    std::shared_ptr<Walnut::Image> m_FinalImage;
    u_int32_t* m_ImageData = nullptr;
};