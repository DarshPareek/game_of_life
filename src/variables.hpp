#ifndef VARIABLES
#define VARIABLES
#define LIVING CLITERAL(Color){203, 166, 247, 255}
#define DEAD                                                                   \
  CLITERAL(Color){30, 30, 46, 255} // CLITERAL(Color){88, 91, 112, 255}
#define BG_COLOR                                                               \
  CLITERAL(Color){88, 91, 112, 255} // CLITERAL(Color){30, 30, 46, 255}
#define PRED_LIVING CLITERAL(Color){166, 227, 161, 200} // rgb(166, 227, 161)
#define PRED_DEADR CLITERAL(Color){243, 139, 168, 100}  // rgb(243, 139, 168)
#define PRED_DEAD CLITERAL(Color){30, 30, 46, 55}
#define FPS 60
#define INPUT_FPS 20
#define CELL_SIZE 10
#define MARGIN 3
#define ROUNDED 0.5
#define ROWS 100
#define COLS 100
#define RAND 1000
#define DDA_DELTA CELL_SIZE / 2.0
#define CONTINUE_1 KEY_ENTER
#define CONTINUE_2 KEY_F
#define QUIT KEY_Q
#define MAX_RATE 120
#define DIALOG_BG CLITERAL(Color){88, 91, 112, 255}
#define DIALOG_BORDER CLITERAL(Color){30, 30, 46, 255}
#define DIALOG_TEXT CLITERAL(Color){203, 166, 247, 255}
#define DIALOG_MARGIN 20
#define DIALOG_PADDING 10
#define DIALOG_WIDTH 290
#define DIALOG_HEIGHT 300
#define DIALOG_ROUNDNESS 0.2
#define DIALOG_BORDER_THICKNESS 3
#define DIALOG_MAX_OPACITY 255
#define DIALOG_FADE_SPEED 3.0f
#define DIALOG_INACTIVITY_TIME 2.0f
#define DIALOG_SEMI_TRANSPARENCY 0.8f
#define DIALOG_DURATION 2.0f
#if defined(PLATFORM_DESKTOP)
#define GLSL_VERSION 330
#else // PLATFORM_ANDROID, PLATFORM_WEB
#define GLSL_VERSION 100

#endif
#endif
