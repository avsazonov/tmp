#ifndef GAMEOBJECT_H_INCLUDED
#define GAMEOBJECT_H_INCLUDED

#include <limits>
#include <string>
#include <map>
#include <stdexcept>

// Настройка для любого объекта в игре.
// Представляет собой число с плавающей точкой.
// При необходимости, можно отнаследовать.
// Содержит признак, установлена или нет.
class Setting {

	float mSetting;
	bool  mSet;

public:
	// Конструкторы настройки: по умолчанию, по значению, и копирования
	Setting() : mSet(false) {}
	Setting(float value) : mSetting(value), mSet(true) {};
	Setting(const Setting& setting) : mSetting(setting.getValue()), mSet(setting.isSet()) {}

	// Получение значения
	virtual float getValue() const {
		if (mSet)
			return mSetting;
		else
			// Возвращаем "ерунду", если запрашивают не установленное значение
			return std::numeric_limits<float>::max();
	}

	// Установка значения настройки
	virtual void setValue(float value) { 
		mSetting = value;
		mSet = true;
	}

	// Проверка, установлена ли настройка
	virtual bool isSet() const {
		return mSet;
	}

	// Виртуальный деструктор
	virtual ~Setting() {}
};

// Любой объект игры содержит настройки.
// Настройки можно устанавливать и получать. 
// Создать абстрактный объект игры нельзя.
class GameObject {

	// Контейнер настроек: название <--> объект
	typedef std::map<std::string, Setting> Settings;
	Settings mSettings;

public:
	// Виртуальный деструктор
	virtual ~GameObject() {}

	// Установка настройки объекта
	void setSetting(const std::string& name, const Setting& setting) {
		mSettings[name] = setting;
	}

	// Получение настройки
	Setting getSetting(const std::string& name) const {
		// Если настройки нет - то её не добавляем в контейнер!
		Settings::const_iterator setting_pair = mSettings.find(name);
		if (setting_pair == mSettings.end())
#ifdef DEBUG_SETTINGS_CHECK
			// в отладочной версии генерируем исключение
			throw std::runtime_error(std::string("Setting '") + name + std::string("' not found"));
#else
			return Setting();
#endif
#ifdef DEBUG_SETTINGS_CHECK
		// не сказать что параноидальная проверка, но весьма полезная
		if (!setting_pair->second.isSet())
			throw std::runtime_error(std::string("Setting '") + name + std::string("' not set"));
#endif
		return setting_pair->second;
	}

	// Поиск настройки
	bool hasSetting(const std::string& name) const {
		return mSettings.find(name) != mSettings.end();
	}

	// Этот чистый виртуальный метод должен определять каждый конечный объект игры
	virtual std::string getObjectName() const = 0;
};


#include "DrawHelper.h"

// Интерфейс для всех отображаемых объектов + объект в помощь отрисовке.
// Таким образом этот абстрактный класс позволяет отображать объект игры.
// Также говорит, откликается ли объект на нажатия. По умолчанию - нет.
class Renderable {
public:
	// Отрисовка. Не может менять объект
	virtual void render() const = 0;
	
	// Нажимается ли
	virtual bool isClickable() const { return false; }

	// Нажата ли область объекта? Координаты игрового мира
	virtual bool isMyAreaClicked(float cellX, float cellY) const { return false; }
protected:

	// Объект, который может отображать что-либо на экране
	DrawHelper mDrawHelper;
};

#endif
