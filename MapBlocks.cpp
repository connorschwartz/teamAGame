#include <SDL.h>
#include "Player.h"
#include "MapBlocks.h"
#include "iostream"
#include <vector>

WallBlock::WallBlock(){};

Stalagmite::Stalagmite()
{
    Stalagmite(1,1);
}
Stalagmite::Stalagmite(int LEVEL_WIDTH, int LEVEL_HEIGHT)
{
    STALAG_ABS_X = rand() % LEVEL_WIDTH;
    STALAG_ABS_Y = LEVEL_HEIGHT;//growing from bottom of cave
    //STALAG_ABS_Y= rand() % LEVEL_HEIGHT;

    // These should be the same first
    STALAG_REL_X = STALAG_ABS_X;
    STALAG_REL_Y = STALAG_ABS_Y;

    STALAG_WIDTH = 10 + (rand() % 30);
    STALAG_HEIGHT = 50 + (rand() % 100);
}

FlyingBlock::FlyingBlock()
{
    FlyingBlock(1, 1);
}

FlyingBlock::FlyingBlock(int LEVEL_WIDTH, int LEVEL_HEIGHT)
{
    BLOCK_ABS_X = rand() % LEVEL_WIDTH;
    BLOCK_ABS_Y = rand() % LEVEL_HEIGHT;

    // These should be the same first
    BLOCK_REL_X = BLOCK_ABS_X;
    BLOCK_REL_Y = BLOCK_ABS_Y;

    BLOCK_WIDTH = 25 + (rand() % 100);
    BLOCK_HEIGHT = 25 + (rand() % 100);
}

MapBlocks::MapBlocks()
{
    MapBlocks(1, 1);
}

MapBlocks::MapBlocks(int LEVEL_WIDTH, int LEVEL_HEIGHT)
{

    blocks_arr = new FlyingBlock[BLOCKS_N];
    stalag_arr = new Stalagmite[STALAG_N];

    int i;
    for (i = 0; i < BLOCKS_N; i++)
    {
        blocks_arr[i] = FlyingBlock(LEVEL_WIDTH, LEVEL_HEIGHT); // Initiating each FlyingBlock
    }
    for (i=0; i < STALAG_N; i++)
    {
        stalag_arr[i] = Stalagmite(LEVEL_WIDTH,LEVEL_HEIGHT);//Initiate the stalagmites
    }
}

bool MapBlocks::checkCollide(int x, int y, int pWidth, int pHeight, int xTwo, int yTwo, int pTwoWidth, int pTwoHeight)
{
    if (x + pWidth < xTwo || x > xTwo + pTwoWidth)
        return false;
    if (y + pHeight < yTwo || y > yTwo + pTwoHeight)
        return false;
    return true;
}

void MapBlocks::moveBlocks(int camX, int camY)
{
    int i;
    for (i = 0; i < BLOCKS_N; i++)
    {
        blocks_arr[i].BLOCK_REL_X = blocks_arr[i].BLOCK_ABS_X - camX;
        blocks_arr[i].BLOCK_REL_Y = blocks_arr[i].BLOCK_ABS_Y - camY;
	}
    for (i = 0; i < STALAG_N; i++)
    {
        stalag_arr[i].STALAG_REL_X = stalag_arr[i].STALAG_ABS_X - camX;
        stalag_arr[i].STALAG_REL_Y = stalag_arr[i].STALAG_ABS_Y-camY - WallBlock::block_side - stalag_arr[i].STALAG_HEIGHT;
    }
}

void MapBlocks::checkCollision(Player *p)
{
	int i;
    for (i = 0; i < BLOCKS_N; i++)
    {
        // If there's a collision, cancel the player's move
		if (checkCollide(p->getPosX(), p->getPosY(), p->PLAYER_WIDTH, p->PLAYER_HEIGHT, blocks_arr[i].BLOCK_REL_X, blocks_arr[i].BLOCK_REL_Y, blocks_arr[i].BLOCK_WIDTH, blocks_arr[i].BLOCK_HEIGHT))
        {
            p->undoXMove();
            p->undoYMove();
			// If there's still a collision, it's due to the scrolling and they need to be moved left accordingly
            if (checkCollide(p->getPosX(), p->getPosY(), p->PLAYER_WIDTH, p->PLAYER_HEIGHT, blocks_arr[i].BLOCK_REL_X, blocks_arr[i].BLOCK_REL_Y, blocks_arr[i].BLOCK_WIDTH, blocks_arr[i].BLOCK_HEIGHT))
            {
                p->setPosX(std::max(blocks_arr[i].BLOCK_REL_X - p->PLAYER_WIDTH, 0));
                p->redoYMove();
            }
        }
    }
    for (i = 0; i < STALAG_N; i++)
    {
        // If there's a collision, cancel the player's move
        if (checkCollide(p->getPosX(), p->getPosY(), p->PLAYER_WIDTH, p->PLAYER_HEIGHT, stalag_arr[i].STALAG_REL_X, stalag_arr[i].STALAG_REL_Y, stalag_arr[i].STALAG_WIDTH, stalag_arr[i].STALAG_HEIGHT))
        {
            p->undoXMove();
            p->undoYMove();
            // If there's still a collision, it's due to the scrolling and they need to be moved left accordingly
            if (checkCollide(p->getPosX(), p->getPosY(), p->PLAYER_WIDTH, p->PLAYER_HEIGHT, stalag_arr[i].STALAG_REL_X, stalag_arr[i].STALAG_REL_Y, stalag_arr[i].STALAG_WIDTH, stalag_arr[i].STALAG_HEIGHT))
            {
                p->setPosX(std::max(stalag_arr[i].STALAG_REL_X - p->PLAYER_WIDTH, 0));
                p->redoYMove();
            }
        }
    }
    if(p->getPosY()>=720-72-p->PLAYER_HEIGHT){//floor collisions stopped working, not sure why, this needs a better fix but this works
        p->setPosY(720-72-p->PLAYER_HEIGHT);
        p->undoYMove();
    }
}

void MapBlocks::checkCollision(Enemy *e)
{
	int i;
    for (i = 0; i < BLOCKS_N; i++)
    {
        // If there's a collision, cancel the enemy's move
		if (checkCollide(e->getX(), e->getY(), e->getWidth(), e->getHeight(), blocks_arr[i].BLOCK_REL_X, blocks_arr[i].BLOCK_REL_Y, blocks_arr[i].BLOCK_WIDTH, blocks_arr[i].BLOCK_HEIGHT))
        {
            e->undoXMove();
            e->undoYMove();
			// If there's still a collision, it's due to the scrolling and they need to be moved left accordingly
            if (checkCollide(e->getX(), e->getY(), e->getWidth(), e->getHeight(), blocks_arr[i].BLOCK_REL_X, blocks_arr[i].BLOCK_REL_Y, blocks_arr[i].BLOCK_WIDTH, blocks_arr[i].BLOCK_HEIGHT))
            {
                e->setPosX(std::max(blocks_arr[i].BLOCK_REL_X - e->getWidth(), 0));
                e->redoYMove();
            }
        }
    }
    for (i = 0; i < STALAG_N; i++)
    {
        // If there's a collision, cancel the player's move
        if (checkCollide(e->getX(), e->getY(), e->getWidth(), e->getHeight(), stalag_arr[i].STALAG_REL_X, stalag_arr[i].STALAG_REL_Y, stalag_arr[i].STALAG_WIDTH, stalag_arr[i].STALAG_HEIGHT))
        {
            e->undoXMove();
            e->undoYMove();
            // If there's still a collision, it's due to the scrolling and they need to be moved left accordingly
            if (checkCollide(e->getX(), e->getY(), e->getWidth(), e->getHeight(), stalag_arr[i].STALAG_REL_X, stalag_arr[i].STALAG_REL_Y, stalag_arr[i].STALAG_WIDTH, stalag_arr[i].STALAG_HEIGHT))
            {
                e->setPosX(std::max(stalag_arr[i].STALAG_REL_X - e->getWidth(), 0));
                e->redoYMove();
            }
        }
    }
}


void MapBlocks::render(int SCREEN_WIDTH, int SCREEN_HEIGHT, SDL_Renderer* gRenderer)
{
    //Draw player as cyan rectangle
    int i;
    for (i = 0; i < BLOCKS_N; i++)
    {
        // Only render the FlyingBlock if will be screen
        if (blocks_arr[i].BLOCK_REL_X < SCREEN_WIDTH && blocks_arr[i].BLOCK_REL_Y < SCREEN_HEIGHT)
        {
            SDL_SetRenderDrawColor(gRenderer, 0xFF, 0x00, 0x00, 0xFF);
            SDL_Rect fillRect = {blocks_arr[i].BLOCK_REL_X, blocks_arr[i].BLOCK_REL_Y, blocks_arr[i].BLOCK_WIDTH, blocks_arr[i].BLOCK_HEIGHT};
            SDL_RenderFillRect(gRenderer, &fillRect);
        }
    }

    // Render walls
    for(i = 0; i < SCREEN_WIDTH; i+= WallBlock::block_side)
    {
        SDL_SetRenderDrawColor(gRenderer, 0xCC, 0x66, 0x00, 0xFF);
        SDL_Rect fillRectWall1 = { i, SCREEN_HEIGHT - WallBlock::block_side, WallBlock::block_side, WallBlock::block_side };
        SDL_RenderFillRect(gRenderer, &fillRectWall1);

        SDL_Rect fillRectWall2 = {i, 0, WallBlock::block_side, WallBlock::block_side};
        SDL_RenderFillRect(gRenderer, &fillRectWall2);
    }

    for (i = 0; i < STALAG_N; i++)
    {
        // Only render the FlyingBlock if will be screen
        if (stalag_arr[i].STALAG_REL_X < SCREEN_WIDTH && stalag_arr[i].STALAG_REL_Y < SCREEN_HEIGHT)
        {
            SDL_SetRenderDrawColor(gRenderer, 0xCC, 0x66, 0x00, 0xFF);
            SDL_Rect fillRect = {stalag_arr[i].STALAG_REL_X, stalag_arr[i].STALAG_REL_Y, stalag_arr[i].STALAG_WIDTH, stalag_arr[i].STALAG_HEIGHT};
            SDL_RenderFillRect(gRenderer, &fillRect);
        }
    }

}

