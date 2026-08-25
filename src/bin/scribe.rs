use fidesc::scribe;

use std::process::exit;

fn main() {
    let args: Vec<String> = std::env::args().skip(1).collect();

    if args.is_empty() {
        scribe::print_help();
        exit(1);
    }

    let mut input: Option<String> = None;
    let mut output_dir = "docs".to_string();
    let mut format = "html".to_string();

    let mut i = 0;
    while i < args.len() {
        match args[i].as_str() {
            "--output" | "-o" => {
                i += 1;
                output_dir = args.get(i).cloned().unwrap_or("docs".to_string());
            }
            "--format" | "-f" => {
                i += 1;
                format = args.get(i).cloned().unwrap_or("html".to_string());
            }
            "--help" | "-h" => {
                scribe::print_help();
                exit(0);
            }
            "--version" | "-v" => {
                println!("scribe 0.1.0 — Fides documentation generator");
                exit(0);
            }
            _ => {
                if input.is_none() {
                    input = Some(args[i].clone());
                } else {
                    eprintln!("Unknown argument: {}", args[i]);
                    exit(1);
                }
            }
        }
        i += 1;
    }

    let input = match input {
        Some(p) => p,
        None => {
            eprintln!("Error: no input file or directory specified");
            eprintln!("Run 'scribe --help' for usage.");
            exit(1);
        }
    };

    let input_path = std::path::Path::new(&input);
    let output_path = std::path::Path::new(&output_dir);

    let result = if input_path.is_dir() {
        scribe::process_directory(input_path, output_path, &format)
    } else if input_path.is_file() {
        scribe::process_file(input_path, output_path, &format)
    } else {
        Err(format!("Input path does not exist: {}", input))
    };

    if let Err(e) = result {
        eprintln!("error: {}", e);
        exit(1);
    }
}
