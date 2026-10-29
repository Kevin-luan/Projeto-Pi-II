
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include "fase.h"

bool fim = false;


// Posicao do personagem
float pos_x = 500.0f;
float pos_y = 800.0f;

// ini migo 

float ini_x = 200.0f;
float ini_y = 200.0f;
void atualizarInimigo( )

{

    float dx = pos_x - ini_x;
    float dy = pos_y - ini_y;
    float distancia = sqrtf(dx * dx + dy * dy);
    // O inimigo persegue o jogador
    if (distancia > 30.0f)
    {
        float velocidade_inimigo = 2.0f;
        ini_x += (dx / distancia) * velocidade_inimigo;
        ini_y += (dy / distancia) * velocidade_inimigo;

    }
   

}

// Desenha o inimigo

void desenharInimigo()
{

    al_draw_filled_circle(ini_x,ini_y,15,al_map_rgb(200, 200, 200));
    if (ini_x == pos_x &&  ini_y == pos_y) {
        al_draw_filled_circle(ini_x+100, ini_y, 15, al_map_rgb(200, 0, 0));
}



}

enum TECLAS {

    CIMA, BAIXO, DIREITA, ESQUERDA, FOGO

};

enum DIRECAO {

    DIRECAO_BAIXO,

    DIRECAO_CIMA,

    DIRECAO_DIREITA,

    DIRECAO_ESQUERDA

};

void fase2(void)

{

    const int largura_t = 1040;

    const int altura_t = 980;

    const int FPS = 60;

    const float velocidade = 5.0f;

    const float velocidade_tiro = 10.0f;

    bool teclas[5] = { false, false, false, false, false };

    enum DIRECAO direcao_atual = DIRECAO_BAIXO;

    int frame_atual = 0;

    int contador_animacao = 0;

    const int velocidade_animacao = 10;

    float fogo_x = 0.0f;

    float fogo_y = 0.0f;

    float fogo_dx = 0.0f;

    float fogo_dy = 0.0f;

    const float fogo_raio = 5.0f;

    bool fogo_ativo = false;

    ALLEGRO_DISPLAY* display = NULL;

    ALLEGRO_TIMER* timer = NULL;

    ALLEGRO_EVENT_QUEUE* fila_eventos = NULL;

    if (!al_init())

        return;

    if (!al_install_keyboard())

        return;

    if (!al_init_image_addon())

        return;

    if (!al_init_primitives_addon())

        return;

    display = al_create_display(largura_t, altura_t);

    if (!display)

        return;

    timer = al_create_timer(1.0 / FPS);

    if (!timer) {

        al_destroy_display(display);

        return;

    }

    fila_eventos = al_create_event_queue();

    if (!fila_eventos) {

        al_destroy_timer(timer);

        al_destroy_display(display);

        return;

    }

    // Sprites do personagem

    ALLEGRO_BITMAP* rudanBaixo1 =

        al_load_bitmap("assets/sprites/spritesRudan/rudanBaixo1.png");

    ALLEGRO_BITMAP* rudanBaixo2 =

        al_load_bitmap("assets/sprites/spritesRudan/rudanBaixo2.png");

    ALLEGRO_BITMAP* rudanCima1 =

        al_load_bitmap("assets/sprites/spritesRudan/rudanCima1.png");

    ALLEGRO_BITMAP* rudanCima2 =

        al_load_bitmap("assets/sprites/spritesRudan/rudanCima2.png");

    ALLEGRO_BITMAP* rudanDireita1 =

        al_load_bitmap("assets/sprites/spritesRudan/rudanDireita1.png");

    ALLEGRO_BITMAP* rudanDireita2 =

        al_load_bitmap("assets/sprites/spritesRudan/rudanDireita2.png");

    ALLEGRO_BITMAP* rudanEsquerda1 =

        al_load_bitmap("assets/sprites/spritesRudan/rudanEsquerda1.png");

    ALLEGRO_BITMAP* rudanEsquerda2 =

        al_load_bitmap("assets/sprites/spritesRudan/rudanEsquerda2.png");

    // Verifica se todos os sprites foram carregados

    if (!rudanBaixo1 || !rudanBaixo2 ||

        !rudanCima1 || !rudanCima2 ||

        !rudanDireita1 || !rudanDireita2 ||

        !rudanEsquerda1 || !rudanEsquerda2) {

        if (rudanBaixo1) al_destroy_bitmap(rudanBaixo1);

        if (rudanBaixo2) al_destroy_bitmap(rudanBaixo2);

        if (rudanCima1) al_destroy_bitmap(rudanCima1);

        if (rudanCima2) al_destroy_bitmap(rudanCima2);

        if (rudanDireita1) al_destroy_bitmap(rudanDireita1);

        if (rudanDireita2) al_destroy_bitmap(rudanDireita2);

        if (rudanEsquerda1) al_destroy_bitmap(rudanEsquerda1);

        if (rudanEsquerda2) al_destroy_bitmap(rudanEsquerda2);

        al_destroy_event_queue(fila_eventos);

        al_destroy_timer(timer);

        al_destroy_display(display);

        return;

    }

    int largura_personagem = al_get_bitmap_width(rudanBaixo1);

    int altura_personagem = al_get_bitmap_height(rudanBaixo1);

    // Registra os eventos

    al_register_event_source(

        fila_eventos, al_get_keyboard_event_source());

    al_register_event_source(

        fila_eventos, al_get_display_event_source(display));

    al_register_event_source(

        fila_eventos, al_get_timer_event_source(timer));

    al_start_timer(timer);


    while (!fim) {

        ALLEGRO_EVENT ev;

        al_wait_for_event(fila_eventos, &ev);

        // Teclas pressionadas

        if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {

            switch (ev.keyboard.keycode) {

            case ALLEGRO_KEY_ESCAPE:

                fim = true;

                break;

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

                if (!fogo_ativo) {

                    fogo_ativo = true;

                    if (direcao_atual == DIRECAO_CIMA) {

                        fogo_x = pos_x + 20;

                        fogo_y = pos_y;

                        fogo_dx = 0;

                        fogo_dy = -velocidade_tiro;

                    }

                    else if (direcao_atual == DIRECAO_BAIXO) {

                        fogo_x = pos_x + 20;

                        fogo_y = pos_y + 40;

                        fogo_dx = 0;

                        fogo_dy = velocidade_tiro;

                    }

                    else if (direcao_atual == DIRECAO_DIREITA) {

                        fogo_x = pos_x + 40;

                        fogo_y = pos_y + 20;

                        fogo_dx = velocidade_tiro;

                        fogo_dy = 0;

                    }

                    else if (direcao_atual == DIRECAO_ESQUERDA) {

                        fogo_x = pos_x;

                        fogo_y = pos_y + 20;

                        fogo_dx = -velocidade_tiro;

                        fogo_dy = 0;

                    }

                }

                break;

            }

        }

        // Teclas soltas

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

        // Fechar janela

        else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {

            fim = true;

        }

        // Atualizacao do jogo

        else if (ev.type == ALLEGRO_EVENT_TIMER) {

            // Movimento do personagem

            if (teclas[CIMA]) pos_y -= velocidade;

            if (teclas[BAIXO]) pos_y += velocidade;

            if (teclas[ESQUERDA]) pos_x -= velocidade;

            if (teclas[DIREITA]) pos_x += velocidade;

            // Limites da tela

            if (pos_x < 0) pos_x = 0;

            if (pos_y < 0) pos_y = 0;

            if (pos_x + largura_personagem > largura_t)

                pos_x = largura_t - largura_personagem;

            if (pos_y + altura_personagem > altura_t)

                pos_y = altura_t - altura_personagem;

            // Animacao

            bool andando =

                teclas[CIMA] || teclas[BAIXO] ||

                teclas[DIREITA] || teclas[ESQUERDA];

            if (andando) {

                contador_animacao++;

                if (contador_animacao >= velocidade_animacao) {

                    contador_animacao = 0;

                    frame_atual = (frame_atual + 1) % 2;

                }

            }

            else {

                frame_atual = 0;

                contador_animacao = 0;

            }

            // Movimento do tiro

            if (fogo_ativo) {

                fogo_x += fogo_dx;

                fogo_y += fogo_dy;

                if (fogo_x + fogo_raio < 0 ||

                    fogo_x - fogo_raio > largura_t ||

                    fogo_y + fogo_raio < 0 ||

                    fogo_y - fogo_raio > altura_t) {

                    fogo_ativo = false;

                }

            }

            // Atualiza a posicao do inimigo a cada timer

            atualizarInimigo();

        

            al_clear_to_color(al_map_rgb(20, 50, 70));

            ALLEGRO_BITMAP* sprite_atual = NULL;

            if (direcao_atual == DIRECAO_BAIXO)

                sprite_atual = frame_atual == 0

                ? rudanBaixo1 : rudanBaixo2;

            else if (direcao_atual == DIRECAO_CIMA)

                sprite_atual = frame_atual == 0

                ? rudanCima1 : rudanCima2;

            else if (direcao_atual == DIRECAO_DIREITA)

                sprite_atual = frame_atual == 0

                ? rudanDireita1 : rudanDireita2;

            else if (direcao_atual == DIRECAO_ESQUERDA)

                sprite_atual = frame_atual == 0

                ? rudanEsquerda1 : rudanEsquerda2;

            // Desenha o personagem

            al_draw_bitmap(sprite_atual, pos_x, pos_y, 0);

            // Desenha o tiro

            if (fogo_ativo) {

                al_draw_filled_circle(

                    fogo_x, fogo_y, fogo_raio,

                    al_map_rgb(255, 0, 0));

            }

            // Desenha o inimigo na posicao atualizada

            desenharInimigo();

            desenharInimigo();

            al_flip_display();

        }

    }

    // Libera os sprites

    al_destroy_bitmap(rudanBaixo1);

    al_destroy_bitmap(rudanBaixo2);

    al_destroy_bitmap(rudanCima1);

    al_destroy_bitmap(rudanCima2);

    al_destroy_bitmap(rudanDireita1);

    al_destroy_bitmap(rudanDireita2);

    al_destroy_bitmap(rudanEsquerda1);

    al_destroy_bitmap(rudanEsquerda2);

    al_destroy_event_queue(fila_eventos);

    al_destroy_timer(timer);

    al_destroy_display(display);

}

