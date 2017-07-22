#ifndef GAME_H
#define GAME_H

#include "playfield.h"

#include <GL/glew.h>
#include <random>

class Game
{
public:
  Game(PlayField* pf);

  void Restart();
  void BeginPlay();
  bool IsGameSetup() { return bgame_setup_; }
  
  GLuint Score() const { return score_; }
  GLuint Level() const { return level_; }
  GLuint Lines() const { return lines_; }
  
  TetroType Next() const { return next_tetro_type_; }
  TetroType Held() const { return held_tetro_type_; }
  
  float LockTimerPercent() const { return lock_frame_counter_ / kLockFrameLimit_; }
  bool IsPausedForLineClear() const { return bpaused_for_line_clear_; }
  float LineClearAnimationProgressPercent() const { return line_clear_frame_counter_ / kPauseForLineClear_; }
  
  void Update();
  bool IsGameOver() const { return bgame_over_; }
  
  void MoveLeft() { bmove_left_ = true; }
  void MoveRight() { bmove_right_ = true; }
  void RotateRight() { brotate_right_ = true; }
  void RotateLeft() { brotate_left_ = true; }
  void SoftDrop() { bsoft_drop_ = true; }
  void HardDrop() { bhard_drop_ = true; }
  void Hold();

  void LevelUp() { level_ = level_ < 30 ? level_ + 1 : level_; }
  void LevelDown() { level_ = level_ > 1 ? level_ - 1 : level_; }

  void GameOver();
  
  
  virtual ~Game();
private:  
  void UpdateScoreForLineClear(GLuint lines);
  
  void SpawnTetro();
  TetroType GenTetroType();
  
  bool IsGrounded();
  
  void CheckLock();
  void Lock();
  
  GLint FramesPerRowForLevel(GLint level);

  PlayField* playfield_;
  
  GLuint score_;
  GLuint level_;
  GLuint lines_;
  
  TetroType next_tetro_type_;
  TetroType held_tetro_type_;
  
  GLint moves_before_lock_;
  GLint lock_frame_counter_;

  GLint frames_per_row_;
  
  GLint move_down_frame_counter_;
  GLint move_das_frame_counter_;
  GLint move_arr_frame_counter_;
  
  GLint line_clear_frame_counter_;
  
  bool bmove_left_;
  bool bmove_right_;
  bool bsoft_drop_;
  bool bhard_drop_;
  bool brotate_right_;
  bool brotate_left_;
  bool bcan_swap_held_tetro_;

  bool bgame_setup_;
  bool bgame_over_;
  
  bool bgrounded_;
  bool bpaused_for_line_clear_;

  static std::default_random_engine rng_;
  static std::uniform_int_distribution<int> distribution_;
  
  static const GLfloat kPauseForLineClear_;
  
  static const GLfloat kLockFrameLimit_;
  static const GLint kLockMovesLimit_;
  
  static const GLint kMaxLevels_;
  
  static const GLfloat kLineClearsPerLevel_;
  
  static const GLfloat kSoftDropMultiplier_;
};


#endif //GAME_H
