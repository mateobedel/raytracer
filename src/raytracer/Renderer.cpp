#include "raytracer/Renderer.h"
#include "Walnut/Random.h"

namespace Utils {

    static uint32_t ConvertToRGBA(const glm::vec4& color) {

        uint8_t r = (uint8_t)(color.r * 255.0f);
        uint8_t g = (uint8_t)(color.g * 255.0f);
        uint8_t b = (uint8_t)(color.b * 255.0f);
        uint8_t a = (uint8_t)(color.a * 255.0f);

        return (a << 24) | (b << 16) | (g << 8) | r;

    }
}

void Renderer::OnResize(u_int32_t width, uint32_t height) {

    if (m_FinalImage) {

        if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
            return;

        m_FinalImage->Resize(width, height);

    } else {
        m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
    }

    delete[] m_ImageData;
    m_ImageData = new uint32_t[width *  height];
}

void Renderer::Render() {

    for(uint32_t y = 0; y < m_FinalImage->GetHeight(); y++) {
        for(uint32_t x=0; x < m_FinalImage->GetWidth(); x++) {

            glm::vec2 coord = {
                (float)x / m_FinalImage->GetWidth(),
                (float)y / m_FinalImage->GetHeight()
            };
            coord = coord*2.0f - 1.0f; //remaping

            int i = x + y*m_FinalImage->GetWidth();
            glm::vec4 color = PerPixel(coord);
            color = glm::clamp(color, glm::vec4(0.0f),glm::vec4(1.0f));
            m_ImageData[i] =  Utils::ConvertToRGBA(color);        
        }
    }
            

    m_FinalImage->SetData(m_ImageData);
}

glm::vec4 Renderer::PerPixel(glm::vec2 coord) {

    glm::vec3 rayOrigin(0.0f, 0.0f, 1.0f);
    glm::vec3 rayDirection(coord.x, coord.y, -1.0f);

    //coord.x *= m_FinalImage->GetWidth()/(float)m_FinalImage->GetHeight();

    float radius = .5f;

    float a = glm::dot(rayDirection,rayDirection);
    float b = 2.0f * glm::dot(rayOrigin, rayDirection);
    float c = glm::dot(rayOrigin, rayOrigin) - radius*radius;

    float delta = b*b - 4.0f * a * c;

    if (delta < 0.0f)
        return glm::vec4(0,0,0,1);

    float t1 = (-b - glm::sqrt(delta)) / (2.0f * a); //Smaller solution

    glm::vec3 hitPoint = rayOrigin + rayDirection * t1;
    glm::vec3 normal = glm::normalize(hitPoint);

    glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));
    float d = glm::max(glm::dot(normal, -lightDir),0.0f);


    glm::vec3 sphereColor(1,0,1);
    sphereColor*=d;
    return glm::vec4(sphereColor,1.0f);

}
