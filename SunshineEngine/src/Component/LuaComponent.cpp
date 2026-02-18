#include "Component/LuaComponent.h"

#include <iostream>
#include <filesystem>
#include <fstream>
#include <EASTL/string.h>

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
	: lua(nullptr), scriptLoaded(false), foundFunction(false) {
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
	foundFunction = false;
	params.clear();
	lastResult.clear();
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

	foundFunction = false;
	params.clear();
	lastResult.clear();
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

bool LuaComponent::FindFunction() {
	if (!scriptLoaded) { //|| functionName.empty()
		printSunshineErrorMessage("Lua not initialized or function name empty!");
		return false;
	}

	//callSolFunction(functionName, 2, 3);

	foundFunction = false;
	params.clear();

	sol::function func = (*lua)[functionName];
	if (func.valid()) {
		foundFunction = true;
		LoadParamsFromLua();
		return true;
	}
	else {
		printSunshineErrorMessage("No such function");
		return false;
	}
}

void LuaComponent::LoadParamsFromLua() {
	params.clear();
	sol::object paramTable = (*lua)[(eastl::string(functionName) + "_params").c_str()];
	if (paramTable.valid() && paramTable.get_type() == sol::type::table)
	{
		sol::table tbl = paramTable;
		for (auto& pair : tbl) {
			ParamEntry entry;
			sol::table paramEntry = pair.second.as<sol::table>();
			entry.name = eastl::string(paramEntry["name"].get<std::string>().c_str());
			entry.type = eastl::string(paramEntry["type"].get<std::string>().c_str());
			params.push_back(entry);
		}
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

bool LuaComponent::CallFunction() {

	sol::function func = (*lua)[functionName];
	if (!func.valid())
	{
		printSunshineErrorMessage("No such function");
		return false;
	}

	eastl::vector<sol::object> args;
	for (const ParamEntry& p : params) {
		eastl::string val(p.value);
		if (EASTLStringEqualsChar(p.type, "userdata")) {
			args.push_back(sol::make_object(*lua, obj));
		}
		else if (EASTLStringEqualsChar(p.type, "number")) {
			double valDbl = strtod(val.c_str(), nullptr);
			args.push_back(sol::make_object(*lua, valDbl));
		}
		else if (EASTLStringEqualsChar(p.type, "bool")) {
			args.push_back(sol::make_object(*lua, EASTLStringEqualsChar(val, "true") || EASTLStringEqualsChar(val, "1")));
		}
		else {
			args.push_back(sol::make_object(*lua, EASTLToStdString(val)));
		}
	}

	sol::protected_function_result result = func(sol::as_args(args));
	if (!result.valid()) {
		sol::error err = result;
		printSunshineErrorMessage(("Lua error: " + eastl::string(err.what())));
		return false;
	}

	sol::object res = result.get<sol::object>();
	if (!res.valid() || res.is<sol::nil_t>()) lastResult = "";
	else if (res.is<std::string>()) lastResult = eastl::string(res.as<std::string>().c_str());
	else if (res.is<double>()) lastResult = eastl::to_string(res.as<double>());
	else if (res.is<bool>()) lastResult = res.as<bool>() ? "true" : "false";
	else lastResult = "<unsupported return type>";
	return true;
}

eastl::vector<eastl::string> LuaComponent::GetAvailableFunctions() const
{
	eastl::vector<eastl::string> functions;
    
	if (scriptPath.m_assetRelativePath.empty()) return functions;
        
	std::ifstream file(scriptPath.GetFullPath().c_str());
	if (!file.is_open()) return functions;

	std::string stdLine;
	while (std::getline(file, stdLine)) 
	{
		eastl::string line = stdLine.c_str();
		size_t funcPos = line.find("function");
		if (funcPos == eastl::string::npos) continue;
        
		size_t parenPos = line.find('(', funcPos + 8);
		if (parenPos == eastl::string::npos) continue;
		
		eastl::string funcName = line.substr(funcPos + 8, parenPos - funcPos - 8);
		
		funcName.erase(0, funcName.find_first_not_of(" \t"));
		funcName.erase(funcName.find_last_not_of(" \t") + 1);
        
		if (!funcName.empty()) 
		{
			functions.push_back(funcName);
		}
	}
    
	file.close();
	return functions;
}
