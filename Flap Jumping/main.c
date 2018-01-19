#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <time.h>

#define MaxTimeMoving 20
#define TimeMoveKano 4
#define TimeFisica 50
#define MinTimeMoving 5
#define TimeChangeSprite 150
#define TimePressSpace 1300
#define TimeWinPontos 80
#define TimePlayJumpSound 35

int Moving=16, KanoY[20]={440,420,360,320,280,250,230,200,180,160,425,385,345,300,265,225,200,165,265,195}, KanoYTamano=20;
int TimeMoving_DeltaTime=0, TimeUping_DeltaTime=0, TimeMoveKano_DeltaTime=0, TimeFisica_DeltaTime=0, TimeChangeSprite_DeltaTime=0,
    TimePressSpace_DeltaTime=0, Pontuacao=0, MaiorPontuacao=0, TimeWinPontos_DeltaTime=0, TimePlayJumpSound_DeltaTime = 100;

SDL_Window * janela;;
SDL_Renderer * render;
SDL_Texture * bird;
SDL_Texture * kano;
SDL_Texture * sky;
SDL_Texture * PressSapce;
SDL_Event evento;

TTF_Font *text_font;
SDL_Texture *textTexto;
SDL_Color font_color = {0, 0, 0};

Mix_Chunk * Sound_Hit = NULL;
Mix_Chunk * Sound_Point = NULL;
Mix_Chunk * Sound_Jump = NULL;
Mix_Music * Musica = NULL;

int i, Jumping=0;

typedef struct{
    int x;
    int y;
    int w;
    int h;
    int Ativo;
    int ImagePoint;
} Sprites;

Sprites Bird={30,200, 56, 45,1,0};
Sprites KanoDown;
Sprites KanoUp;

void PressSpaceToStart(){
    if(Pontuacao>MaiorPontuacao){
        MaiorPontuacao=Pontuacao;
    }
    int Largura=500, Altura=500;
    TimePressSpace_DeltaTime=0;
    AtualizarPrints();
    SDL_Rect pOrigem = {0, 0, 1626, 1626};
    SDL_Rect pDestino = {70, 5, Largura, Altura};
    SDL_RenderCopy(render, PressSapce, &pOrigem, &pDestino);
    PrintarPontuacao(363, 428, MaiorPontuacao);
    SDL_RenderPresent(render);
    do{
        int KeyBoard = SDL_PollEvent(&evento);
        TimePressSpace_DeltaTime+=1;
        if(KeyBoard>0 && TimePressSpace_DeltaTime>TimePressSpace){
            if(evento.type == SDL_KEYDOWN){
                if(evento.key.keysym.sym == SDLK_RETURN){
                    RestartFase();
                    break;
                }
            }
        }
    }while(1);
    Jumping=0;
    Pontuacao=0;
    TimeWinPontos_DeltaTime=0;
}

void RestartFase(){
    KanoLoad();
    Bird.y = 200;
    TimePlayJumpSound_DeltaTime = 0;
}

void PrintarPontuacao(int x, int y, int ponto){
    char char_pontuacao[9];
    itoa(ponto, char_pontuacao, 10);
    SDL_Surface * surText = TTF_RenderText_Solid(text_font, char_pontuacao, font_color);
    textTexto = SDL_CreateTextureFromSurface(render, surText);
    int w=110, h=110;
    SDL_QueryTexture(textTexto, 0, NULL, &w, &h);
    SDL_Rect rOrigem = {0, 0, w, h};
	SDL_Rect rDestino = {x, y, w, h};
    SDL_RenderCopy(render, textTexto, &rOrigem, &rDestino);

    SDL_FreeSurface(surText);
    free(surText);
    surText = NULL;
    SDL_DestroyTexture(textTexto);
    free(textTexto);
}

void KanoLoad(){
    //Cano de baixo.
    KanoDown.x=-650;
    KanoDown.y=KanoY[rand()%KanoYTamano];
    KanoDown.w=72;
    KanoDown.h=600;

    //Cano de cima.
    KanoUp.x=-650;
    KanoUp.y=KanoDown.y-720;
    KanoUp.w=72;
    KanoUp.h=600;
}


void ImageLoad(){
    //Carrega o paçaro.
    SDL_Surface * temp0 = IMG_Load("./Sprites/Bird.png");
    bird = SDL_CreateTextureFromSurface(render, temp0);

    //Carrega o cano de baixo.
    SDL_Surface * temp1 = IMG_Load("./Sprites/Kano.png");
    kano = SDL_CreateTextureFromSurface(render, temp1);

    //Carregar sky.
    SDL_Surface * temp2 = IMG_Load("./Sprites/Sky.png");
    sky = SDL_CreateTextureFromSurface(render, temp2);

    //Carregar a tela de precionar spaço.
    SDL_Surface * temp3 = IMG_Load("./Sprites/PressSpaceToStart.png");
    PressSapce = SDL_CreateTextureFromSurface(render, temp3);

    //Limpa as areas dememórias criadas.
    SDL_FreeSurface(temp0); SDL_FreeSurface(temp1);
    SDL_FreeSurface(temp2); SDL_FreeSurface(temp3);
}
void SoundLoad(){
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

    //Load the music
    Sound_Hit = Mix_LoadWAV  ("./Sprites/Hit.wav");
    Sound_Jump = Mix_LoadWAV  ("./Sprites/Jump.wav");
    Sound_Point = Mix_LoadWAV  ("./Sprites/Point.wav");
    Musica = Mix_LoadMUS ("./Sprites/Music.mp3");

    Mix_PlayMusic( Musica, -1 );

    //Mix_VolumeMusic(MIX_MAX_VOLUME/4);

    if (Sound_Hit == NULL || Sound_Jump == NULL || Sound_Point == NULL || Musica == NULL){
        printf("Algum efeito sonoro nao localizada!!! \n");
    }
}
void AtualizarPrints(){
    // Limpar a tela do jogo.
    SDL_SetRenderDrawColor(render, 0, 0, 0, 1);
    SDL_RenderClear(render);

    //Printa o cenario
    SDL_Rect cOrigem = {0, 0, 1045, 776};
    SDL_Rect cDestino = {0, 0, 640, 480};
    SDL_RenderCopy(render, sky, &cOrigem, &cDestino);

    //Printa o canos de baixo.
    SDL_Rect kdOrigem = {0, 0, 112, 640};
    SDL_Rect kdDestino = {KanoDown.x, KanoDown.y, KanoDown.w, KanoDown.h};
    SDL_RenderCopy(render, kano, &kdOrigem, &kdDestino);
    //Printa o canos de cima.
    SDL_Rect kuOrigem = {0, 0, 112, 640};
    SDL_Rect kuDestino = {KanoUp.x, KanoUp.y, KanoUp.w, KanoUp.h};
    SDL_RenderCopy(render, kano, &kuOrigem, &kuDestino);

    //Printa o passaro.
    TimeChangeSprite_DeltaTime+=1;
    if(TimeChangeSprite_DeltaTime>TimeChangeSprite){

        TimeChangeSprite_DeltaTime=0;
        if(Bird.ImagePoint==0){
            Bird.ImagePoint=769;
        }
        else{
            Bird.ImagePoint=0;
        }
    }
    SDL_Rect bOrigem = {Bird.ImagePoint, 0, 769, 617};
    SDL_Rect bDestino = {Bird.x, Bird.y, Bird.w, Bird.h};
    SDL_RenderCopy(render, bird, &bOrigem, &bDestino);


    //Atualiza a tela do jogo.

}

void LerTeclado(){
    int KeyBoard = SDL_PollEvent(&evento);
    if(KeyBoard>0){
        if(evento.type == SDL_KEYDOWN){
            if(evento.key.keysym.sym == SDLK_SPACE && Jumping==0){
                Jumping=1;
                Moving=MaxTimeMoving;
            }
        }
        else if(evento.type == SDL_KEYUP && Jumping==1){
            if(evento.key.keysym.sym == SDLK_SPACE){
                Jumping=0;
                Moving=MaxTimeMoving;
            }
        }
    }
}

void Mecanica(){
    TimeMoving_DeltaTime+=1; //Tempo de movimentação independente se é para cima ou para baixo.
    if(TimeMoving_DeltaTime>Moving){
        TimeMoving_DeltaTime=0;
        TimePlayJumpSound_DeltaTime+=1;
        if(Jumping==1 && Bird.y > - 60){
            Bird.y-=1;
            if (TimePlayJumpSound_DeltaTime > TimePlayJumpSound){
                TimePlayJumpSound_DeltaTime = 0;
                Mix_PlayChannel( -1, Sound_Jump, 0 );
            }
        }
        else if (Bird.y < 480){
            Bird.y+=1;
        }
    }

    TimeFisica_DeltaTime+=1;
    if(TimeFisica_DeltaTime>TimeFisica && Moving>MinTimeMoving){
        TimeFisica_DeltaTime=0;
        Moving-=1;
    }

    //Pontuação
    TimeWinPontos_DeltaTime+=1;
    if(Bird.x > KanoDown.x - 5 && Bird.x < KanoDown.x + 5 && TimeWinPontos_DeltaTime>TimeWinPontos){
        Mix_PlayChannel( -1, Sound_Point, 0 );
        TimeWinPontos_DeltaTime=0;
        Pontuacao+=1;
        Mix_PlayChannel( -1, Sound_Point, 0 );
    }

}

void KanoIA(){
    TimeMoveKano_DeltaTime+=1; //Movimentação do cano.
    if(TimeMoveKano_DeltaTime>TimeMoveKano){
        TimeMoveKano_DeltaTime=0;
        KanoDown.x-=1;
        KanoUp.x-=1;
    }

    if(KanoDown.x<-122){
        KanoDown.x=650;
        KanoDown.y=KanoY[rand()%KanoYTamano];

        KanoUp.x=650;
        KanoUp.y=KanoDown.y-720;
    }
}

void CalculaColisao(){
    SDL_Rect obj1 = {Bird.x, Bird.y, Bird.w, Bird.h};
    SDL_Rect obj2 = {KanoDown.x, KanoDown.y, KanoDown.w, KanoDown.h};
    if(SDL_HasIntersection((&obj1), (&obj2))){ // || Bird.y<-30
        Mix_PlayChannel( -1, Sound_Hit, 0 );
        PressSpaceToStart();
    }

    SDL_Rect obj3 = {KanoUp.x, KanoUp.y, KanoUp.w, KanoUp.h};
    if(SDL_HasIntersection((&obj1), (&obj3))){ // || Bird.y>466
        Mix_PlayChannel( -1, Sound_Hit, 0 );
        PressSpaceToStart();
    }

}

void Jogando(){
    LerTeclado();
    Mecanica();
    KanoIA();
    AtualizarPrints();
    PrintarPontuacao(10, 10, Pontuacao);
    SDL_RenderPresent(render);
    CalculaColisao();
}

int main(int c, char *arts[]){
    srand(time(NULL));
    IMG_Init(IMG_INIT_JPG + IMG_INIT_PNG);
    SDL_Init(SDL_INIT_EVERYTHING);
    janela = SDL_CreateWindow("ShootingGame", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
    if(janela != NULL){
        render = SDL_CreateRenderer(janela, -1, 0);
        if(render != NULL){
            if(TTF_Init() >=0){
                text_font =  TTF_OpenFont("arial.ttf", 30);
                if(text_font == 0){
                    printf("erro na font!!!\n");
                }
                ImageLoad();
                SoundLoad();
                KanoLoad();
                PressSpaceToStart();
                while(1){
                    Jogando();
                }
            }
        }
    }
    Mix_Quit();
    Mix_CloseAudio();
}
