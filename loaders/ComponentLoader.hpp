#pragma once

#include <string>
#include "LibraryLoader.hpp"
#include <libecs-cpp/ecs.hpp>

using ComponentCreator = ecs::Component *(*)(void *);

namespace ComponentLoader
{
    class Loader
    {
      public:
        Loader(std::string library);
        ~Loader();
        ecs::Component *ComponentCreate();
        ecs::Component *ComponentCreate(void *data);
        ComponentCreator ComponentGet();
      private:
        LibraryLoader *library;
    };

    extern std::map<std::string, ComponentLoader::Loader *> component_loaders;

    ecs::Component *Create(std::string component);
    ecs::Component *Create(std::string component, void *data);
    ComponentCreator Get(std::string);
}
