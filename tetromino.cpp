#include "tetromino.h"

const std::map< std::pair<enum RotationState, enum RotationState>, std::vector< std::pair<GLint, GLint> > > kicks_jlstz_ =
  {
    { {kRsZero,  kRsRight }, { {0, 0},  {-1, 0},  {-1, 1},  {0,-2}, {-1,-2} } },
    { {kRsRight, kRsZero  }, { {0, 0},  { 1, 0},  { 1,-1},  {0, 2}, { 1, 2} } },
    { {kRsRight, kRsTwo   }, { {0, 0},  { 1, 0},  { 1,-1},  {0, 2}, { 1, 2} } },
    { {kRsTwo,   kRsRight }, { {0, 0},  {-1, 0},  {-1, 1},  {0,-2}, {-1,-2} } },
    { {kRsTwo,   kRsLeft  }, { {0, 0},  { 1, 0},  { 1, 1},  {0,-2}, { 1,-2} } },
    { {kRsLeft,  kRsTwo   }, { {0, 0},  {-1, 0},  {-1,-1},  {0, 2}, {-1, 2} } },
    { {kRsLeft,  kRsZero  }, { {0, 0},  {-1, 0},  {-1,-1},  {0, 2}, {-1, 2} } },
    { {kRsZero,  kRsLeft  }, { {0, 0},  { 1, 0},  { 1, 1},  {0,-2}, { 1,-2} } }
  };

const std::map< std::pair<enum RotationState, enum RotationState>, std::vector< std::pair<GLint, GLint> > > kicks_i_ =
  {
    { {kRsZero,  kRsRight }, { {0, 0},  {-2, 0},  { 1, 0},  {-2,-1},  { 1, 2}, } },
    { {kRsRight, kRsZero  }, { {0, 0},  { 2, 0},  {-1, 0},  { 2, 1},  {-1,-2}, } },
    { {kRsRight, kRsTwo   }, { {0, 0},  {-1, 0},  { 2, 0},  {-1, 2},  { 2,-1}, } },
    { {kRsTwo,   kRsRight }, { {0, 0},  { 1, 0},  {-2, 0},  { 1,-2},  {-2, 1}, } },
    { {kRsTwo,   kRsLeft  }, { {0, 0},  { 2, 0},  {-1, 0},  { 2, 1},  {-1,-2}, } },
    { {kRsLeft,  kRsTwo   }, { {0, 0},  {-2, 0},  { 1, 0},  {-2,-1},  { 1, 2}, } },
    { {kRsLeft,  kRsZero  }, { {0, 0},  { 1, 0},  {-2, 0},  { 1,-2},  {-2, 1}, } },
    { {kRsZero,  kRsLeft  }, { {0, 0},  {-1, 0},  { 2, 0},  {-1, 2},  { 2,-1}, } }
  };

Tetromino::Tetromino(TetroType type) : color_(static_cast<TileColor>(type)), type_(type),
				       rotation_state_(kRsZero)
{
  TileColor _ = kEmpty;
  TileColor x = static_cast<TileColor>(type);
  switch (type)
    {
    case kTetroI:
      {
	template_side_length_ = 4;
	kicks_ = kicks_i_;
	TileColor shape[] = { _, _, _, _,
			      x, x, x, x,
			      _, _, _, _,
			      _, _, _, _  };
	shape_.assign(shape, shape + template_side_length_ * template_side_length_);
	break;
      }
    case kTetroJ:
      {
	template_side_length_ = 3;
	kicks_ = kicks_jlstz_;
	TileColor shape[] =  { x, _, _,
			       x, x, x,
			       _, _, _  };
	shape_.assign(shape, shape + template_side_length_ * template_side_length_);
	break;
      }
    case kTetroL:
      {
	template_side_length_ = 3;	
	kicks_ = kicks_jlstz_;
	TileColor shape[] = { _, _, x,
			      x, x, x,
			      _, _, _  };
	shape_.assign(shape, shape + template_side_length_ * template_side_length_);
	break;
      }
    case kTetroO:
      {
	template_side_length_ = 2;
	kicks_ = kicks_jlstz_;
	TileColor shape[] = { x, x,
			      x, x  };
	shape_.assign(shape, shape + template_side_length_ * template_side_length_);
	break;
      }
    case kTetroS:
      {
	template_side_length_ = 3;
	kicks_ = kicks_jlstz_;
	TileColor shape[] = { _, x, x,
			      x, x, _,
			      _, _, _  };
	shape_.assign(shape, shape + template_side_length_ * template_side_length_);
	break;
      }
    case kTetroT:
      {
	template_side_length_ = 3;
	kicks_ = kicks_jlstz_;
        TileColor shape[] = { _, x, _,
			      x, x, x,
			      _, _, _  };
	shape_.assign(shape, shape + template_side_length_ * template_side_length_);
	break;
      }
    case kTetroZ:
      {
	template_side_length_ = 3;
	kicks_ = kicks_jlstz_;
	TileColor shape[] = { x, x, _,
			      _, x, x,
			      _, _, _  };
	shape_.assign(shape, shape + template_side_length_ * template_side_length_);
	break;
      }
    case kNone:
      template_side_length_ = 0;
      
    default:
      template_side_length_ = 0;
      break;
    }    
}

Tetromino& Tetromino::operator=(Tetromino&& other)
{
  if (this != &other)
    {
      type_ = kNone;
      color_ = kEmpty;
      template_side_length_ = 0;
      shape_.clear();
      kicks_.clear();

      type_ = other.type_;
      color_ = other.color_;
      template_side_length_ = other.template_side_length_;
      shape_ = other.shape_;
      kicks_ = other.kicks_;

      other.type_ = kNone;
      other.color_ = kEmpty;
      other.template_side_length_ = 0;
      other.shape_.clear();
      other.kicks_.clear();
    }
  return *this;
}

void Tetromino::Rotate(Rotation rotation)
{
  std::vector<TileColor> rotated_shape(shape_.size());
  GLint index = 0;

  if (rotation == kRight)
    {
      for (GLint col = template_side_length_ - 1; col >= 0; --col)
	{
	  for (GLuint row = 0; row < template_side_length_; ++row)
	    {
	      rotated_shape[row * template_side_length_ + col] = shape_[index++];
	    }
	}
      rotation_state_ = static_cast<enum RotationState>((rotation_state_ + 1) % 4);
    }
  else
    {
      for (GLuint col = 0; col < template_side_length_; ++col)
	{
	  for (GLint row = template_side_length_ - 1; row >= 0; --row)
	    {
	      rotated_shape[row * template_side_length_ + col] = shape_[index++];
	    }
	}
      rotation_state_ = static_cast<enum RotationState>((rotation_state_ + 3) % 4);
    }
  shape_ = rotated_shape;
}

Tetromino::~Tetromino()
{
  
}
