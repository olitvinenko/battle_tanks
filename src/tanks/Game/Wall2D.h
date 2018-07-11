#ifndef WALL_H
#define WALL_H
//------------------------------------------------------------------------
//
//  Name:   Wall2D.h
//
//  Desc:   class to create and render 2D walls. Defined as the two 
//          vectors A - B with a perpendicular normal. 
//          
//
//  Author: Mat Buckland (fup@ai-junkie.com)
//
//------------------------------------------------------------------------
#include "misc/Cgdi.h"
#include "Vector2D.h"
#include <fstream>
#include "DrawingContext.h"
#include "TextureManager.h"


class Wall2D 
{
protected:

  Vector2D    m_vA,
              m_vB,
              m_vN;

  void CalculateNormal()
  {
    Vector2D temp = Vec2DNormalize(m_vB - m_vA);

    m_vN.x = -temp.y;
    m_vN.y = temp.x;
  }

private:
	size_t m_wallLine;

	void FindSprite(const TextureManager& tm)
	{
		m_wallLine = tm.FindSprite("lightning");
	}

public:

  Wall2D(){}

  Wall2D(Vector2D A, Vector2D B, const TextureManager& tm):m_vA(A), m_vB(B)
  {
    CalculateNormal();
	FindSprite(tm);
  }

  Wall2D(Vector2D A, Vector2D B, Vector2D N, const TextureManager& tm):m_vA(A), m_vB(B), m_vN(N)
  {
	  FindSprite(tm);
  }

  Wall2D(std::fstream& in, const TextureManager& tm)
  {
	  Read(in);
	  FindSprite(tm);
  }

  virtual void Render(DrawingContext& dc, bool RenderNormals = false)const
  {
	dc.DrawLine(m_wallLine, SpriteColor(255.0f, 255.0f, 255.0f, 255.0f), m_vA.x, m_vA.y, m_vB.x, m_vB.y, 0);

    //render the normals if rqd
    if (RenderNormals)
    {
      int MidX = (int)((m_vA.x+m_vB.x)/2);
      int MidY = (int)((m_vA.y+m_vB.y)/2);

	  dc.DrawLine(m_wallLine, SpriteColor(255.0f, 255.0f, 255.0f, 255.0f), MidX, MidY, (int)(MidX + (m_vN.x * 5)), (int)(MidY + (m_vN.y * 5)), 0);
    }
  }

  Vector2D From()const  {return m_vA;}
  void     SetFrom(Vector2D v){m_vA = v; CalculateNormal();}

  Vector2D To()const    {return m_vB;}
  void     SetTo(Vector2D v){m_vB = v; CalculateNormal();}
  
  Vector2D Normal()const{return m_vN;}
  void     SetNormal(Vector2D n){m_vN = n;}
  
  Vector2D Center()const{return (m_vA+m_vB)/2.0;}

  std::ostream& Wall2D::Write(std::ostream& os)const
  {
    os << std::endl;
    os << From() << ",";
    os << To() << ",";
    os << Normal();
    return os;
  }

 void Read(std::fstream& in)
  {
    double x,y;

    in >> x >> y;
    SetFrom(Vector2D(x,y));

    in >> x >> y;
    SetTo(Vector2D(x,y));

     in >> x >> y;
    SetNormal(Vector2D(x,y));
  }
  
};

#endif