#include "Component/LuaComponent.h"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <EASTL/string.h>

#include "../../../SunshineEditor/include/LogManager.h"
#include "Component/TransformComponent.h"
#include "Utils/StringUtils.h"
#include "Utils/DebugUtils.h"
#include "Utils/FileSystemWrapper.h"
#include "Scripting/ComponentBindings.h"
#include <GameObject/GameObject.h>
#include <Utils/StringHelper.h>

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
	
	/*
	std::error_code ec;
	if (!std::filesystem::exists(scriptPath.GetFullPath().c_str()))
	{
		wprintf(L"Lua file not found: %ls\n", scriptPath.GetFullPath().c_str());
	}
	*/
}

void LuaComponent_Info::InitLuaFile()
{
	ScanLuaFiles();
	printSunshineMessage(scriptPath.GetFullPath());
}

LuaComponent::LuaComponent()
	: lua(nullptr), scriptLoaded(false) {
}

LuaComponent::~LuaComponent() {
	//Cleanup();
}

void LuaComponent::Init(GameObject* obj, AssetPath inScriptPath) {

	this->obj = obj;
	scriptPath = inScriptPath;

	//InitLuaFile();
	LoadScript();
}

void LuaComponent::registerComponents()
{
    ScriptingBindings::RegisterAll(*lua);
}

void LuaComponent::Cleanup() {
	if (behaviorInitialized && scriptComponent.destroy.valid())
	{
		scriptComponent.destroy(scriptComponent.self);
	}

	if (lua) {
		lua = nullptr;
	}
	ClearState();
}

void LuaComponent::ClearState() {
	scriptLoaded = false;
}

void LuaComponent::LoadScript() {

	Cleanup();

	//InitLuaFile();

	lua = eastl::make_unique<sol::state>();
	lua->open_libraries(
		sol::lib::base,
		sol::lib::package,
		sol::lib::math,
		sol::lib::string,
		sol::lib::table,
		sol::lib::os
	);

	registerComponents();

	auto result = lua->script_file(WStringToUtf8(scriptPath.GetFullPath()).c_str());
	if (!result.valid()) 
	{
		sol::error err = result;
		printSunshineErrorMessage((eastl::string("Error running Lua script: ") + err.what()));
		return;
	}

	scriptLoaded = true;

	// InitializeBehavior();

	//scriptPath = assetsPath + "/" + luaFiles[selectedLuaFile];
	//printSunshineMessage(("%s is loaded!\n", scriptPath.c_str()));
}

void LuaComponent::LuaUpdate(float deltaTime)
{
	if (!behaviorInitialized || !scriptComponent.update.valid()){return;}
	auto result = scriptComponent.update(scriptComponent.self, deltaTime);
	if (!result.valid())
	{
		sol::error err = result;
		printSunshineErrorMessage((eastl::string("Error in update hook: ") + err.what()));
	}
}

void LuaComponent::InitializeBehavior()
{
	if (!scriptLoaded) {
		wprintf(L"%ls: InitializeBehavior: scriptLoaded is false!", scriptPath.GetFullPath().c_str());
		return;
	}

	if (!obj) {
		printSunshineErrorMessage("InitializeBehavior: obj is nullptr!");
		return;
	}

	if (!lua) {
		printSunshineErrorMessage("InitializeBehavior: lua is nullptr!");
		return;
	}

	sol::object behaviorObj = (*lua)["behavior"];
	if (!behaviorObj.valid()) {
		printSunshineErrorMessage("InitializeBehavior: behaviorObj is not valid!");
		return;
	}

	if (behaviorObj.get_type() != sol::type::table) {
		//printSunshineErrorMessage(eastl::string("InitializeBehavior: behavior is not a table, it's ") +
		//	std::to_string((int)behaviorObj.get_type()));
		return;
	}

	scriptComponent.self = behaviorObj.as<sol::table>();
	scriptComponent.self["id"] = reinterpret_cast<uintptr_t>(obj);
	scriptComponent.self["owner"] = obj;

	scriptComponent.start = scriptComponent.self["start"];
	scriptComponent.update = scriptComponent.self["update"];
	scriptComponent.destroy = scriptComponent.self["destroy"];

	if (scriptComponent.start.valid())
	{
		auto result = scriptComponent.start(scriptComponent.self);
		if (!result.valid())
		{
			sol::error err = result;
			printSunshineErrorMessage((eastl::string("Error in start hook: ") + err.what()));
		}
	}
	behaviorInitialized = true;
}
