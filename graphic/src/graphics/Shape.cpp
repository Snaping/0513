#include "Shape.h"

namespace GEngine {

Shape::Shape()
    : m_color(Color::Black)
    , m_lineWidth(1.0)
    , m_isSelected(false)
    , m_isVisible(true)
    , m_id(-1)
{
}

} 
