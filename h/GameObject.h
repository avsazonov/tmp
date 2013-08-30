#ifndef GAMEOBJECT_H_INCLUDED
#define GAMEOBJECT_H_INCLUDED

#include <limits>
#include <string>
#include <map>
#include <stdexcept>

// ��������� ��� ������ ������� � ����.
// ������������ ����� ����� � ��������� ������.
// ��� �������������, ����� �������������.
// �������� �������, ����������� ��� ���.
class Setting {

	float mSetting;
	bool  mSet;

public:
	// ������������ ���������: �� ���������, �� ��������, � �����������
	Setting() : mSet(false) {}
	Setting(float value) : mSetting(value), mSet(true) {};
	Setting(const Setting& setting) : mSetting(setting.getValue()), mSet(setting.isSet()) {}

	// ��������� ��������
	virtual float getValue() const {
		if (mSet)
			return mSetting;
		else
			// ���������� "������", ���� ����������� �� ������������� ��������
			return std::numeric_limits<float>::max();
	}

	// ��������� �������� ���������
	virtual void setValue(float value) { 
		mSetting = value;
		mSet = true;
	}

	// ��������, ����������� �� ���������
	virtual bool isSet() const {
		return mSet;
	}

	// ����������� ����������
	virtual ~Setting() {}
};

// ����� ������ ���� �������� ���������.
// ��������� ����� ������������� � ��������. 
// ������� ����������� ������ ���� ������.
class GameObject {

	// ��������� ��������: �������� <--> ������
	typedef std::map<std::string, Setting> Settings;
	Settings mSettings;

public:
	// ����������� ����������
	virtual ~GameObject() {}

	// ��������� ��������� �������
	void setSetting(const std::string& name, const Setting& setting) {
		mSettings[name] = setting;
	}

	// ��������� ���������
	Setting getSetting(const std::string& name) const {
		// ���� ��������� ��� - �� � �� ��������� � ���������!
		Settings::const_iterator setting_pair = mSettings.find(name);
		if (setting_pair == mSettings.end())
#ifdef DEBUG_SETTINGS_CHECK
			// � ���������� ������ ���������� ����������
			throw std::runtime_error(std::string("Setting '") + name + std::string("' not found"));
#else
			return Setting();
#endif
#ifdef DEBUG_SETTINGS_CHECK
		// �� ������� ��� �������������� ��������, �� ������ ��������
		if (!setting_pair->second.isSet())
			throw std::runtime_error(std::string("Setting '") + name + std::string("' not set"));
#endif
		return setting_pair->second;
	}

	// ����� ���������
	bool hasSetting(const std::string& name) const {
		return mSettings.find(name) != mSettings.end();
	}

	// ���� ������ ����������� ����� ������ ���������� ������ �������� ������ ����
	virtual std::string getObjectName() const = 0;
};


#include "DrawHelper.h"

// ��������� ��� ���� ������������ �������� + ������ � ������ ���������.
// ����� ������� ���� ����������� ����� ��������� ���������� ������ ����.
// ����� �������, ����������� �� ������ �� �������. �� ��������� - ���.
class Renderable {
public:
	// ���������. �� ����� ������ ������
	virtual void render() const = 0;
	
	// ���������� ��
	virtual bool isClickable() const { return false; }

	// ������ �� ������� �������? ���������� �������� ����
	virtual bool isMyAreaClicked(float cellX, float cellY) const { return false; }
protected:

	// ������, ������� ����� ���������� ���-���� �� ������
	DrawHelper mDrawHelper;
};

#endif
