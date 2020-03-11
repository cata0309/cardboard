#include <wlr_cpp/util/log.h>

#include <sys/socket.h>
#include <unistd.h>

#include <cerrno>
#include <cstdint>
#include <cstring>
#include <cstdio>
#include <vector>
#include <optional>

#include "IPC.h"
#include "Server.h"

using ParsedCommand = std::vector<std::string>;

static IPCCommandResult run_command(ParsedCommand cmd, [[maybe_unused]] Server* server)
{
    std::string stringified;
    for (auto& arg : cmd) {
        stringified += arg + ' ';
    }
    wlr_log(WLR_DEBUG, "got command %s", stringified.c_str());
    return {0, ""};
}

static std::optional<ParsedCommand> parse_command(uint8_t* cmd, ssize_t len)
{
    // The wire format is the following: commands are sent as words, the first word
    // being the command name, the others being the arguments. Each word is preceded
    // by its length in one byte. The final byte on the wire is 0.
    ParsedCommand r;
    int i = 0;
    while (i < len && cmd[i] != 0) {
        uint8_t segment_size = cmd[i];
        i++;
        r.push_back({});

        std::string& arg = r.back();
        arg.reserve(segment_size);

        if (i + segment_size >= len) {
            return std::nullopt;
        }
        for (int j = 0; j < segment_size; j++) {
            arg.push_back(cmd[i + j]);
        }

        i += segment_size;
    }

    if (i >= len || (i < len && cmd[i] != 0)) {
        return std::nullopt;
    }

    return r;
}

// This function implements wl_event_loop_fd_func_t
int ipc_read_command(int fd, [[maybe_unused]] uint32_t mask, void* data)
{
    Server* server = static_cast<Server*>(data);
    const int client_fd = accept(fd, nullptr, nullptr);
    if (client_fd == -1) {
        wlr_log(WLR_ERROR, "Failed to accept on IPC socket %s: %s", server->ipc_sock_address.sun_path, strerror(errno));
        return 1;
    }

    uint8_t cmd[BUFSIZ] = {};
    ssize_t len = recv(client_fd, cmd, sizeof(cmd) - 1, 0);
    std::optional<ParsedCommand> parsed = parse_command(cmd, len);

    if (!parsed) {
        constexpr std::string_view error = "Malformed command";
        send(client_fd, error.data(), error.size(), 0);
        close(client_fd);

        return 1;
    }

    IPCCommandResult result = run_command(*parsed, server);
    if (!result.message.empty()) {
        send(client_fd, result.message.c_str(), result.message.size(), 0);
    }
    close(client_fd);

    return result.code;
}