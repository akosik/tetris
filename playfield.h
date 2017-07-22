#ifndef PLAYFIELD_H
#define PLAYFIELD_H

#include "tetromino.h"

#include <vector>
#include <GL/glew.h>
#include <map>

class PlayField
{
public:
  PlayField(GLuint nrows, GLuint ncols);
  
  bool SpawnTetro(const TetroType type);
  bool LockFallingTetro();
  
  const Tetromino& FallingTetro() const { return falling_tetro_; }
  TetroType FallingTetroType() const { return falling_tetro_.Type(); }
  GLint FallingTetroRow() const { return falling_tetro_row_; }
  GLint FallingTetroCol() const { return falling_tetro_col_; }
  
  GLint GhostRow() const { return ghost_row_; }
  GLint GhostCol() const { return ghost_col_; }
  
  void SetTile(TileColor color, GLint row, GLint col);
  void Clear();
  
  TileColor GetTileColor(GLint row, GLint col) const;
  
  bool IsTileOpen(GLint row, GLint col) const;
  bool IsPositionOpen(GLint row, GLint col, const Tetromino& tetro) const;
  bool IsGrounded() const;
  
  void UpdateLineClears();
  void ClearLines();
  GLint NumLinesCleared() const { return lines_to_clear_.size(); }
  const std::vector<int>& LinesToClear() const { return lines_to_clear_; }

  GLint MoveFallingTetroHorizontal(GLint delta_right);
  GLint MoveFallingTetroVertical(GLint delta_up);
  bool RotateFallingTetro(Rotation rotation);

  void UpdateGhost();

  virtual ~PlayField();
private:
  Tetromino falling_tetro_;
  GLint falling_tetro_row_;
  GLint falling_tetro_col_;
  GLint ghost_row_;
  GLint ghost_col_;
  
  const GLint ncols_;
  const GLint nrows_;
  // Tetris Guidlines proscribe 2 hidden lines above the visible playfield
  static const GLint kHiddenLines_;
  std::vector<TileColor> tile_colors_;
  std::vector<TileColor> tile_colors_after_clear_;
  std::vector<int> lines_to_clear_;
  const std::map< std::pair<RotationState, RotationState>, std::vector< std::pair<GLint, GLint> > > kicks_jlstz_, kicks_i_;
};


#endif // PLAYFIELD_H
