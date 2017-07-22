#include "playfield.h"

const GLint PlayField::kHiddenLines_ = 2;

PlayField::PlayField(GLuint nrows, GLuint ncols) : nrows_(nrows),
						   ncols_(ncols),
						   tile_colors_((nrows + kHiddenLines_) * ncols, kEmpty),
						   falling_tetro_(kNone),
						   falling_tetro_row_(21),
						   falling_tetro_col_(5),
						   ghost_row_(21),
						   ghost_col_(5)
{ }

bool PlayField::SpawnTetro(const TetroType type)
{
  falling_tetro_ = Tetromino(type);
  falling_tetro_row_ = 21;
  falling_tetro_col_ = (ncols_ - falling_tetro_.TemplateSideLength()) / 2;

  if (!IsPositionOpen(falling_tetro_row_, falling_tetro_col_, falling_tetro_))
    return false;

  // According to Tetris Guideline games | link: https://harddrop.com/wiki/Spawn_Location
  if (type != kTetroI)
    {
      GLint i = 0;
      for ( ; i < 2; ++i)
	{
	  if(!IsPositionOpen(falling_tetro_row_ - (i + 1), falling_tetro_col_, falling_tetro_))
	    break;
	}
      falling_tetro_row_ -= i;
    }
  UpdateGhost();
  return true;
}

bool PlayField::LockFallingTetro()
{ 
  GLint template_side_length = falling_tetro_.TemplateSideLength();
  const std::vector<TileColor>& shape = falling_tetro_.Shape();
  bool top_out = true;
  GLint shape_index = 0;
  
  for (GLint row = 0; row < template_side_length; ++row)
    {
      for (GLint col = 0; col < template_side_length; ++col)
	{
	  if (shape[shape_index] != kEmpty)
	    {
	      SetTile(shape[shape_index], falling_tetro_row_ - row, falling_tetro_col_ + col);
	      if (row < 20)
		top_out = false;
	    }
	  ++shape_index;
	}
    }
  UpdateLineClears();
  falling_tetro_ = Tetromino(kNone);
  return !top_out;
}

void PlayField::SetTile(TileColor color, GLint row, GLint col)
{
  tile_colors_[(row * ncols_) + col] = color;
}

void PlayField::Clear()
{
  std::fill(tile_colors_.begin(), tile_colors_.end(), kEmpty);
}

TileColor PlayField::GetTileColor(GLint row, GLint col) const
{
  return tile_colors_[(row * ncols_) + col];
}

bool PlayField::IsTileOpen(GLint row, GLint col) const
{
  return row >= 0 && row < 22 && col >= 0 && col < 10 && GetTileColor(row, col) == kEmpty;
}

bool PlayField::IsPositionOpen(GLint row, GLint col, const Tetromino& tetro) const
{
  if (tetro.Type() == kNone)
    return false;
  
  const std::vector<TileColor>& shape = tetro.Shape();
  GLuint template_side_length = tetro.TemplateSideLength();

  GLuint template_index = 0;
  for (GLuint template_row = 0; template_row < template_side_length; ++template_row)
    {
      for (GLuint template_col = 0; template_col < template_side_length; ++template_col)
	{
	  if (shape[template_index++] != kEmpty &&
	      !IsTileOpen(row - template_row, col + template_col))
	    {
	      return false;
	    }
	}
    }
  return true;
}

GLint PlayField::MoveFallingTetroHorizontal(GLint delta_right)
{
  if (delta_right != 0  && IsPositionOpen(falling_tetro_row_, falling_tetro_col_ + delta_right, falling_tetro_))
    {
      falling_tetro_col_ += delta_right;
      UpdateGhost();
      return delta_right;
    }
  return 0;
}

GLint PlayField::MoveFallingTetroVertical(GLint delta_up)
{
  if (delta_up != 0  && IsPositionOpen(falling_tetro_row_ + delta_up, falling_tetro_col_, falling_tetro_))
    {
      falling_tetro_row_ += delta_up;
      UpdateGhost();
      return delta_up;
    }
  return 0;
}

bool PlayField::RotateFallingTetro(Rotation rotation)
{
  Tetromino test(falling_tetro_);
  test.Rotate(rotation);

  RotationState final_rotation_state;
  if(rotation == kLeft)
    {
      final_rotation_state = static_cast<RotationState>((falling_tetro_.RotationState() + 3) % 4);
    }
  else
    {
      final_rotation_state = static_cast<RotationState>((falling_tetro_.RotationState() + 1) % 4);
    }
    
  for (const auto kick : falling_tetro_.Kicks(falling_tetro_.RotationState(), final_rotation_state))
    {
      GLint delta_row = kick.first;
      GLint delta_col = kick.second;
      if (IsPositionOpen(falling_tetro_row_ + delta_row, falling_tetro_col_ + delta_col, test))
	{
	falling_tetro_ = test;
	falling_tetro_row_ += delta_row;
	falling_tetro_col_ += delta_col;
	UpdateGhost();
	return true;
      }
    }
            
  return false;
}

void PlayField::UpdateGhost()
{
  // Cannot use binary search with tetris' naive gravity
  ghost_row_ = falling_tetro_row_;
  ghost_col_ = falling_tetro_col_;
  while(IsPositionOpen(ghost_row_ - 1, ghost_col_, falling_tetro_))
    --ghost_row_;
}

bool PlayField::IsGrounded() const
{
  return !IsPositionOpen(falling_tetro_row_ - 1, falling_tetro_col_, falling_tetro_);
}

void PlayField::UpdateLineClears()
{
  tile_colors_after_clear_ = tile_colors_;
  GLint lines_cleared_below = 0;
  GLint clear_start_row = 0;
  // this loop just tracks a row index into tile_colors_
  // and tile_colors_after_clear_ so that uncleared rows
  // "fall" into the right positions
  for (GLint row = 0; row < nrows_; ++row)
    {
      bool row_filled = true;
      for (GLint col = 0; col < ncols_; ++col)
	{
	  if (tile_colors_[(row * ncols_) + col] == kEmpty)
	    {
	      row_filled = false;
	      break;
	    }
	}
      if (row_filled)
	{
	  ++lines_cleared_below;
	  lines_to_clear_.push_back(row);
	}
      else if(lines_cleared_below > 0)
	{
	  GLint clear_start = clear_start_row * ncols_;
	  GLint clear_end = clear_start + lines_cleared_below * ncols_;
	  // replace the first cleared line by the first uncleared line above it
	  std::copy(tile_colors_.begin() + clear_end,
		    tile_colors_.begin() + clear_end + ncols_,
		    tile_colors_after_clear_.begin() + clear_start);
	  ++clear_start_row;
	}
      else
	{
	  ++clear_start_row;
	}
    }
  std::fill(tile_colors_after_clear_.begin() + clear_start_row * ncols_,
	    tile_colors_after_clear_.end(),
	    kEmpty);
}

void PlayField::ClearLines()
{
  if (lines_to_clear_.empty())
    return;

  lines_to_clear_.clear();
  tile_colors_ = tile_colors_after_clear_;
}

PlayField::~PlayField()
{
  
}
