#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <time.h>

#define TimeChangeSprite 200
#define TimeExitDown 1400
#define TimeMovePlatform 5
#define JumpHigh 20
#define TimeWinPoint 300

int TimeChangeSprite_DeltaTime=0, TimeExitDown_DeltaTime=0, TimeMovePlatform_DeltaTime=0, TimeWinPoint_DeltaTime=0;

typedef struct{
    int x;
    int y;
    int w;
    int h;
    int i;
    int ImageIndex;
} Sprite;

Sprite Player = {20, 210, 100, 100, 0, 0};

typedef struct{
    int x;
    int y;
    int w;
    int h;
}Obj;

Obj Platform1 = {0, 310, 1981, 197}; //Tem 660 cara um dos blocks unidos.
Obj ObestaculoFloor = {640, 280, 20, 60};
Obj ObestaculoNotFloor = {640, 0, 50, 240};
Obj CaixaDeColisao = {0, 0, 37, 76}; //Só é usada para testar a colisão depulo e correndo, quando da umarasteira se usao proprio sprite.

SDL_Renderer * render;
SDL_Window * janela;

SDL_Texture * SpriteSheet;
SDL_Texture * Cenario;
SDL_Texture * Plataforma;
SDL_Texture * ObjFloor;
SDL_Texture * BoxColider;
SDL_Texture * PressEnterToPlay;

SDL_Event evento;

//Variáveis necessarias para printar o texto na tela.
TTF_Font *text_font;
SDL_Texture *textTexto;
SDL_Color font_colorWhite = {255, 255, 255};
SDL_Color font_colorBack = {0, 0, 0};

Mix_Music * musica = NULL;

int j=0, witchOne=0, HighScore=0, Score=0;

void FunctionLoadImage(){
    //Cenario.
    SDL_Surface * temp0 = IMG_Load("./Docs/Cenario.png");
    Cenario = SDL_CreateTextureFromSurface(render, temp0);

    //Cenario.
    SDL_Surface * temp1 = IMG_Load("./Docs/SpriteSheet.png");
    SpriteSheet = SDL_CreateTextureFromSurface(render, temp1);

    //Plataforma.
    SDL_Surface * temp2 = IMG_Load("./Docs/Platform1.png");
    Plataforma = SDL_CreateTextureFromSurface(render, temp2);

    //Obestaculos.
    SDL_Surface * temp3 = IMG_Load("./Docs/ObjFloor.png");
    ObjFloor = SDL_CreateTextureFromSurface(render, temp3);

    //Caixa de colisão.
    SDL_Surface * temp4 = IMG_Load("./Docs/BoxColider.png");
    BoxColider = SDL_CreateTextureFromSurface(render, temp4);

    //Menu do jogo.
    SDL_Surface * temp5 = IMG_Load("./Docs/PressEnterDoPlay.png");
    PressEnterToPlay = SDL_CreateTextureFromSurface(render, temp5);

    SDL_FreeSurface(temp0); SDL_FreeSurface(temp1); SDL_FreeSurface(temp2);
    SDL_FreeSurface(temp3); SDL_FreeSurface(temp4); SDL_FreeSurface(temp5);
}
void FunctionloadSounds(){
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

    //Load the sounds.
    musica = Mix_LoadMUS( "./Docs/Music.mp3" );

    if (musica == NULL){
        printf("Algum efeito sonoro nao foi localizada!!! \n");
    }

    Mix_PlayMusic( musica, -1 );

}
void FunctionPrintText(int x, int y, int int_Texto, SDL_Color TextColor){
    char char_Texto[9];
    itoa(int_Texto, char_Texto, 10);
    SDL_Surface * surText = TTF_RenderText_Solid(text_font, char_Texto, TextColor);
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

void FunctionPressEnter(){
    if (HighScore < Score){ //Ve sea pontuação do jogador foi maior que a ja salva.
        HighScore = Score;
    }

    int qtd = SDL_PollEvent(&evento);
    FunctionPrintImages();
    //Printa a mensaggem de menu.
    SDL_Rect mOrigem = {0, 0, 446, 259};
    SDL_Rect mDestino = {120, 20, 446, 259};
    SDL_RenderCopy(render, PressEnterToPlay, &mOrigem, &mDestino);
    FunctionPrintText(325, 255, HighScore, font_colorBack);
    SDL_RenderPresent(render);

    //Zera as variáveis para iniciar um novo jogo.
    TimeChangeSprite_DeltaTime=0; TimeExitDown_DeltaTime=0; TimeMovePlatform_DeltaTime=0; TimeWinPoint_DeltaTime=0;
    ObestaculoFloor.x=650;
    ObestaculoNotFloor.x=650;
    Score=0;
    Player.y=210; Player.i=0; Player.ImageIndex=0;

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
}
void FunctionKeyBoard(){
    int qtd = SDL_PollEvent(&evento);
    if (qtd > 0){
        if (evento.type == SDL_KEYDOWN){
            if (evento.key.keysym.sym == SDLK_DOWN && Player.ImageIndex==0){
                Player.ImageIndex = 200;
                TimeExitDown_DeltaTime=0;
            }
            if (evento.key.keysym.sym == SDLK_UP && Player.ImageIndex==0){
                Player.i=0;
                Player.ImageIndex = 100;
            }
        }
    }
}
void FunctionPrintImages(){ //Faz junto a movimentação da plataforma e dos obestaculos.
    // Limpar a tela.
    SDL_SetRenderDrawColor(render, 0, 0, 0, 1);
    SDL_RenderClear(render);

    //Printa o cenario.
    SDL_Rect cOrigem = {0, 0, 650, 420};
    SDL_Rect cDestino = {0, 0, 650, 420};
    SDL_RenderCopy(render, Cenario, &cOrigem, &cDestino);

    //Printa o sprite sheet do personagem.
    TimeChangeSprite_DeltaTime+=1; //Tempo de mudar a animação, <o FPS do sprite>.
    if (TimeChangeSprite_DeltaTime>TimeChangeSprite){
        TimeChangeSprite_DeltaTime=0;
        Player.i+=100;
        if (Player.i>=800&&Player.ImageIndex!=100){ //Zera a animação para o começo.
            Player.i=0;
        }
        if (Player.ImageIndex==100){ //Condições exclisiva para a animação de pular.
            if (Player.i>=800){
                Player.ImageIndex=0;
                Player.i=0;
            }
            else if (Player.i<400){ //Enquando for menos que 400 a animação, o personagem vai para cima.
                Player.y-=JumpHigh;
            }
            else if (Player.i>400){ //E quando for maior que 400 a animação, o personagem começa a cair.
                Player.y+=JumpHigh;
            }
        }
    }
    if (Player.ImageIndex==200){
        TimeExitDown_DeltaTime+=1;
        if (TimeExitDown_DeltaTime>TimeExitDown){
            Player.ImageIndex=0;
            Player.i=2;
        }
    }
    //Printa o player na tela.
    SDL_Rect pOrigem = {Player.i, Player.ImageIndex, 100, 100};
    SDL_Rect pDestino = {Player.x, Player.y, Player.w, Player.h};
    SDL_RenderCopy(render, SpriteSheet, &pOrigem, &pDestino);
    CaixaDeColisao.x = Player.x+37;
    CaixaDeColisao.y = Player.y+13;
    //Printa a caixa de colisão da tela.
    //SDL_Rect bcOrigem = {0, 0, CaixaDeColisao.w, CaixaDeColisao.h};
    //SDL_Rect bcDestino = {CaixaDeColisao.x, CaixaDeColisao.y, CaixaDeColisao.w, CaixaDeColisao.h    };
    //SDL_RenderCopy(render, BoxColider, &bcOrigem, &bcDestino);

    //Fas a movimentação da plataforma na tela.
    TimeMovePlatform_DeltaTime+=1;
    if (TimeMovePlatform_DeltaTime>TimeMovePlatform){ //Move a posição "x" da plataforma.
        Platform1.x-=1;
        if (witchOne==0){
            ObestaculoFloor.x-=1;
        }
        else{
            ObestaculoNotFloor.x-=1;
        }

        TimeMovePlatform_DeltaTime=0;
    }
    if (Platform1.x<-1321){ //Se a plataforma chegar nesse ponto, a sua posição volta para o inicio, porque nesse ponto o sprite da plataforma acaba.
        Platform1.x=0;
    }
    if (ObestaculoFloor.x<-20){ //Se a plataforma chegar nesse ponto, a sua posição volta para o inicio, porque nesse ponto o sprite da plataforma acaba.
        ObestaculoFloor.x=650;
        witchOne=rand()%2;
    }

    if (ObestaculoNotFloor.x<-20){ //Se a plataforma chegar nesse ponto, a sua posição volta para o inicio, porque nesse ponto o sprite da plataforma acaba.
        ObestaculoNotFloor.x=650;
        witchOne=rand()%2;
    }

    //Printa a plataforma na tela.
    SDL_Rect p1Origem = {0, 0, 1981, 197};
    SDL_Rect p1Destino = {Platform1.x, Platform1.y, Platform1.w, Platform1.h};
    SDL_RenderCopy(render, Plataforma, &p1Origem, &p1Destino);

    //Printa o obestaculo do chão na tela.
    SDL_Rect ofOrigem = {0, 0, 90, 127};
    SDL_Rect ofDestino = {ObestaculoFloor.x, ObestaculoFloor.y, ObestaculoFloor.w, ObestaculoFloor.h};
    SDL_RenderCopy(render, ObjFloor, &ofOrigem, &ofDestino);
    //Printa o obestaculo do ar na tela.
    SDL_Rect onfOrigem = {0, 0, 90, 127};
    SDL_Rect onfDestino = {ObestaculoNotFloor.x, ObestaculoNotFloor.y, ObestaculoNotFloor.w, ObestaculoNotFloor.h};
    SDL_RenderCopy(render, ObjFloor, &onfOrigem, &onfDestino);
}
void FunctionCollision(){
    SDL_Rect obj1 = {CaixaDeColisao.x, CaixaDeColisao.y, CaixaDeColisao.w, CaixaDeColisao.h};
    SDL_Rect obj2 = {ObestaculoFloor.x, ObestaculoFloor.y, ObestaculoFloor.w, ObestaculoFloor.h};
    if (SDL_HasIntersection((&obj1), (&obj2))){
        FunctionPressEnter();
    }
    SDL_Rect obj3 = {ObestaculoNotFloor.x, ObestaculoNotFloor.y, ObestaculoNotFloor.w, ObestaculoNotFloor.h};
    if (SDL_HasIntersection((&obj1), (&obj3)) && Player.ImageIndex!=200){
        FunctionPressEnter();
    }

}

void FunctionPlay(){
    FunctionKeyBoard();
    FunctionPrintImages();
    FunctionPrintText(10, 10, Score, font_colorWhite);
    SDL_RenderPresent(render);
    FunctionCollision();

    TimeWinPoint_DeltaTime+=1;
    if ((Player.x-5 >ObestaculoFloor.x || Player.x-5 >ObestaculoNotFloor.x)&& TimeWinPoint_DeltaTime>TimeWinPoint){
        Score+=1;
        TimeWinPoint_DeltaTime=0;
    }
}

int main(int c, char *arts[]){
    srand(time(NULL));
    witchOne=rand()%2;
    IMG_Init(IMG_INIT_JPG + IMG_INIT_PNG);
    SDL_Init(SDL_INIT_EVERYTHING);
    janela = SDL_CreateWindow("Running",100, 100, 640, 420, SDL_WINDOW_SHOWN);
    render = SDL_CreateRenderer(janela, -1 , 0);
    if (janela != NULL && render != NULL){
        if (TTF_Init() >=0){
            text_font =  TTF_OpenFont("./Docs/msgothic.ttf", 25);
            if(text_font==NULL){
                printf("Fonte nao localizada!!! \n");
            }
            FunctionLoadImage();
            FunctionloadSounds();
            FunctionPressEnter();
            do{
                FunctionPlay();
            }while(1);
        }
    }

    Mix_Quit();
    Mix_CloseAudio();
    return 0;
}
