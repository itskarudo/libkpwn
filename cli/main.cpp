#include "cmds/cmds.h"
#include <argparse/argparse.hpp>
#include <kpwn.h>
#include <string>

int main(int argc, char *argv[]) {
  argparse::ArgumentParser program("kpwn");
  program.add_description("kpwn - Kernel Pwn Toolkit");

  argparse::ArgumentParser init_command("init");
  init_command.add_description("initialize a new kpwn project");

  init_command.add_argument("directory")
      .help("directory to initialize the project in")
      .default_value(std::string{"."});

  program.add_subparser(init_command);

  if (argc < 2) {
    std::cout << program << std::endl;
    return 1;
  }

  try {
    program.parse_args(argc, argv);
  } catch (const std::exception &err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    return 1;
  }

  if (program.is_subcommand_used("init"))
    cmds::init(init_command.get<std::string>("directory"));
}
