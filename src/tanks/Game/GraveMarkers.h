#ifndef GRAVE_MARKERS_H
#define GRAVE_MARKERS_H
#pragma warning (disable : 4786)
//-----------------------------------------------------------------------------
//
//  Name:   GraveMarkers.h
//
//  Author: Mat Buckland (ai-junkie.com)
//
//  Desc:   Class to record and render graves at the site of a bot's death
//
//-----------------------------------------------------------------------------
#include <list>
#include <vector>
#include "vector2d.h"
#include "crudetimer.h"

class DrawingContext;
class TextureManager;

class GraveMarkers
{
private:
  
  struct GraveRecord
  {
    Vector2D Position;
    double    TimeCreated;

    GraveRecord(Vector2D pos):Position(pos),
                              TimeCreated(Clock->GetCurrentTime())
    {}
  };

private:
  
  typedef std::list<GraveRecord> GraveList;

private:

  //how long a grave remains on screen
  double m_dLifeTime;

  //when a bot dies, a grave is rendered to mark the spot.
  std::vector<Vector2D>   m_vecRIPVB;
  std::vector<Vector2D>   m_vecRIPVBTrans;
  GraveList               m_GraveList;

  size_t m_fontTexture;

public:

  GraveMarkers(double lifetime, const TextureManager& tm);

  void Update();
  void Render(DrawingContext& dc);
  void AddGrave(Vector2D pos);

};

#endif