use fidesc::ark;

use std::process::exit;

fn main() {
    let args: Vec<String> = std::env::args().skip(1).collect();

    if args.is_empty() {
        ark::print_help();
        exit(1);
    }

    let command = &args[0];
    let rest = &args[1..];

    let result = match command.as_str() {
        "init" => ark::cmd_init(rest),
        "add" => ark::cmd_add(rest),
        "install" | "i" => ark::cmd_install(rest),
        "remove" | "rm" => ark::cmd_remove(rest),
        "list" | "ls" => ark::cmd_list(rest),
        "lock" => ark::cmd_lock(rest),
        "search" => ark::cmd_search(rest),
        "publish" => ark::cmd_publish(rest),
        "update" => ark::cmd_update(rest),
        "help" | "--help" | "-h" => {
            ark::print_help();
            Ok(())
        }
        "version" | "--version" | "-v" => {
            println!("ark 0.1.0 — Fides package manager");
            Ok(())
        }
        _ => {
            eprintln!("Unknown command: {}", command);
            eprintln!("Run 'ark help' for usage.");
            exit(1);
        }
    };

    if let Err(e) = result {
        eprintln!("error: {}", e);
        exit(1);
    }
}
