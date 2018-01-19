#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <time.h>

#define TempoMoverBarra 6
#define TempoMoverInimigos 18
#define TempoMoverBalaJogador 3
#define TempoMoverNoBolinha 4

int TempoMoverBarra_DeltaTime=0, TempoMoverBalaJogador_DeltaTime=0,
    TempoMoverNoBolinha_DeltaTime=0, TempoMoverInimigos_DeltaTime=0;

SDL_Renderer * render;
SDL_Window * janela;
SDL_Event evento;

SDL_Texture *Textu_Cenario;
SDL_Texture *Textu_Bolinha;
SDL_Texture *Textu_Barra;
SDL_Texture *Textu_Inimigo;
SDL_Texture *Textu_Espinhos;
SDL_Texture *Textu_Menu;

TTF_Font *text_font;
SDL_Texture *textTexto;
SDL_Color font_color = {255, 255, 255};

Mix_Music * musica = NULL;
Mix_Chunk * tiro = NULL;
Mix_Chunk * InimigoColisao = NULL;
Mix_Chunk * FimDeJogo = NULL;

typedef struct{
    int x;
    int y;
    int w;
    int h;
    int DirecaoMover; //-1 -> Esquerda     1 -> Direita.            //Tambëm usada como ativo para outros objetos.
    int FoiJogada; //Usada nos inimigos para salvar a posição "x" sorteada na matriz.
} SpriteSimples;

SpriteSimples Struc_Barra = {290, 400, 60, 18, 0, 0};
SpriteSimples Struc_Bolinha = {308, 379, 22, 22, 0, 0};
SpriteSimples Struc_Inimigos[5];

int rotacao=0, PosicoesInimigo[11]={30, 83, 136, 189, 242, 295, 348, 401, 454, 507, 560}, i=0,
    PosicoesInimigoUsadas[11]={0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, MaiorPontuacao=0, Pontuacao=0;

void Func_CarimbarTexto(int x, int y, int Numero){
    char texto[9];
    itoa(Numero, texto, 10);
    SDL_Surface * surText = TTF_RenderText_Solid(text_font, texto, font_color);
    textTexto = SDL_CreateTextureFromSurface(render, surText);

    int w=110, h=110;
    SDL_QueryTexture(textTexto, 0, NULL, &w, &h);
    SDL_Rect rOrigem = {0, 0, w, h};
	SDL_Rect rDestino = {x, y, w, h};
    SDL_RenderCopy(render, textTexto, &rOrigem, &rDestino);

    SDL_FreeSurface(surText);
    free(surText);
    surText=NULL;
    SDL_DestroyTexture(textTexto);
    free(textTexto);
}

void Func_PressioneEnter(){
    if (MaiorPontuacao < Pontuacao){ //Ve sea pontuação do jogador foi maior que a ja salva.
        MaiorPontuacao = Pontuacao;
    }

    int qtd = SDL_PollEvent(&evento);
    Func_AtuallizarTela();
    //Printa a mensaggem de menu.
    SDL_Rect mOrigem = {0, 0, 409, 281};
    SDL_Rect mDestino = {130, 80, 408, 281};
    SDL_RenderCopy(render, Textu_Menu, &mOrigem, &mDestino);
    Func_CarimbarTexto(316, 335, MaiorPontuacao);
    SDL_RenderPresent(render);

    do{
        qtd = SDL_PollEvent(&evento);
        if (qtd > 0){
            if (evento.type == SDL_KEYDOWN){
                if (evento.key.keysym.sym == SDLK_RETURN || evento.key.keysym.sym == SDLK_RETURN2){
                    break;
                }
            }
        }
    }while(1);
    //Zera as variáveis para iniciar um novo jogo.
    TempoMoverBarra_DeltaTime=0; TempoMoverBalaJogador_DeltaTime=0;
    TempoMoverNoBolinha_DeltaTime=0; TempoMoverInimigos_DeltaTime=0;
    Struc_Barra.x=290; Struc_Barra.DirecaoMover=0;
    Struc_Bolinha.FoiJogada=0; Struc_Bolinha.x=308; Struc_Bolinha.y=379;
    Pontuacao=0;
    for(i=0; i<11; i++){
        PosicoesInimigoUsadas[i]=0;
    }
    Func_CriarInimigos();
}

void Func_SortearXinimigo(int QualInimigo){
    int Temp_x;
    PosicoesInimigoUsadas[QualInimigo]=0;
    do{
        Temp_x=rand()%11;
        if (PosicoesInimigoUsadas[Temp_x]==0){
            break;
        }
    }while(1);
    Struc_Inimigos[QualInimigo].x=PosicoesInimigo[Temp_x];
    Struc_Inimigos[QualInimigo].FoiJogada=Temp_x;
    PosicoesInimigoUsadas[Temp_x]=1;
}
void Func_CriarInimigos(){

    for(i=0; i<5; i++){
        Func_SortearXinimigo(i);
        Struc_Inimigos[i].y=(rand()%640)*-1;
        Struc_Inimigos[i].w=46;
        Struc_Inimigos[i].h=37;
        Struc_Inimigos[i].DirecaoMover=1;
    }
}

void Func_CaregarImagens(){
    //Cenario.
    SDL_Surface * temp0 = IMG_Load("./Arquivos/Cenario.png");
    Textu_Cenario = SDL_CreateTextureFromSurface(render, temp0);

    //Barra de movimenta,ão.
    SDL_Surface * temp1 = IMG_Load("./Arquivos/Barra.png");
    Textu_Barra = SDL_CreateTextureFromSurface(render, temp1);

    //bolinha do jogo.
    SDL_Surface * temp2 = IMG_Load("./Arquivos/Bolinha.png");
    Textu_Bolinha = SDL_CreateTextureFromSurface(render, temp2);

    //inimigo.
    SDL_Surface * temp3 = IMG_Load("./Arquivos/Inimigo.png");
    Textu_Inimigo = SDL_CreateTextureFromSurface(render, temp3);

    //Espinhos.
    SDL_Surface * temp4 = IMG_Load("./Arquivos/Espinhos.png");
    Textu_Espinhos = SDL_CreateTextureFromSurface(render, temp4);

    //Menu.
    SDL_Surface * temp5 = IMG_Load("./Arquivos/Menu.png");
    Textu_Menu = SDL_CreateTextureFromSurface(render, temp5);


    SDL_FreeSurface(temp0); SDL_FreeSurface(temp1);
    SDL_FreeSurface(temp2); SDL_FreeSurface(temp3);
    SDL_FreeSurface(temp4); SDL_FreeSurface(temp5);
}
void Func_CaregarSonds(){
    int flags = MIX_INIT_OGG|MIX_INIT_MOD|MIX_INIT_MP3;
    int initted=Mix_Init(flags);
    if( initted&flags != flags) {
        printf("Mix_Init: Failed to init required ogg and mod support!\n");
        printf("Mix_Init: %s\n", Mix_GetError());
    }

    if(SDL_Init(SDL_INIT_AUDIO)==-1) {
        printf("SDL_Init: %s\n", SDL_GetError());
        return -1;
    }

    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) == -1 ) {
        printf("Erro ao inicializar o sistema de som \n");
        return -1;
    }

    //Carrega os sons do jogo.
    musica = Mix_LoadMUS("./Arquivos/Musica.mp3");
    tiro = Mix_LoadWAV("./Arquivos/Tiro.wav");
    InimigoColisao = Mix_LoadWAV("./Arquivos/InimigoColisao.wav");
    FimDeJogo = Mix_LoadWAV("./Arquivos/FimDeJogo.wav");


    if (musica==NULL || tiro==NULL || InimigoColisao==NULL || FimDeJogo==NULL){
        printf("Algum son nao foi encontrado!!! \n");
    }

    Mix_PlayMusic( musica, -1 );
}
void Func_LerTecaldo(){
    int qtd = SDL_PollEvent(&evento);
    if (qtd > 0){
        if (evento.type == SDL_KEYDOWN){
            if (evento.key.keysym.sym == SDLK_LEFT){ //Movimentação do jogador.
                Struc_Barra.DirecaoMover=-1;
            }
            if (evento.key.keysym.sym == SDLK_RIGHT){ //Movimentação do jogador.
                Struc_Barra.DirecaoMover=1;
            }
            if (evento.key.keysym.sym == SDLK_SPACE && Struc_Bolinha.FoiJogada==0){ //Tiro do jogador.
                Struc_Bolinha.DirecaoMover=-1;
                Struc_Bolinha.FoiJogada=1;
                Mix_PlayChannel(-1, tiro, 0);
            }
        }
        else if (evento.type == SDL_KEYUP){
            if (evento.key.keysym.sym == SDLK_LEFT && Struc_Barra.DirecaoMover!=1){
                Struc_Barra.DirecaoMover=0;
            }
            if (evento.key.keysym.sym == SDLK_RIGHT && Struc_Barra.DirecaoMover!=-1){
                Struc_Barra.DirecaoMover=0;
            }
        }
    }
}
void Func_Mecanica(){
    TempoMoverBarra_DeltaTime+=1;
    if (TempoMoverBarra_DeltaTime>TempoMoverBarra){
        TempoMoverBarra_DeltaTime=0;
        if(Struc_Barra.DirecaoMover == -1 && Struc_Barra.x>30){
            Struc_Barra.x-=1;
            if (Struc_Bolinha.FoiJogada==0){
                Struc_Bolinha.x-=1;
            }
        }
        else if (Struc_Barra.DirecaoMover == 1 && Struc_Barra.x<550){
            Struc_Barra.x+=1;
            if (Struc_Bolinha.FoiJogada==0){
                Struc_Bolinha.x+=1;
            }
        }
    }

    //Movimentação da bolinha.
    TempoMoverNoBolinha_DeltaTime+=1;
    if (TempoMoverNoBolinha_DeltaTime>TempoMoverNoBolinha && Struc_Bolinha.FoiJogada==1){
        TempoMoverNoBolinha_DeltaTime=0;
        Struc_Bolinha.y+=Struc_Bolinha.DirecaoMover;
    }
    if (Struc_Bolinha.y<55 || Struc_Bolinha.y>420){ //Caso a bolinha enconste nos espinho ou sai for ada tela o jogo acaba.
        Mix_PlayChannel(-1, FimDeJogo, 0);
        Func_PressioneEnter();
    }

    //Movimentação dos inimigos.
    TempoMoverInimigos_DeltaTime+=1;
    if (TempoMoverInimigos_DeltaTime>TempoMoverInimigos){
        TempoMoverInimigos_DeltaTime=0;
        for(i=0; i<5; i++){
            Struc_Inimigos[i].y+=1;
             if( Struc_Inimigos[i].y>460){
                Struc_Inimigos[i].y=(rand()%640)*-1;
                Func_SortearXinimigo(i);
            }
        }
     }

}
void Func_Colisao(){
    SDL_Rect obj1 = {Struc_Bolinha.x, Struc_Bolinha.y, Struc_Bolinha.w, Struc_Bolinha.h}; //Bolinha.
    SDL_Rect obj3 = {Struc_Barra.x, Struc_Barra.y, Struc_Barra.w, Struc_Barra.h}; //Barra de movimentação.
    for(i=0; i<5; i++){
        SDL_Rect obj2 = {Struc_Inimigos[i].x, Struc_Inimigos[i].y, Struc_Inimigos[i].w, Struc_Inimigos[i].h}; //Inimigo.
        if (SDL_HasIntersection((&obj1), (&obj2)) && Struc_Bolinha.FoiJogada==1){ //Caso a bolinha colida com o inimigo, a movmentação dela vair mudar para baixo.
            Struc_Bolinha.DirecaoMover=1;
            Struc_Inimigos[i].y=(rand()%640)*-1;
            Func_SortearXinimigo(i);
            Pontuacao+=10;
            Mix_PlayChannel(-1, InimigoColisao, 0);
        }
        if (SDL_HasIntersection((&obj3), (&obj2))){ //Inimigo colidir com a barra de movimentacao.
            Mix_PlayChannel(-1, FimDeJogo, 0);
            Func_PressioneEnter();
        }
    }
    if (SDL_HasIntersection((&obj1), (&obj3)) && Struc_Bolinha.y<383){ //Caso a bolinha colida com a barra de movimentação, ela vai volta ao estato zero.
        Struc_Bolinha.FoiJogada=0;
    }
}
void Func_AtuallizarTela(){
    //Função para limpar a "janela" renderizada.
    SDL_SetRenderDrawColor(render, 0, 0, 0, 1);
    SDL_RenderClear(render);

    //Printa o cenario.
    SDL_Rect cOrigem = {0, 0, 640, 456};
    SDL_Rect cDestino = {-30, -10, 700, 500};
    SDL_RenderCopy(render, Textu_Cenario, &cOrigem, &cDestino);

    //Printa a bolinha na tela.
    SDL_Rect blOrigem = {0, 0, Struc_Bolinha.w, Struc_Bolinha.h};
    SDL_Rect blDestino = {Struc_Bolinha.x, Struc_Bolinha.y, Struc_Bolinha.w, Struc_Bolinha.h};
    SDL_RenderCopy(render, Textu_Bolinha, &blOrigem, &blDestino);

    //Printa o barra de movimentação.
    SDL_Rect bOrigem = {0, 0, Struc_Barra.w, Struc_Barra.h};
    SDL_Rect bDestino = {Struc_Barra.x, Struc_Barra.y, Struc_Barra.w, Struc_Barra.h};
    SDL_RenderCopy(render, Textu_Cenario, &bOrigem, &bDestino);

    //Printa o inimigo.
    for(i=0; i<5; i++){
        SDL_Rect iOrigem = {0, 0, Struc_Inimigos[i].w, Struc_Inimigos[i].h};
        SDL_Rect iDestino = {Struc_Inimigos[i].x, Struc_Inimigos[i].y, Struc_Inimigos[i].w, Struc_Inimigos[i].h};
        SDL_RenderCopy(render, Textu_Inimigo, &iOrigem, &iDestino);
    }

    //Printa o espinhos.
    SDL_Rect eOrigem = {0, 0, 640, 77};
    SDL_Rect eDestino = {-30, -20, 700, 77};
    SDL_RenderCopy(render, Textu_Espinhos, &eOrigem, &eDestino);
}

void Func_Jogar(){
    Func_LerTecaldo();
    Func_Mecanica();
    Func_Colisao();
    Func_AtuallizarTela();
    Func_CarimbarTexto(10, 10, Pontuacao);
    SDL_RenderPresent(render);
}

int main(int c, char *arts[]){
    srand(time(NULL));
    Func_CriarInimigos();
    IMG_Init(IMG_INIT_JPG + IMG_INIT_PNG);
    SDL_Init(SDL_INIT_EVERYTHING);
    janela = SDL_CreateWindow("PongAlone",100, 100, 640, 420, SDL_WINDOW_SHOWN);
    render = SDL_CreateRenderer(janela, -1 , 0);
    if (janela != NULL && render != NULL && TTF_Init() >=0){
        text_font = TTF_OpenFont("./Arquivos/arial.ttf", 28);
        Func_CaregarImagens();
        Func_CaregarSonds();
        Func_PressioneEnter();
        while(1){
            Func_Jogar();
        }
    }
}
