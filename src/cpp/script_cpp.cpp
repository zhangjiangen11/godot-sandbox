#include "script_cpp.h"

#include "script_language_cpp.h"
#include "script_cpp_instance.h"
#include "../elf/script_instance.h"
#include "../sandbox_project_settings.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/classes/file_access.hpp>
#include <godot_cpp/classes/resource_loader.hpp>
static constexpr bool VERBOSE_LOGGING = false;

bool CPPScript::DetectCMakeOrSConsProject() {
	static bool detected = false;
	static bool detected_value = false;
	// Avoid file system checks if the project type has already been detected
	if (detected) {
		return detected_value;
	}
	// If the project root contains a CMakeLists.txt file, or a cmake/CMakeLists.txt,
	// build the project using CMake
	// Get the project root using res://
	String project_root = "res://";
	detected = true;

	// Check for CMakeLists.txt in the project root
	const bool cmake_root = FileAccess::file_exists(project_root + "CMakeLists.txt");
	if (cmake_root) {
		detected_value = true;
		return true;
	}
	const bool cmake_dir = FileAccess::file_exists(project_root + "cmake/CMakeLists.txt");
	if (cmake_dir) {
		detected_value = true;
		return true;
	}
	const bool scons_root = FileAccess::file_exists(project_root + "SConstruct");
	if (scons_root) {
		detected_value = true;
		return true;
	}
	detected_value = false;
	return false;
}

bool CPPScript::_editor_can_reload_from_file() {
	return true;
}
void CPPScript::_placeholder_erased(void *p_placeholder) {}
bool CPPScript::_can_instantiate() const {
	return true;
}
Ref<Script> CPPScript::_get_base_script() const {
	return Ref<Script>();
}
StringName CPPScript::_get_global_name() const {
	return PathToGlobalName(this->path);
}
bool CPPScript::_inherits_script(const Ref<Script> &p_script) const {
	return false;
}
StringName CPPScript::_get_instance_base_type() const {
	return StringName("Sandbox");
}
void *CPPScript::_instance_create(Object *p_for_object) const {
	CPPScriptInstance *instance = memnew(CPPScriptInstance(p_for_object, Ref<CPPScript>(this)));
	instances.insert(instance);
	return ScriptInstanceExtension::create_native_instance(instance);
}
void *CPPScript::_placeholder_instance_create(Object *p_for_object) const {
	return _instance_create(p_for_object);
}
bool CPPScript::_instance_has(Object *p_object) const {
	return false;
}
bool CPPScript::_has_source_code() const {
	return true;
}
String CPPScript::_get_source_code() const {
	return source_code;
}
void CPPScript::_set_source_code(const String &p_code) {
	source_code = p_code;
}
Error CPPScript::_reload(bool p_keep_state) {
	this->set_file(this->path);
	return Error::OK;
}
TypedArray<Dictionary> CPPScript::_get_documentation() const {
	return TypedArray<Dictionary>();
}
String CPPScript::_get_class_icon_path() const {
	return String("res://addons/godot_sandbox/CPPScript.svg");
}
bool CPPScript::_has_method(const StringName &p_method) const {
	if (p_method == StringName("_init"))
		return true;
	if (instances.is_empty()) {
		if constexpr (VERBOSE_LOGGING) {
			ERR_PRINT("CPPScript::has_method: No instances available.");
		}
		return false;
	}
	CPPScriptInstance *instance = *instances.begin();
	if (instance == nullptr) {
		if constexpr (VERBOSE_LOGGING) {
			ERR_PRINT("CPPScript::has_method: Instance is null.");
		}
		return false;
	}
	ELFScriptInstance *elf = instance->get_script_instance();
	if (elf == nullptr) {
		return false;
	}
	// Get the method information from the ELFScriptInstance
	return elf->get_elf_script()->has_method(p_method);
}
bool CPPScript::_has_static_method(const StringName &p_method) const {
	return false;
}
Dictionary CPPScript::_get_method_info(const StringName &p_method) const {
	if (instances.is_empty()) {
		if constexpr (VERBOSE_LOGGING) {
			ERR_PRINT("CPPScript::_get_method_info: No instances available.");
		}
		return Dictionary();
	}
	CPPScriptInstance *instance = *instances.begin();
	if (instance == nullptr) {
		if constexpr (VERBOSE_LOGGING) {
			ERR_PRINT("CPPScript::_get_method_info: Instance is null.");
		}
		return Dictionary();
	}
	ELFScriptInstance *elf = instance->get_script_instance();
	if (elf == nullptr) {
		return Dictionary();
	}
	// Get the method information from the ELFScriptInstance
	return elf->get_elf_script()->_get_method_info(p_method);
}
bool CPPScript::_is_tool() const {
	return true;
}
bool CPPScript::_is_valid() const {
	return true;
}
bool CPPScript::_is_abstract() const {
	return false;
}
ScriptLanguage *CPPScript::_get_language() const {
	return CPPScriptLanguage::get_singleton();
}
bool CPPScript::_has_script_signal(const StringName &p_signal) const {
	return false;
}
TypedArray<Dictionary> CPPScript::_get_script_signal_list() const {
	return TypedArray<Dictionary>();
}
bool CPPScript::_has_property_default_value(const StringName &p_property) const {
	return false;
}
Variant CPPScript::_get_property_default_value(const StringName &p_property) const {
	return Variant();
}
void CPPScript::_update_exports() {}
TypedArray<Dictionary> CPPScript::_get_script_method_list() const {
	if (instances.is_empty()) {
		if constexpr (VERBOSE_LOGGING) {
			ERR_PRINT("CPPScript::_get_script_method_list: No instances available.");
		}
		return {};
	}
	CPPScriptInstance *instance = *instances.begin();
	if (instance == nullptr) {
		if constexpr (VERBOSE_LOGGING) {
			ERR_PRINT("CPPScript::_get_script_method_list: Instance is null.");
		}
		return {};
	}
	ELFScriptInstance *elf = instance->get_script_instance();
	if (elf == nullptr) {
		return {};
	}
	// Get the method information from the ELFScriptInstance
	return elf->get_elf_script()->_get_script_method_list();
}
TypedArray<Dictionary> CPPScript::_get_script_property_list() const {
	TypedArray<Dictionary> properties;
	Dictionary property;
	property["name"] = "associated_script";
	property["type"] = Variant::OBJECT;
	property["hint"] = PROPERTY_HINT_NODE_TYPE;
	property["hint_string"] = "Node";
	property["usage"] = PROPERTY_USAGE_DEFAULT | PROPERTY_USAGE_SCRIPT_VARIABLE;
	//property["default_value"] = source_code;
	properties.push_back(property);
	if (instances.is_empty()) {
		if constexpr (VERBOSE_LOGGING) {
			ERR_PRINT("CPPScript::_get_script_property_list: No instances available.");
		}
		return properties;
	}
	CPPScriptInstance *instance = *instances.begin();
	if (instance == nullptr) {
		if constexpr (VERBOSE_LOGGING) {
			ERR_PRINT("CPPScript::_get_script_property_list: Instance is null.");
		}
		return properties;
	}
	ELFScriptInstance *elf = instance->get_script_instance();
	if (elf == nullptr) {
		return properties;
	}
	// Get the method information from the ELFScriptInstance
	properties.append_array(elf->get_elf_script()->_get_script_property_list());
	return properties;
}
int32_t CPPScript::_get_member_line(const StringName &p_member) const {
	return 0;
}
Dictionary CPPScript::_get_constants() const {
	return Dictionary();
}
TypedArray<StringName> CPPScript::_get_members() const {
	return TypedArray<StringName>();
}
bool CPPScript::_is_placeholder_fallback_enabled() const {
	return false;
}
Variant CPPScript::_get_rpc_config() const {
	return Variant();
}

CPPScript::CPPScript() {
	source_code = R"C0D3(#include "api.hpp"

PUBLIC Variant public_function(String arg) {
    print("Arguments: ", arg);
    return "Hello from the other side";
}
)C0D3";
}

void CPPScript::set_file(const String &p_path) {
	if (p_path.is_empty()) {
		WARN_PRINT("CPPScript::set_file: Empty resource path.");
		return;
	}
	this->path = p_path;
	this->source_code = FileAccess::get_file_as_string(p_path);
}

void CPPScript::remove_instance(CPPScriptInstance *p_instance) {
	instances.erase(p_instance);
	if (instances.is_empty()) {
		this->elf_script = nullptr;
	}
}
