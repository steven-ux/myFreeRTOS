#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "main.h"
#include "game.h"

#include "FreeRTOS.h"
#include "task.h"

#define BALL_SIZE 5

//== non-exported struct & function declaration =======

// Point
typedef struct {
    int16_t x;
    int16_t y;
} _Point;

// Size
typedef struct {
    uint16_t width;
    uint16_t height;
} _Size;

// Rectangle Region
typedef struct {
    _Point loc; // location
    _Size  size;
} _RectRegion;

static void _Point_add(_Point*, _Size*);

// determines whether the RectRegion contains the Point
static bool _RectRegion_contains(_RectRegion*, _Point*);

// _RectRegion::getX2()
static int16_t _Rect_getX2(_RectRegion*);

// _RectRegion::getY2()
static int16_t _Rect_getY2(_RectRegion*);

// draw Rectangle by RectRegion
static void _draw_Rect(_RectRegion*);

// common code2
static void _f2(uint16_t playerX, uint16_t playerY, uint16_t playerW, uint16_t playerH);

// common code1
static void _f1();

//== static global variable ============================

// player1 object
static _RectRegion rect_player1;
static uint8_t player1IsReversed = 1;

// player2 object
static _RectRegion rect_player2;
static uint8_t player2IsReversed = 0;

// ball object
static _RectRegion rect_ball;
static _Size vector_ball;
static uint8_t ballIsRun = 0;

// game object
static _RectRegion rect_game;

// game status
uint8_t demoMode = 0;

//== static function difinition ========================

static void _Point_add(_Point* loc, _Size* size) {
    loc->x += size->width;
    loc->y += size->height;
}

static bool _Rect_contains(_RectRegion* rect, _Point* point) {
    if( point->x > _Rect_getX2(rect) ) return false;
    if( point->x < (rect->loc.x) ) return false;
    if( point->y > _Rect_getY2(rect) ) return false;
    if( point->y < (rect->loc.y) ) return false;
    return true;
}

static int16_t _Rect_getX2(_RectRegion* rect) {
    return rect->loc.x + rect->size.width - 1;
}

static int16_t _Rect_getY2(_RectRegion* rect) {
    return rect->loc.y + rect->size.height - 1;
}

static void _draw_Rect(_RectRegion* rect) {
    LCD_DrawFullRect(
        rect->loc.x, 
        rect->loc.y, 
        rect->size.width, 
        rect->size.height
    );
}

static void _f2(uint16_t playerX, uint16_t playerY, uint16_t playerW, uint16_t playerH) {
    
    if( ballX - ballSize <= playerY + playerW/4 ){
        ballVY =-3;
        ballVX =-7;
    } else if( ballX >= playerY + playerW - playerW/4 ){
        ballVY =-3;
        ballVX = 7;
    } else if( ballX + ballSize < playerY + playerW/2 ){
        ballVY =-7;
        ballVX =-3;
    } else if( ballX > playerY + playerW/2 ){
        ballVY =-7;
        ballVX = 3;
    } else {
        ballVY =-9;
        ballVX = 0;
    }
}

static void _f1() {
        //Ball
        if( ballIsRun == 1 ){

            LCD_SetTextColor( LCD_COLOR_BLACK );
            _draw_Rect(&rect_ball);

           _Point_add(&rect_ball.loc, &vector_ball);
           
            

            //Touch wall
            ballX += ballVX;
            if( ballX <= 0 ){
                ballX = 0;
                ballVX *= -1;
            } else if( ballX + ballSize >= LCD_PIXEL_WIDTH ){
                ballX = LCD_PIXEL_WIDTH - ballSize;
                ballVX *= -1;
            }

            //PONG!
            ballY += ballVY;
            if( ballY + ballSize >= player2Y ){
                if( ballX + ballSize >= player2X && ballX <= player2X + player2W ){
                    
                    _f2(player2X, player2Y, player2W, player2H);

                } else {
                    BallReset();
                }
            }

            if( ballY <= player1Y + player1H ){
                if( ballX + ballSize >= player1X && ballX <= player1X + player1W ){
                    
                    _f2(player1X, player1Y, player1W, player1H);
                    
                } else {
                    BallReset();
                }
            }
        }
}

static void _GAME_reset() {
    rect_game= (_RectRegion) {
        .loc= (_Point) {
            .x= 0,
            .y= 0
        },
        .size= (_Size) {
            .width= LCD_PIXEL_WIDTH,
            .height= LCD_PIXEL_HEIGHT
        }
    };
    
    rect_player1= (_RectRegion) {
        .loc= (_Point) {
            .x= 10,
            .y= 10
        },
        .size= (_Size) {
            .width= 60,
            .height= 10
        }
    };

    
    rect_player2= (_RectRegion) {
        .loc= (_Point) {
            .x= LCD_PIXEL_WIDTH - 20,
            .y= LCD_PIXEL_HEIGHT - 20
        },
        .size= (_Size) {
            .width= 60,
            .height= 10
        }
    };

    rect_ball= (_RectRegion) {
        .loc= (_Point) {
            .x= ( LCD_PIXEL_WIDTH - 5 ) / 2,
            .y= ( LCD_PIXEL_HEIGHT - 5 ) / 2
        },
        .size= (_Size) {
            .width= BALL_SIZE,
            .height= BALL_SIZE
        }
    };

    vector_ball= (_Size) {
        .width= 5,
        .height= 5
    };

}

//== exported function difinition ======================

void GAME_init() {
    _GAME_reset();
}

void game_test1() {
    _f2(0,0,0,0);
}

void GAME_EventHandler1()
{
    if( STM_EVAL_PBGetState( BUTTON_USER ) ){

        player1IsReversed = 0;

        while( STM_EVAL_PBGetState( BUTTON_USER ) )
            ;

        player1IsReversed = 1;
    }
}

void GAME_EventHandler2()
{
    if( IOE_TP_GetState()->TouchDetected ){

        player2IsReversed = 1;

        while( IOE_TP_GetState()->TouchDetected )
            ;

        player2IsReversed = 0;
    }
}

void GAME_EventHandler3()
{
    if( ballIsRun == 0 ){
        BallReset();
    }
}

void GAME_Update()
{
    //Player1
    LCD_SetTextColor( LCD_COLOR_BLACK );
    LCD_DrawFullRect( player1X, player1Y, player1W, player1H );
    LCD_DrawFullRect( player2X, player2Y, player2W, player2H );

    if( demoMode == 0 ){

        if( player1IsReversed )
            player1X -= 5;
        else
            player1X += 5;

        if( player1X <= 0 )
            player1X = 0;
        else if( player1X + player1W >= LCD_PIXEL_WIDTH )
            player1X = LCD_PIXEL_WIDTH - player1W;

        //Player2
        if( player2IsReversed )
            player2X -= 5;
        else
            player2X += 5;

        if( player2X <= 0 )
            player2X = 0;
        else if( player2X + player2W >= LCD_PIXEL_WIDTH )
            player2X = LCD_PIXEL_WIDTH - player2W;

        _f1();
        
    } else {   //if demoMode == 1

        //Player1 move
        if( ballVY < 0 ){
            if( player1X + player1W/2 < ballX + ballSize/2 ){
                player1X += 8;
                player2X += 2;
            } else {
                player1X -= 8;
                player2X -= 2;
            }
        }

        //Player2 move
        if( ballVY > 0 ){
            if( player2X + player2W/2 < ballX + ballSize/2 ){
                player1X += 2;
                player2X += 8;
            } else {
                player1X -= 2;
                player2X -= 8;
            }

        }

        if( player1X <= 0 )
            player1X = 0;
        else if( player1X + player1W >= LCD_PIXEL_WIDTH )
            player1X = LCD_PIXEL_WIDTH - player1W;

        if( player2X <= 0 )
            player2X = 0;
        else if( player2X + player2W >= LCD_PIXEL_WIDTH )
            player2X = LCD_PIXEL_WIDTH - player2W;
        
        _f1();
    }
}

void GAME_Render()
{
    LCD_SetTextColor( LCD_COLOR_WHITE );
    _draw_Rect(&rect_player1);
    _draw_Rect(&rect_player2);
    //LCD_DrawFullRect( player1X, player1Y, player1W, player1H );
    //LCD_DrawFullRect( player2X, player2Y, player2W, player2H );
    LCD_DrawFullRect( ballX, ballY, ballSize, ballSize );
    LCD_DrawLine( 10, LCD_PIXEL_HEIGHT / 2, LCD_PIXEL_WIDTH - 20, LCD_DIR_HORIZONTAL );
}

