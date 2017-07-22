#include "game.h"

#include <ctime>

const GLfloat Game::kPauseForLineClear_ = 30;
const GLfloat Game::kLockFrameLimit_ = 30;
const GLint Game::kLockMovesLimit_ = 15;
const GLint Game::kMaxLevels_ = 20;
const GLfloat Game::kLineClearsPerLevel_ = 10;
const GLfloat Game::kSoftDropMultiplier_ = 20;

std::default_random_engine Game::rng_(time(0));
std::uniform_int_distribution<int> Game::distribution_(0,6);

Game::Game(PlayField* pf) : playfield_(pf),
			    next_tetro_type_(GenTetroType()),
			    held_tetro_type_(kNone),
			    level_(1),
			    score_(0),
			    lines_(0),
			    bgame_setup_(false)
{ }

void Game::Restart()
{
  score_ = 0;
  lines_ = 0;

  next_tetro_type_ = GenTetroType();
  held_tetro_type_ = kNone;
  
  moves_before_lock_ = 0;
  lock_frame_counter_ = 0;
  
  move_down_frame_counter_ = 0;

  line_clear_frame_counter_ = 0;

  bmove_left_ = false;
  bmove_right_ = false;
  bsoft_drop_ = false;
  bhard_drop_ = false;
  brotate_right_ = false;
  brotate_left_ = false;
  bcan_swap_held_tetro_ = true;

  bgame_over_ = false;
  bgrounded_ = false;
  bpaused_for_line_clear_ = false;
  
  playfield_->Clear();
  bgame_setup_ = true;
}

void Game::BeginPlay()
{
  frames_per_row_ = FramesPerRowForLevel(level_);
  SpawnTetro();
}

void Game::Update()
{
  if (bpaused_for_line_clear_)
    {
      ++line_clear_frame_counter_;
      if (line_clear_frame_counter_ < kPauseForLineClear_)
	return;
      UpdateScoreForLineClear(playfield_->NumLinesCleared());
      playfield_->ClearLines();
      SpawnTetro();
      bpaused_for_line_clear_ = false;
    }

  ++move_down_frame_counter_;

  if (bgrounded_)
    {
      ++lock_frame_counter_;
    }
  else
    {
      lock_frame_counter_ = 0;
    }
  
  // move horizontal
  if (bmove_left_)
    {
      playfield_->MoveFallingTetroHorizontal(-1);
      bmove_left_ = false;
    }
  if (bmove_right_)
    {
      playfield_->MoveFallingTetroHorizontal(1);
      bmove_right_ = false;
    }
  
  // rotate
  if (brotate_right_ && brotate_left_)
    {
      brotate_right_ = false;
      brotate_left_ = false;
    }
  else if (brotate_right_)
    {
      playfield_->RotateFallingTetro(kRight);
      brotate_right_ = false;
    }
  else if (brotate_left_)
    {
      playfield_->RotateFallingTetro(kLeft);
      brotate_left_ = false;
    }

  // move vertical
  if (!bhard_drop_)
    {
      double softdrop_multiplier = bsoft_drop_ ? kSoftDropMultiplier_ : 1;
      if (move_down_frame_counter_ >= frames_per_row_ / softdrop_multiplier)
	{
	  if (playfield_->MoveFallingTetroVertical(-1) && bsoft_drop_)
	    score_ += 1;
	  move_down_frame_counter_ = 0;
	}
      CheckLock();
    }
  
  else if (playfield_->FallingTetroType() != kNone)
    {
      // Could optimize this with special HardDrop Function that doesn't check
      // if we CAN move to desired position. we know we can.
      // maintaining uniformity by treating it like a soft drop.
      GLint rows_to_bottom = playfield_->FallingTetroRow() - playfield_->GhostRow();
      playfield_->MoveFallingTetroVertical(-rows_to_bottom);
      score_ += 2 * level_ * rows_to_bottom;
      Lock();
    }
  // Both drop requests should be taken care of by now
  bhard_drop_ = false;
  bsoft_drop_ = false;
}

void Game::UpdateScoreForLineClear(GLuint lines)
{
  // http://tetris.wikia.com/wiki/Scoring
  int deltaScore = 0;
  switch (lines)
    {
    case 1:
      deltaScore = 40 * level_;
      break;
    case 2:
      deltaScore = 100 * level_;
      break;
    case 3:
      deltaScore = 300 * level_;
      break;
    case 4:
      deltaScore = 1200 * level_;
      break;
    default:
      break;
    }
    lines_ += lines;
    score_ += deltaScore * level_;
    if (level_ < kMaxLevels_ && lines_ >= kLineClearsPerLevel_ * level_) {
        ++level_;
        frames_per_row_ = FramesPerRowForLevel(level_);
    }
}

void Game::Hold()
{
  if (bcan_swap_held_tetro_ && !bpaused_for_line_clear_)
    {
      TetroType falling = playfield_->FallingTetro().Type();
      if (held_tetro_type_ == kNone)
	{
	  playfield_->SpawnTetro(GenTetroType());
	}
      else
	{
	  playfield_->SpawnTetro(held_tetro_type_);
	}
      held_tetro_type_ = falling;
      bcan_swap_held_tetro_ = false;
    }
}

void Game::SpawnTetro()
{
  if (!playfield_->SpawnTetro(next_tetro_type_))
    GameOver();

  next_tetro_type_ = GenTetroType();

  moves_before_lock_ = 0;
}

bool Game::IsGrounded()
{
  return playfield_->IsGrounded();
}

TetroType Game::GenTetroType()
{
  return static_cast<TetroType>(distribution_(rng_));
}

Game::~Game()
{
  
}

void Game::CheckLock()
{
    if (playfield_->IsGrounded())
      {
	bgrounded_ = true;
	
	if (lock_frame_counter_ >= kLockFrameLimit_ || moves_before_lock_ >= kLockMovesLimit_)
	  Lock();
      }
    else
      {
	bgrounded_ = false;
	return;
      }
}

void Game::Lock()
{
  lock_frame_counter_ = 0;
  bgrounded_ = false;
  bcan_swap_held_tetro_ = true;
    
    if (!playfield_->LockFallingTetro()) {
      GameOver();
      return;
    }
    
    if (playfield_->NumLinesCleared() > 0)
      {
	bpaused_for_line_clear_ = true;
	line_clear_frame_counter_ = 0;
      }
    else
      {
	SpawnTetro();
      }
  
}

GLint Game::FramesPerRowForLevel(GLint level)
{
  // https://tetris.wiki/Tetris_(NES,_Nintendo)
  if (level < 10)
    {
      return 48 - (5 * level);
    }
  else if (level == 10)
    {
      return 6;
    }
  else if (level > 10 && level < 20)
    {
      return 6 - (((level - 10) / 3) + 1);
    }
  else if (level >= 20 && level < 30)
    {
      return 2;
    }
  else if (level >= 30)
    {
      return 1;
    }
  return -1;
}

void Game::GameOver()
{
  bgame_over_ = true;
  bgame_setup_ = false;
}
