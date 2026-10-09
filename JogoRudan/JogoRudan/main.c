
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

    // Barco
    int barco_direcao = 0;
    float barco_x = 500.0f;
    float barco_y = 300.0f;
    bool barcoVivo = true;

    // Controle
    bool fim = false;

    // Allegro
    ALLEGRO_DISPLAY* display = NULL;
    ALLEGRO_TIMER* timer = NULL;
    ALLEGRO_EVENT_QUEUE* fila_eventos = NULL;

    // Inicialização
    if (!al_init()) {
        return 1;
    }

    // Teclado
    if (!al_install_keyboard()) {
        return 1;
    }

    // Display
    display = al_create_display(largura_t, altura_t);

    if (!display) {
        return 1;
    }

    // Addons
    if (!al_init_image_addon()) {
        al_destroy_display(display);
        return 1;
    }

    al_init_primitives_addon();

    // Timer
    timer = al_create_timer(1.0 / FPS);

    if (!timer) {
        al_destroy_display(display);
        return 1;
    }

    // Sprites do personagem
    ALLEGRO_BITMAP* rudanBaixo1 = al_load_bitmap("assets/sprites/spritesRudan/rudanBaixo1.png");
    ALLEGRO_BITMAP* rudanBaixo2 = al_load_bitmap("assets/sprites/spritesRudan/rudanBaixo2.png");

    ALLEGRO_BITMAP* rudanCima1 = al_load_bitmap("assets/sprites/spritesRudan/rudanCima1.png");
    ALLEGRO_BITMAP* rudanCima2 = al_load_bitmap("assets/sprites/spritesRudan/rudanCima2.png");

    ALLEGRO_BITMAP* rudanDireita1 = al_load_bitmap("assets/sprites/spritesRudan/rudanDireita1.png");
    ALLEGRO_BITMAP* rudanDireita2 = al_load_bitmap("assets/sprites/spritesRudan/rudanDireita2.png");

    ALLEGRO_BITMAP* rudanEsquerda1 = al_load_bitmap("assets/sprites/spritesRudan/rudanEsquerda1.png");
    ALLEGRO_BITMAP* rudanEsquerda2 = al_load_bitmap("assets/sprites/spritesRudan/rudanEsquerda2.png");

    ALLEGRO_BITMAP* mapa1 = al_load_bitmap("assets/sprites/spritesRudan/telaMar.png");

    // Sprites do barco
    ALLEGRO_BITMAP* barcoDireita = al_load_bitmap("assets/sprites/spritesRudan/barcoDireita.png");
    ALLEGRO_BITMAP* barcoEsquerda = al_load_bitmap("assets/sprites/spritesRudan/barcoEsquerda.png");

    // Verifica se os sprites foram carregados
    if (!rudanBaixo1 || !rudanBaixo2 || !rudanCima1 || !rudanCima2 ||
        !rudanDireita1 || !rudanDireita2 || !rudanEsquerda1 || !rudanEsquerda2 ||
        !mapa1 || !barcoDireita || !barcoEsquerda) {

        al_show_native_message_box(display, "Erro!", "Erro ao carregar os sprites.", NULL, 0);

        if (rudanBaixo1) al_destroy_bitmap(rudanBaixo1);
        if (rudanBaixo2) al_destroy_bitmap(rudanBaixo2);
        if (rudanCima1) al_destroy_bitmap(rudanCima1);
        if (rudanCima2) al_destroy_bitmap(rudanCima2);
        if (rudanDireita1) al_destroy_bitmap(rudanDireita1);
        if (rudanDireita2) al_destroy_bitmap(rudanDireita2);
        if (rudanEsquerda1) al_destroy_bitmap(rudanEsquerda1);
        if (rudanEsquerda2) al_destroy_bitmap(rudanEsquerda2);
        if (mapa1) al_destroy_bitmap(mapa1);
        if (barcoDireita) al_destroy_bitmap(barcoDireita);
        if (barcoEsquerda) al_destroy_bitmap(barcoEsquerda);

        al_destroy_timer(timer);
        al_destroy_display(display);
        return 1;
    }

    // Tamanho do barco
    int barco_largura = al_get_bitmap_width(barcoDireita);
    int barco_altura = al_get_bitmap_height(barcoDireita);

    // Tamanho do tiro
    const float fogo_raio = 5.0f;

    // Fila de eventos
    fila_eventos = al_create_event_queue();

    if (!fila_eventos) {
        al_destroy_bitmap(rudanBaixo1);
        al_destroy_bitmap(rudanBaixo2);
        al_destroy_bitmap(rudanCima1);
        al_destroy_bitmap(rudanCima2);
        al_destroy_bitmap(rudanDireita1);
        al_destroy_bitmap(rudanDireita2);
        al_destroy_bitmap(rudanEsquerda1);
        al_destroy_bitmap(rudanEsquerda2);
        al_destroy_bitmap(mapa1);
        al_destroy_bitmap(barcoDireita);
        al_destroy_bitmap(barcoEsquerda);
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

                if (!fogo_ativo) {

                    fogo_ativo = true;

                    if (direcao_atual == DIRECAO_CIMA) {
                        fogo_x = pos_x + 20;
                        fogo_y = pos_y;
                        fogo_dx = 0.0f;
                        fogo_dy = -velocidade_tiro;
                    }
                    else if (direcao_atual == DIRECAO_BAIXO) {
                        fogo_x = pos_x + 20;
                        fogo_y = pos_y + 40;
                        fogo_dx = 0.0f;
                        fogo_dy = velocidade_tiro;
                    }
                    else if (direcao_atual == DIRECAO_DIREITA) {
                        fogo_x = pos_x + 40;
                        fogo_y = pos_y + 20;
                        fogo_dx = velocidade_tiro;
                        fogo_dy = 0.0f;
                    }
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

            // Movimento do personagem
            if (teclas[CIMA]) pos_y -= velocidade;
            if (teclas[BAIXO]) pos_y += velocidade;
            if (teclas[ESQUERDA]) pos_x -= velocidade;
            if (teclas[DIREITA]) pos_x += velocidade;

            // Tamanho do personagem
            int largura_personagem = al_get_bitmap_width(rudanBaixo1);
            int altura_personagem = al_get_bitmap_height(rudanBaixo1);

            // Limita o personagem à tela
            if (pos_x < 0) pos_x = 0;
            if (pos_y < 0) pos_y = 0;

            if (pos_x + largura_personagem > largura_t) pos_x = largura_t - largura_personagem;
            if (pos_y + altura_personagem > altura_t) pos_y = altura_t - altura_personagem;

            // Verifica se está andando
            bool andando = teclas[CIMA] || teclas[BAIXO] || teclas[DIREITA] || teclas[ESQUERDA];

            // Animação
            if (andando) {
                contador_animacao++;

                if (contador_animacao >= velocidade_animacao) {
                    contador_animacao = 0;
                    frame_atual++;

                    if (frame_atual >= 2) frame_atual = 0;
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

                if (fogo_x + fogo_raio < 0 || fogo_x - fogo_raio > largura_t ||
                    fogo_y + fogo_raio < 0 || fogo_y - fogo_raio > altura_t) {
                    fogo_ativo = false;
                }
            }

            // Colisão do tiro com o barco
            if (barcoVivo && fogo_ativo) {

                if (fogo_x + fogo_raio >= barco_x &&
                    fogo_x - fogo_raio <= barco_x + barco_largura &&
                    fogo_y + fogo_raio >= barco_y &&
                    fogo_y - fogo_raio <= barco_y + barco_altura) {

                    barcoVivo = false;
                    fogo_ativo = false;
                }
            }

            // Desenha o mapa
            al_draw_bitmap(mapa1, 0, 0, 0);

            // Sprite atual
            ALLEGRO_BITMAP* sprite_atual = NULL;

            if (direcao_atual == DIRECAO_BAIXO) {
                sprite_atual = (frame_atual == 0) ? rudanBaixo1 : rudanBaixo2;
            }
            else if (direcao_atual == DIRECAO_CIMA) {
                sprite_atual = (frame_atual == 0) ? rudanCima1 : rudanCima2;
            }
            else if (direcao_atual == DIRECAO_DIREITA) {
                sprite_atual = (frame_atual == 0) ? rudanDireita1 : rudanDireita2;
            }
            else if (direcao_atual == DIRECAO_ESQUERDA) {
                sprite_atual = (frame_atual == 0) ? rudanEsquerda1 : rudanEsquerda2;
            }

            // Desenha o personagem
            if (sprite_atual) {
                al_draw_bitmap(sprite_atual, pos_x, pos_y, 0);
            }

            // Movimento e desenho do barco
            if (barcoVivo) {

                switch (barco_direcao) {

                case 0:
                    al_draw_bitmap(barcoDireita, barco_x, barco_y, 0);
                    barco_x += 1.0f;

                    if (barco_x >= largura_t - barco_largura) {
                        barco_x = largura_t - barco_largura;
                        barco_direcao = 1;
                    }
                    break;

                case 1:
                    al_draw_bitmap(barcoEsquerda, barco_x, barco_y, 0);
                    barco_x -= 1.0f;

                    if (barco_x <= 0) {
                        barco_x = 0;
                        barco_direcao = 0;
                    }
                    break;
                }
            }

            // Desenha o tiro
            if (fogo_ativo) {
                al_draw_filled_circle(fogo_x, fogo_y, fogo_raio, al_map_rgb(255, 0, 0));
            }

            // Atualiza a tela
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
    al_destroy_bitmap(mapa1);
    al_destroy_bitmap(barcoDireita);
    al_destroy_bitmap(barcoEsquerda);

    al_destroy_timer(timer);
    al_destroy_event_queue(fila_eventos);
    al_destroy_display(display);

    return 0;
}