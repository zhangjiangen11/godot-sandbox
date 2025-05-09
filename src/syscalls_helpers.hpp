#pragma once
#include <libriscv/machine.hpp>

#define APICALL(func) static void func(machine_t &machine [[maybe_unused]])

#ifdef ENABLE_SYSCALL_TRACE
#define SYS_TRACE(name, result, ...) sys_trace(name, result, ##__VA_ARGS__)
#else
#define SYS_TRACE(name, result, ...)
#endif

namespace riscv {

inline Sandbox &emu(machine_t &m) {
	return *m.get_userdata<Sandbox>();
}

// clang-format off
template <typename Result, typename... Args>
static inline void sys_trace(const String &name, Result result, Args &&...args) {
	char buffer[512];
	char *ptr = buffer;
	ptr += snprintf(ptr, sizeof(buffer), "[TRACE] %s (", name.utf8().ptr());
	([&] {
		if constexpr (std::is_same_v<Args, const char *>) {
			ptr += snprintf(ptr, sizeof(buffer) - (ptr - buffer), "%s", args);
		} else if constexpr (std::is_same_v<Args, String>) {
			ptr += snprintf(ptr, sizeof(buffer) - (ptr - buffer), "%s", args.utf8().ptr());
		} else if constexpr (std::is_same_v<Args, StringName>) {
			ptr += snprintf(ptr, sizeof(buffer) - (ptr - buffer), "%s", String(args).utf8().ptr());
		} else if constexpr (std::is_same_v<Args, GuestVariant *>) {
			ptr += snprintf(ptr, sizeof(buffer) - (ptr - buffer), "Variant(type=%d %s)", args->type, GuestVariant::type_name(args->type));
		} else if constexpr (std::is_pointer_v<Args>) {
			ptr += snprintf(ptr, sizeof(buffer) - (ptr - buffer), "%p", args);
		} else if constexpr (std::is_floating_point_v<Args>) {
			ptr += snprintf(ptr, sizeof(buffer) - (ptr - buffer), "%f", args);
		} else if constexpr (std::is_same_v<Args, gaddr_t>) {
			ptr += snprintf(ptr, sizeof(buffer) - (ptr - buffer), "0x%lX", long(args));
		} else {
			ptr += snprintf(ptr, sizeof(buffer) - (ptr - buffer), "%ld", long(args));
		}
	}(), ...);
	ptr += snprintf(ptr, sizeof(buffer) - (ptr - buffer), ") -> ");
	if constexpr (std::is_pointer_v<Result>) {
		ptr += snprintf(ptr, sizeof(buffer) - (ptr - buffer), "%p", result);
	} else if constexpr (std::is_same_v<Result, String>) {
		ptr += snprintf(ptr, sizeof(buffer) - (ptr - buffer), "%s", result.utf8().ptr());
	} else if constexpr (std::is_same_v<Result, Variant>) {
		ptr += snprintf(ptr, sizeof(buffer) - (ptr - buffer), "Variant(type=%d %s)", result.get_type(), GuestVariant::type_name(result.get_type()));
	} else if constexpr (std::is_floating_point_v<Result>) {
		ptr += snprintf(ptr, sizeof(buffer) - (ptr - buffer), "%f", result);
	} else if constexpr (std::is_same_v<Result, gaddr_t>) {
		ptr += snprintf(ptr, sizeof(buffer) - (ptr - buffer), "0x%lX", long(result));
	} else {
		ptr += snprintf(ptr, sizeof(buffer) - (ptr - buffer), "%ld", long(result));
	}
	ptr += snprintf(ptr, sizeof(buffer) - (ptr - buffer), "\n");
	if (ptr >= buffer + sizeof(buffer)) {
		ptr = buffer + sizeof(buffer) - 1;
	}
	fwrite(buffer, 1, ptr - buffer, stderr);
	fflush(stderr);
}
// clang-format on

	template <typename T>
	using CppVector = riscv::GuestStdVector<RISCV_ARCH, T>;

	using CppString = riscv::GuestStdString<RISCV_ARCH>;

} // namespace riscv
