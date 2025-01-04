#pragma once

#include "Walnut/Image.h"
#include "Walnut/Camera.h"
#include "raytracer/Ray.h"
#include "Scene.h"
#include <memory>

#include <glm/glm.hpp>

 
class Renderer {

public:
    struct Settings {
        bool Accumulate = true;
    };


    Renderer() = default;

    void OnResize(u_int32_t width, uint32_t height);
    void Render(const Scene& scene, const Camera& camera);
    std::shared_ptr<Walnut::Image> GetFinalImage() const {return m_FinalImage; };

    void ResetFrameIndex() { m_FrameIndex = 1;};
    Settings& GetSettings(){return m_Settings;}

    ~Renderer() {
        delete[] m_AccumulationData;
        delete[] m_ImageData;
    }

private:

    glm::vec4 PerPixel(uint32_t x, uint32_t y); //Raygen
    HitPayLoad TraceRay(const Ray& ray);

    const Scene* m_ActiveScene = nullptr;
    const Camera* m_ActiveCamera = nullptr;

    std::shared_ptr<Walnut::Image> m_FinalImage;
    u_int32_t* m_ImageData = nullptr;
    glm::vec4* m_AccumulationData = nullptr;

    uint32_t m_FrameIndex = 1;
    Settings m_Settings;

    std::vector<uint32_t> m_ImageHorizontalIterator, m_ImageVerticalIterator;

};