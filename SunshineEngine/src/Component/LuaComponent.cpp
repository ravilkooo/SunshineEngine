#include "Component/LuaComponent.h"
#include "Component/TransformComponent.h"

#include <Utils/DebugUtils.h>
#include "Utils/StringUtils.h"
#include "Utils/FileSystemWrapper.h"
#include <Utils/StringHelper.h>

#include "Scripting/ComponentBindings.h"
#include <Scripting/LuaManager.h>

#include <GameObject/GameObject.h>

#include <iostream>
#include <filesystem>
#include <fstream>
#include <EASTL/string.h>

eastl::vector<AssetPath> LuaComponent_Info::luaFiles;

void LuaComponent_Info::ScanLuaFiles()
{
	luaFiles.clear();
	std::error_code ec;

	AssetPath scriptDirPath(L"Scripts/", AssetPath::AssetSource::Project);
	AssetPath currentScript(L"Scripts/", AssetPath::AssetSource::Project);
	//for (eastlfs::directory_iterator it(dirPath); it != eastlfs::directory_iterator(""); ++it) {
	eastlfs::directory_iterator end;
	for (eastlfs::directory_iterator it(WStringToUtf8(scriptDirPath.GetFullPath()), ec); it != end; ++it)
	{
		auto& entry = it.entry();
		if (eastlfs::is_regular_file(entry))
		{
			eastl::string filename = eastlfs::filename(entry);
			currentScript.m_assetRelativePath = L"Scripts/" + Utf8ToWString(filename);

			if (filename.size() > 4 && EASTLStringEqualsChar(filename.substr(filename.size() - 4), ".lua"))
				luaFiles.push_back(currentScript);
		}
	}
	if (ec)
	{
		wprintf(L"Lua dir not found: %ls\n", scriptDirPath.GetFullPath().c_str());
	}
}

void LuaComponent_Info::InitLuaFile()
{
	ScanLuaFiles();
	printSunshineMessage(scriptPath.GetFullPath());
}

LuaComponent::LuaComponent()
	: scriptLoaded(false) {
}

LuaComponent::~LuaComponent() {
	//Cleanup();
}

void LuaComponent::Init(GameObject* obj, AssetPath inScriptPath) {

	this->obj = obj;
	scriptPath = inScriptPath;
}

void LuaComponent::Cleanup() {
	if (behaviorInitialized && scriptData.destroy.valid())
	{
		scriptData.destroy(scriptData.self);
	}

	ClearState();
}

void LuaComponent::ClearState() {
	scriptLoaded = false;
}

void LuaComponent::LoadScript() {

	Cleanup();

	scriptData.self = LuaManager::GetInstance().LoadScript(scriptPath);
	if (!scriptData.self.valid()) {
		printSunshineErrorMessage((eastl::string("Failed to load Lua script: ") + WStringToUtf8(scriptPath.GetFullPath())));
		return;
	}
	
	scriptLoaded = true;
}

void LuaComponent::LuaUpdate(float deltaTime)
{
	if (!behaviorInitialized || !scriptData.update.valid()) { return; }
	auto result = scriptData.update(scriptData.self, deltaTime);
	if (!result.valid())
	{
		sol::error err = result;
		printSunshineErrorMessage((eastl::string("Error in update hook: ") + err.what()));
	}
}

void LuaComponent::InitializeBehavior()
{
	LoadScript();
	if (!scriptLoaded) {
		printSunshineErrorMessage(L"InitializeBehavior: scriptLoaded is false! " + scriptPath.GetFullPath());
		return;
	}

	if (!obj) {
		printSunshineErrorMessage("InitializeBehavior: obj is nullptr!");
		return;
	}
	
	LoadScript();
	
	//scriptData.self = behaviorObj.as<sol::table>();
	scriptData.self["id"] = reinterpret_cast<uintptr_t>(obj);
	scriptData.self["owner"] = obj;

	scriptData.start = scriptData.self["start"];
	scriptData.update = scriptData.self["update"];
	scriptData.destroy = scriptData.self["destroy"];

	if (scriptData.start.valid())
	{
		auto result = scriptData.start(scriptData.self);
		if (!result.valid())
		{
			sol::error err = result;
			printSunshineErrorMessage((eastl::string("Error in start hook: ") + err.what()));
		}
	}
	behaviorInitialized = true;
}
