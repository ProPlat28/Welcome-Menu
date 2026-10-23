#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <ctime>

using namespace geode::prelude;

static const char* DAYS[] = {
	"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};
static const char* MONTHS[] = {
	"January", "February", "March", "April", "May", "June",
	"July", "August", "September", "October", "November", "December"
};

class $modify(ClockMenuLayer, MenuLayer) {
	struct Fields {
		CCLabelBMFont* label = nullptr;
		std::string lastText;
	};

	static const char* greetingFor(int hour) {
		if (hour < 5) return "Good night";
		if (hour < 12) return "Good morning";
		if (hour < 18) return "Good afternoon";
		return "Good evening";
	}

	static const char* ordinalSuffix(int day) {
		if (day >= 11 && day <= 13) return "th";
		switch (day % 10) {
			case 1: return "st";
			case 2: return "nd";
			case 3: return "rd";
			default: return "th";
		}
	}

	void updateClock(float) {
		auto label = m_fields->label;
		if (!label) return;

		auto mod = Mod::get();
		bool showGreeting = mod->getSettingValue<bool>("show-greeting");
		bool showDate = mod->getSettingValue<bool>("show-date");
		bool showSeconds = mod->getSettingValue<bool>("show-seconds");
		bool use24h = mod->getSettingValue<bool>("use-24h");
		int opacity = static_cast<int>(mod->getSettingValue<int64_t>("opacity"));

		std::time_t now = std::time(nullptr);
		std::tm* lt = std::localtime(&now);
		if (!lt) return;

		std::string text;
		if (showGreeting) {
			text += greetingFor(lt->tm_hour);
			text += "\n";
		}

		int hour = lt->tm_hour;
		std::string suffix;
		if (!use24h) {
			suffix = hour >= 12 ? " PM" : " AM";
			hour = hour % 12;
			if (hour == 0) hour = 12;
		}
		if (showSeconds) {
			text += fmt::format("{:02}:{:02}:{:02}{}", hour, lt->tm_min, lt->tm_sec, suffix);
		} else {
			text += fmt::format("{:02}:{:02}{}", hour, lt->tm_min, suffix);
		}

		if (showDate) {
			text += fmt::format(
				"\n{}, {} {}{}",
				DAYS[lt->tm_wday], MONTHS[lt->tm_mon], lt->tm_mday, ordinalSuffix(lt->tm_mday)
			);
		}

		label->setOpacity(static_cast<GLubyte>(255 * opacity / 100));

		if (text != m_fields->lastText) {
			m_fields->lastText = text;
			label->setString(text.c_str());
		}
	}

	bool init() {
		if (!MenuLayer::init()) return false;

		auto win = CCDirector::get()->getWinSize();

		auto label = CCLabelBMFont::create("", "bigFont.fnt");
		label->setID("clock-label"_spr);
		label->setScale(0.4f);
		label->setAlignment(kCCTextAlignmentLeft);
		label->setAnchorPoint({ 0.f, 1.f });
		label->setPosition({ 8.f, win.height - 8.f });
		this->addChild(label, 10);

		m_fields->label = label;

		this->updateClock(0.f);
		this->schedule(schedule_selector(ClockMenuLayer::updateClock), 0.5f);

		return true;
	}
};