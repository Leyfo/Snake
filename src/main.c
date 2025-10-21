#include <SDL3/SDL.h>
#include <stdio.h>
#include <GL/gl.h>

int main(void) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Erro ao iniciar SDL: %s\n", SDL_GetError());
        return 1;
    }

    // Configura atributos do contexto OpenGL
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window *window = SDL_CreateWindow(
        "Snake SDL3 + OpenGL", 
        800, 600, 
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );
    if (!window) {
        printf("Erro ao criar janela: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (!glContext) {
        printf("Erro ao criar contexto OpenGL: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Loop principal
    int rodando = 1;
    SDL_Event e;
    while (rodando) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_EVENT_QUIT) {
                rodando = 0;
            }
        }

        // Define a cor de fundo (verde)
        glClearColor(0.1f, 0.8f, 0.2f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Troca buffers (exibe o que foi desenhado)
        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DestroyContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
