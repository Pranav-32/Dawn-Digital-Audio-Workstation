#include <catch2/catch_test_macros.hpp>

#include "core/resource/resource_manager.h"
#include "resources.h"

#include <thread>
#include <vector>

using namespace dawn::core;
using namespace dawn::resources;

TEST_CASE("ResourceManager initializes successfully", "[core][resource]")
{
    REQUIRE_NOTHROW(ResourceManager::init());

    const auto& logo = ResourceManager::getResource(ImageID::dawn_logo);

    REQUIRE(logo.isValid());
}

TEST_CASE("ResourceManager initialization is idempotent", "[core][resource]")
{
    REQUIRE_NOTHROW(ResourceManager::init());
    REQUIRE_NOTHROW(ResourceManager::init());
    REQUIRE_NOTHROW(ResourceManager::init());

    const auto& logo = ResourceManager::getResource(ImageID::dawn_logo);

    REQUIRE(logo.isValid());
}

TEST_CASE("ResourceManager initialization is thread-safe", "[core][resource][thread]")
{
    constexpr std::size_t threadCount = 32;

    std::vector<std::jthread> threads;
    threads.reserve(threadCount);

    for (std::size_t i = 0; i < threadCount; ++i)
    {
        threads.emplace_back([]() { ResourceManager::init(); });
    }

    const auto& logo = ResourceManager::getResource(ImageID::dawn_logo);

    REQUIRE(logo.isValid());
}

TEST_CASE("ResourceManager always returns the same cached object", "[core][resource]")
{
    ResourceManager::init();

    const auto& a = ResourceManager::getResource(ImageID::dawn_logo);

    const auto& b = ResourceManager::getResource(ImageID::dawn_logo);

    REQUIRE(&a == &b);
}

TEST_CASE("Loaded embedded image is valid", "[core][resource]")
{
    ResourceManager::init();

    const auto& logo = ResourceManager::getResource(ImageID::dawn_logo);

    REQUIRE(logo.isValid());
}

TEST_CASE("Embedded image dimensions are correct", "[core][resource]")
{
    ResourceManager::init();

    const auto& logo = ResourceManager::getResource(ImageID::dawn_logo);

    REQUIRE(logo.isValid());

    REQUIRE(logo.getWidth() == 2000);
    REQUIRE(logo.getHeight() == 2000);
}

TEST_CASE("Concurrent resource access", "[core][resource]")
{
    ResourceManager::init();

    constexpr std::size_t threadCount = 16;
    constexpr std::size_t iterations = 10000;

    std::atomic_bool success = true;

    std::vector<std::jthread> threads;

    for (std::size_t t = 0; t < threadCount; ++t)
    {
        threads.emplace_back(
            [&]
            {
                for (std::size_t i = 0; i < iterations; ++i)
                {

                    if (const auto& logo = ResourceManager::getResource(ImageID::dawn_logo);
                        !logo.isValid())
                    {
                        success = false;
                        return;
                    }
                }
            });
    }

    REQUIRE(success);
}