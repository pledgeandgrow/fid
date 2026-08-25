use fidesc::pacte;

use std::process::exit;

fn main() {
    let args: Vec<String> = std::env::args().skip(1).collect();

    if args.is_empty() {
        pacte::print_help();
        exit(1);
    }

    let command = &args[0];
    let rest = &args[1..];

    let result = match command.as_str() {
        "init" => pacte::cmd_init(rest),
        "add" => pacte::cmd_add(rest),
        "install" | "i" => pacte::cmd_install(rest),
        "remove" | "rm" => pacte::cmd_remove(rest),
        "list" | "ls" => pacte::cmd_list(rest),
        "lock" => pacte::cmd_lock(rest),
        "search" => pacte::cmd_search(rest),
        "publish" => pacte::cmd_publish(rest),
        "update" => pacte::cmd_update(rest),
        "help" | "--help" | "-h" => {
            pacte::print_help();
            Ok(())
        }
        "version" | "--version" | "-v" => {
            println!("pacte 0.1.0 — Fides package manager");
            Ok(())
        }
        _ => {
            eprintln!("Unknown command: {}", command);
            eprintln!("Run 'pacte help' for usage.");
            exit(1);
        }
    };

    if let Err(e) = result {
        eprintln!("error: {}", e);
        exit(1);
    }
}
