var metadata = {
    name: "Find-Flags",
    description: "Search the filesystem for common CTF flag files and display their contents"
};

/// COMMANDS

var cmd_find_flags = ax.create_command(
    "find_flags",
    "Recursively search C:\\ for flag files (user.txt, root.txt, local.txt, proof.txt, secret.txt, flag.txt) and print their contents",
    "find_flags"
);

cmd_find_flags.setPreHook(function (id, cmdline, parsed_json, ...parsed_lines) {
    let bof_path = ax.script_dir() + "find_flags." + ax.arch(id) + ".o";
    ax.execute_alias(id, cmdline, `execute bof "${bof_path}"`, "Task: Searching for flags...");
});

var cmd_group = ax.create_commands_group("Find-Flags", [cmd_find_flags]);
ax.register_commands_group(cmd_group, ["beacon", "gopher", "kharon"], ["windows"], []);
