
#include <stdio.h>
#include <stdbool.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>

enum TECLAS { CIMA, BAIXO, DIREITA, ESQUERDA, FOGO };

enum DIRECAO {
    DIRECAO_BAIXO,
    DIRECAO_CIMA,
    DIRECAO_DIREITA,
    DIRECAO_ESQUERDA
};

int main() {

    // Configurações
    const int largura_t = 1040;
    const int altura_t = 980;
    const int FPS = 60;

    const float velocidade = 5.0f;
    const float velocidade_tiro = 10.0f;

    // Posição do jogador
    float pos_x = 100.0f;
    float pos_y = 100.0f;

    // Tiro
    float fogo_x = 0.0f;
    float fogo_y = 0.0f;

    // Direção do tiro
    float fogo_dx = 0.0f;
    float fogo_dy = 0.0f;

    bool fogo_ativo = false;

    // Teclas
    bool teclas[] = { false, false, false, false, false };

    // Direção do personagem
    enum DIRECAO direcao_atual = DIRECAO_BAIXO;

    // Animação
    int frame_atual = 0;
    int contador_animacao = 0;
    const int velocidade_animacao = 10;

    // Controle
    bool fim = false;

    // Allegro
    ALLEGRO_DISPLAY* display = NULL;
    ALLEGRO_TIMER* timer = NULL;
    ALLEGRO_EVENT_QUEUE* fila_eventos = NULL;

    // Inicialização
    if (!al_init()) {
        al_show_native_message_box(NULL, "Erro!", "Erro ao inicializar o Allegro.", NULL, 0);
        return 1;
    }

    // Teclado
    if (!al_install_keyboard()) {
        al_show_native_message_box(NULL, "Erro!", "Erro ao inicializar o teclado.", NULL, 0);
        return 1;
    }

    // Display
    display = al_create_display(largura_t, altura_t);

    if (!display) {
        al_show_native_message_box(NULL, "Erro!", "Erro ao criar o display.", NULL, 0);
        return 1;
    }

    // Addons
    if (!al_init_image_addon()) {
        al_show_native_message_box(display, "Erro!", "Erro ao inicializar o addon de imagens.", NULL, 0);
        al_destroy_display(display);
        return 1;
    }

    al_init_primitives_addon();

    // Timer
    timer = al_create_timer(1.0 / FPS);

    if (!timer) {
        al_show_native_message_box(display, "Erro!", "Erro ao criar o timer.", NULL, 0);
        al_destroy_display(display);
        return 1;
    }

    // Sprites
    ALLEGRO_BITMAP* rudanBaixo1 = al_load_bitmap("assets/sprites/spritesRudan/rudanBaixo1.png");
    ALLEGRO_BITMAP* rudanBaixo2 = al_load_bitmap("assets/sprites/spritesRudan/rudanBaixo2.png");

    ALLEGRO_BITMAP* rudanCima1 = al_load_bitmap("assets/sprites/spritesRudan/rudanCima1.png");
    ALLEGRO_BITMAP* rudanCima2 = al_load_bitmap("assets/sprites/spritesRudan/rudanCima2.png");

    ALLEGRO_BITMAP* rudanDireita1 = al_load_bitmap("assets/sprites/spritesRudan/rudanDireita1.png");
    ALLEGRO_BITMAP* rudanDireita2 = al_load_bitmap("assets/sprites/spritesRudan/rudanDireita2.png");

    ALLEGRO_BITMAP* rudanEsquerda1 = al_load_bitmap("assets/sprites/spritesRudan/rudanEsquerda1.png");
    ALLEGRO_BITMAP* rudanEsquerda2 = al_load_bitmap("assets/sprites/spritesRudan/rudanEsquerda2.png");

    // Verifica se os sprites foram carregados
    if (!rudanBaixo1 || !rudanBaixo2 || !rudanCima1 || !rudanCima2 ||
        !rudanDireita1 || !rudanDireita2 || !rudanEsquerda1 || !rudanEsquerda2) {

        al_show_native_message_box(display, "Erro!", "Erro ao carregar um ou mais sprites.", NULL, 0);

        if (rudanBaixo1) {
            al_destroy_bitmap(rudanBaixo1);
        }
           

        if (rudanBaixo2) {
            al_destroy_bitmap(rudanBaixo2);

        }

        if (rudanCima1) {
            al_destroy_bitmap(rudanCima1);

        }

        if (rudanCima2) {
            al_destroy_bitmap(rudanCima2);

        }

        if (rudanDireita1) {
            al_destroy_bitmap(rudanDireita1);

        }


        if (rudanDireita2) {
            al_destroy_bitmap(rudanDireita2);

        }

        if (rudanEsquerda1) {
            al_destroy_bitmap(rudanEsquerda1);

        }

        if (rudanEsquerda2) {
            al_destroy_bitmap(rudanEsquerda2);

        }

        al_destroy_timer(timer);
        al_destroy_display(display);

        return 1;
    }

    // Fila de eventos
    fila_eventos = al_create_event_queue();

    if (!fila_eventos) {

        al_show_native_message_box(display, "Erro!", "Erro ao criar a fila de eventos.", NULL, 0);

        al_destroy_bitmap(rudanBaixo1);
        al_destroy_bitmap(rudanBaixo2);
        al_destroy_bitmap(rudanCima1);
        al_destroy_bitmap(rudanCima2);
        al_destroy_bitmap(rudanDireita1);
        al_destroy_bitmap(rudanDireita2);
        al_destroy_bitmap(rudanEsquerda1);
        al_destroy_bitmap(rudanEsquerda2);

        al_destroy_timer(timer);
        al_destroy_display(display);

        return 1;
    }

    // Eventos
    al_register_event_source(fila_eventos, al_get_keyboard_event_source());
    al_register_event_source(fila_eventos, al_get_display_event_source(display));
    al_register_event_source(fila_eventos, al_get_timer_event_source(timer));

    al_start_timer(timer);

    // Loop principal
    while (!fim) {

        ALLEGRO_EVENT ev;
        al_wait_for_event(fila_eventos, &ev);

        // Tecla pressionada
        if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {

            if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                fim = true;
            }

            switch (ev.keyboard.keycode) {

            case ALLEGRO_KEY_UP:
                teclas[CIMA] = true;
                direcao_atual = DIRECAO_CIMA;
                break;

            case ALLEGRO_KEY_DOWN:
                teclas[BAIXO] = true;
                direcao_atual = DIRECAO_BAIXO;
                break;

            case ALLEGRO_KEY_RIGHT:
                teclas[DIREITA] = true;
                direcao_atual = DIRECAO_DIREITA;
                break;

            case ALLEGRO_KEY_LEFT:
                teclas[ESQUERDA] = true;
                direcao_atual = DIRECAO_ESQUERDA;
                break;

            case ALLEGRO_KEY_D:

                teclas[FOGO] = true;

                // Só cria outro tiro quando não existe um ativo
                if (!fogo_ativo) {

                    fogo_ativo = true;

                    // Tiro para cima
                    if (direcao_atual == DIRECAO_CIMA) {
                        fogo_x = pos_x + 20;
                        fogo_y = pos_y;
                        fogo_dx = 0.0f;
                        fogo_dy = -velocidade_tiro;
                    }

                    // Tiro para baixo
                    else if (direcao_atual == DIRECAO_BAIXO) {
                        fogo_x = pos_x + 20;
                        fogo_y = pos_y + 40;
                        fogo_dx = 0.0f;
                        fogo_dy = velocidade_tiro;
                    }

                    // Tiro para direita
                    else if (direcao_atual == DIRECAO_DIREITA) {
                        fogo_x = pos_x + 40;
                        fogo_y = pos_y + 20;
                        fogo_dx = velocidade_tiro;
                        fogo_dy = 0.0f;
                    }

                    // Tiro para esquerda
                    else if (direcao_atual == DIRECAO_ESQUERDA) {
                        fogo_x = pos_x;
                        fogo_y = pos_y + 20;
                        fogo_dx = -velocidade_tiro;
                        fogo_dy = 0.0f;
                    }
                }

                break;
            }
        }

        // Tecla solta
        else if (ev.type == ALLEGRO_EVENT_KEY_UP) {

            switch (ev.keyboard.keycode) {

            case ALLEGRO_KEY_UP:
                teclas[CIMA] = false;
                break;

            case ALLEGRO_KEY_DOWN:
                teclas[BAIXO] = false;
                break;

            case ALLEGRO_KEY_RIGHT:
                teclas[DIREITA] = false;
                break;

            case ALLEGRO_KEY_LEFT:
                teclas[ESQUERDA] = false;
                break;

            case ALLEGRO_KEY_D:
                teclas[FOGO] = false;
                break;
            }
        }

        // Janela fechada
        else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            fim = true;
        }

        // Timer
        else if (ev.type == ALLEGRO_EVENT_TIMER) {

            // Movimento
            if (teclas[CIMA])
                pos_y -= velocidade;

            if (teclas[BAIXO])
                pos_y += velocidade;

            if (teclas[ESQUERDA])
                pos_x -= velocidade;

            if (teclas[DIREITA])
                pos_x += velocidade;

            // Tamanho do personagem
            int largura_personagem = al_get_bitmap_width(rudanBaixo1);
            int altura_personagem = al_get_bitmap_height(rudanBaixo1);

            // Limita o personagem à tela
            if (pos_x < 0)
                pos_x = 0;

            if (pos_y < 0)

                pos_y = 0;

            if (pos_x + largura_personagem > largura_t)
                pos_x = largura_t - largura_personagem;

            if (pos_y + altura_personagem > altura_t)
                pos_y = altura_t - altura_personagem;

            // Verifica se está andando
            bool andando = teclas[CIMA] || teclas[BAIXO] || teclas[DIREITA] || teclas[ESQUERDA];

            // Animação
            if (andando) {

                contador_animacao++;

                if (contador_animacao >= velocidade_animacao) {

                    contador_animacao = 0;
                    frame_atual++;

                    if (frame_atual >= 2)
                        frame_atual = 0;
                }
            }
            else {

                frame_atual = 0;
                contador_animacao = 0;
            }

            // =================================================
            // MOVIMENTO DO TIRO
            // =================================================
            
     


            if (fogo_ativo) {

                fogo_x += fogo_dx;
                fogo_y += fogo_dy;

                // Saiu pela direita
                if (fogo_x > largura_t)
                    fogo_ativo = false;

                // Saiu pela esquerda
                if (fogo_x < 0)
                    fogo_ativo = false;

                // Saiu por baixo
                if (fogo_y > altura_t)
                    fogo_ativo = false;

                // Saiu por cima
                if (fogo_y < 0)
                    fogo_ativo = false;
            }

            // Limpa a tela
            al_clear_to_color(al_map_rgb(0, 0, 0));

            // Sprite atual
            ALLEGRO_BITMAP* sprite_atual = NULL;

            // Baixo
            if (direcao_atual == DIRECAO_BAIXO) {

                if (frame_atual == 0)
                    sprite_atual = rudanBaixo1;
                else
                    sprite_atual = rudanBaixo2;
            }

            // Cima
            else if (direcao_atual == DIRECAO_CIMA) {

                if (frame_atual == 0)
                    sprite_atual = rudanCima1;
                else
                    sprite_atual = rudanCima2;
            }

            // Direita
            else if (direcao_atual == DIRECAO_DIREITA) {

                if (frame_atual == 0)
                    sprite_atual = rudanDireita1;
                else
                    sprite_atual = rudanDireita2;
            }

            // Esquerda
            else if (direcao_atual == DIRECAO_ESQUERDA) {

                if (frame_atual == 0)
                    sprite_atual = rudanEsquerda1;
                else
                    sprite_atual = rudanEsquerda2;
            }

            // Desenha o personagem
            al_draw_bitmap(sprite_atual, pos_x, pos_y, 0);

            // Desenha o tiro
            if (fogo_ativo)
                al_draw_filled_circle(fogo_x, fogo_y, 5, al_map_rgb(255, 0, 0));

            // Mostra a tela
            al_flip_display();
        }
    }

    // Finalização
    al_destroy_bitmap(rudanBaixo1);
    al_destroy_bitmap(rudanBaixo2);

    al_destroy_bitmap(rudanCima1);
    al_destroy_bitmap(rudanCima2);

    al_destroy_bitmap(rudanDireita1);
    al_destroy_bitmap(rudanDireita2);

    al_destroy_bitmap(rudanEsquerda1);
    al_destroy_bitmap(rudanEsquerda2);

    al_destroy_timer(timer);
    al_destroy_event_queue(fila_eventos);
    al_destroy_display(display);

    return 0;
}

