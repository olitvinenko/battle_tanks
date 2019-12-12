#include "GameViewHarness.h"
#include "Camera.h"
#include "ctx/WorldController.h"
#include "gc/Explosion.h"
#include "gc/Player.h"
#include "gc/Vehicle.h"
#include "gc/World.h"
#include "render/WorldView.h"
#include <cassert>


RectInt GetCameraViewport(int screenW, int screenH, unsigned int camCount, unsigned int camIndex)
{
	assert(camCount > 0 && camCount <= 4);
	assert(camIndex < camCount);

	RectInt viewports[4];

	switch( camCount )
	{
		case 1:
			viewports[0] = RectInt{      0,             0,            screenW,         screenH };
			break;
		case 2:
			if (screenW >= screenH)
			{
				viewports[0] = RectInt{ 0,              0,            screenW / 2 - 1, screenH };
				viewports[1] = RectInt{screenW / 2 + 1, 0,            screenW,         screenH };
			}
			else
			{
				viewports[0] = RectInt{ 0,               0,           screenW,         screenH / 2 - 1 };
				viewports[1] = RectInt{ 0, screenH / 2 + 1,           screenW,         screenH };
			}
			break;
		case 3:
			viewports[0] = RectInt{ 0,               0,               screenW / 2 - 1, screenH / 2 - 1 };
			viewports[1] = RectInt{ screenW / 2 + 1, 0,               screenW,         screenH / 2 - 1 };
			viewports[2] = RectInt{ screenW / 4,     screenH / 2 + 1, screenW * 3 / 4, screenH};
			break;
		case 4:
			viewports[0] = RectInt{ 0,               0,               screenW / 2 - 1, screenH / 2 - 1 };
			viewports[1] = RectInt{ screenW / 2 + 1, 0,               screenW,         screenH / 2 - 1 };
			viewports[2] = RectInt{ 0,               screenH / 2 + 1, screenW / 2 - 1, screenH };
			viewports[3] = RectInt{ screenW / 2 + 1, screenH / 2 + 1, screenW,         screenH };
			break;
		default:
			assert(false);
	}

	return viewports[camIndex];
}

GameViewHarness::GameViewHarness(World &world, WorldController &worldController)
  : _world(world)
  , _scale(1)
  , _maxShakeCamera(nullptr)
{
    _world.eGC_RigidBodyStatic.AddListener(*this);
    _world.eGC_Explosion.AddListener(*this);

    for (GC_Player *player: worldController.GetLocalPlayers())
    {
        assert(player);
        Vector2 pos = player->GetVehicle() ? player->GetVehicle()->GetPos() : Center(_world._bounds);
        _cameras.emplace_back(pos, *player);
    }
}

GameViewHarness::~GameViewHarness()
{
    _world.eGC_Explosion.RemoveListener(*this);
    _world.eGC_RigidBodyStatic.RemoveListener(*this);
}

GameViewHarness::CanvasToWorldResult GameViewHarness::CanvasToWorld(unsigned int viewIndex, int x, int y) const
{
    assert(viewIndex < _cameras.size());
    CanvasToWorldResult result;
    const Camera &camera = IsSingleCamera() ? GetMaxShakeCamera() :_cameras[viewIndex];
    RectInt viewport = camera.GetViewport();
    x -= viewport.left;
    y -= viewport.top;
    result.visible = (0 <= x && x < viewport.right && 0 <= y && y < viewport.bottom);
	result.worldPos = camera.GetCameraPos() + (Vector2{ (float)x, (float)y } -Vector2{ (float)WIDTH(viewport), (float)HEIGHT(viewport) } / 2) / _scale;
    return result;
}

Vector2 GameViewHarness::WorldToCanvas(unsigned int viewIndex, Vector2 worldPos) const
{
    assert(viewIndex < _cameras.size());
    const Camera &camera = IsSingleCamera() ? GetMaxShakeCamera() : _cameras[viewIndex];
    RectInt viewport = camera.GetViewport();
	Vector2 viewPos = (worldPos - camera.GetCameraPos()) * _scale + Vector2{ (float)WIDTH(viewport), (float)HEIGHT(viewport) } / 2;
	return viewPos + Vector2{ (float)viewport.left, (float)viewport.top };
}

void GameViewHarness::SetCanvasSize(int pxWidth, int pxHeight, float scale)
{
    _pxWidth = pxWidth;
    _pxHeight = pxHeight;
    _scale = scale;

    unsigned int camCount = static_cast<unsigned int>(_cameras.size());
    for (unsigned int camIndex = 0; camIndex != camCount; ++camIndex)
    {
        auto effectiveCount = IsSingleCamera() ? 1 : camCount;
        auto effectiveIndex = IsSingleCamera() ? 0 : camIndex;
        _cameras[camIndex].SetViewport(GetCameraViewport(_pxWidth, _pxHeight, effectiveCount, effectiveIndex));
    }
}

void GameViewHarness::RenderGame(DrawingContext &dc, const WorldView &worldView) const
{
    if( !_cameras.empty() )
    {
        if (IsSingleCamera())
        {
            Vector2 eye = GetMaxShakeCamera().GetCameraPos();
            worldView.Render(dc, _world, GetMaxShakeCamera().GetViewport(), eye, _scale, false, false, _world.GetNightMode());
        }
        else for( auto &camera: _cameras )
        {
            Vector2 eye = camera.GetCameraPos();
            worldView.Render(dc, _world, camera.GetViewport(), eye, _scale, false, false, _world.GetNightMode());
        }
    }
    else
    {
        Vector2 eye = Center(_world._bounds);
        float zoom = std::max(_pxWidth / WIDTH(_world._bounds), _pxHeight / HEIGHT(_world._bounds));

        RectInt viewport{ 0, 0, _pxWidth, _pxHeight };
        worldView.Render(dc, _world, viewport, eye, zoom, false, false, _world.GetNightMode());
    }
}

static const Camera* FindMaxShakeCamera(const std::vector<Camera> &cameras)
{
    const Camera *maxShakeCamera = nullptr;
    for (auto &camera : cameras)
    {
        if (!maxShakeCamera || camera.GetShake() > maxShakeCamera->GetShake())
        {
            maxShakeCamera = &camera;
        }
    }
    return maxShakeCamera;
}

void GameViewHarness::Step(float dt)
{
    for (auto &camera: _cameras)
    {
        camera.CameraTimeStep(_world, dt, _scale);
    }
    _maxShakeCamera = nullptr;
}

const Camera& GameViewHarness::GetMaxShakeCamera() const
{
    if (!_maxShakeCamera)
    {
        _maxShakeCamera = FindMaxShakeCamera(_cameras);
        assert(_maxShakeCamera);
    }
    return *_maxShakeCamera;
}

bool GameViewHarness::IsSingleCamera() const
{
    return _pxWidth / _scale >= WIDTH(_world._bounds) && _pxHeight / _scale >= HEIGHT(_world._bounds);
}

void GameViewHarness::OnBoom(GC_Explosion &obj, float radius, float damage)
{
    for( auto &camera: _cameras )
    {
        RectInt viewport = camera.GetViewport();
		Vector2 viewSize{ (float)WIDTH(viewport), (float)HEIGHT(viewport) };
        Vector2 lt = camera.GetCameraPos() - viewSize / 2;
        Vector2 rb = lt + viewSize;

        float distanceLeft = lt.x - obj.GetPos().x;
        float distanceTop = lt.y - obj.GetPos().y;
        float distanceRight = obj.GetPos().x - rb.x;
        float distanceBottom = obj.GetPos().y - rb.y;
        float maxDistance = std::max(std::max(distanceLeft, distanceTop), std::max(distanceRight, distanceBottom));

        if (maxDistance < radius)
        {
            float sizeFactor = std::max(0.f, (radius - 50) / 100);
            float damageFactor = damage / 100;
            float distanceFactor = 1.0f - std::max(.0f, maxDistance) / radius;
            camera.Shake(sizeFactor * damageFactor * distanceFactor);
            _maxShakeCamera = nullptr;
        }
    }
}

void GameViewHarness::OnDamage(GC_RigidBodyStatic &obj, const DamageDesc &dd)
{
    for( auto &camera: _cameras )
    {
        if( &obj == camera.GetPlayer().GetVehicle() )
        {
            camera.Shake(obj.GetHealth() <= 0 ? 2.0f : dd.damage / obj.GetHealthMax());
            _maxShakeCamera = nullptr;
            break;
        }
    }
}
