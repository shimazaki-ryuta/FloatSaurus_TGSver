
#pragma once
#include "MyMath.h"
#include<variant>
#include<string>
#include <map>
#include<json.hpp>
class GlovalVariables {
	using json = nlohmann::json;
public:
	static GlovalVariables* GetInstance();
    void Update();
	void CreateGroup(const std::string& groupName);
	void SetValue(const std::string& groupName, const std::string& key, int32_t value);
	void SetValue(const std::string& groupName, const std::string& key, float value);
	void SetValue(const std::string& groupName, const std::string& key, Vector3& value);
	
	void SaveFile(const std::string& groupName);
	void LoadFile(const std::string& groupName);
	void LoadFiles();
	void AddItem(const std::string& groupName, const std::string& key, int32_t value);
	void AddItem(const std::string& groupName, const std::string& key, float value);
	void AddItem(const std::string& groupName, const std::string& key, Vector3 value);

	int32_t GetIntValue(const std::string& groupName, const std::string& key);
	float GetFloatValue(const std::string& groupName, const std::string& key);
	Vector3 GetVector3Value(const std::string& groupName, const std::string& key);

private:
	GlovalVariables() = default;
	~GlovalVariables() = default;
	GlovalVariables(const GlovalVariables& obj) = default;
	GlovalVariables& operator=(const GlovalVariables& obj) = default;

public:
	struct Item {
		std::variant<int32_t, float, Vector3> value;
	};
	struct Group {
		std::map<std::string, Item> items;
	};
	std::map<std::string, Group> datas_;
	const std::string kDirectoryPath = "Resource/GlovalVariables/";
};
