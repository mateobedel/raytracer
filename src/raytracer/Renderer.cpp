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

Renderer::HitPayLoad Renderer::TraceRay(const Ray& ray) {

    int closestSphere = -1;
    float hitDistance = std::numeric_limits<float>::max();

    for(size_t i = 0; i < m_ActiveScene->Spheres.size(); i++) {

        const Sphere& sphere = m_ActiveScene->Spheres[i];
        glm::vec3 origin = ray.Origin - sphere.Position;

        float a = glm::dot(ray.Direction,ray.Direction);
        float b = 2.0f * glm::dot(origin, ray.Direction);
        float c = glm::dot(origin, origin) - sphere.Radius*sphere.Radius;

        float delta = b*b - 4.0f * a * c;

        if (delta < 0.0f)
            continue;

        float closestT = (-b - glm::sqrt(delta)) / (2.0f * a); //Smaller solution

        if (closestT > 0.0f && closestT < hitDistance) {
            closestSphere = (int)i;
            hitDistance = closestT;
        }
    }

    if (closestSphere < 0)
        return Miss(ray);

    return ClosestHit(ray, hitDistance, closestSphere);

}


glm::vec4 Renderer::PerPixel(uint32_t x, uint32_t y) {
    Ray ray; 
    ray.Origin = m_ActiveCamera->GetPosition();
    ray.Direction = m_ActiveCamera->GetRayDirections()[x + y*m_FinalImage->GetWidth()];

    glm::vec3 color(.0f);
    float multiplier = 1.0f;

    int bounces = 5;

    for(int i = 0; i < bounces; i++) {

        Renderer::HitPayLoad payload = TraceRay(ray);
        
        if (payload.HitDistance < 0.0f) {

            glm::vec3 skyColor = glm::vec3(.6f, .7f, .9f);
            color += skyColor * multiplier;
            break;
        }
        
        glm::vec3 lightDir = glm::normalize(glm::vec3(-1, -1, -1));
        float lightIntensity = glm::max(glm::dot(payload.WorldNormal, -lightDir),0.0f);

        const Sphere& sphere = m_ActiveScene->Spheres[payload.ObjectIndex];
        const Material& material = m_ActiveScene->Materials[sphere.MaterialIndex];

        

        glm::vec3 sphereColor = material.Albedo;
        sphereColor*=lightIntensity;
        color += sphereColor*multiplier;

        multiplier *= .5f;

        glm::vec3 RoughnessVector = material.Roughness * Walnut::Random::Vec3(-0.5f, 0.5f);
        
        ray.Origin = payload.WorldPosition + payload.WorldNormal * .0001f;
        ray.Direction = glm::reflect(ray.Direction, payload.WorldNormal + RoughnessVector);
    }


    
    return glm::vec4(color, 1.0f);
}
 

Renderer::HitPayLoad Renderer::ClosestHit(const Ray& ray, float hitDistance, int objectIndex) {

    Renderer::HitPayLoad payload;
    payload.HitDistance = hitDistance;
    payload.ObjectIndex = objectIndex;

    const Sphere& closestSphere = m_ActiveScene->Spheres[objectIndex];

    glm::vec3 origin = ray.Origin - closestSphere.Position;
    payload.WorldPosition = origin + ray.Direction * hitDistance;
    payload.WorldNormal = glm::normalize(payload.WorldPosition);
    payload.WorldPosition += closestSphere.Position;

    return payload;

}

Renderer::HitPayLoad Renderer::Miss(const Ray& ray) {
    Renderer::HitPayLoad payload;
    payload.HitDistance = -1;
    return payload;

}