#pragma once

#include <format>
#include <queue>
#include <string>
#include <unordered_map>

#include "common.h"
#include "../logger.h"

GLAB_NAMESPACE_BEGIN()

class ResourceManager {
public:
    static ResourceManager& instance() noexcept;

    ~ResourceManager();

    template <ResourceLike Resource>
    ResourceHandle<Resource> load(const std::string& path);

    template <ResourceLike Resource, typename... Args>
    ResourceHandle<Resource> make(Args&&... args) {
        ResourceHandle<Resource> handle{new Resource(std::forward<Args>(args)...)};
        handle->init();
        handle->id = m_next_id++;
        handle->name = std::format("Resource.{:03}", handle->id);
        LOG_DEBUG("Building a resource: id={}", handle->id);
        return handle;
    }

    template <ResourceLike Resource>
    ResourceHandle<Resource> get(const std::string& name) {
        if (!m_id_map.contains(name)) {
            return {};
        }
        return m_handle_map.at(m_id_map.at(name));
    }

    void pushDestroyQueue(IResource* resource);
    void flushDestroyQueue();

private:
    ResourceID m_next_id{0};
    std::queue<IResource*> m_destroy_queue;

    std::unordered_map<std::string, ResourceID> m_id_map;
    std::unordered_map<std::string, ResourceID> m_path_map;
    std::unordered_map<ResourceID, ResourceHandle<Mesh>> m_handle_map;
};

GLAB_NAMESPACE_END()
