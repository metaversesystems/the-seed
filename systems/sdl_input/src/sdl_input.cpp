#include <sdl_input.hpp>
#include <SDL.h>
#include "../../../loaders/ComponentLoader.hpp"
#include <iostream>

sdl_input::sdl_input() 
{ 
    this->Handle = "sdl_input";
}

sdl_input::sdl_input(Json::Value config)
{
    this->Handle = "sdl_input";
}

Json::Value sdl_input::save()
{
    Json::Value config;
    return config;
}

void sdl_input::Update(uint32_t dt)
{
    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        Json::Value input_config;
        switch(event.type)
        {
            case SDL_MOUSEBUTTONUP:
                switch(event.button.button)
                {
                    case SDL_BUTTON_LEFT:
                        input_config["action"] = "left_click";
                        input_config["content"]["x"] = event.motion.x;
                        input_config["content"]["y"] = event.motion.y;
                        break;
                    default:
                        break;
                }
                break;
            case SDL_QUIT:
                input_config["action"] = "quit";
            case SDL_KEYUP:
                input_config["action"] = "keyup";
                switch(event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        input_config["action"] = "quit";
                        break;
                    case SDLK_SPACE:
                        input_config["content"]["key"] = " ";
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }

        if(input_config.size())
        {
            ecs::Entity *e = this->Container->Entity();
            e->Component(ComponentLoader::Create("input", &input_config));
        }
    }

}

extern "C"
{
    ecs::System *create_system(void *p)
    {
        if(p == nullptr) return new sdl_input();

        Json::Value *config = (Json::Value *)p;
        return new sdl_input(*config);
    }
}