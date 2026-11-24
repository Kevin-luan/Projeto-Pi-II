#include <stdio.h>
#include <stdbool.h>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
// NOVO
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#include "fase.h"

enum TECLAS {
    CIMA, BAIXO, DIREITA, ESQUERDA
};

enum DIRECAO {
    DIRECAO_BAIXO,
    DIRECAO_CIMA,
    DIRECAO_DIREITA,
    DIRECAO_ESQUERDA
};

void fase1E(void) {

    // Configurações
    const int largura_t = 1920;
    const int altura_t = 1200;
    const int FPS = 60;

    // NPC (canto superior esquerdo)
    const float npc_x = 250.0f;
    const float npc_y = 150.0f;
    const int npc_escala = 3;   // o sprite é 32x32, então 3 = 96x96 na tela

    // NOVO - Fala do NPC
    const char* texto_npc =
        "Guerreiro, rápido! Nossas praias estão sob ataque! "
        "Homens de pele clara vindos de além-mar desembarcaram na costa. "
        "O líder da vanguarda, Nicolau Coelho, foi o primeiro a cravar os pés "
        "em Pindorama — nossa amada terra das palmeiras. "
        "Empunhe suas armas e lute conosco para expulsar esses invasores";

    // NOVO - Distância para abrir o diálogo (em pixels)
    const float distancia_dialogo = 200.0f;

    // Posição e velocidade do jogador
    float pos_x = 500.0f;
    float pos_y = 800.0f;
    const float velocidade = 5.0f;

    // Teclas
    bool teclas[] = { false, false, false, false };

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

    bool portal = false;

    // Inicialização
    if (!al_init()) {
        return;
    }

    // Teclado
    if (!al_install_keyboard()) {
        return;
    }

    // Display
    display = al_create_display(largura_t, altura_t);

    if (!display) {
        return;
    }

    // Addons
    if (!al_init_image_addon()) {
        al_destroy_display(display);
        return;
    }

    al_init_primitives_addon();

    // NOVO
    al_init_font_addon();
    al_init_ttf_addon();

    // Timer
    timer = al_create_timer(1.0 / FPS);

    if (!timer) {
        al_destroy_display(display);
        return;
    }

    // Imagem de fundo
    ALLEGRO_BITMAP* mapa3 =
        al_load_bitmap("assets/sprites/spritesRudan/mapaE1.jpg");

    if (!mapa3) {
        printf("Erro ao carregar o mapaE1\n");
        al_destroy_timer(timer);
        al_destroy_display(display);
        return;
    }

    // Sprite do NPC
    ALLEGRO_BITMAP* npcMestre =
        al_load_bitmap("assets/sprites/spritesRudan/npcMestre.png");

    if (!npcMestre) {
        printf("Erro ao carregar o npcMestre\n");
        al_destroy_bitmap(mapa3);
        al_destroy_timer(timer);
        al_destroy_display(display);
        return;
    }

    // Fila de eventos
    fila_eventos = al_create_event_queue();

    if (!fila_eventos) {
        al_destroy_bitmap(mapa3);
        al_destroy_bitmap(npcMestre);
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

    // Verifica se os sprites foram carregados
    if (!rudanBaixo1 || !rudanBaixo2 ||
        !rudanCima1 || !rudanCima2 ||
        !rudanDireita1 || !rudanDireita2 ||
        !rudanEsquerda1 || !rudanEsquerda2) {

        printf("Erro ao carregar os sprites do Rudan\n");

        if (rudanBaixo1) al_destroy_bitmap(rudanBaixo1);
        if (rudanBaixo2) al_destroy_bitmap(rudanBaixo2);
        if (rudanCima1) al_destroy_bitmap(rudanCima1);
        if (rudanCima2) al_destroy_bitmap(rudanCima2);
        if (rudanDireita1) al_destroy_bitmap(rudanDireita1);
        if (rudanDireita2) al_destroy_bitmap(rudanDireita2);
        if (rudanEsquerda1) al_destroy_bitmap(rudanEsquerda1);
        if (rudanEsquerda2) al_destroy_bitmap(rudanEsquerda2);

        al_destroy_bitmap(mapa3);
        al_destroy_bitmap(npcMestre);
        al_destroy_event_queue(fila_eventos);
        al_destroy_timer(timer);
        al_destroy_display(display);
        return;
    }

    // NOVO - Fonte do diálogo (se não achar a Arial, usa a fonte padrão)
    ALLEGRO_FONT* fonte =
        al_load_ttf_font("C:/Windows/Fonts/arial.ttf", 32, 0);

    if (!fonte) {
        fonte = al_create_builtin_font();
    }

    // Tamanho do personagem
    int largura_personagem = al_get_bitmap_width(rudanBaixo1);
    int altura_personagem = al_get_bitmap_height(rudanBaixo1);

    // Registra eventos
    al_register_event_source(
        fila_eventos, al_get_keyboard_event_source()
    );

    al_register_event_source(
        fila_eventos, al_get_display_event_source(display)
    );

    al_register_event_source(
        fila_eventos, al_get_timer_event_source(timer)
    );

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

            // Limita o personagem à tela
            if (pos_x < 0) pos_x = 0;
            if (pos_y < 0) pos_y = 0;

            if (pos_x + largura_personagem > largura_t)
                pos_x = largura_t - largura_personagem;

            if (pos_y + altura_personagem > altura_t)
                pos_y = altura_t - altura_personagem;

            // Verifica se está andando
            bool andando =
                teclas[CIMA] || teclas[BAIXO] ||
                teclas[DIREITA] || teclas[ESQUERDA];

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

            // NOVO - Verifica se o jogador está perto do NPC
            float dx_npc =
                (pos_x + largura_personagem / 2.0f) -
                (npc_x + al_get_bitmap_width(npcMestre) * npc_escala / 2.0f);

            float dy_npc =
                (pos_y + altura_personagem / 2.0f) -
                (npc_y + al_get_bitmap_height(npcMestre) * npc_escala / 2.0f);

            bool perto_do_npc =
                (dx_npc * dx_npc + dy_npc * dy_npc) <
                (distancia_dialogo * distancia_dialogo);

            // Desenha o mapa ocupando toda a janela
            al_draw_scaled_bitmap(
                mapa3,
                0, 0,
                al_get_bitmap_width(mapa3),
                al_get_bitmap_height(mapa3),
                0, 0,
                largura_t,
                altura_t,
                0
            );

            // Desenha o NPC parado
            al_draw_scaled_bitmap(
                npcMestre,
                0, 0,
                al_get_bitmap_width(npcMestre),
                al_get_bitmap_height(npcMestre),
                npc_x, npc_y,
                al_get_bitmap_width(npcMestre) * npc_escala,
                al_get_bitmap_height(npcMestre) * npc_escala,
                0
            );

            // Seleciona o sprite atual
            ALLEGRO_BITMAP* sprite_atual = NULL;

            if (direcao_atual == DIRECAO_BAIXO) {
                sprite_atual =
                    (frame_atual == 0) ? rudanBaixo1 : rudanBaixo2;
            }
            else if (direcao_atual == DIRECAO_CIMA) {
                sprite_atual =
                    (frame_atual == 0) ? rudanCima1 : rudanCima2;
            }
            else if (direcao_atual == DIRECAO_DIREITA) {
                sprite_atual =
                    (frame_atual == 0) ? rudanDireita1 : rudanDireita2;
            }
            else if (direcao_atual == DIRECAO_ESQUERDA) {
                sprite_atual =
                    (frame_atual == 0) ? rudanEsquerda1 : rudanEsquerda2;
            }

            // Desenha o personagem
            if (sprite_atual) {
                al_draw_bitmap(sprite_atual, pos_x, pos_y, 0);
            }

            // NOVO - Caixa de diálogo do NPC
            if (perto_do_npc && fonte) {

                al_draw_filled_rectangle(
                    200, 880, largura_t - 200, 1150,
                    al_map_rgb(20, 20, 20)
                );

                al_draw_rectangle(
                    200, 880, largura_t - 200, 1150,
                    al_map_rgb(255, 255, 255),
                    4
                );

                al_draw_text(
                    fonte,
                    al_map_rgb(255, 215, 0),
                    230, 900,
                    0,
                    "Mestre"
                );

                al_draw_multiline_text(
                    fonte,
                    al_map_rgb(255, 255, 255),
                    230, 950,
                    largura_t - 460,
                    40,
                    0,
                    texto_npc
                );
             
                portal = true;
            }

          
            if (portal) {
                al_draw_filled_circle(
                    1500, 300, 100,
                    al_map_rgb(255, 0, 0)
                );

                if (pos_x >= 1400 && pos_x <= 1600 &&
                    pos_y >= 200 && pos_y <= 400) {

                    printf("Entrou no portal!\n");
                    fim = true;

                }
                
            }
           



            // Atualiza a tela
            al_flip_display();
        }
    }

    // Finalização
    // NOVO
    if (fonte) al_destroy_font(fonte);

    al_destroy_bitmap(rudanBaixo1);
    al_destroy_bitmap(rudanBaixo2);
    al_destroy_bitmap(rudanCima1);
    al_destroy_bitmap(rudanCima2);
    al_destroy_bitmap(rudanDireita1);
    al_destroy_bitmap(rudanDireita2);
    al_destroy_bitmap(rudanEsquerda1);
    al_destroy_bitmap(rudanEsquerda2);

    al_destroy_bitmap(mapa3);
    al_destroy_bitmap(npcMestre);
    al_destroy_event_queue(fila_eventos);
    al_destroy_timer(timer);
    al_destroy_display(display);
    fase1();
}