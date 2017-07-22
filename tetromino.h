#ifndef TETROMINO_H
#define TETROMINO_H

#include <vector>
#include <GL/glew.h>
#include <map>

enum TetroType
  {
    kNone = -1,
    kTetroI,
    kTetroJ,
    kTetroL,
    kTetroO,
    kTetroS,
    kTetroT,
    kTetroZ
  };

enum TileColor
  {
    kEmpty = -1,
    kCyan,
    kBlue,
    kOrange,
    kYellow,
    kGreen,
    kPurple,
    kRed
  };

enum RotationState
  {
    kRsZero,
    kRsRight,
    kRsTwo,
    kRsLeft
  };

enum Rotation
  {
    kRight,
    kLeft
  };

class Tetromino
{
public:
  explicit Tetromino(TetroType type);
  Tetromino(const Tetromino& other) = default;
  Tetromino& operator=(const Tetromino& other) = default;
  Tetromino& operator=(Tetromino&& tetro);
  TetroType Type() const { return type_; }
  TileColor Color() const { return color_; }
  const std::vector< std::pair<GLint, GLint> >& Kicks(RotationState beginning, RotationState end) const { return kicks_.at(std::make_pair(beginning, end)); }
  RotationState RotationState() const { return rotation_state_; }
  
  const std::vector<TileColor>& Shape() const { return shape_; }
  GLuint TemplateSideLength() const { return template_side_length_; }
  void Rotate(Rotation rotation);
  
  virtual ~Tetromino();

private:
  enum RotationState rotation_state_;
  TetroType type_;
  TileColor color_;
  GLuint template_side_length_;
  std::vector<TileColor> shape_;
  std::map< std::pair<enum RotationState, enum RotationState>, std::vector< std::pair<GLint, GLint> > > kicks_;
};


#endif // TETROMINO_H
