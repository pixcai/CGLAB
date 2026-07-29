#pragma once

#include <concepts>
#include <cstdint>
#include <limits>

#include "../namespace.h"

GLAB_NAMESPACE_BEGIN()

class Scene;
class Renderer;

struct Entity {
    static constexpr auto kInvalidID = std::numeric_limits<std::uint32_t>::max();

    std::uint32_t id{kInvalidID};
    std::uint32_t generation{0};

    bool operator==(const Entity& other) const noexcept {
        return id == other.id && generation == other.generation;
    }
};

struct IComponent {
    IComponent() = default;
    virtual ~IComponent() = default;
};

template <typename T>
concept ComponentLike = std::derived_from<T, IComponent>;

struct Mesh;
struct Material;
struct Shader;

struct IResource;

template <typename T>
concept ResourceLike = std::derived_from<T, IResource>;

template <typename T>
class ResourceHandle;

using ResourceID = std::uint32_t;

GLAB_NAMESPACE_END()
