#include <sdl.hpp>
#include <SDL_image.h>
#include "../../components/position/src/position.hpp"
#include "../../components/texture/src/texture.hpp"
#include "../../components/shape/src/shape.hpp"

sdl::sdl() 
{ 
    this->Handle = "sdl";
}

sdl::sdl(Json::Value config)
{
    this->Handle = "sdl";
    this->height = config["height"].asUInt();
    this->width = config["width"].asUInt();
    if(config["scale"].asString().size()) this->scale = config["scale"].asFloat();
    this->title = config["title"].asString();
    this->columns = config["columns"].asUInt();
    this->rows = config["rows"].asUInt();
    this->images = config["images"];

    this->ComponentRequest("texture");
    this->ComponentRequest("shape");
}

void sdl::Init()
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        throw std::string("Couldn't initialize SDL video.");
    }

    this->window = SDL_CreateWindow(this->title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                    this->width, this->height, SDL_WINDOW_SHOWN);
    if(this->window == nullptr)
    {
        std::string message = "Window could not be created! SDL_Error: " + std::string(SDL_GetError());
        throw message;
    }

    SDL_SetWindowFullscreen(this->window, SDL_WINDOW_FULLSCREEN_DESKTOP);

    this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED);

    this->screen_surface = SDL_GetWindowSurface(this->window);

    for(uint16_t counter = 0; counter < this->images.size(); counter++)
    {
        std::string filename = this->images[counter].asString();
        this->image_cache[filename] = IMG_Load(filename.c_str());
        this->tex_cache[filename] = SDL_CreateTextureFromSurface(this->renderer, this->image_cache[filename]);
    }
}

Json::Value sdl::save()
{
    throw std::string("save() not implemented");
    Json::Value config;
    return config;
}

void sdl::Update(uint32_t dt)
{
    if(!running)
    {
        SDL_DestroyWindow(this->window);
        SDL_Quit();
        this->Container->ManagerGet()->Shutdown();
        return;
    }

    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_QUIT:
                this->running = false;
            case SDL_KEYUP:
                switch(event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        this->running = false;
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }

    SDL_SetRenderDrawColor(this->renderer, 0, 0, 0, 0xFF );
    SDL_RenderClear(this->renderer);

    std::map<std::string, ecs::ComponentList> Components = this->ComponentsGet();

    for(auto &c : Components["texture"])
    {
        auto t = (texture *)c;
        auto p = (position *)this->Container->Entity(t->EntityHandle)->ComponentGet("position");

        SDL_Rect src = { t->col * t->width, t->row * t->height, t->width, t->height };
        SDL_Rect dest = { p->x * (t->width * this->scale), p->y * (t->height * this->scale), t->width * this->scale, t->height * this->scale };
        SDL_SetTextureColorMod(this->tex_cache[t->tex_filename], t->r, t->g, t->b);
        SDL_RenderCopy(this->renderer, this->tex_cache[t->tex_filename], &src, &dest);
    }

    for(auto &c : Components["shape"])
    {
        auto s = (shape *)c;
        auto p = (position *)this->Container->Entity(s->EntityHandle)->ComponentGet("position");

        SDL_Rect rect = { p->x * this->scale, p->y * this->scale, s->width * this->scale, s->height * this->scale };
        SDL_SetRenderDrawColor(this->renderer, s->r, s->g, s->b, s->a);
        SDL_RenderFillRect(this->renderer, &rect);
    }

    SDL_SetRenderDrawBlendMode(this->renderer, SDL_BLENDMODE_BLEND);
    SDL_RenderPresent(this->renderer);
}

extern "C"
{
    ecs::System *create_system(void *p)
    {
        if(p == nullptr) return new sdl();

        Json::Value *config = (Json::Value *)p;
        return new sdl(*config);
    }

    void *get_system()
    {
        return (void *)create_system;
    }
}