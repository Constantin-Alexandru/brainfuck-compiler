#include <stack>
#include <string>
#include <fstream>
#include <string.h>
#include <iostream>
#include <assert.h>
#include <stdio.h>
#include <conio.h>

#define mem_size 30000

void interpret(std::string program);
// void compile(std::string program);

const char *help();

std::string getProgramFromFile(const char *filename);

int main(int argc, char *argv[])
{
  if (argc < 2)
  {
    std::cout << "Invalid number of arguments: " << argc << '\n'
              << help();
    return -1;
  }

  int ext_offset = strlen(argv[2]) - 3;

  if (strcmp(argv[2] + ext_offset, ".bf") != 0)
  {
    std::cout << "ERROR: invalid file " << argv[2];
    return -1;
  }

  std::string program = getProgramFromFile(argv[2]);

  if (program == "")
  {
    return -1;
  }

  if (strcmp(argv[1], "int") == 0)
  {
    interpret(program);
  }
  else if (strcmp(argv[2], "com") == 0)
  {
    // compile(program);
  }
  else
  {
    std::cout << "Invalid command: " << argv[1] << '\n';
    std::cout << help();
  }

  return 0;
}

const char *help()
{
  return "The program requires one of the following commands:\n   - int <filename> to interpret the program\n   - com <filename> to compile the program ";
}

std::string getProgramFromFile(const char *filename)
{
  std::string program;

  std::ifstream in(filename);

  if (!in.good())
  {
    std::cout << "ERROR: invalid file " << filename;
    return "";
  }

  while (!in.eof())
  {
    std::string line;

    std::getline(in, line);

    program += line;
  }

  in.close();

  return program;
}

void showNitems(char arr[], int size, int pos)
{
  for (int i = 0; i < size; i++)
  {
    if (i == pos)
    {
      std::cout << " > ";
    }

    std::cout << (int)arr[i] << " ";
  }

  std::cout << '\n';
}

void interpret(std::string program)
{
  char arr[mem_size] = {0};
  int pos = 0;

  std::stack<int> loops;

  int ops = 0;

  for (int i = 0; i < program.size(); i++)
  {

    if (program[i] == '>')
    {
      pos++;

      if (pos >= mem_size)
      {
        pos = pos - mem_size;
      }
    }
    else if (program[i] == '<')
    {
      pos--;

      if (pos < 0)
      {
        pos = mem_size + pos;
      }
    }
    else if (program[i] == '+')
    {
      arr[pos]++;
    }
    else if (program[i] == '-')
    {
      arr[pos]--;
    }
    else if (program[i] == '.')
    {
      std::cout << arr[pos];
    }
    else if (program[i] == ',')
    {
      arr[pos] = getch();
    }
    else if (program[i] == '[')
    {
    }
    else if (program[i] == ']')
    {
      if (loops.size() == 0)
      {
        std::cout << "ERROR: Unopened loop\n";
        return;
      }
    }
  }
}