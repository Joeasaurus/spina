#include "messages/messages.hpp"
#include "messages/socketer.hpp"
#include "exceptions/exceptions.hpp"
using namespace spina::messages;
using namespace spina::exceptions::spina;
using namespace spina::exceptions::uri;

#include "spina/spina.hpp"

namespace spina {

void Spina::command_moduleLoaded(MUri& mu, map<string, string>& variables) {
	if (variables.find("name") != variables.end()) {
		auto modName = variables["name"];
		registerModule(modName);
		Message m(name(), modName);
		m.setChannel(CHANNEL::Cmd);
		m.payload(modName + "://module/loaded?success=true");
		_socketer->sendMessage(m);
	}
}

}
