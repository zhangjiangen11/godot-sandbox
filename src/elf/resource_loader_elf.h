#pragma once

#include <godot_cpp/classes/resource_format_loader.hpp>
#include <godot_cpp/classes/resource_loader.hpp>

using namespace godot;

class ResourceFormatLoaderELF : public ResourceFormatLoader {
	GDCLASS(ResourceFormatLoaderELF, ResourceFormatLoader);

protected:
	static void _bind_methods() {}

public:
	virtual Variant _load(const String &path, const String &original_path, bool use_sub_threads, int32_t cache_mode) const override;
	virtual PackedStringArray _get_recognized_extensions() const override;
	virtual bool _recognize_path(const String &path, const StringName &type) const override;
	virtual bool _handles_type(const StringName &type) const override;
	virtual String _get_resource_type(const String &p_path) const override;
};
