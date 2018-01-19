#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
//#include <SDL_ttf.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <time.h>
#include <string.h>

#define CubeSize 32
#define TimeToDown 800 //Velocidade que os cubos cai, quanto mais proceimo de zero mais rapido!!!

int TimeToDown_DeltaTime=0;

int Matriz[14][15], CubeIndexImage[19]={0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 480, 512, 544, 576},
    PlayerCubeX=7, PlayerCubeY=0, KeyIsUp=1, HighScore=0, Score=0,
    MyCube=0;

SDL_Renderer *render;
SDL_Window *janela;

SDL_Event evento;

SDL_Texture *Textu_Cubers;
SDL_Texture *Textu_Menu;
SDL_Texture *Textu_Score;
SDL_Texture *Textu_Playing;

Mix_Music * Music = NULL;

int i, i0; //Variaveis usadas em "for".

void Func_GerationNewCube(){
    if(Matriz[0][7]!=0){
        //Limpa a tela.
        SDL_SetRenderDrawColor(render, 0, 0, 0, 1);
        SDL_RenderClear(render);

        if(HighScore<Score){
            HighScore=Score;
        }
        Func_Menu();
    }
    MyCube=(rand()%17)+1;
    //MyCube=17;
    PlayerCubeX=7;
    PlayerCubeY=0;
}

void Func_LoadImage(){
    //Cubes.
    SDL_Surface * temp0 = IMG_Load("./Folther/Cubes.png");
    Textu_Cubers = SDL_CreateTextureFromSurface(render, temp0);

    SDL_Surface * temp1 = IMG_Load("./Folther/Menu.png");
    Textu_Menu = SDL_CreateTextureFromSurface(render, temp1);

    SDL_Surface * temp2 = IMG_Load("./Folther/Score.png");
    Textu_Score = SDL_CreateTextureFromSurface(render, temp2);

    SDL_Surface * temp3 = IMG_Load("./Folther/Playing.png");
    Textu_Playing = SDL_CreateTextureFromSurface(render, temp3);


    //Limpa as variaveis de surface.
    SDL_FreeSurface(temp0); SDL_FreeSurface(temp1);
    SDL_FreeSurface(temp2); SDL_FreeSurface(temp3);
}
void Func_LoadMixer(){
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

    //Carregar musica.
    Music = Mix_LoadMUS( "./Folther/Music.mp3" );

    if(Music == NULL){
        printf("Musica nao encontrada!!!! \n");
    }

    Mix_PlayMusic( Music, -1 );
}
void Func_CleanMatriz(){
    for(i=0; i<14; i++){
        for(i0=0; i0<15; i0+=1){
            Matriz[i][i0]=0;
        }
    }
}

void Func_Menu(){
    int qtd, int_score;
    char char_score[9];
    itoa(HighScore, char_score, 10);
    SDL_Rect mOrigem = {0, 0, 544, 512};
    SDL_Rect mDestino = {0, 0, 544, 512};
    SDL_RenderCopy(render, Textu_Menu, &mOrigem, &mDestino);

    for(i=0; i<strlen(char_score); i++){
        int_score = ((int) char_score[i])-48;
        SDL_Rect sOrigem = {int_score*32, 0, 32, 32};
        SDL_Rect sDestino = {(i*32)+40, 350, 32, 32};
        SDL_RenderCopy(render, Textu_Score, &sOrigem, &sDestino);
    }

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

    //Zera as variaveis.
    Func_CleanMatriz();
    Score=0; TimeToDown_DeltaTime=0;
    Func_GerationNewCube();

}

void Func_KeyBoard(){
    int WitchKey = SDL_PollEvent(&evento);
    if(WitchKey > 0){
        if(evento.type == SDL_KEYDOWN && KeyIsUp==1){
            if(evento.key.keysym.sym == SDLK_RIGHT && PlayerCubeX<14 && Matriz[PlayerCubeY][PlayerCubeX+1]==0){
                KeyIsUp=0;
                Func_PrintCubeMatriz(1, 0);
            }
            if(evento.key.keysym.sym == SDLK_LEFT && PlayerCubeX>0 && Matriz[PlayerCubeY][PlayerCubeX-1]==0){
                KeyIsUp=0;
                Func_PrintCubeMatriz(-1, 0);
            }
            if(evento.key.keysym.sym == SDLK_DOWN){
                KeyIsUp=0;
                Func_PrintCubeMatriz(0, 1);
            }
            //if(evento.key.keysym.sym == SDLK_SPACE){
                //Matriz[PlayerCubeY][PlayerCubeX]=MyCube;
                //Func_GerationNewCube();
            //}
        }
        else if(evento.type == SDL_KEYUP && KeyIsUp==0){
            if(evento.key.keysym.sym == SDLK_RIGHT){
                KeyIsUp=1;
            }
            if(evento.key.keysym.sym == SDLK_LEFT){
                KeyIsUp=1;
            }
            if(evento.key.keysym.sym == SDLK_DOWN){
                KeyIsUp=1;
            }
        }
    }
}
void Func_PrintCubeMatriz(int DirectionX, int DirectionY){
    int MoveCube=1;
    if(PlayerCubeY+DirectionY>13){
        MoveCube=0;
        Matriz[13][PlayerCubeX]=MyCube;
        Func_GerationNewCube();
        return 0;
    }

    //system("CLS");for(i=0; i<14; i++){for(i0=0; i0<15; i0++){printf("%d",Matriz[i][i0]);}printf("\n");}

    if(Matriz[PlayerCubeY+DirectionY][PlayerCubeX]!=0){
        MoveCube=0;
        Matriz[PlayerCubeY][PlayerCubeX]=MyCube;
        Func_GerationNewCube();
    }

    if(MoveCube==1){
        PlayerCubeX+=DirectionX;
        PlayerCubeY+=DirectionY;
    }
}
void Func_KillCube(){
    int KillCube;
    for(i=0;i<14;i++){
        for(i0=0; i0<15; i0++){
            if(Matriz[i][i0]!=0){
                KillCube=Matriz[i][i0];
                //Linha.
                if(i0<12 && Matriz[i][i0]==KillCube && Matriz[i][i0+1]==KillCube && Matriz[i][i0+2]==KillCube && Matriz[i][i0+3]==KillCube){
                    Matriz[i][i0]=0; Matriz[i][i0+1]=0; Matriz[i][i0+2]=0; Matriz[i][i0+3]=0; //Mata a fileira
                    Score+=10;
                }
                //Coluna.
                if(i<11 && Matriz[i][i0]==KillCube && Matriz[i+1][i0]==KillCube && Matriz[i+2][i0]==KillCube && Matriz[i+3][i0]==KillCube){
                    Matriz[i][i0]=0; Matriz[i+1][i0]=0; Matriz[i+2][i0]=0; Matriz[i+3][i0]=0; //Mata a fileira
                    Score+=10;
                }
            }
        }
    }
}
void Func_PrintImage(){
    int Temp_Numb;

    //Printar cenario.
    SDL_Rect ceOrigem = {0, 0, 544, 510};
    SDL_Rect ceDestino = {0, 0, 544, 510};
    SDL_RenderCopy(render, Textu_Playing, &ceOrigem, &ceDestino);

    //Printa a matriz na tela ja atualizada com as posições de cada cubo.
    for(i=0; i<14; i++){
        for(i0=0; i0<15; i0++){
            Temp_Numb=Matriz[i][i0];
            SDL_Rect cOrigem = {CubeIndexImage[Temp_Numb], 0, CubeSize, CubeSize};
            SDL_Rect cDestino = {(i0*CubeSize) + 32, (i*CubeSize) + 32, CubeSize, CubeSize};
            SDL_RenderCopy(render, Textu_Cubers, &cOrigem, &cDestino);
        }
    }

    SDL_Rect cOrigem = {MyCube*CubeSize, 0, CubeSize, CubeSize};
    SDL_Rect cDestino = {(PlayerCubeX*CubeSize) + 32, (PlayerCubeY*CubeSize) + 32, CubeSize, CubeSize};
    SDL_RenderCopy(render, Textu_Cubers, &cOrigem, &cDestino);
}

void Func_Playing(){
    Func_KeyBoard();
    TimeToDown_DeltaTime+=1;
    //Desce ucubo na tela se o temo de descer ja estiver passado.
    if(TimeToDown_DeltaTime>TimeToDown){
        TimeToDown_DeltaTime=0;
        Func_PrintCubeMatriz(0, 1);
    }
    Func_KillCube();
    Func_PrintImage();
    //Printa os pontos na tela.
    char char_score[9]; int int_score;
    itoa(Score, char_score, 10);
    for(i=0; i<strlen(char_score); i++){
        int_score = ((int) char_score[i])-48;
        SDL_Rect sOrigem = {int_score*32, 0, 32, 32};
        SDL_Rect sDestino = {(i*22)+145, 482, 22, 22};
        SDL_RenderCopy(render, Textu_Score, &sOrigem, &sDestino);
    }
    SDL_RenderPresent(render);
}

int main(int c, char *arts[]){
    srand(time(NULL));
    IMG_Init(IMG_INIT_JPG + IMG_INIT_PNG);
    SDL_Init(SDL_INIT_EVERYTHING);
    janela = SDL_CreateWindow("CubeDown", 100, 100, 544, 512, SDL_WINDOW_SHOWN);
    render = SDL_CreateRenderer(janela, -1, 0);
    if(janela != NULL && render != NULL && TTF_Init() >= 0){
        Func_LoadImage();
        Func_LoadMixer();
        Func_Menu();
        while(1){
            Func_Playing();
        }
    }


    return 0;
}
