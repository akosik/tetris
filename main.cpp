#include "shader.h"
#include "text_renderer.h"
#include "texture_renderer.h"
#include "playfield_renderer.h"
#include "playfield.h"
#include "tetromino_renderer.h"
#include "hud_renderer.h"
#include "game.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/ext.hpp>

const GLfloat kMargin = 30.0f;
const GLfloat kTileSize = 30.0f;
const GLint kPlayFieldNumRows = 22;
const GLint kPlayFieldNumCols = 10;
const GLint kHiddenRows = 2;
const GLfloat kGoldenRatio = 1.61803398875;
const GLfloat kPlayFieldWidth = kTileSize * kPlayFieldNumCols;
const GLfloat kPlayFieldHeight = kTileSize * (kPlayFieldNumRows - kHiddenRows);
const GLfloat kHudWidth = kPlayFieldWidth / kGoldenRatio;
const GLfloat kHudHeight = kPlayFieldHeight;
const GLfloat kHudx = kPlayFieldWidth + 2 * kMargin;
const GLfloat kHudy = kMargin;
const GLfloat kWidth = 3 * kMargin + kHudWidth + kPlayFieldWidth;
const GLfloat kHeight = 2 * kMargin + kPlayFieldHeight;
const GLfloat kFrameRate = 60.0f;
const GLfloat kUpdateTimeStep = 1.0f / 60.0f;

enum GameState
  {
    kGameStart,
    kGameRunning,
    kGamePaused,
    kGameOver
  };

GameState game_state = kGameStart;

PlayField playfield(kPlayFieldNumRows, kPlayFieldNumCols);
Game tetris(&playfield);

float delta_time = 0.0f;
float last_frame = 0.0f;

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

int main()
{
  if(!glfwInit())
    {
      std::cerr << "GLFW failed to initialize." << std::endl;
      exit(EXIT_FAILURE);
    }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWwindow* window = glfwCreateWindow(kWidth, kHeight, "Tetris", NULL, NULL);

  if(!window)
    {
      std::cerr << "Failed to create GLFW window" << std::endl;
      glfwTerminate();
      exit(EXIT_FAILURE);
    }

  glfwMakeContextCurrent(window);
  
  int actual_width, actual_height;
  glfwGetFramebufferSize(window, &actual_width, &actual_height);
  glViewport(0, 0, actual_width, actual_height);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  glewExperimental = GL_TRUE;
  glewInit();

  glfwSetKeyCallback(window, KeyCallback);

  glm::mat4 projection = glm::ortho(0.0f, (float)kWidth, 0.0f, (float)kHeight);

  std::string tile_colors[] = { "cyan", "blue", "orange", "yellow", "green", "purple", "red" };
  std::vector<Texture> tetro_textures;
  std::vector<Texture> ghost_textures;
  for (GLint i = 0; i < 7; ++i)
    {
      tetro_textures.push_back(Texture("assets/" + tile_colors[i] + ".png", GL_RGBA));
      ghost_textures.push_back(Texture("assets/ghost_" + tile_colors[i] + ".png", GL_RGBA));
    }

  // Render Engines
  TextRenderer text_renderer("assets/SpaceMono-Regular.ttf",
			     48,
			     projection,
			     "shaders/text");
  
  TextureRenderer texture_renderer("shaders/texture",
				   "shaders/color",
				   projection);
  
  TetrominoRenderer tetromino_renderer(kTileSize,
				       texture_renderer,
				       tetro_textures,
				       kMargin,
				       kMargin,
				       kPlayFieldNumRows,
				       kPlayFieldNumCols);
  
  TetrominoRenderer ghost_renderer(kTileSize,
				   texture_renderer,
				   ghost_textures,
				   kMargin,
				   kMargin,
				   kPlayFieldNumRows,
				   kPlayFieldNumCols);
  
  HudRenderer hud_renderer(kHudx,
			   kHudy,
			   kHudWidth,
			   kHudHeight,
			   kHiddenRows,
			   kPlayFieldNumRows,
			   kPlayFieldNumCols,
			   kTileSize,
			   tetromino_renderer,
			   texture_renderer,
			   text_renderer);
  
  PlayFieldRenderer playfield_renderer("shaders/playfield",
				       projection,
				       kMargin,
				       kMargin,
				       kPlayFieldNumRows,
				       kPlayFieldNumCols,
				       kTileSize,
				       texture_renderer,
				       tetro_textures);

  GLfloat previous = glfwGetTime();
  GLfloat lag = 0.f;
  // game loop
  while(!glfwWindowShouldClose(window))
    {
      float current_frame = glfwGetTime();
      delta_time = current_frame - last_frame;
      last_frame = current_frame;
      lag += delta_time;
      

      hud_renderer.RenderBackground(playfield, tetris);

      switch(game_state)
	{
	case kGameStart:

	  if (!tetris.IsGameSetup())
	    {
	      tetris.Restart();
	    }
	  
	  hud_renderer.RenderHud(tetris.Next(), tetris.Held(), tetris.Score(), tetris.Lines(), tetris.Level());
	  playfield_renderer.Render(playfield);
	  text_renderer.RenderCentered("Press 'SPACE' to play.", kMargin, kMargin + 15 * kTileSize, kPlayFieldWidth, kPlayFieldHeight, .5, glm::vec4(1,1,1,1));
	  text_renderer.RenderCentered("Choose a level with the Arrow Keys.", kMargin, kMargin + 14 * kTileSize, kPlayFieldWidth, kPlayFieldHeight, .25, glm::vec4(1,1,1,1));
	  
	  break;
	  
	case kGameRunning:
	  
	  while (lag >= kUpdateTimeStep)
	    {
	      tetris.Update();
	      if (tetris.IsGameOver())
		{
		  game_state = kGameOver;
		}
	      lag -= kUpdateTimeStep;
	    }
	  
	  hud_renderer.RenderHud(tetris.Next(), tetris.Held(), tetris.Score(), tetris.Lines(), tetris.Level());
	  playfield_renderer.Render(playfield);
	  
	  if (tetris.IsPausedForLineClear())
	    {
	      playfield_renderer.LineClearAnimation(playfield, tetris.LineClearAnimationProgressPercent());
	    }
	  else
	    {
	      tetromino_renderer.RenderOnPlayfield(playfield.FallingTetroRow(), playfield.FallingTetroCol(),  playfield.FallingTetro());
	      ghost_renderer.RenderOnPlayfield(playfield.GhostRow(), playfield.GhostCol(), playfield.FallingTetro());
	    }
	  
	  break;
	    
	case kGamePaused:
	  hud_renderer.RenderHud(tetris.Next(), tetris.Held(), tetris.Score(), tetris.Lines(), tetris.Level());
	  playfield_renderer.Render(playfield);
	  text_renderer.RenderCentered("Game Paused", kMargin, kMargin + 15 * kTileSize, kPlayFieldWidth, kPlayFieldHeight, 1, glm::vec4(1,1,1,1));
	  text_renderer.RenderCentered("Press 'SPACE' to Resume", kMargin, kMargin + 14 * kTileSize, kPlayFieldWidth, kPlayFieldHeight, .5, glm::vec4(1,1,1,1));
	  text_renderer.RenderCentered("Press 'R' to Restart", kMargin, kMargin + 13 * kTileSize, kPlayFieldWidth, kPlayFieldHeight, .5, glm::vec4(1,1,1,1));
	  text_renderer.RenderCentered("Press 'Q' to Quit", kMargin, kMargin + 12 * kTileSize, kPlayFieldWidth, kPlayFieldHeight, .5, glm::vec4(1,1,1,1));
	  break;

	case kGameOver:
	  hud_renderer.RenderHud(tetris.Next(), tetris.Held(), tetris.Score(), tetris.Lines(), tetris.Level());
	  playfield_renderer.Render(playfield);
	  text_renderer.RenderCentered("Game Over.", kMargin, kMargin + 15 * kTileSize, kPlayFieldWidth, kPlayFieldHeight, 1, glm::vec4(1,1,1,1));
	  
	  break;
	}
      
      glfwSwapBuffers(window);
      glfwPollEvents();
    }
  glfwTerminate();
  return 0;
}


void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
  switch (game_state)
    {
    case kGameStart:
      if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
	  tetris.BeginPlay();
	  game_state = kGameRunning;
	}
      if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	tetris.LevelUp();
      if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	tetris.LevelDown();
	break;

    case kGameRunning:
      if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
	game_state = kGamePaused;
      if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	tetris.HardDrop();
      if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
	tetris.Hold();
      if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
	tetris.RotateRight();
      if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
	tetris.RotateLeft();
      if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
	tetris.SoftDrop();
      if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
	tetris.MoveLeft();
      if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
	tetris.MoveRight();
      break;

    case kGamePaused:
      if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	game_state = kGameRunning;
      if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
	  tetris.GameOver();
	  game_state = kGameStart;
	}
      if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	glfwSetWindowShouldClose(window, GLFW_TRUE);
      break;

    case kGameOver:
      if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
	game_state = kGameStart;
      break;

    default:
      break;
    }
}
