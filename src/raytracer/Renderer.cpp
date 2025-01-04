#include "raytracer/Renderer.h"
#include "Walnut/Random.h"

#include <execution>
#include <cstring>
#include <algorithm>

namespace Utils {

    static uint32_t ConvertToRGBA(const glm::vec4& color) {

        uint8_t r = (uint8_t)(color.r * 255.0f);
        uint8_t g = (uint8_t)(color.g * 255.0f);
        uint8_t b = (uint8_t)(color.b * 255.0f);
        uint8_t a = (uint8_t)(color.a * 255.0f);

        return (a << 24) | (b << 16) | (g << 8) | r;
    }

    static uint32_t PCG_Hash(uint32_t input) {
        uint32_t state = input  * 747796405u + 2891336453u;
        uint32_t word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
        return (word >> 22u) ^ word;
    }

    static float RandomFastFloat(uint32_t& seed) {
        seed = PCG_Hash(seed);
        return seed/(float)UINT32_MAX;
    }

    static glm::vec3 InUnitSphere(uint32_t& seed) {
        return glm::normalize(glm::vec3(
            RandomFastFloat(seed)*2.0f -1.0f,
            RandomFastFloat(seed)*2.0f -1.0f,
            RandomFastFloat(seed)*2.0f -1.0f)
        );
    }
}

void Renderer::OnResize(u_int32_t width, uint32_t height) {

    if (m_FinalImage) {

        if (m_FinalImage->GetWidth() == width && m_FinalImage->GetHeight() == height)
            return;

        m_FinalImage->Resize(width, height);
        ResetFrameIndex();

    } else {
        m_FinalImage = std::make_shared<Walnut::Image>(width, height, Walnut::ImageFormat::RGBA);
    }

    delete[] m_ImageData;
    m_ImageData = new uint32_t[width *  height];

    delete[] m_AccumulationData;
    m_AccumulationData = new glm::vec4[width *  height];

    m_ImageHorizontalIterator.resize(width);
    m_ImageVerticalIterator.resize(height);
    for(uint32_t i = 0; i < width; i++) m_ImageHorizontalIterator[i] = i;
    for(uint32_t i = 0; i < height; i++) m_ImageVerticalIterator[i] = i;
}

void Renderer::Render(const Scene& scene, const Camera& camera) {

    m_ActiveScene = &scene;
    m_ActiveCamera = &camera;

    if(m_FrameIndex == 1) 
        memset(m_AccumulationData, 0, m_FinalImage->GetWidth() * m_FinalImage->GetHeight() * sizeof(glm::vec4));

    std::for_each(std::execution::par, m_ImageVerticalIterator.begin(), m_ImageVerticalIterator.end(), [this](uint32_t y) {
        std::for_each(std::execution::par, m_ImageHorizontalIterator.begin(), m_ImageHorizontalIterator.end(), [this, y](uint32_t x) {

            glm::vec4 color = PerPixel(x,y);
            m_AccumulationData[x + y*m_FinalImage->GetWidth()] += color;

            glm::vec4 accumulateColor = m_AccumulationData[x + y*m_FinalImage->GetWidth()] / (float)m_FrameIndex;

            accumulateColor = glm::clamp(accumulateColor, glm::vec4(0.0f),glm::vec4(1.0f));
            m_ImageData[x + y*m_FinalImage->GetWidth()] =  Utils::ConvertToRGBA(accumulateColor); 

        });
    });

    m_FinalImage->SetData(m_ImageData);

    if (m_Settings.Accumulate)
        m_FrameIndex++;
    else
        m_FrameIndex = 1;

}

HitPayLoad Renderer::TraceRay(const Ray& ray) {

    Shape* closestShape = nullptr;
    float hitDistance = std::numeric_limits<float>::max();

    for(size_t i = 0; i < m_ActiveScene->Shapes.size(); i++) {

        Shape* shape = m_ActiveScene->Shapes[i];

        float intersectionT;
        bool doesIntersect = shape->intersect(ray, intersectionT);

        if (!doesIntersect) continue;

        if (intersectionT < hitDistance) {
            closestShape = m_ActiveScene->Shapes[i];
            hitDistance = intersectionT;
        }
    }

    if (closestShape == nullptr) return closestShape->Miss(ray);
    return closestShape->ClosestHit(ray, hitDistance);

}


glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y) {
    
    Ray ray; 
    ray.Origin = m_ActiveCamera->GetPosition();
    ray.Direction = m_ActiveCamera->GetRayDirections()[x + y*m_FinalImage->GetWidth()];

    glm::vec3 light(.0f);
    glm::vec3 contribution(1.0f);

    uint32_t seed = x + y*m_FinalImage->GetWidth();
    seed *= m_FrameIndex;

    int bounces = 5;

    for(int i = 0; i < bounces; i++) {

        seed += i;

        HitPayLoad payload = TraceRay(ray);
        
        if (payload.HitDistance < 0.0f) {

            glm::vec3 skyColor = glm::vec3(.6f, .7f, .9f);
            light += skyColor * contribution;
            break;
        }
        
        // glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));
        // float lightIntensity = glm::max(glm::dot(payload.WorldNormal, -lightDir),0.0f);

        const Material& material = m_ActiveScene->Materials[payload.HitShape->MaterialIndex];

        contribution*= material.Albedo;
        light += material.GetEmission();

        //glm::vec3 RoughnessVector = material.Roughness * Walnut::Random::Vec3(-0.5f, 0.5f);
        
        //TODO : specular

        ray.Origin = payload.WorldPosition + payload.WorldNormal * .0001f;
        //ray.Direction = glm::reflect(ray.Direction, payload.WorldNormal + RoughnessVector);

        ray.Direction = glm::normalize(payload.WorldNormal + Utils::InUnitSphere(seed));
    }

    return glm::vec4(light, 1.0f);
}
 