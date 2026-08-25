use fidesc::veil;

use std::process::exit;

fn main() {
    let args: Vec<String> = std::env::args().skip(1).collect();

    if args.is_empty() {
        veil::print_help();
        exit(1);
    }

    let mut input: Option<String> = None;
    let mut output_dir = "coverage".to_string();
    let mut format = "text".to_string();

    let mut i = 0;
    while i < args.len() {
        match args[i].as_str() {
            "--format" | "-f" => {
                i += 1;
                format = args.get(i).cloned().unwrap_or("text".to_string());
            }
            "--output" | "-o" => {
                i += 1;
                output_dir = args.get(i).cloned().unwrap_or("coverage".to_string());
            }
            "--help" | "-h" => {
                veil::print_help();
                exit(0);
            }
            "--version" | "-v" => {
                println!("veil 0.1.0 — Fides code coverage tool");
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
            eprintln!("Error: no input file specified");
            eprintln!("Run 'veil --help' for usage.");
            exit(1);
        }
    };

    let input_path = std::path::Path::new(&input);
    let output_path = std::path::Path::new(&output_dir);

    if let Err(e) = veil::run(input_path, &format, output_path) {
        eprintln!("error: {}", e);
        exit(1);
    }
}
