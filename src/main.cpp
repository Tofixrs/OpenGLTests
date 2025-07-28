#include "app.hpp"
#include <print>
int main() {
	auto app_res = App::create();
	if (!app_res) {
		std::println("[APP_ERROR]: {}", app_res.error());
	}

	auto app = std::move(*app_res);

	app.run();
}
