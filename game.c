
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#define SCREEN_WIDTH 500
#define SCREEN_HEIGHT 800
#define TRUCK_SPEED 2
#define ROAD_SCROLL_SPEED 2
#define ROAD_LEFT_BOUND 90
#define ROAD_RIGHT_BOUND 450
#define OBSTACLE_COUNT 3
#define COLLECTIBLE_COUNT 5
#define MIN_OBSTACLE_GAP 200
#define LANE_WIDTH 250

SDL_Texture *collectibleTexture;
SDL_Rect collectibles[COLLECTIBLE_COUNT];
int collectiblesCollected = 0;//Global collectibles counter.

void renderText(SDL_Renderer *renderer, TTF_Font *font, const char *text, SDL_Color textColor, SDL_Color outlineColor, int x, int y) {
    if (!font) return;
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, text, textColor);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = {x, y, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void resetGame(bool *gameStarted, bool *collision, SDL_Rect *truckRect, SDL_Rect obstacles[], SDL_Rect collectibles[],SDL_Rect obstacle1[]) {
    *gameStarted = false;
    *collision = false;
    collectiblesCollected = 0;

    truckRect->x = (SCREEN_WIDTH - truckRect->w) / 2;
    truckRect->y = SCREEN_HEIGHT - truckRect->h - 20;

    for (int i = 0; i < OBSTACLE_COUNT; i++) {

        obstacles[i].x = (ROAD_LEFT_BOUND+(rand()%4)  * (LANE_WIDTH - 20))+20;
        obstacles[i].y = -(i * MIN_OBSTACLE_GAP) - rand() % SCREEN_HEIGHT;
        obstacles[i].w = 64;
        obstacles[i].h = 64;
	obstacle1[i].x = (ROAD_LEFT_BOUND+(rand()%5) * (LANE_WIDTH-20))+15;
	obstacle1[i].y = -(i*MIN_OBSTACLE_GAP)-rand()%SCREEN_HEIGHT-20;
	obstacle1[i].w = 64;
	obstacle1[i].h = 64;
        
    }
    for (int i = 0; i < COLLECTIBLE_COUNT; i++) {
    bool validPlacement = false;
    while (!validPlacement) {
        collectibles[i].x =( ROAD_LEFT_BOUND + (rand() % 3) * (LANE_WIDTH - 20))-10 ;
        collectibles[i].y = -rand() % SCREEN_HEIGHT;
        collectibles[i].w = 32;
        collectibles[i].h = 32;
        
        validPlacement = true;
        for (int j = 0; j < OBSTACLE_COUNT; j++) {
            if (SDL_HasIntersection(&collectibles[i], &obstacles[j])) {
                validPlacement = false;
                break;
            }
        }
    }
}
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
    
    SDL_Window *window = SDL_CreateWindow("Bread-Route", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT,2,2048);
    int volm = MIX_MAX_VOLUME/2;
    Mix_VolumeMusic(volm);
    Mix_Music *music = Mix_LoadMUS("music.wav");
    Mix_PlayMusic(music,-1); 
    SDL_Texture *bgTexture = IMG_LoadTexture(renderer, "road.png");
    SDL_Texture *truckTexture = IMG_LoadTexture(renderer, "truck.png");
    SDL_Texture *obstacleTexture = IMG_LoadTexture(renderer, "obstacle.png");
    collectibleTexture = IMG_LoadTexture(renderer, "image.png");
    SDL_Texture *obstacle1Texture = IMG_LoadTexture(renderer,"'cars(1).png'");
    SDL_Rect obstacle1[OBSTACLE_COUNT];
    Mix_Chunk *sound = Mix_LoadWAV("sound.wav");  
    TTF_Font *font = TTF_OpenFont("arcade.ttf", 30);
    TTF_Font *font1 = TTF_OpenFont("arcade.ttf",20);
    SDL_Color textColor = {255, 255, 255};
    SDL_Color outlineColor = {0, 0, 0};
    
    bool running = true, gameStarted = false, collision = false;
    SDL_Event event;
    SDL_Rect truckRect = {SCREEN_WIDTH / 2 - 32, SCREEN_HEIGHT - 70, 64, 80};
    SDL_Rect obstacles[OBSTACLE_COUNT];
    resetGame(&gameStarted, &collision, &truckRect, obstacles, collectibles,obstacle1);
    
    int roadY = 0;
    
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_KEYDOWN) {
                if (!gameStarted && event.key.keysym.sym == SDLK_RETURN){ gameStarted = true; Mix_ResumeMusic();}
                 if (collision && event.key.keysym.sym == SDLK_RETURN) resetGame(&gameStarted, &collision, &truckRect, obstacles, collectibles,obstacle1);                            if (event.key.keysym.sym == SDLK_LEFT && truckRect.x > ROAD_LEFT_BOUND) truckRect.x -= TRUCK_SPEED;
                 if (event.key.keysym.sym == SDLK_RIGHT && truckRect.x + truckRect.w < ROAD_RIGHT_BOUND) truckRect.x += TRUCK_SPEED;
            }
        }
        
        if (gameStarted && !collision) {
            roadY += ROAD_SCROLL_SPEED;
            if (roadY >= SCREEN_HEIGHT) roadY = 0;
            
            for (int i = 0; i < OBSTACLE_COUNT; i++) {
                obstacles[i].y += ROAD_SCROLL_SPEED;
                if (obstacles[i].y > SCREEN_HEIGHT) {
                    obstacles[i].y = -MIN_OBSTACLE_GAP - rand() % SCREEN_HEIGHT;
                    obstacles[i].x = ROAD_LEFT_BOUND + (rand() % 2) * (ROAD_RIGHT_BOUND - ROAD_LEFT_BOUND) / 2;
		    obstacles[i].w = 64 ;
		    obstacles[i].h = 64 ;
		    obstacle1[i].x= ROAD_LEFT_BOUND + (rand()%5) * (ROAD_RIGHT_BOUND - ROAD_LEFT_BOUND)/2;
		    obstacle1[i].y= -MIN_OBSTACLE_GAP - (rand()*3)%SCREEN_HEIGHT;
		    obstacle1[i].w= 64;
		    obstacle1[i].h= 64;
                }

                if (SDL_HasIntersection(&truckRect, &obstacles[i])) collision = true;
		
            }
            
            for (int i = 0; i < COLLECTIBLE_COUNT; i++) {
                collectibles[i].y += ROAD_SCROLL_SPEED;
                if (collectibles[i].y > SCREEN_HEIGHT) {
                    collectibles[i].y = -rand() % SCREEN_HEIGHT;
                    collectibles[i].x = ROAD_LEFT_BOUND + rand() % (ROAD_RIGHT_BOUND - ROAD_LEFT_BOUND - 50);
		    
		    

                }
                if (SDL_HasIntersection(&truckRect, &collectibles[i])) {
                    collectibles[i].y = -rand() % SCREEN_HEIGHT;
                    collectiblesCollected++;
		    Mix_PlayChannel(-1,sound,0);
                }
		if(collectiblesCollected ==  100)
		{
			collectibles[i].y = INT_MAX;
			collectibles[i].x = INT_MAX;
		}
            }
        }
        
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, bgTexture, NULL, &(SDL_Rect){0, roadY - SCREEN_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT});
        SDL_RenderCopy(renderer, bgTexture, NULL, &(SDL_Rect){0, roadY, SCREEN_WIDTH, SCREEN_HEIGHT});
        SDL_RenderCopy(renderer, truckTexture, NULL, &truckRect);
        
        for (int i = 0; i < OBSTACLE_COUNT; i++) SDL_RenderCopy(renderer, obstacleTexture, NULL, &obstacles[i]);
        for (int i = 0; i < COLLECTIBLE_COUNT; i++) SDL_RenderCopy(renderer, collectibleTexture, NULL, &collectibles[i]);
	for(int i=0; i< OBSTACLE_COUNT; i++) SDL_RenderCopy(renderer, obstacle1Texture,NULL,&obstacle1[i]);
        if (!gameStarted){
	       	renderText(renderer, font1, "OH-NO, Your Delivery Truck Dropped Bread on its way!", textColor,outlineColor, 25, 350);
		renderText(renderer,font1,"Avoid Obstacles and collect Bread_Hit enter to start!",textColor,outlineColor,25,375);
	        renderText(renderer,font1,"Mission-1: Collect 100 bread",textColor,outlineColor,25,385);	
	}
        if (collision){ 
	
		renderText(renderer, font1, "Game Over! Hit ENTER to Restart", textColor,outlineColor, 25, 350);
		if(collectiblesCollected<100){
		     renderText(renderer,font,"Mission Failed:",textColor,outlineColor,25,375);
		     renderText(renderer,font,"PLease Collect 100 Bread",textColor,outlineColor,25,395);
		   }
                Mix_PauseMusic();
	}
        char scoreText[50];
        sprintf(scoreText, "Bread Collected: %d", collectiblesCollected);
        renderText(renderer, font, scoreText, textColor,outlineColor, 20, 20);
        
        SDL_RenderPresent(renderer);
        SDL_RenderPresent(renderer);
    }
    return 0;
}
