#include "GlobalVariables.h"
#include "ImGuiManger.h"
#include <fstream>
GlovalVariables* GlovalVariables::GetInstance() {
	static GlovalVariables instance;
	return &instance;
}

void GlovalVariables::CreateGroup(const std::string& groupName) { datas_[groupName]; }
void GlovalVariables::SetValue(
	const std::string& groupName, const std::string& key, int32_t value) {
	Group& group = datas_[groupName];
	Item newItems{};

	newItems.value = value;
	group.items[key] = newItems;
}

void GlovalVariables::SetValue(const std::string& groupName, const std::string& key, float value) {
	Group& group = datas_[groupName];
	Item newItems{};

	newItems.value = value;
	group.items[key] = newItems;
}

void GlovalVariables::SetValue(
	const std::string& groupName, const std::string& key, Vector3& value) {
	Group& group = datas_[groupName];
	Item newItems{};

	newItems.value = value;
	group.items[key] = newItems;
}

void GlovalVariables::Update() {

	if (!ImGui::Begin("Global Variables", nullptr, ImGuiWindowFlags_MenuBar)) {
		ImGui::End();
		return;
	}

	if (!ImGui::BeginMenuBar())
		return;

	for (std::map<std::string, Group>::iterator itGroup = datas_.begin(); itGroup != datas_.end();
		++itGroup) {
		const std::string& groupName = itGroup->first;
		Group& group = itGroup->second;

		if (!ImGui::BeginMenu(groupName.c_str()))
			continue;

		for (std::map<std::string, Item>::iterator itItem = group.items.begin();
			itItem != group.items.end(); ++itItem) {
			const std::string& itemName = itItem->first;
			Item& item = itItem->second;

			if (std::holds_alternative<int32_t>(item.value)) {
				int32_t* ptr = std::get_if<int32_t>(&item.value);
				ImGui::SliderInt(itemName.c_str(), ptr, 0, 100);
			}
			else if (std::holds_alternative<float>(item.value)) {
				float* ptr = std::get_if<float>(&item.value);
				ImGui::DragFloat(itemName.c_str(), ptr, 0.01f);
			}
			else if (std::holds_alternative<Vector3>(item.value)) {
				Vector3* ptr = std::get_if<Vector3>(&item.value);
				ImGui::DragFloat3(itemName.c_str(), reinterpret_cast<float*>(ptr), 0.01f);
			}
		}
		ImGui::Text("\n");




		if (ImGui::Button("Save")) {
			SaveFile(groupName);
			std::string message = std::format("{}.json saved", groupName);
			MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		}

		ImGui::EndMenu();
	}

	ImGui::EndMenuBar();
	ImGui::End();

}

void GlovalVariables::SaveFile(const std::string& groupName) {
	std::map<std::string, Group>::iterator itGroup = datas_.find(groupName);

	assert(itGroup != datas_.end());

	json root;
	root = json::object();

	root[groupName] = json::object();

	for (std::map<std::string, Item>::iterator itItem = itGroup->second.items.begin();
		itItem != itGroup->second.items.end(); ++itItem) {
		const std::string& itemName = itItem->first;
		Item& item = itItem->second;

		if (std::holds_alternative<int32_t>(item.value)) {
			root[groupName][itemName] = std::get<int32_t>(item.value);
		}
		else if (std::holds_alternative<float>(item.value)) {
			root[groupName][itemName] = std::get<float>(item.value);
		}

		if (std::holds_alternative<Vector3>(item.value)) {
			Vector3 value = std::get<Vector3>(item.value);
			root[groupName][itemName] = json::array({ value.x, value.y, value.z });
		}
	}
	std::filesystem::path dir(kDirectoryPath);

	if (!std::filesystem::exists(kDirectoryPath)) {
		std::filesystem::create_directory(kDirectoryPath);
	}
	std::string filePath = kDirectoryPath + groupName + ".json";
	std::ofstream ofs{};
	ofs.open(filePath);

	if (ofs.fail()) {
		std::string message = "Failed Open Data File Fow Write";
		MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		assert(0);
		return;
	}

	ofs << std::setw(4) << root << std::endl;
	ofs.close();
}

void GlovalVariables::LoadFile(const std::string& groupName) {
	std::string filePath = kDirectoryPath + groupName + ".json";

	std::ifstream ifs;
	ifs.open(filePath);

	if (ifs.fail()) {
		std::string message = "Failed Open Data File For Write";
		MessageBoxA(nullptr, message.c_str(), "GlobalVariables", 0);
		assert(0);
		return;
	}

	json root;
	ifs >> root;
	ifs.close();

	json::iterator itGroup = root.find(groupName);

	assert(itGroup != root.end());

	for (json::iterator itItem = itGroup->begin(); itItem != itGroup->end(); ++itItem) {
		const std::string& itemName = itItem.key();

		if (itItem->is_number_integer()) {
			int32_t value = itItem->get<int32_t>();
			SetValue(groupName, itemName, value);
		}
		else if (itItem->is_number_float()) {
			double value = itItem->get<double>();
			SetValue(groupName, itemName, static_cast<float>(value));
		}
		else if (itItem->is_array() && itItem->size() == 3) {
			Vector3 value = { itItem->at(0), itItem->at(1), itItem->at(2) };
			SetValue(groupName, itemName, value);
		}
	}
}

void GlovalVariables::LoadFiles() {
	std::filesystem::path dir(kDirectoryPath);

	if (!std::filesystem::exists(kDirectoryPath)) {
		return;
	}

	std::filesystem::directory_iterator dir_it(kDirectoryPath);

	for (const std::filesystem::directory_entry& entry : dir_it) {
		const std::filesystem::path& filePath = entry.path();
		std::string extension = filePath.extension().string();

		if (extension.compare(".json") != 0) {
			continue;
		}

		LoadFile(filePath.stem().string());
	}
}
void GlovalVariables::AddItem(const std::string& groupName, const std::string& key, int32_t value) {
	Group& group = datas_[groupName];

	if (group.items.find(key) == group.items.end()) {
		SetValue(groupName, key, value);
	}
}

void GlovalVariables::AddItem(const std::string& groupName, const std::string& key, float value) {
	Group& group = datas_[groupName];

	if (group.items.find(key) == group.items.end()) {
		SetValue(groupName, key, value);
	}
}

void GlovalVariables::AddItem(const std::string& groupName, const std::string& key, Vector3 value) {
	Group& group = datas_[groupName];

	if (group.items.find(key) == group.items.end()) {
		SetValue(groupName, key, value);
	}
}

int32_t GlovalVariables::GetIntValue(const std::string& groupName, const std::string& key) {
	assert(datas_.find(groupName) != datas_.end());

	Group& group = datas_[groupName];
	assert(group.items.find(key) != group.items.end());

	return std::get<int32_t>(group.items[key].value);
}

float GlovalVariables::GetFloatValue(const std::string& groupName, const std::string& key) {
	assert(datas_.find(groupName) != datas_.end());

	Group& group = datas_[groupName];
	assert(group.items.find(key) != group.items.end());

	return std::get<float>(group.items[key].value);
}

Vector3 GlovalVariables::GetVector3Value(const std::string& groupName, const std::string& key) {
	assert(datas_.find(groupName) != datas_.end());

	Group& group = datas_[groupName];
	assert(group.items.find(key) != group.items.end());

	return std::get<Vector3>(group.items[key].value);
}