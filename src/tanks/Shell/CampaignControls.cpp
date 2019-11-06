#include "CampaignControls.h"
#include <Deathmatch.h>
#include <Button.h>
#include <DataSource.h>
#include <LayoutContext.h>
#include <Rating.h>
#include <StackLayout.h>

using namespace UI::DataSourceAliases;

namespace
{
	class DeathmatchRatingBinding : public UI::DataSource<unsigned int>
	{
	public:
		DeathmatchRatingBinding(const Deathmatch &deathmatch)
			: _deathmatch(deathmatch)
		{}

		// UI::DataSource<unsigned int>
		unsigned int GetValue(const UI::StateContext &sc) const override
		{
			return _deathmatch.GetRating();
		}

	private:
		const Deathmatch &_deathmatch;
	};
}

CampaignControls::CampaignControls(UI::LayoutManager &manager, TextureManager &texman, const Deathmatch &deathmatch, CampaignControlCommands commands)
	: UI::Window(manager)
	, _content(std::make_shared<UI::StackLayout>(manager))
	, _rating(std::make_shared<UI::Rating>(manager, texman))
{
	_content->SetFlowDirection(UI::FlowDirection::Horizontal);
	_content->SetSpacing(20);
	AddFront(_content);

	auto replay = std::make_shared<UI::Button>(manager, texman);
	replay->Resize(130, 30);
	replay->SetFont(texman, "font_default");
	replay->SetText("Replay"_txt);
	replay->eventClick = std::move(commands.replayCurrent);
	_content->AddFront(replay);

	_rating->SetRating(std::make_shared<DeathmatchRatingBinding>(deathmatch));
	_content->AddFront(_rating);

	auto next = std::make_shared<UI::Button>(manager, texman);
	next->Resize(130, 30);
	next->SetFont(texman, "font_default");
	next->SetText("Next"_txt);
	next->eventClick = std::move(commands.playNext);
	_content->AddFront(next);

}

FRECT CampaignControls::GetChildRect(TextureManager &texman, const UI::LayoutContext &lc, const UI::StateContext &sc, const UI::Window &child) const
{
	if (_content.get() == &child)
	{
		return MakeRectWH(lc.GetPixelSize());
	}
	return UI::Window::GetChildRect(texman, lc, sc, child);
}

vec2d CampaignControls::GetContentSize(TextureManager &texman, const UI::StateContext &sc, float scale) const
{
	return _content->GetContentSize(texman, sc, scale);
}
